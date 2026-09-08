# -*- coding: utf-8 -*-
"""Tests for fixes #3 (public-key pinning) and #4 (store hardening, locks, drive type)."""
import struct
import sys
import tempfile
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))

from cryptography.hazmat.primitives import serialization
from cryptography.hazmat.primitives.asymmetric import rsa

import activation as A
import pigeon_store as P
from pigeon_store import MailboxError


class FakeSerial:
    value = 0x11223344


A.read_volume_serial = lambda root: FakeSerial.value
T_serial = FakeSerial  # shared

# ---- helpers ------------------------------------------------------------


def make_root(tmp, name):
    root = Path(tmp) / name
    root.mkdir(parents=True, exist_ok=True)
    return root


def make_key():
    return rsa.generate_private_key(public_exponent=65537, key_size=2048)


def fp(public_key):
    return P.fingerprint(public_key)


def init_store(root, author_pub, temp_pub, capacity=8 * 1024 * 1024):
    store = root / ".pigeon.store"
    P.initialize_store(store, author_pub, temp_pub, capacity)
    return store


# ---- fix #3: public key pinning -----------------------------------------


def test_pubkey_pins_written_and_verified(tmp):
    root = make_root(tmp, "pin")
    mk = A.generate_master_key()
    author_pub, temp_pub = make_key().public_key(), make_key().public_key()
    code = A.generate_activation_code("11223344", mk)
    A.deploy_license(root, mk, code, fp(author_pub), fp(temp_pub))
    data = (root / A.LICENSE_NAME).read_bytes()
    assert len(data) == 180, f"PLF3 with pins should be 180 bytes, got {len(data)}"
    assert data[116:148] == fp(author_pub) and data[148:180] == fp(temp_pub)
    # matching keys pass; substitution is detected
    A.verify_public_keys(root, fp(author_pub), fp(temp_pub))
    try:
        A.verify_public_keys(root, fp(make_key().public_key()), fp(temp_pub))
        raise AssertionError("substituted author key must be detected")
    except MailboxError as exc:
        assert "公钥" in str(exc)
    try:
        A.verify_public_keys(root, fp(author_pub), fp(make_key().public_key()))
        raise AssertionError("substituted temporary key must be detected")
    except MailboxError:
        pass
    print("PASS: public-key fingerprints pinned in PLF3 and verified")


def test_pubkey_pins_activation_still_works(tmp):
    root = make_root(tmp, "pin2")
    mk = A.generate_master_key()
    author_pub, temp_pub = make_key().public_key(), make_key().public_key()
    code = A.generate_activation_code("11223344", mk)
    A.deploy_license(root, mk, code, fp(author_pub), fp(temp_pub))
    assert A.verify_and_save(root, code) is True
    assert A.is_activated(root) is True
    assert A.get_cached_code(root) == code
    assert (root / A.LICENSE_NAME).stat().st_size == 180 + 20
    print("PASS: activation roundtrip unaffected by pin slots")


def test_pubkey_pins_noop_for_legacy_or_missing(tmp):
    root = make_root(tmp, "pin3")
    # missing license -> no-op
    A.verify_public_keys(root, b"\x01" * 32, b"\x02" * 32)
    # legacy license -> no-op (old drives)
    (root / A.LICENSE_NAME).write_bytes(b"\x00" * 32 + b"a" * 64)
    A.verify_public_keys(root, b"\x01" * 32, b"\x02" * 32)
    print("PASS: pin verification is no-op for missing/legacy licenses")


# ---- fix #4: store hardening --------------------------------------------


def test_append_extract_roundtrip_with_header_aad(tmp):
    root = make_root(tmp, "store1")
    author_priv, temp_priv = make_key(), make_key()
    store = init_store(root, author_priv.public_key(), temp_priv.public_key())
    src = root / "hello.txt"
    src.write_bytes(b"hello secure pigeon" * 1000)
    total, free, used = P.append_file(store, author_priv.public_key(), temp_priv.public_key(), src)
    files, corrupted = P.extract_all(store, author_priv, root / "out")
    assert corrupted == 0 and len(files) == 1
    assert files[0].read_bytes() == src.read_bytes()
    # temporary key must read via the temporary label too
    files2, corrupted2 = P.extract_all(store, temp_priv, root / "out2", temporary=True)
    assert corrupted2 == 0 and len(files2) == 1
    print("PASS: append/extract roundtrip with header-bound metadata AAD")


def test_legacy_constant_aad_records_still_extract(tmp):
    root = make_root(tmp, "store2")
    author_priv = make_key()
    store = init_store(root, author_priv.public_key(), author_priv.public_key())
    src = root / "legacy.txt"
    src.write_bytes(b"legacy record" * 10)
    # write a record with the OLD constant-AAD metadata (v2 original binary)
    P._BIND_META_AAD = False
    try:
        P.append_file(store, author_priv.public_key(), author_priv.public_key(), src)
    finally:
        P._BIND_META_AAD = True
    files, corrupted = P.extract_all(store, author_priv, root / "out")
    assert corrupted == 0 and len(files) == 1
    assert files[0].read_bytes() == src.read_bytes()
    print("PASS: v2 constant-AAD records still extractable (reader falls back)")


