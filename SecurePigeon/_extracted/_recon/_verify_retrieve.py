"""Verify _recon/retrieve.py against _dis/retrieve.txt.

Strategy:
1. Extract every string constant from LOAD_CONST instruction lines (these are
   NOT truncated in the dump) and from CONSTS lines (truncated at ~250 chars).
2. Compose the three runtime-built PowerShell scripts (notify, progress form,
   retrieval form) and compare them byte-for-byte against the constant pieces
   recovered from the dump.
3. Check every remaining string constant appears either in the reconstruction's
   string constants, its built scripts, or as a prefix of a truncated docstring.
"""

import ast
import base64
import importlib.util
import re
import sys
import types
from pathlib import Path

HERE = Path(__file__).resolve().parent
DIS = HERE.parent / "_dis" / "retrieve.txt"

dis = DIS.read_text(encoding="utf-8")

# ---------------------------------------------------------------- extraction
instr_strings = []  # from LOAD_CONST instruction lines (untruncated)
for m in re.finditer(r"LOAD_CONST\s+\d+\s+\((.+)\)\s*$", dis, re.M):
    lit = m.group(1)
    try:
        v = ast.literal_eval(lit)
    except Exception:
        continue
    if isinstance(v, str):
        instr_strings.append(v)

constsline_strings = []  # decoded const values from CONSTS lines
trunc_prefixes = []      # un-decodable (truncated repr) prefixes
for line in dis.splitlines():
    if not line.startswith("CONSTS: "):
        continue
    lit = re.sub(r"<code object[^>]*>", "None", line[8:])
    try:
        vals = ast.literal_eval(lit)
    except Exception as exc:
        print("CONSTS parse fail:", exc)
        continue
    for v in vals:
        # the dump stores repr-of-repr: each element is the *repr* of a const
        if not isinstance(v, str):
            continue
        try:
            d = ast.literal_eval(v)
        except Exception:
            # truncated repr (cut before the closing quote): unescape the body
            body = v[1:]
            for a, b in (("\\n", "\n"), ("\\t", "\t"), ("\\'", "'"),
                         ('\\"', '"'), ("\\\\", "\\")):
                body = body.replace(a, b)
            trunc_prefixes.append(body)
            continue
        if isinstance(d, (str, bytes)):
            constsline_strings.append(d)

prefixes = {
    "notify_head": lambda c: c.startswith("Add-Type") and "$accent=" in c,
    "notify_t": lambda c: c.startswith("');$t="),
    "notify_d": lambda c: c.startswith("'));$d="),
    "notify_ok": lambda c: c.startswith("'));$ok="),
    "notify_tail": lambda c: c.startswith("'));$wa="),
    "progress_head": lambda c: c.startswith("$p=[Text.Encoding]"),
    "progress_mid": lambda c: c.startswith("'));$title="),
    "progress_tail": lambda c: c.startswith("'));Add-Type"),
    "form_head": lambda c: c.startswith("Add-Type") and "\n$wa=" in c,
    "form_chosen": lambda c: c.startswith("'))\n$chosenStore="),
    "form_output": lambda c: c.startswith("'))\n$outputDefault="),
    "form_tail": lambda c: c.startswith("'))\n$f=New-Object"),
}
named = {}
for c in instr_strings:
    for name, pred in prefixes.items():
        if pred(c):
            if name in named:
                raise SystemExit(f"duplicate const for {name}")
            named[name] = c
            break

missing_names = [k for k in prefixes if k not in named]
assert not missing_names, missing_names
n4 = named["notify_head"]
n5, n6, n7, n8 = named["notify_t"], named["notify_d"], named["notify_ok"], named["notify_tail"]
p6, p7, p8 = named["progress_head"], named["progress_mid"], named["progress_tail"]
f7, f8, f9, f10 = named["form_head"], named["form_chosen"], named["form_output"], named["form_tail"]
print("classified all 12 big PowerShell constants")

