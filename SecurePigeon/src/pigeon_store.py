"""Encrypted append-only storage for Secure Pigeon version 2."""

from __future__ import annotations

import ctypes
import ctypes.wintypes
import hashlib
import json
import os
import struct
import sys
import time
from collections.abc import Callable
from dataclasses import dataclass
from pathlib import Path

from cryptography.hazmat.primitives import hashes, serialization
from cryptography.hazmat.primitives.asymmetric import padding, rsa
from cryptography.hazmat.primitives.ciphers.aead import AESGCM

MAGIC = b"PIGEON02"
VERSION = 2
HEADER_SIZE = 4096
HEADER_FMT = "<8sI32s32sQ"   # magic(8) version(u32) author_fp(32) temporary_fp(32) capacity(u64)
RECORD_MAGIC = b"MSG3"
RECORD_FMT = "<4sQHHIIQ8s"   # marker(4) total(u64) author_len temporary_len meta_len(u16x3) chunk_size plain_size(u32x2) nonce_prefix(8)
RECORD_SIZE = struct.calcsize(RECORD_FMT)   # = 40
CHUNK_SIZE = 1048576         # 1 MiB
AUTHOR_LABEL = b"pigeon-v2-author"
TEMPORARY_LABEL = b"pigeon-v2-temporary"
_META_DOMAIN = b"pigeon-meta-v2"
# 元数据 AAD 是否绑定记录头。写死为 True；置 False 仅用于构造旧版
# （v2 原始程序）格式记录的兼容性测试。
_BIND_META_AAD = True


def _meta_aad(header_bytes: bytes) -> bytes:
    if _BIND_META_AAD:
        return _META_DOMAIN + header_bytes
    return _META_DOMAIN


def _try_meta_decrypt(aead, nonce: bytes, meta_cipher: bytes, header_bytes: bytes) -> bytes | None:
    """先按绑定记录头的 AAD（v3）解元数据，失败回落固定 AAD（v2 旧记录）。"""
    try:
        return aead.decrypt(nonce, meta_cipher, _META_DOMAIN + header_bytes)
    except Exception:
        pass
    try:
        return aead.decrypt(nonce, meta_cipher, _META_DOMAIN)
    except Exception:
        return None


class MailboxError(RuntimeError):
    """信箱操作错误。"""


@dataclass(frozen=True)
class StoreInfo:
    path: Path
    capacity: int
    used: int
    author_fingerprint: str
    temporary_fingerprint: str


def fingerprint(public_key: rsa.RSAPublicKey) -> bytes:
    wire = public_key.public_bytes(
        serialization.Encoding.DER, serialization.PublicFormat.SubjectPublicKeyInfo
    )
    return hashlib.sha256(wire).digest()


def load_public_key(path: Path) -> rsa.RSAPublicKey:
    key = serialization.load_pem_public_key(path.read_bytes())
    if not isinstance(key, rsa.RSAPublicKey):
        raise MailboxError("RSA public key required.")
    return key


def load_private_key(path: Path, password: str) -> rsa.RSAPrivateKey:
    key = serialization.load_pem_private_key(path.read_bytes(), password.encode("utf-8"))
    if not isinstance(key, rsa.RSAPrivateKey):
        raise MailboxError("RSA private key required.")
    return key


def new_rsa_keypair() -> rsa.RSAPrivateKey:
    return rsa.generate_private_key(public_exponent=65537, key_size=3072)


def write_author_keypair(private_path: Path, public_path: Path, password: str) -> None:
    if len(password) < 10:
        raise MailboxError("Private-key password must contain at least 10 characters.")
    private_key = new_rsa_keypair()
    private_path.write_bytes(
        private_key.private_bytes(
            serialization.Encoding.PEM,
            serialization.PrivateFormat.PKCS8,
            serialization.BestAvailableEncryption(password.encode("utf-8")),
        )
    )
    public_path.write_bytes(
        private_key.public_key().public_bytes(
            serialization.Encoding.PEM, serialization.PublicFormat.SubjectPublicKeyInfo
        )
    )


