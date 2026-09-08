"""Offline TOTP temporary-access capability for a Secure Pigeon USB.

设计要点（v3，重构）
====================

历史缺陷
--------
v1：seed 明文在文件头 + sha256(seed) 直接派生私钥加密密钥——持盘者一步解出
临时 RSA 私钥并自行生成所有动态码。

v2：私钥加密密钥改为 ``sha256(seed ‖ code)``（GCM 校验通过即码正确），但存在
三个问题：

1. **输入的码没有参与验证**——``verify_and_consume`` 只用窗口候选时间片的码
   试解密，用户输入的 6 位数除格式检查外完全未被使用：任何 6 位数字都能
   通过验证（原始程序的真实缺陷，已从字节码核实）；
2. **凭据被绑定在部署时刻的时间片**——加密用的是 ``current_counter()+1`` 的
   码，而窗口只有 ±3 片（约 90 秒）：部署后两分钟内没有完成首次临时取件，
   该凭据就永远失效；
3. **seed 明文可读**——持盘者可直接从文件头读出 TOTP 种子，自行生成任何
   时刻的动态码。

v3 的修正
---------
1. **seed 掩码存储**：seed 以 ``XOR scrypt(卷序列号, salt)`` 混淆后写入文件头
   （salt 每次部署随机），与激活 license 的 PLF3 同一哲学——还原 seed 需要
   理解格式 + 一次 scrypt，而不是"读文件头"。
2. **输入码真正生效**：解密密钥只由 seed 派生（``scrypt(HMAC(domain, seed))``，
   与码无关，因此授权可以发生在任意时刻），但验证时强制要求输入码与当前
   窗口内某一候选时间片的码常数时间匹配——手机端（作者）的动态码第一次
   真正成为门禁。
3. **窗口检查与解密分离**：修掉"部署 90 秒后失效"缺陷，任意时刻授权均可。
4. **AAD 绑定**：magic/version/还原后的 seed/nonce 进入 GCM AAD；篡改即解密
   失败。fail_count/last_used 仍可原地更新（每次授权后要写回）。
5. **版本号 3**：v2/v1 凭据不再接受，旧盘需重新部署。
6. 保留：``candidate > last_used`` 单调防重放、失败计数节流（阈值后烧掉当前
   时间片）、msvcrt 独占锁（并修正解锁区间错位）。

物理边界（诚实声明）
--------------------
只要"验证秘密"与"被保护数据"在同一块可写 U 盘上，离线方案对**理解格式的
持盘者**就只能提高成本而不能根除（与 PLF3 激活方案一致）。v3 把攻击成本从
"用十六进制编辑器读 seed"提高到"逆向格式 + 两次 scrypt + 重新实现方案"。
已知残余：持盘者备份旧版凭据文件 + 录得某枚已用码后，可回滚 last_used 重放；
fail_count/last_used 为明文可写字段，可被清除节流。两者都要求物理占有。
"""

from __future__ import annotations

import base64
import hashlib
import hmac
import msvcrt
import os
import struct
import time
from pathlib import Path

from cryptography.hazmat.primitives import serialization
from cryptography.hazmat.primitives.asymmetric import rsa
from cryptography.hazmat.primitives.ciphers.aead import AESGCM

from pigeon_store import MailboxError, private_key_pem

ACCESS_NAME = '.pigeon.temp.access'
PUBLIC_NAME = '.pigeon.temp.public.pem'
MAGIC = b'TPA2'
VERSION = 3

HEADER_FMT = '<4sI20s16s12sIIQ'
HEADER_SIZE = struct.calcsize(HEADER_FMT)
LAST_USED_OFFSET = HEADER_SIZE - 8
FAIL_COUNT_OFFSET = HEADER_SIZE - 12
PERIOD_SECONDS = 30
KEY_DOMAIN = b'secure-pigeon-temporary-access-v3'
AEAD_AAD = b'pigeon-temp-key-v3'
FAIL_THRESHOLD = 5

