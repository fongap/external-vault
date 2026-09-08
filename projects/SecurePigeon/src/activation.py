"""Trial and permanent activation for Secure Pigeon USB drives.

Device identity
---------------
Each USB is identified by its NTFS volume serial number (a 32-bit integer
assigned when the volume is formatted).  The serial is displayed as 8 hex
chars (e.g. ``ABCD1234``) and used as the ``device_id``.

Master key
----------
During deployment a random 32-byte *master key* is generated.  An obfuscated
copy is stored on the USB so the inbox can verify activation codes without
phoning home.  The plain-text master key is saved to the author's key folder
and is needed by the activation-code generator tool.

Activation code
---------------

    device_id       = format_serial(usb_volume_serial)
    activation_code = HMAC-SHA256(master_key, device_id).hex()[:16]
                      → "XXXX-XXXX-XXXX-XXXX" (19 chars with dashes)

License file (``.pigeon.license``) — version 3 (``PLF3``)
----------------------------------------------------------
    magic    b"PLF3"                            (4)
    salt     scrypt salt                        (16)
    obf_key  master_key XOR scrypt(serial,salt) (32)
    tag      HMAC-SHA256(master_key,
                         b"pigeon-activation-v3" || serial_le32 || code)
                                                    (64 ascii hex)
    author_fp SHA-256 of the author public key's SPKI DER   (32, or zeros)
    temp_fp   SHA-256 of the temporary public key's SPKI DER (32, or zeros)
    [opt]    b"\\n" + "XXXX-XXXX-XXXX-XXXX"      (cached verified code)

Key properties versus the legacy format (unkeyed SHA-256 of the code):

* The stored tag is keyed with the master key, so a forged license requires
  recovering the master key first — the obfuscation mask is now scrypt
  stretched with a per-deployment random salt instead of a bare SHA-256 of
  the serial, and the code itself is a 64-bit HMAC value, so there is no
  "write any code + its plain hash" bypass any more.
* The tag binds the volume serial, so copying an activated license file to
  another drive fails verification (the master key is un-masked with the
  *current* drive's serial; a cloned file yields garbage).
* The two public-key fingerprints pin the drive's author/temporary public
  keys: ``verify_public_keys`` lets the inbox detect a swapped
  ``.pigeon.public.pem`` / ``.pigeon.temp.public.pem`` (which would silently
  redirect future deliveries to the attacker's key).  Replacing the public
  keys now also requires forging the license, i.e. recovering the master key.
* Legacy licenses (32-byte obfuscated key + 64-hex plain hash) are still
  recognised for drives deployed with older builds only when
  ``ACCEPT_LEGACY_LICENSES`` is explicitly enabled; new deployments always
  write the PLF3 format.

Trial
-----
On first inbox launch a trial start date is written to the USB.  The inbox
works for 7 calendar days.  After expiry the device ID is shown and an
activation code—obtained from the author—is required.  Dates are written in
UTC and also parsed as UTC (``calendar.timegm``), keeping start/elapsed on
one time base.
"""

from __future__ import annotations

import calendar
import ctypes
import hashlib
import hmac
import secrets
import struct
import time
from pathlib import Path

TRIAL_NAME = ".pigeon.trial"
LICENSE_NAME = ".pigeon.license"

TRIAL_DAYS = 7
MASTER_KEY_BYTES = 32
CODE_BYTES = 8

# PLF3 license layout
LICENSE_MAGIC = b"PLF3"
SALT_BYTES = 16
_SCRYPT_N = 1 << 14          # 16384 — ~100–300 ms on old hardware, no maxmem trap
_SCRYPT_R = 8
_SCRYPT_P = 1
_SCRYPT_MAXMEM = 64 * 1024 * 1024
_TAG_DOMAIN = b"pigeon-activation-v3"

# Legacy (pre-PLF3) layout offsets
_LEGACY_TAG_OFFSET = MASTER_KEY_BYTES              # 32: plain SHA-256 hex
_LEGACY_CODE_OFFSET = MASTER_KEY_BYTES + 64        # 96: optional cached code
_PLF3_TAG_OFFSET = 4 + SALT_BYTES + MASTER_KEY_BYTES   # 52
_PLF3_AUTHOR_FP_OFFSET = _PLF3_TAG_OFFSET + 64         # 116
_PLF3_TEMP_FP_OFFSET = _PLF3_AUTHOR_FP_OFFSET + 32     # 148
_PLF3_CODE_OFFSET = _PLF3_TEMP_FP_OFFSET + 32          # 180