def private_key_pem(private_key: rsa.RSAPrivateKey) -> bytes:
    return private_key.private_bytes(
        serialization.Encoding.PEM,
        serialization.PrivateFormat.PKCS8,
        serialization.NoEncryption(),
    )


def _record_end(handle, capacity) -> int:
    pos = HEADER_SIZE
    while pos + RECORD_SIZE <= capacity:
        handle.seek(pos)
        raw = handle.read(RECORD_SIZE)
        if raw == b"\x00" * RECORD_SIZE:          # 全零记录槽 = 数据区结束
            return pos
        try:
            marker, total, author_len, temporary_len, meta_len, chunk_size, plain_size, nonce_prefix = struct.unpack(
                RECORD_FMT, raw
            )
        except struct.error:
            return pos
        if not _valid_record_header(marker, total, author_len, temporary_len, meta_len, chunk_size, plain_size):
            return pos
        if pos + total > capacity:
            return pos
        pos += total
    return pos


def _valid_record_header(marker, total, author_len, temporary_len, meta_len, chunk_size, plain_size) -> bool:
    """记录头字段一致性校验（长度自洽 + 合理上限）。

    记录头本身明文且（在 v2 中）无 MAC，恶意/损坏的仓可能带任意长度字段；
    本校验确保 total 与各长度字段严格自洽，防止 extract_all 按伪造的
    plain_size 触发超大分配。meta 密文的 GCM AAD 绑定整个记录头（见
    append_file），字段被篡改时元数据解密必然失败。
    """
    if marker != RECORD_MAGIC:
        return False
    if author_len != temporary_len:
        return False
    if not (256 <= author_len <= 4096):        # RSA-3072 OAEP 密文 = 384
        return False
    if not (16 <= meta_len <= 8192):
        return False
    if not (4096 <= chunk_size <= 4194304):
        return False
    if plain_size < 0:
        return False
    chunks = (plain_size + chunk_size - 1) // chunk_size
    minimum = RECORD_SIZE + author_len + temporary_len + meta_len + plain_size + chunks * 16
    return total == minimum


def _read_header(path: Path) -> tuple[bytes, bytes, int]:
    """只读 4 KiB 文件仓头，返回 ``(author_fp, temporary_fp, capacity)``。"""
    with path.open("rb") as handle:
        raw = handle.read(struct.calcsize(HEADER_FMT))
        try:
            magic, version, author_fp, temporary_fp, capacity = struct.unpack(HEADER_FMT, raw)
        except struct.error as exc:
            raise MailboxError("Invalid mailbox format.") from exc
        if magic != MAGIC:
            raise MailboxError(f"Mailbox header magic mismatch: expected {MAGIC!r}, got {magic!r}.")
        if version != VERSION:
            raise MailboxError(f"Mailbox version mismatch: expected {VERSION}, got {version}.")
        if capacity != path.stat().st_size:
            raise MailboxError(
                f"Mailbox size mismatch: header says {capacity} bytes, file is {path.stat().st_size} bytes."
            )
        return author_fp, temporary_fp, capacity


def inspect_store(
    path: Path,
    author_public_key: rsa.RSAPublicKey | None = None,
    temporary_public_key: rsa.RSAPublicKey | None = None,
) -> StoreInfo:
    if not path.exists():
        raise MailboxError("Mailbox store was not found.")
    with path.open("rb") as handle:
        raw = handle.read(struct.calcsize(HEADER_FMT))
        try:
            magic, version, author_fp, temporary_fp, capacity = struct.unpack(HEADER_FMT, raw)
        except struct.error as exc:
            raise MailboxError("Invalid mailbox format.") from exc
        if magic != MAGIC:
            raise MailboxError(f"Mailbox header magic mismatch: expected {MAGIC!r}, got {magic!r}.")
        if version != VERSION:
            raise MailboxError(f"Mailbox version mismatch: expected {VERSION}, got {version}.")
        if capacity != path.stat().st_size:
            raise MailboxError(
                f"Mailbox size mismatch: header says {capacity} bytes, file is {path.stat().st_size} bytes."
            )
        if author_public_key is not None and author_fp != fingerprint(author_public_key):
            raise MailboxError("Author public key does not match this mailbox.")
        if temporary_public_key is not None and temporary_fp != fingerprint(temporary_public_key):
            raise MailboxError("Temporary-access public key does not match this mailbox.")
        used = _record_end(handle, capacity)
    return StoreInfo(path, capacity, used, author_fp.hex()[:16], temporary_fp.hex()[:16])


