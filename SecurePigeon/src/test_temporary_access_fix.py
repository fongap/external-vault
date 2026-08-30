# -*- coding: utf-8 -*-
"""Tests for temporary_access v3 (issue #2 fixes)."""
import struct
import sys
import tempfile
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))

from cryptography.hazmat.primitives.asymmetric import rsa

import temporary_access as T
from pigeon_store import MailboxError


class FakeClock:
    now = 1_700_000_000.0


T.current_counter = lambda now=None: int((FakeClock.now if now is None else now) // T.PERIOD_SECONDS)


def make_key():
    return rsa.generate_private_key(public_exponent=65537, key_size=2048)


def fresh_root(tmp, name):
    root = Path(tmp) / name
    root.mkdir()
    seed = T.provision(root, make_key())
    return root, seed


def code_for(root_seed, counter):
    return T.code_for_counter(root_seed, counter)


def test_header_layout_and_seed_masking(tmp):
    root, seed = fresh_root(tmp, "d0")
    data = (root / T.ACCESS_NAME).read_bytes()
    magic, version, masked_seed, salt, nonce, cipher_len, fc, lu = struct.unpack(
        '<4sI20s16s12sIIQ', data[:T.HEADER_SIZE])
    assert magic == b"TPA2" and version == 3
    assert masked_seed != seed, "seed must NOT be stored in plaintext"
    assert T._unmask_seed(masked_seed, T.read_volume_serial(root), salt) == seed
    print("PASS: seed stored masked; recoverable only via scrypt(serial, salt)")


def test_provision_and_verify(tmp):
    root, seed = fresh_root(tmp, "d1")
    code = code_for(seed, T.current_counter())
    got_seed, private_key, used = T.verify_and_consume(root, code)
    assert got_seed == seed and used == T.current_counter()
    # replay of the same code rejected (counter consumed)
    try:
        T.verify_and_consume(root, code)
        raise AssertionError("replayed code must be rejected")
    except MailboxError:
        pass
    print("PASS: provision/verify roundtrip; replay rejected")


def test_entered_code_must_be_valid(tmp):
    # The original binary's flaw: ANY 6-digit input passed.  v3 must check it.
    root, seed = fresh_root(tmp, "d2")
    current = T.current_counter()
    bad_codes = [c for c in ("000000", "123456", "999999") if c != code_for(seed, current)]
    for bad in bad_codes:
        try:
            T.verify_and_consume(root, bad)
            raise AssertionError(f"meaningless code {bad} must be rejected")
        except MailboxError:
            pass
    print("PASS: arbitrary 6-digit codes rejected (entered code is significant)")


def test_grant_any_time(tmp):
    # v2 bound the credential to the deployment-time slice (~90 s lifetime);
    # v3 must accept a code generated hours later.
    root, seed = fresh_root(tmp, "d3")
    FakeClock.now += 3600 * 24  # one day later
    code = code_for(seed, T.current_counter())
    _, private_key, used = T.verify_and_consume(root, code)
    assert used == T.current_counter()
    print("PASS: authorization works at any later time (no 90 s expiry)")


def test_replay_after_advance(tmp):
    root, seed = fresh_root(tmp, "d4")
    c0 = T.current_counter()
    T.verify_and_consume(root, code_for(seed, c0))
    FakeClock.now += 30 * 5  # 5 slices later; window includes c0-3.. — replay must fail
    try:
        T.verify_and_consume(root, code_for(seed, c0))
        raise AssertionError("old code replayed in window must be rejected")
    except MailboxError:
        pass
    print("PASS: used code rejected even inside later window (monotonic gate)")


def test_throttle_burns_slice(tmp):
    root, seed = fresh_root(tmp, "d5")
    current = T.current_counter()
    ok = code_for(seed, current)
    bad = "000000" if ok != "000000" else "000001"
    for _ in range(5):
        try:
            T.verify_and_consume(root, bad)
        except MailboxError:
            pass
    try:
        T.verify_and_consume(root, ok)
        raise AssertionError("current slice should be burned after 5 failures")
    except MailboxError:
        pass  # correct code for the burned slice must also be rejected now
    print("PASS: online brute force throttled (slice burned at threshold)")


def test_tamper_detection(tmp):
    root, seed = fresh_root(tmp, "d6")
    path = root / T.ACCESS_NAME
    data = bytearray(path.read_bytes())
    data[8] ^= 0xFF  # masked seed bit flip → unmask yields wrong seed → AAD mismatch
    path.write_bytes(bytes(data))
    try:
        T.verify_and_consume(root, code_for(seed, T.current_counter()))
        raise AssertionError("tampered header must fail")
    except MailboxError as exc:
        assert "篡改" in str(exc) or "损坏" in str(exc)
    print("PASS: header tampering detected via AAD")


def test_mutable_fields_stay_writable(tmp):
    root, seed = fresh_root(tmp, "d7")
    T.verify_and_consume(root, code_for(seed, T.current_counter()))
    data = bytearray((root / T.ACCESS_NAME).read_bytes())
    data[T.FAIL_COUNT_OFFSET:T.FAIL_COUNT_OFFSET + 4] = b"\x00" * 4
    data[T.LAST_USED_OFFSET:T.LAST_USED_OFFSET + 8] = b"\x00" * 8
    (root / T.ACCESS_NAME).write_bytes(bytes(data))
    FakeClock.now += 30 * 10
    _, private_key, used = T.verify_and_consume(root, code_for(seed, T.current_counter()))
    assert used == T.current_counter()
    print("PASS: mutable counters writable in place; ciphertext intact (residual replay risk documented)")


def test_v2_rejected(tmp):
    root, seed = fresh_root(tmp, "d8")
    path = root / T.ACCESS_NAME
    data = bytearray(path.read_bytes())
    struct.pack_into("<I", data, 4, 2)
    path.write_bytes(bytes(data))
    try:
        T.verify_and_consume(root, "123456")
        raise AssertionError("v2 record must be rejected")
    except MailboxError as exc:
        assert "重新部署" in str(exc)
    print("PASS: legacy v2 credentials rejected with re-deploy hint")


def test_scrypt_present(tmp):
    import time
    seed = bytes(range(20))
    nonce = bytes(range(12))
    t0 = time.perf_counter()
    T._store_key(seed, nonce)
    dt = time.perf_counter() - t0
    assert dt > 0.03, f"scrypt too fast ({dt*1000:.1f} ms)"
    print(f"PASS: store key derivation scrypt-stretched ({dt*1000:.0f} ms)")


if __name__ == "__main__":
    with tempfile.TemporaryDirectory() as tmp:
        test_scrypt_present(tmp)
        test_header_layout_and_seed_masking(tmp)
        test_provision_and_verify(tmp)
        test_entered_code_must_be_valid(tmp)
        test_grant_any_time(tmp)
        test_replay_after_advance(tmp)
        test_throttle_burns_slice(tmp)
        test_tamper_detection(tmp)
        test_mutable_fields_stay_writable(tmp)
        test_v2_rejected(tmp)
    print("\nALL TESTS PASSED")
