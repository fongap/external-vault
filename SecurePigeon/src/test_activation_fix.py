# -*- coding: utf-8 -*-
"""Tests for the PLF3 activation fix (issue #1: activation bypass)."""
import hashlib
import sys
import tempfile
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))

import activation as A


class FakeSerial:
    """Monkeypatchable serial source."""

    value = 0x11223344


def fake_read_volume_serial(root: Path) -> int:
    return FakeSerial.value


A.read_volume_serial = fake_read_volume_serial


def make_root(tmp, name):
    root = Path(tmp) / name
    root.mkdir(parents=True, exist_ok=True)
    return root


def deploy(root, master_key, code):
    A.deploy_license(root, master_key, code)


def test_roundtrip(tmp):
    root = make_root(tmp, "roundtrip")
    mk = A.generate_master_key()
    code = A.generate_activation_code("11223344", mk)
    deploy(root, mk, code)
    data = (root / A.LICENSE_NAME).read_bytes()
    assert data[:4] == b"PLF3", "new deployments must write PLF3"
    assert len(data) == 180, f"header+tag+pins should be 180 bytes, got {len(data)}"
    assert not A.is_activated(root), "not activated before verification"
    assert A.verify_and_save(root, code) is True
    assert A.is_activated(root) is True
    assert A.get_cached_code(root) == code
    print("PASS: deploy -> verify -> activated roundtrip")


def test_wrong_code(tmp):
    root = make_root(tmp, "wrong")
    mk = A.generate_master_key()
    code = A.generate_activation_code("11223344", mk)
    deploy(root, mk, code)
    assert A.verify_and_save(root, "AAAA-BBBB-CCCC-DDDD") is False
    assert A.verify_and_save(root, code[:-1] + ("0" if code[-1] != "0" else "1")) is False
    assert A.is_activated(root) is False
    print("PASS: wrong codes rejected")


def test_forged_license(tmp):
    # The old one-minute bypass: write zeroed key + sha256(attacker code) + code
    root = make_root(tmp, "forged")
    mk = A.generate_master_key()
    deploy(root, mk, A.generate_activation_code("11223344", mk))
    attacker_code = "AAAA-BBBB-CCCC-DDDD"
    legacy_forgery = (
        b"\x00" * 32
        + hashlib.sha256(attacker_code.replace("-", "").encode()).hexdigest().encode()
        + b"\n"
        + attacker_code.encode()
    )
    (root / A.LICENSE_NAME).write_bytes(legacy_forgery)
    assert A.is_activated(root) is False, "legacy-style forgery must NOT activate"
    assert A.verify_and_save(root, attacker_code) is False, "forged tag must not verify"
    print("PASS: old trivial forgery rejected on PLF3")


def test_clone_to_other_drive(tmp):
    root1 = make_root(tmp, "drive1")
    root2 = make_root(tmp, "drive2")
    mk = A.generate_master_key()
    code = A.generate_activation_code("11223344", mk)
    deploy(root1, mk, code)
    assert A.verify_and_save(root1, code) is True
    # copy activated license to a "different drive" (different serial)
    FakeSerial.value = 0x55667788
    (root2 / A.LICENSE_NAME).write_bytes((root1 / A.LICENSE_NAME).read_bytes())
    assert A.is_activated(root2) is False, "cloned license must not activate on other serial"
    assert A.verify_and_save(root2, code) is False, "same code must not verify on other serial"
    # and the original drive still works
    FakeSerial.value = 0x11223344
    assert A.is_activated(root1) is True
    print("PASS: license clone to another drive rejected; original still valid")


def test_serial_change_after_activation(tmp):
    root = make_root(tmp, "reformat")
    mk = A.generate_master_key()
    code = A.generate_activation_code("11223344", mk)
    deploy(root, mk, code)
    assert A.verify_and_save(root, code) is True
    # drive got reformatted -> new volume serial
    FakeSerial.value = 0xAABBCCDD
    assert A.is_activated(root) is False, "activated state must not survive reformat"
    FakeSerial.value = 0x11223344
    print("PASS: reformat (serial change) invalidates activation")