def _make_sparse(handle) -> None:
    """Mark an open file handle as an NTFS sparse file so zero-filled regions
    occupy no disk blocks.  Reads of sparse regions still return zeros on
    Windows, so ``_record_end`` scanning is unaffected."""
    if sys.platform != "win32":
        return
    import msvcrt

    FSCTL_SET_SPARSE = 590020   # 0x00090004
    kernel32 = ctypes.WinDLL("kernel32", use_last_error=True)
    raw_handle = msvcrt.get_osfhandle(handle.fileno())
    result = ctypes.wintypes.DWORD()
    kernel32.DeviceIoControl(
        ctypes.wintypes.HANDLE(raw_handle),
        FSCTL_SET_SPARSE,
        None,
        0,
        None,
        0,
        ctypes.byref(result),
        None,
    )


def initialize_store(
    path: Path,
    author_public_key: rsa.RSAPublicKey,
    temporary_public_key: rsa.RSAPublicKey,
    capacity: int,
    progress: Callable[[int, int], None] | None = None,
) -> None:
    if capacity <= HEADER_SIZE + 1048576:
        raise MailboxError("At least 2 MB of free space is required.")
    total_data = capacity - HEADER_SIZE
    with path.open("w+b") as handle:
        handle.write(
            struct.pack(
                HEADER_FMT,
                MAGIC,
                VERSION,
                fingerprint(author_public_key),
                fingerprint(temporary_public_key),
                capacity,
            )
        )
        handle.write(b"\x00" * (HEADER_SIZE - handle.tell()))
        if sys.platform == "win32":
            _make_sparse(handle)
            handle.seek(capacity - 1)
            handle.write(b"\x00")
            if progress is not None:
                progress(total_data, total_data)
        else:
            remaining = total_data
            zeros = b"\x00" * 4194304
            written = 0
            next_report = 67108864
            while remaining:
                amount = min(remaining, len(zeros))
                handle.write(zeros[:amount])
                remaining -= amount
                written += amount
                if progress is not None and (written >= next_report or not remaining):
                    progress(written, total_data)
                    next_report += 67108864
        handle.flush()
        os.fsync(handle.fileno())


def _record_length(author_len: int, temporary_len: int, meta_len: int, file_size: int, chunk_size: int) -> int:
    chunks = (file_size + chunk_size - 1) // chunk_size
    return RECORD_SIZE + author_len + temporary_len + meta_len + file_size + chunks * 16