# Accept legacy (pre-PLF3, unkeyed) licenses.  MUST stay False in production:
# a legacy license is just "any code + its plain SHA-256", so an attacker can
# write one onto a brand-new drive and bypass activation entirely — the legacy
# path cannot be distinguished from a forgery.  Old drives therefore need to
# be re-deployed; flip this to True only if you accept that bypass.
ACCEPT_LEGACY_LICENSES = False


def read_volume_serial(root: Path) -> int:
    """Return the NTFS volume serial number for the volume containing *root*.

    Returns 0 if the call fails for any reason.
    """
    try:
        volume = ctypes.create_unicode_buffer(261)
        filesystem = ctypes.create_unicode_buffer(261)
        serial = ctypes.c_uint32()
        maximum = ctypes.c_uint32()
        flags = ctypes.c_uint32()
        ok = ctypes.windll.kernel32.GetVolumeInformationW(
            str(root),
            volume,
            len(volume),
            ctypes.byref(serial),
            ctypes.byref(maximum),
            ctypes.byref(flags),
            filesystem,
            len(filesystem),
        )
        if ok:
            return serial.value
        return 0
    except OSError:
        return 0


def get_device_id(root: Path) -> str:
    """Return the human-readable device identifier (8 hex chars)."""
    serial = read_volume_serial(root)
    return f"{serial & 0xFFFFFFFF:08X}"


def generate_master_key() -> bytes:
    """Return a fresh 32-byte master key for a new deployment."""
    return secrets.token_bytes(MASTER_KEY_BYTES)


def _obfuscation_mask(serial: int) -> bytes:
    """Legacy mask: bare SHA-256 of the serial (kept for old deployments)."""
    return hashlib.sha256(serial.to_bytes(4, "little")).digest()


def _stretched_mask(serial: int, salt: bytes) -> bytes:
    """PLF3 mask: scrypt-stretched with a per-deployment random salt.

    Recovering the master key from the USB therefore costs a full scrypt
    evaluation and cannot be precomputed across drives.
    """
    return hashlib.scrypt(
        serial.to_bytes(4, "little"),
        salt=salt,
        n=_SCRYPT_N,
        r=_SCRYPT_R,
        p=_SCRYPT_P,
        dklen=32,
        maxmem=_SCRYPT_MAXMEM,
    )


def _xor(a: bytes, b: bytes) -> bytes:
    return bytes(x ^ y for x, y in zip(a, b))


def obfuscate_key(master_key: bytes, serial: int) -> bytes:
    """Obfuscate *master_key* with the volume serial (legacy scheme)."""
    return _xor(master_key, _obfuscation_mask(serial))


def generate_activation_code(device_id: str, master_key: bytes) -> str:
    """Produce a human-friendly activation code for *device_id*.

    Returns a string like ``A3F2-B8D1-5C9E-7F04``.
    """
    raw = hmac.new(master_key, device_id.encode("ascii"), hashlib.sha256).hexdigest()
    code = raw[: CODE_BYTES * 2].upper()
    return f"{code[:4]}-{code[4:8]}-{code[8:12]}-{code[12:16]}"


def _normalise_code(code: str) -> str:
    return code.upper().replace("-", "").strip()


def code_hash(code: str) -> str:
    """SHA-256 hex digest of a normalised activation code.

    Only used by the legacy license format; PLF3 uses ``_activation_tag``.
    """
    return hashlib.sha256(_normalise_code(code).encode("ascii")).hexdigest()


def _activation_tag(master_key: bytes, serial: int, normalised_code: str) -> str:
    message = _TAG_DOMAIN + serial.to_bytes(4, "little") + normalised_code.encode("ascii")
    return hmac.new(master_key, message, hashlib.sha256).hexdigest()


def _format_code(normalised: str) -> str:
    return f"{normalised[:4]}-{normalised[4:8]}-{normalised[8:12]}-{normalised[12:16]}"