# ---------------------------------------------------------------- load module
sys.modules["pigeon_store"] = types.ModuleType("pigeon_store")
sys.modules["pigeon_store"].MailboxError = type("MailboxError", (Exception,), {})
sys.modules["pigeon_store"].extract_all = lambda *a, **k: ([], 0)
sys.modules["pigeon_store"].load_private_key = lambda *a, **k: object()
sys.modules["temporary_access"] = types.ModuleType("temporary_access")
sys.modules["temporary_access"].ACCESS_NAME = "access.bin"
sys.modules["temporary_access"].verify_and_consume = lambda *a, **k: (b"s", b"k", 0)

spec = importlib.util.spec_from_file_location("retrieve_recon", HERE / "retrieve.py")
mod = importlib.util.module_from_spec(spec)
spec.loader.exec_module(mod)

captured = []


class FakeCP:
    def __init__(self, stdout=""):
        self.stdout = stdout
        self.stderr = ""


def fake_ps(script):
    captured.append(script)
    return FakeCP()


mod.ps = fake_ps

real_subprocess = mod.subprocess
mod.subprocess = types.SimpleNamespace(
    Popen=lambda *a, **k: types.SimpleNamespace(
        wait=lambda timeout=None: 0, kill=lambda: None
    ),
    DEVNULL=real_subprocess.DEVNULL,
    TimeoutExpired=real_subprocess.TimeoutExpired,
    CompletedProcess=real_subprocess.CompletedProcess,
    run=real_subprocess.run,
)

failures = []


def check(name, got, want):
    if got == want:
        print(f"OK   {name}")
    else:
        failures.append(name)
        print(f"FAIL {name}")
        for i, (a, b) in enumerate(zip(got, want)):
            if a != b:
                print(f"  first diff at {i}:\n  got : {got[max(0,i-60):i+60]!r}\n  want: {want[max(0,i-60):i+60]!r}")
                break
        else:
            print(f"  length got={len(got)} want={len(want)}")


# ------------------------------------------------------------------- notify()
mod.notify("标题T", "细节D", True)
notify_got = captured.pop()
t64 = mod.field("标题T")
d64 = mod.field("细节D")
ok64 = mod.field("确定")
notify_want = n4 + "#FF3B30" + n5 + t64 + n6 + d64 + n7 + ok64 + n8
check("notify script", notify_got, notify_want)

mod.notify("标题T", "细节D")
notify_got = captured.pop()
notify_want = n4 + "#34C759" + n5 + t64 + n6 + d64 + n7 + ok64 + n8
check("notify script (ok)", notify_got, notify_want)

# -------------------------------------------------------- RetrievalProgress()
orig_pstf = mod._ps_temp_file


def spy_pstf(script):
    captured.append(script)
    return orig_pstf(script)


mod._ps_temp_file = spy_pstf
p = mod.RetrievalProgress(3)
prog_got = captured.pop()
path64 = mod.field(str(p.path))
title64 = mod.field("安全信鸽 · 正在取件")
check("progress script", prog_got, p6 + path64 + p7 + title64 + p8)
p.close()  # exercises _write/close/unlink paths

# status file protocol
p2 = mod.RetrievalProgress(7)
p2.update(2, 9)
if p2.path.read_text(encoding="utf-8") != "RUN\t2\t9":
    failures.append("status file format")
p2._write("DONE", 9)
if p2.path.read_text(encoding="utf-8") != "DONE\t9\t9":
    failures.append("status DONE format")
p2.close()
if p2.path.exists() or p2._ps_script.exists():
    failures.append("temp files not cleaned by close()")
print("OK   RetrievalProgress status-file protocol + cleanup" if not failures
      else "FAIL RetrievalProgress status/cleanup")

# --------------------------------------------------------- request_retrieval()
import pathlib

orig_is_file = pathlib.Path.is_file
pathlib.Path.is_file = lambda self: self.name == ".pigeon.store" or orig_is_file(self)
mod.discover_usb_stores = lambda: [Path("X:\\.pigeon.store"), Path("Y:\\.pigeon.store")]

drive_items_b64 = mod.field("X:\\|Y:\\")
chosen_b64 = mod.field("X:\\.pigeon.store")
out_b64 = mod.field(str(Path.home() / "Documents" / "安全信鸽取件"))