def append_file(
    store_path: Path,
    author_public_key: rsa.RSAPublicKey,
    temporary_public_key: rsa.RSAPublicKey,
    source: Path,
    progress: Callable[[int, int], None] | None = None,
    used_hint: int | None = None,
) -> tuple[int, int, int]:
    """追加加密一个文件到 store。"""
    if not source.is_file():
        raise MailboxError("所选文件无法读取，请重新选择。")
    if used_hint is not None:
        author_fp, temporary_fp, capacity = _read_header(store_path)
        if author_fp != fingerprint(author_public_key):
            raise MailboxError("Author public key does not match this mailbox.")
        if temporary_fp != fingerprint(temporary_public_key):
            raise MailboxError("Temporary-access public key does not match this mailbox.")
        used = used_hint
        if used > capacity:
            info = inspect_store(store_path, author_public_key, temporary_public_key)
            used, capacity = info.used, info.capacity
    else:
        info = inspect_store(store_path, author_public_key, temporary_public_key)
        used, capacity = info.used, info.capacity

    file_size = source.stat().st_size
    data_key = AESGCM.generate_key(bit_length=256)
    author_wrapped = author_public_key.encrypt(
        data_key,
        padding.OAEP(
            mgf=padding.MGF1(hashes.SHA256()), algorithm=hashes.SHA256(), label=AUTHOR_LABEL
        ),
    )
    temporary_wrapped = temporary_public_key.encrypt(
        data_key,
        padding.OAEP(
            mgf=padding.MGF1(hashes.SHA256()), algorithm=hashes.SHA256(), label=TEMPORARY_LABEL
        ),
    )
    nonce_prefix = os.urandom(8)
    metadata = json.dumps(
        {
            "name": source.name,
            "size": file_size,
            "added_utc": time.strftime("%Y-%m-%dT%H:%M:%SZ", time.gmtime()),
        },
        ensure_ascii=False,
        separators=(",", ":"),
    ).encode("utf-8")
    aead = AESGCM(data_key)
    # 记录头（含各长度字段）绑定进元数据 GCM 的 AAD：篡改记录头的任何字段
    # 都会使元数据解密失败，从而在导出时被识别为损坏记录。
    meta_cipher_len = len(metadata) + 16
    total = _record_length(len(author_wrapped), len(temporary_wrapped), meta_cipher_len, file_size, CHUNK_SIZE)
    header_bytes = struct.pack(
        RECORD_FMT,
        RECORD_MAGIC,
        total,
        len(author_wrapped),
        len(temporary_wrapped),
        meta_cipher_len,
        CHUNK_SIZE,
        file_size,
        nonce_prefix,
    )
    meta_cipher = aead.encrypt(
        nonce_prefix + (0).to_bytes(4, "big"),
        metadata,
        _meta_aad(header_bytes),
    )
    if used + total > capacity:
        raise MailboxError("文件仓空间不足，请先取件释放空间后再试。")

    with store_path.open("r+b") as target, source.open("rb") as source_handle:
        locked = False
        if sys.platform == "win32":
            import msvcrt

            # 锁文件最后一个字节做跨进程互斥：该字节只在 initialize_store 时
            # 写过、读写路径都不触碰，且 Windows 区锁是强制的——锁字节 0 会
            # 连 inspect_store 读头部都一并阻塞。
            target.seek(capacity - 1)
            try:
                msvcrt.locking(target.fileno(), msvcrt.LK_LOCK, 1)
                locked = True
            except OSError as exc:
                raise MailboxError("文件仓正被其他程序使用，请稍后重试。") from exc
        try:
            target.seek(used + RECORD_SIZE)
            target.write(author_wrapped)
            target.write(temporary_wrapped)
            target.write(meta_cipher)
            index = 1
            written = 0
            if progress is not None:
                progress(0, file_size)
            while chunk := source_handle.read(CHUNK_SIZE):
                target.write(aead.encrypt(nonce_prefix + index.to_bytes(4, "big"), chunk, b"pigeon-file-v2"))
                written += len(chunk)
                index += 1
                if progress is not None:
                    progress(written, file_size)
            target.flush()
            os.fsync(target.fileno())
            # 先写数据、最后写记录头，保证崩溃安全
            target.seek(used)
            target.write(header_bytes)
            target.flush()
            os.fsync(target.fileno())
        finally:
            if locked:
                try:
                    target.seek(capacity - 1)
                    msvcrt.locking(target.fileno(), msvcrt.LK_UNLCK, 1)
                except OSError:
                    pass
    new_used = used + total
    return total, capacity - new_used, new_used


def _next_output_name(destination: Path, safe_name: str) -> Path:
    """返回 dest 下不存在的输出名，必要时加 (1)/(2) 后缀。"""
    output = destination / safe_name
    suffix = 1
    while output.exists():
        output = destination / f"{Path(safe_name).stem} ({suffix}){Path(safe_name).suffix}"
        suffix += 1
    return output