def deploy_license(
    root: Path,
    master_key: bytes,
    code: str,
    author_fingerprint: bytes | None = None,
    temporary_fingerprint: bytes | None = None,
) -> None:
    """Write the activation license to the USB (PLF3 format).

    Called once during deployment.  *code* is the plain activation code as
    produced by ``generate_activation_code`` (``XXXX-XXXX-XXXX-XXXX``) —
    NOT its hash.  The stored tag binds the master key, the volume serial
    and the code.  The optional fingerprints (32-byte SHA-256 digests of the
    public keys' SPKI DER, as produced by ``pigeon_store.fingerprint``) pin
    the drive's public keys so the inbox can detect key substitution.
    """
    serial = read_volume_serial(root)
    normalised = _normalise_code(code)
    salt = secrets.token_bytes(SALT_BYTES)
    obfuscated = _xor(master_key, _stretched_mask(serial, salt))
    tag = _activation_tag(master_key, serial, normalised)
    author_fp = author_fingerprint if author_fingerprint is not None else b"\x00" * 32
    temp_fp = temporary_fingerprint if temporary_fingerprint is not None else b"\x00" * 32
    (root / LICENSE_NAME).write_bytes(
        LICENSE_MAGIC + salt + obfuscated + tag.encode("ascii") + author_fp + temp_fp
    )


def verify_public_keys(root: Path, author_fingerprint: bytes, temporary_fingerprint: bytes) -> None:
    """Verify the drive's public keys against the PLF3 license pins.

    Raises ``MailboxError`` when the license pins a non-zero fingerprint that
    does not match the key files currently on the drive — i.e. the public keys
    were substituted after deployment.  No-op when there is no PLF3 license
    (legacy drives / not yet deployed).
    """
    path = root / LICENSE_NAME
    try:
        data = path.read_bytes()
    except OSError:
        return
    if data[: len(LICENSE_MAGIC)] != LICENSE_MAGIC or len(data) < _PLF3_CODE_OFFSET:
        return
    import hashlib as _hashlib

    stored_author = data[_PLF3_AUTHOR_FP_OFFSET:_PLF3_AUTHOR_FP_OFFSET + 32]
    stored_temp = data[_PLF3_TEMP_FP_OFFSET:_PLF3_TEMP_FP_OFFSET + 32]
    if stored_author != b"\x00" * 32 and not hmac.compare_digest(stored_author, author_fingerprint):
        from pigeon_store import MailboxError

        raise MailboxError("U 盘上的作者公钥与激活凭据不一致，公钥可能已被替换。为安全起见已停止投递，请联系作者核实。")
    if stored_temp != b"\x00" * 32 and not hmac.compare_digest(stored_temp, temporary_fingerprint):
        from pigeon_store import MailboxError

        raise MailboxError("U 盘上的临时取件公钥与激活凭据不一致，公钥可能已被替换。为安全起见已停止投递，请联系作者核实。")


def _load_license(path: Path) -> tuple[str, bytes, int] | None:
    """Read the license file.

    Returns ``("plf3", data, serial)`` or ``("legacy", data, serial)``, or
    ``None`` when the file is missing/unreadable.
    """
    try:
        data = path.read_bytes()
    except OSError:
        return None
    serial = read_volume_serial(path.parent)
    if data[: len(LICENSE_MAGIC)] == LICENSE_MAGIC:
        return "plf3", data, serial
    return "legacy", data, serial


def _plf3_master_key(data: bytes, serial: int) -> bytes | None:
    """Un-mask the master key stored in a PLF3 license using *serial*."""
    if len(data) < _PLF3_CODE_OFFSET:
        return None
    salt = data[4 : 4 + SALT_BYTES]
    obfuscated = data[4 + SALT_BYTES : _PLF3_TAG_OFFSET]
    try:
        return _xor(obfuscated, _stretched_mask(serial, salt))
    except (OSError, ValueError):
        return None


def trial_started(root: Path) -> bool:
    """Return ``True`` only when a valid trial start date has been recorded.

    During deployment an empty ``TRIAL_NAME`` file (a pending marker) is
    created; the real start date is written on first launch.
    """
    path = root / TRIAL_NAME
    if not path.exists():
        return False
    try:
        start_str = path.read_text(encoding="ascii").strip()
        time.strptime(start_str, "%Y-%m-%d")
        return True
    except (OSError, ValueError):
        return False


def start_trial(root: Path) -> None:
    if trial_started(root):
        return
    path = root / TRIAL_NAME
    payload = time.strftime("%Y-%m-%d", time.gmtime()).encode("ascii")

    with path.open("w+b") as handle:
        handle.write(payload)
        handle.flush()


def days_remaining(root: Path) -> int:
    if not trial_started(root):
        return TRIAL_DAYS
    try:
        start_str = (root / TRIAL_NAME).read_text(encoding="ascii").strip()
        start = time.strptime(start_str, "%Y-%m-%d")
        # The date is written in UTC (see start_trial); parse it as UTC too.
        # The previous build used time.mktime here, mixing UTC writes with
        # local-zone parses and skewing the trial by the UTC offset.
        start_ts = calendar.timegm(start)
        elapsed = (time.time() - start_ts) / 86400.0
        return TRIAL_DAYS - int(elapsed)
    except (OSError, ValueError):
        return 0