def test_master_key_not_trivially_recoverable(tmp):
    root = make_root(tmp, "mask")
    mk = A.generate_master_key()
    code = A.generate_activation_code("11223344", mk)
    deploy(root, mk, code)
    data = (root / A.LICENSE_NAME).read_bytes()
    salt = data[4:20]
    obf = data[20:52]
    serial = 0x11223344
    # the OLD mask (bare sha256 of serial) must NOT reveal the master key any more
    old_mask = hashlib.sha256(serial.to_bytes(4, "little")).digest()
    assert bytes(a ^ b for a, b in zip(obf, old_mask)) != mk
    # the new stretched mask does
    new_mask = A._stretched_mask(serial, salt)
    assert bytes(a ^ b for a, b in zip(obf, new_mask)) == mk
    print("PASS: master key no longer recoverable with bare sha256(serial) mask")


def test_legacy_compatibility(tmp):
    root = make_root(tmp, "legacy")
    mk = A.generate_master_key()
    code = A.generate_activation_code("11223344", mk)
    legacy = mk  # first 32 bytes = "obfuscated key" (content irrelevant to legacy path)
    legacy_data = (
        legacy
        + hashlib.sha256(code.replace("-", "").encode()).hexdigest().encode()
    )
    (root / A.LICENSE_NAME).write_bytes(legacy_data)

    # Default: legacy path is OFF — an old-format file (which is trivially
    # forgeable) must neither activate nor verify.
    assert A.ACCEPT_LEGACY_LICENSES is False
    assert A.is_activated(root) is False
    assert A.verify_and_save(root, code) is False
    assert A.is_activated(root) is False

    # Opt-in (re-deploying old drives is the real answer; this flag exists
    # only if the author consciously accepts the bypass risk):
    A.ACCEPT_LEGACY_LICENSES = True
    try:
        assert A.is_activated(root) is False, "legacy without cached code: not yet activated"
        assert A.verify_and_save(root, code) is True, "legacy drives must keep working when opted in"
        assert A.is_activated(root) is True
        assert A.get_cached_code(root) == code
    finally:
        A.ACCEPT_LEGACY_LICENSES = False
    assert A.is_activated(root) is False, "flag must be restored to the safe default"
    print("PASS: legacy format gated behind explicit opt-in (default off)")


def test_trial_timezone(tmp):
    import time as _t
    root = make_root(tmp, "trial")
    # simulate a start date written 3 days ago in UTC
    old = _t.time() - 3 * 86400
    (root / A.TRIAL_NAME).write_text(_t.strftime("%Y-%m-%d", _t.gmtime(old)), encoding="ascii")
    remaining = A.days_remaining(root)
    assert 3 <= remaining <= 5, f"3 days elapsed -> expect ~4 remaining, got {remaining}"
    # start_trial is idempotent and does not crash on a missing file
    (root / A.TRIAL_NAME).unlink()
    A.start_trial(root)
    assert A.trial_started(root) is True
    assert A.days_remaining(root) in (7, 8)
    print("PASS: trial date parsed on one (UTC) time base; start_trial robust")


def test_tampered_tag(tmp):
    root = make_root(tmp, "tamper")
    mk = A.generate_master_key()
    code = A.generate_activation_code("11223344", mk)
    deploy(root, mk, code)
    assert A.verify_and_save(root, code) is True
    data = bytearray((root / A.LICENSE_NAME).read_bytes())
    data[-1] ^= 0x01  # flip a bit in the tag
    (root / A.LICENSE_NAME).write_bytes(bytes(data))
    assert A.is_activated(root) is False, "tampered tag must deactivate"
    print("PASS: tag tampering detected")


if __name__ == "__main__":
    with tempfile.TemporaryDirectory() as tmp:
        test_roundtrip(tmp)
        test_wrong_code(tmp)
        test_forged_license(tmp)
        test_clone_to_other_drive(tmp)
        test_serial_change_after_activation(tmp)
        test_master_key_not_trivially_recoverable(tmp)
        test_legacy_compatibility(tmp)
        test_trial_timezone(tmp)
        test_tampered_tag(tmp)
    print("\nALL TESTS PASSED")