def test_tampered_record_header_detected(tmp):
    root = make_root(tmp, "store3")
    author_priv = make_key()
    store = init_store(root, author_priv.public_key(), author_priv.public_key())
    src = root / "victim.txt"
    src.write_bytes(b"secret payload" * 500)
    P.append_file(store, author_priv.public_key(), author_priv.public_key(), src)
    # flip a nonce bit (offset 32 in the record header): lengths stay
    # self-consistent, so the record still parses — but the metadata AAD no
    # longer matches, i.e. the header is authenticated against the ciphertext.
    data = bytearray(store.read_bytes())
    data[P.HEADER_SIZE + 32] ^= 0x01  # nonce_prefix first byte
    store.write_bytes(bytes(data))
    files, corrupted = P.extract_all(store, author_priv, root / "out")
    assert corrupted == 1 and len(files) == 0, (
        "tampered header must fail metadata authentication and count as corrupted"
    )
    print("PASS: record header tampering detected (AAD-bound) on extract")


def test_absurd_plain_size_rejected_without_allocation(tmp):
    root = make_root(tmp, "store4")
    author_priv = make_key()
    store = init_store(root, author_priv.public_key(), author_priv.public_key())
    src = root / "small.txt"
    src.write_bytes(b"x" * 100)
    P.append_file(store, author_priv.public_key(), author_priv.public_key(), src)
    # forge a header with plain_size = 4 GB but keep total small — the strict
    # total==minimum consistency check must reject it without allocating
    data = bytearray(store.read_bytes())
    plain_size_off = P.HEADER_SIZE + 24  # marker(4)+total(8)+author(2)+temp(2)+meta(4) = 24
    struct.pack_into("<I", data, plain_size_off, 0xFFFFFFFF)
    store.write_bytes(bytes(data))
    import time

    t0 = time.perf_counter()
    files, corrupted = P.extract_all(store, author_priv, root / "out")
    dt = time.perf_counter() - t0
    assert len(files) == 0
    assert dt < 5, f"extract took {dt:.1f}s — length sanity check missing?"
    print(f"PASS: forged plain_size rejected fast ({dt*1000:.0f} ms), no huge allocation")


def test_append_file_lock(tmp):
    root = make_root(tmp, "store5")
    author_priv = make_key()
    store = init_store(root, author_priv.public_key(), author_priv.public_key())
    src = root / "f.txt"
    src.write_bytes(b"lock test")
    # hold a conflicting lock on the last byte like a concurrent inbox would
    import msvcrt

    with open(store, "r+b") as other:
        other.seek(0)
        other.seek(8 * 1024 * 1024 - 1)
        msvcrt.locking(other.fileno(), msvcrt.LK_NBLCK, 1)
        try:
            try:
                P.append_file(store, author_priv.public_key(), author_priv.public_key(), src)
                raise AssertionError("append must fail while another process holds the lock")
            except MailboxError as exc:
                assert "文件仓正被其他程序使用" in str(exc)
        finally:
            other.seek(8 * 1024 * 1024 - 1)
            msvcrt.locking(other.fileno(), msvcrt.LK_UNLCK, 1)
    # after release, append works
    P.append_file(store, author_priv.public_key(), author_priv.public_key(), src)
    print("PASS: append_file serialized by exclusive store lock")


def test_drive_type_filter(tmp):
    import retrieve as R

    # C: (fixed) with a marker file must be filtered out on this machine
    tmpd = make_root(tmp, "notusb")
    (tmpd / ".pigeon.store").write_bytes(b"")
    # _drive_type for a fixed drive: use a real path on C:
    real = Path(tmpd.anchor or "C:\\")
    R._drive_type = lambda p: 3  # simulate DRIVE_FIXED
    assert R._is_removable_drive(tmpd) is False
    stores = R.discover_usb_stores.__wrapped__ if hasattr(R.discover_usb_stores, "__wrapped__") else None
    # monkeypatch enumeration to just our root
    orig_roots = R._logical_roots
    R._logical_roots = lambda: [str(tmpd)]
    try:
        assert R.discover_usb_stores() == [], "fixed drive with marker file must be skipped"
        R._drive_type = lambda p: 2  # removable
        got = R.discover_usb_stores()
        assert len(got) == 1 and got[0].name == ".pigeon.store"
    finally:
        R._logical_roots = orig_roots
    # resolve_store must also refuse non-removable locations
    R._drive_type = lambda p: 3
    try:
        R.resolve_store(str(tmpd))
        raise AssertionError("resolve_store must refuse non-removable location")
    except MailboxError as exc:
        assert "可移动" in str(exc)
    print("PASS: store discovery/resolve restricted to removable drives")


if __name__ == "__main__":
    with tempfile.TemporaryDirectory() as tmp:
        test_pubkey_pins_written_and_verified(tmp)
        test_pubkey_pins_activation_still_works(tmp)
        test_pubkey_pins_noop_for_legacy_or_missing(tmp)
        test_append_extract_roundtrip_with_header_aad(tmp)
        test_legacy_constant_aad_records_still_extract(tmp)
        test_tampered_record_header_detected(tmp)
        test_absurd_plain_size_rejected_without_allocation(tmp)
        test_append_file_lock(tmp)
        test_drive_type_filter(tmp)
    print("\nALL TESTS PASSED")