def is_activated(root: Path) -> bool:
    """Return ``True`` if the USB carries a verified permanent activation."""
    loaded = _load_license(root / LICENSE_NAME)
    if loaded is None:
        return False
    kind, data, serial = loaded

    if kind == "plf3":
        master_key = _plf3_master_key(data, serial)
        if master_key is None:
            return False
        try:
            stored_tag = data[_PLF3_TAG_OFFSET : _PLF3_AUTHOR_FP_OFFSET].decode("ascii")
            extra = data[_PLF3_CODE_OFFSET:]
        except UnicodeDecodeError:
            return False
        if not extra:
            return False
        cached_code = extra.decode("ascii", "replace").strip().split("\n")[0]
        if len(cached_code) != 19:
            return False
        expected = _activation_tag(master_key, serial, _normalise_code(cached_code))
        return hmac.compare_digest(expected, stored_tag)

    # Legacy format: unkeyed hash comparison.  Gated behind
    # ACCEPT_LEGACY_LICENSES because it is trivially forgeable.
    if not ACCEPT_LEGACY_LICENSES:
        return False
    if len(data) < MASTER_KEY_BYTES + 64:
        return False
    try:
        stored_hash = data[_LEGACY_TAG_OFFSET:_LEGACY_CODE_OFFSET].decode("ascii")
        extra = data[_LEGACY_CODE_OFFSET:]
        if not extra:
            return False
        cached_code = extra.decode("ascii").strip().split("\n")[0]
        if len(cached_code) == 19:
            return hmac.compare_digest(code_hash(cached_code), stored_hash)
    except (UnicodeDecodeError, ValueError):
        pass
    except OSError:
        pass
    return False


def verify_and_save(root: Path, entered_code: str) -> bool:
    """Check *entered_code* against the license and persist on success."""
    loaded = _load_license(root / LICENSE_NAME)
    if loaded is None:
        return False
    kind, data, serial = loaded
    normalised = _normalise_code(entered_code)

    if kind == "plf3":
        master_key = _plf3_master_key(data, serial)
        if master_key is None:
            return False
        try:
            stored_tag = data[_PLF3_TAG_OFFSET : _PLF3_AUTHOR_FP_OFFSET].decode("ascii")
        except UnicodeDecodeError:
            return False
        expected = _activation_tag(master_key, serial, normalised)
        if not hmac.compare_digest(expected, stored_tag):
            return False
        payload = data[:_PLF3_CODE_OFFSET] + b"\n" + _format_code(normalised).encode("ascii")
        try:
            with (root / LICENSE_NAME).open("r+b") as handle:
                handle.seek(0)
                handle.write(payload)
                handle.truncate()
                handle.flush()
        except OSError:
            return False
        return True

    # Legacy format: unkeyed hash comparison.  Gated behind
    # ACCEPT_LEGACY_LICENSES because it is trivially forgeable.
    if not ACCEPT_LEGACY_LICENSES:
        return False
    if len(data) < MASTER_KEY_BYTES + 64:
        return False
    try:
        stored_hash = data[_LEGACY_TAG_OFFSET:_LEGACY_CODE_OFFSET].decode("ascii")
    except (OSError, UnicodeDecodeError):
        return False

    if not hmac.compare_digest(code_hash(normalised), stored_hash):
        return False

    payload = data[:_LEGACY_CODE_OFFSET] + b"\n" + _format_code(normalised).encode("ascii")
    try:
        with (root / LICENSE_NAME).open("r+b") as handle:
            handle.seek(0)
            handle.write(payload)
            handle.truncate()
            handle.flush()
    except OSError:
        return False
    return True


def get_cached_code(root: Path) -> str | None:
    """Return the previously-verified code, if any."""
    loaded = _load_license(root / LICENSE_NAME)
    if loaded is None:
        return None
    kind, data, _serial = loaded
    offset = _PLF3_CODE_OFFSET if kind == "plf3" else _LEGACY_CODE_OFFSET
    try:
        if len(data) <= offset:
            return None
        code = data[offset:].decode("ascii").strip().split("\n")[0]
        if len(code) == 19:
            return code
        return None
    except (OSError, UnicodeDecodeError):
        return None