def extract_all(
    store_path: Path,
    private_key: rsa.RSAPrivateKey,
    destination: Path,
    *,
    temporary: bool = False,
    progress: Callable[[int, int], None] | None = None,
) -> tuple[list[Path], int]:
    """导出 store 中所有文件（安全复制，原仓记录保持不变）。"""
    info = inspect_store(store_path)
    destination.mkdir(parents=True, exist_ok=True)
    for stale in destination.glob(".pigeon-tmp-*"):
        try:
            stale.unlink()
        except OSError:
            continue
    extracted: list[Path] = []
    corrupted = 0
    label = TEMPORARY_LABEL if temporary else AUTHOR_LABEL
    with store_path.open("rb") as handle:
        # 第一遍：只数记录条数（用于进度条分母）
        record_count = 0
        pos = HEADER_SIZE
        while pos + RECORD_SIZE <= info.used:
            handle.seek(pos)
            raw = handle.read(RECORD_SIZE)
            if raw == b"\x00" * RECORD_SIZE:
                break
            try:
                marker, total, author_len, temporary_len, meta_len, chunk_size, plain_size, nonce_prefix = struct.unpack(
                    RECORD_FMT, raw
                )
            except struct.error:
                break
            if not _valid_record_header(marker, total, author_len, temporary_len, meta_len, chunk_size, plain_size):
                break
            if pos + total > info.used:
                break
            record_count += 1
            pos += total
        if progress is not None:
            progress(0, record_count)
        # 第二遍：逐条解密导出
        done = 0
        pos = HEADER_SIZE
        while pos + RECORD_SIZE <= info.used:
            handle.seek(pos)
            raw = handle.read(RECORD_SIZE)
            try:
                marker, total, author_len, temporary_len, meta_len, chunk_size, plain_size, nonce_prefix = struct.unpack(
                    RECORD_FMT, raw
                )
            except struct.error:
                break
            if not _valid_record_header(marker, total, author_len, temporary_len, meta_len, chunk_size, plain_size):
                # 记录头不自洽：无法可靠跳到下一条，剩余全部计为损坏
                corrupted += max(0, record_count - done)
                break
            tmp_output = None
            try:
                author_wrapped = handle.read(author_len)
                temporary_wrapped = handle.read(temporary_len)
                wrapped = temporary_wrapped if temporary else author_wrapped
                data_key = private_key.decrypt(
                    wrapped,
                    padding.OAEP(
                        mgf=padding.MGF1(hashes.SHA256()),
                        algorithm=hashes.SHA256(),
                        label=label,
                    ),
                )
                aead = AESGCM(data_key)
                meta_nonce = nonce_prefix + (0).to_bytes(4, "big")
                meta_cipher = handle.read(meta_len)
                # 优先按绑定记录头的 AAD 解元数据（v3 格式）；失败则回落到
                # 固定 AAD（v2 原始程序写入的旧记录）。
                metadata_bytes = _try_meta_decrypt(aead, meta_nonce, meta_cipher, header_bytes=raw)
                if metadata_bytes is None:
                    raise MailboxError("record metadata failed authentication")
                metadata = json.loads(metadata_bytes)
                safe_name = Path(str(metadata["name"])).name or "unnamed-file"
                final_output = _next_output_name(destination, safe_name)
                tmp_name = f".pigeon-tmp-{os.urandom(6).hex()}{Path(safe_name).suffix}"
                tmp_output = destination / tmp_name
                remaining = plain_size
                with tmp_output.open("xb") as out:
                    for index in range(1, (plain_size + chunk_size - 1) // chunk_size + 1):
                        cipher_len = min(chunk_size, remaining) + 16
                        encrypted = handle.read(cipher_len)
                        out.write(
                            aead.decrypt(
                                nonce_prefix + index.to_bytes(4, "big"),
                                encrypted,
                                b"pigeon-file-v2",
                            )
                        )
                        remaining -= min(chunk_size, remaining)
                os.replace(tmp_output, final_output)
                extracted.append(final_output)
            except Exception:
                if tmp_output is not None and tmp_output.exists():
                    try:
                        tmp_output.unlink()
                    except OSError:
                        pass
                corrupted += 1
            done += 1
            if progress is not None:
                progress(done, record_count)
            pos += total
    return extracted, corrupted