_SCRYPT_N = 1 << 15          # 32768 — 每次约 0.1–0.5 s
_SCRYPT_R = 8
_SCRYPT_P = 1
_SCRYPT_MAXMEM = 64 * 1024 * 1024


def read_volume_serial(root: Path) -> int:
    """读取卷序列号（与 activation 相同语义；独立实现避免模块环依赖）。"""
    import ctypes
    try:
        volume = ctypes.create_unicode_buffer(261)
        filesystem = ctypes.create_unicode_buffer(261)
        serial = ctypes.c_uint32()
        maximum = ctypes.c_uint32()
        flags = ctypes.c_uint32()
        ok = ctypes.windll.kernel32.GetVolumeInformationW(
            str(root), volume, len(volume), ctypes.byref(serial),
            ctypes.byref(maximum), ctypes.byref(flags), filesystem, len(filesystem),
        )
        return serial.value if ok else 0
    except OSError:
        return 0


def _stretched_mask(serial: int, salt: bytes) -> bytes:
    return hashlib.scrypt(
        serial.to_bytes(4, "little"), salt=salt,
        n=_SCRYPT_N, r=_SCRYPT_R, p=_SCRYPT_P, dklen=20,
        maxmem=_SCRYPT_MAXMEM,
    )


def _xor(a: bytes, b: bytes) -> bytes:
    return bytes(x ^ y for x, y in zip(a, b))


def _store_key(seed: bytes, nonce: bytes) -> bytes:
    """私钥密文的对称密钥：仅由 seed 派生（与动态码无关 → 任意时刻可授权）。"""
    base = hmac.new(KEY_DOMAIN, seed, hashlib.sha256).digest()
    return hashlib.scrypt(
        base, salt=nonce,
        n=_SCRYPT_N, r=_SCRYPT_R, p=_SCRYPT_P, dklen=32,
        maxmem=_SCRYPT_MAXMEM,
    )


def _aad(seed: bytes, nonce: bytes) -> bytes:
    """GCM AAD：绑定 magic/version/还原后的 seed/nonce。

    fail_count/last_used 不在内（每次授权后要原地更新）；
    cipher_length 由 GCM tag 隐式认证。
    """
    return AEAD_AAD + MAGIC + bytes([VERSION]) + seed + nonce


def provision(root: Path, temporary_private_key: rsa.RSAPrivateKey) -> bytes:
    """生成 U 盘上的公开临时公钥与受动态码门控的临时私钥凭据，返回 TOTP seed。

    seed 的明文只返回给调用方（用于生成二维码，保存在作者端）；写入 U 盘的
    是 scrypt 掩码混淆后的副本。手机端用 seed 生成动态码，U 盘端用 seed 解
    私钥 + 校验输入码是否为当前有效时间片的码。
    """
    seed = os.urandom(20)
    nonce = os.urandom(12)
    salt = os.urandom(16)
    serial = read_volume_serial(root)
    masked_seed = _xor(seed, _stretched_mask(serial, salt))
    key = _store_key(seed, nonce)
    cipher = AESGCM(key).encrypt(nonce, private_key_pem(temporary_private_key), _aad(seed, nonce))
    (root / PUBLIC_NAME).write_bytes(
        temporary_private_key.public_key().public_bytes(
            serialization.Encoding.PEM, serialization.PublicFormat.SubjectPublicKeyInfo
        )
    )
    header = struct.pack('<4sI20s16s12sIIQ', MAGIC, VERSION, masked_seed, salt, nonce, len(cipher), 0, 0)
    with (root / ACCESS_NAME).open('wb') as handle:
        handle.write(header)
        handle.write(cipher)
    return seed


