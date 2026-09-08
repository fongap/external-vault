import ast
import re

RECON = r"D:\900 AIWork\910 GitHub\FongHub\external-vault\SecurePigeon\_extracted\_recon\activation_tool.py"
DIS = r"D:\900 AIWork\910 GitHub\FongHub\external-vault\SecurePigeon\_extracted\_dis\activation_tool.txt"

src = open(RECON, encoding="utf-8").read()
tree = ast.parse(src)

my_strings = set()
my_bytes = set()
for node in ast.walk(tree):
    if isinstance(node, ast.Constant):
        if isinstance(node.value, str):
            my_strings.add(node.value)
        elif isinstance(node.value, bytes):
            my_bytes.add(node.value)


def deep_eval(x, depth=2):
    for _ in range(depth):
        if isinstance(x, str):
            try:
                x = ast.literal_eval(x)
            except Exception:
                return x
        else:
            return x
    return x


dis = open(DIS, encoding="utf-8").read()

expected = []  # (kind, value)
for line in dis.splitlines():
    if not line.startswith("CONSTS: "):
        continue
    lit = line[len("CONSTS: "):]
    lit = re.sub(r"<code object[^>]*>", "None", lit)
    consts = ast.literal_eval(lit)
    for c in consts:
        v = deep_eval(c)
        if isinstance(v, tuple):
            for item in v:
                expected.append(("str", item))
        elif isinstance(v, str):
            expected.append(("str", v))
        elif isinstance(v, bytes):
            expected.append(("bytes", v))

fails = 0
checked = 0
for kind, v in expected:
    if kind == "str":
        if len(v) > 8 and v not in my_strings:
            print("MISSING str:", repr(v[:90]))
            fails += 1
        elif len(v) > 8:
            checked += 1
    elif kind == "bytes":
        if v not in my_bytes:
            print("MISSING bytes:", v)
            fails += 1
        else:
            checked += 1

print(f"checked={checked} missing={fails}")