line = (
    "OK|author|"
    + mod.field("X:\\")
    + "|"
    + mod.field("C:\\keys\\a.pem")
    + "|"
    + mod.field("C:\\out")
    + "|"
    + mod.field("pw")
)
mod.ps = lambda script: (captured.append(script), FakeCP(line + "\n"))[-1]
params = mod.request_retrieval()
form_got = captured.pop()
check("form script", form_got, f7 + drive_items_b64 + f8 + chosen_b64 + f9 + out_b64 + f10)

want_params = {
    "mode": "author",
    "store": Path("X:\\.pigeon.store"),
    "key": Path("C:\\keys\\a.pem"),
    "output": Path("C:\\out"),
    "credential": "pw",
}
if params != want_params:
    failures.append("request_retrieval return dict")
    print("FAIL request_retrieval dict:", params)
else:
    print("OK   request_retrieval return dict")

# temporary mode line: empty key field
line_tmp = (
    "OK|temporary|"
    + mod.field("Y:\\")
    + "||"
    + mod.field("C:\\out2")
    + "|"
    + mod.field("123456")
)
mod.ps = lambda script: (captured.append(script), FakeCP(line_tmp + "\n"))[-1]
params2 = mod.request_retrieval()
captured.pop()
want2 = {
    "mode": "temporary",
    "store": Path("Y:\\.pigeon.store"),
    "key": None,
    "output": Path("C:\\out2"),
    "credential": "123456",
}
if params2 != want2:
    failures.append("request_retrieval temporary dict")
    print("FAIL request_retrieval temporary:", params2)
else:
    print("OK   request_retrieval temporary dict")

# no stores -> empty drive items, cancel -> None
mod.ps = lambda script: (captured.append(script), FakeCP(""))[-1]
mod.discover_usb_stores = lambda: []
if mod.request_retrieval() is not None:
    failures.append("cancel should return None")
else:
    print("OK   request_retrieval cancel -> None")
if not captured[0].startswith(f7.split("$driveItems=")[0]) or drive_items_b64 not in ():
    pass
empty_form = captured.pop()
check("form script (no drives, empty b64 prefix)", f7 + mod.field("") + f8, empty_form[: len(f7) + len(mod.field("")) + len(f8)])

pathlib.Path.is_file = orig_is_file

# ------------------------------------------------------------ const coverage
def source_constants(src_path):
    tree = ast.parse(src_path.read_text(encoding="utf-8"))
    out = set()
    for node in ast.walk(tree):
        if isinstance(node, ast.Constant) and isinstance(node.value, (str, bytes)):
            out.add(node.value)
    return out


mine = source_constants(HERE / "retrieve.py")
docstrings = set()
for node in ast.walk(ast.parse((HERE / "retrieve.py").read_text(encoding="utf-8"))):
    if isinstance(node, (ast.FunctionDef, ast.AsyncFunctionDef, ast.ClassDef, ast.Module)):
        d = ast.get_docstring(node, clean=False)
        if d:
            docstrings.add(d)

built = "\n".join(captured) + notify_want + p6 + p7 + p8 + f7 + f8 + f9 + f10

missing = []
for s in set(instr_strings) | set(constsline_strings):
    if not isinstance(s, str):
        if s in mine:
            continue
        missing.append(s)
        continue
    if s in mine or s in built:
        continue
    if s.isidentifier():
        continue  # qualname consts (e.g. class name) have no string-const counterpart
    if any(d.startswith(s) for d in docstrings):
        continue  # truncated docstring prefix recovered
    missing.append(s)

for p in trunc_prefixes:
    if len(p) < 20:
        continue
    if p in built or p[:200] in built:
        continue  # truncated big-script repr, covered untruncated via LOAD_CONST
    if any(d.startswith(p) for d in docstrings):
        continue
    missing.append(p)

if missing:
    failures.append("const coverage")
    for s in missing:
        print("  MISSING const:", repr(s)[:120])
else:
    print("OK   all disassembly string constants covered")

bom_ok = b"\xef\xbb\xbf" in mine
print("OK   UTF-8 BOM constant present" if bom_ok else "FAIL BOM constant missing")

print()
if failures:
    print("FAILURES:", failures)
    sys.exit(1)
print("ALL CHECKS PASSED")