def _parse_header(raw: bytes) -> tuple[bytes, bytes, bytes, int, int, int]:
    """解析文件头，返回 (masked_seed, salt, nonce, fail_count, last_used, cipher_length)。

    校验失败抛 MailboxError。v1/v2 凭据不再接受（旧盘需重新部署）。
    """
    fmt = HEADER_FMT
    try:
        magic, version, masked_seed, salt, nonce, cipher_length, fail_count, last_used = struct.unpack(fmt, raw)
    except struct.error as exc:
        raise MailboxError('Temporary access record is invalid.') from exc
    if magic != MAGIC or version != VERSION or cipher_length < 16:
        if magic == MAGIC and version < VERSION:
            raise MailboxError('临时取件凭据为旧版本格式，请重新部署 U 盘。')
        raise MailboxError('Temporary access record is invalid.')
    return masked_seed, salt, nonce, fail_count, last_used, cipher_length


def _unmask_seed(masked_seed: bytes, serial: int, salt: bytes) -> bytes:
    return _xor(masked_seed, _stretched_mask(serial, salt))


def _try_decrypt(nonce: bytes, cipher: bytes, key: bytes, aad: bytes) -> bytes | None:
    try:
        return AESGCM(key).decrypt(nonce, cipher, aad)
    except Exception:
        return None


def code_for_counter(seed: bytes, counter: int) -> str:
    digest = hmac.new(seed, struct.pack('>Q', counter), hashlib.sha1).digest()
    offset = digest[-1] & 15
    value = (struct.unpack('>I', digest[offset : offset + 4])[0] & 0x7FFFFFFF) % 1000000
    return f'{value:06d}'


def current_counter(now: float | None = None) -> int:
    return int(now // PERIOD_SECONDS if now is not None else time.time() // PERIOD_SECONDS)


def _window_candidates(counter: int, last_used: int) -> list[int]:
    """返回窗口内满足 ``candidate > last_used`` 的候选 counter，从小到大排列。"""
    out = []
    for offset in (0, -1, -2, -3, 1):
        candidate = counter + offset
        if candidate > last_used:
            out.append(candidate)
    out.sort()
    return out


def _match_window_code(seed: bytes, entered: str, counter: int, last_used: int) -> int | None:
    """常数时间比较输入码与窗口候选码；命中返回对应 counter，否则 None。"""
    for candidate in _window_candidates(counter, last_used):
        if hmac.compare_digest(entered, code_for_counter(seed, candidate)):
            return candidate
    return None


def verify_and_consume(root: Path, code: str, now: float | None = None) -> tuple[bytes, rsa.RSAPrivateKey, int]:
    """以独占文件锁原子完成「解私钥 → 校验输入码是否为当前有效动态码 → 写回计数器」。

    流程：还原 seed（scrypt）→ 解密临时私钥（scrypt，密钥与码无关）→ 将输入码
    与窗口内候选时间片的码常数时间比对（``candidate > last_used`` 单调防重放）
    → 成功则清零失败计数并把命中的 counter 写回 ``last_used``；失败则
    ``fail_count`` 加 1，达到阈值后把当前 counter 写回 ``last_used``（烧掉本片
    强制冷却）。

    返回 ``(seed, private_key, counter)``。
    """
    normalized = ''.join(ch for ch in code if ch.isdigit())
    if len(normalized) != 6:
        raise MailboxError('临时取件码格式不正确，请输入 6 位数字。')
    counter = current_counter(now)
    path = root / ACCESS_NAME
    fd = os.open(path, os.O_RDWR | os.O_BINARY)
    try:
        try:
            msvcrt.locking(fd, msvcrt.LK_LOCK, 1)
        except OSError as exc:
            raise MailboxError('无法锁定临时取件凭据，请稍后重试。') from exc
        try:
            with os.fdopen(fd, 'r+b', closefd=False) as handle:
                raw = handle.read(HEADER_SIZE)
                masked_seed, salt, nonce, fail_count, last_used, cipher_length = _parse_header(raw)
                cipher = handle.read(cipher_length)
                serial = read_volume_serial(root)
                seed = _unmask_seed(masked_seed, serial, salt)
                pem = _try_decrypt(nonce, cipher, _store_key(seed, nonce), _aad(seed, nonce))
                if pem is None:
                    # 密文/头部被篡改（AAD 或 tag 校验失败）
                    raise MailboxError('临时取件凭据已损坏或被篡改。')
                matched = _match_window_code(seed, normalized, counter, last_used)
                if matched is None:
                    new_fail = fail_count + 1
                    forced_used = last_used
                    if new_fail >= FAIL_THRESHOLD and counter > last_used:
                        forced_used = counter
                        new_fail = 0
                    handle.seek(FAIL_COUNT_OFFSET)
                    handle.write(struct.pack('<I', new_fail))
                    if forced_used != last_used:
                        handle.seek(LAST_USED_OFFSET)
                        handle.write(struct.pack('<Q', forced_used))
                    handle.flush()
                    os.fsync(handle.fileno())
                    if forced_used != last_used:
                        raise MailboxError('连续多次失败，请等待下一枚动态码后重试。')
                    raise MailboxError('临时取件码无效、已过期或已被使用。')
                handle.seek(FAIL_COUNT_OFFSET)
                handle.write(struct.pack('<I', 0))
                handle.seek(LAST_USED_OFFSET)
                handle.write(struct.pack('<Q', matched))
                handle.flush()
                os.fsync(handle.fileno())
        finally:
            # 解锁前必须回到加锁的字节区间（读文件后 fd 位置已移动）
            try:
                os.lseek(fd, 0, os.SEEK_SET)
                msvcrt.locking(fd, msvcrt.LK_UNLCK, 1)
            except OSError:
                pass
        try:
            private_key = serialization.load_pem_private_key(pem, password=None)
        except Exception as exc:
            raise MailboxError('Temporary access record is damaged.') from exc
        if not isinstance(private_key, rsa.RSAPrivateKey):
            raise MailboxError('Temporary access private key is invalid.')
        return seed, private_key, matched
    finally:
        try:
            os.close(fd)
        except OSError:
            pass


def load_private_key(root: Path, code: str) -> tuple[bytes, rsa.RSAPrivateKey, int]:
    """旧接口兼容入口：不锁文件、不写回计数器、不计失败次数。

    仅供离线/测试场景；生产路径必须走 ``verify_and_consume``。
    """
    path = root / ACCESS_NAME
    with path.open('rb') as handle:
        raw = handle.read(HEADER_SIZE)
        masked_seed, salt, nonce, _fail_count, _last_used, cipher_length = _parse_header(raw)
        cipher = handle.read(cipher_length)
    normalized = ''.join(ch for ch in code if ch.isdigit())
    if len(normalized) != 6:
        raise MailboxError('临时取件码格式不正确。')
    serial = read_volume_serial(root)
    seed = _unmask_seed(masked_seed, serial, salt)
    pem = _try_decrypt(nonce, cipher, _store_key(seed, nonce), _aad(seed, nonce))
    if pem is None:
        raise MailboxError('Temporary access record is invalid.')
    try:
        private_key = serialization.load_pem_private_key(pem, password=None)
    except Exception as exc:
        raise MailboxError('Temporary access record is damaged.') from exc
    if not isinstance(private_key, rsa.RSAPrivateKey):
        raise MailboxError('Temporary access private key is invalid.')
    return seed, private_key, 0


def totp_uri(seed: bytes, label: str = 'Secure Pigeon') -> str:
    account = base64.urlsafe_b64encode(hashlib.sha256(seed).digest()[:6]).decode('ascii').rstrip('=')
    secret = base64.b32encode(seed).decode('ascii').rstrip('=')
    return f'otpauth://totp/{label}:{account}?secret={secret}&issuer={label}&algorithm=SHA1&digits=6&period=30'


def save_qr(seed: bytes, path: Path) -> None:
    import qrcode
    qrcode.make(totp_uri(seed)).save(path)
