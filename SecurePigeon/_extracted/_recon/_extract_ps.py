import ast
import re

BS = chr(92)  # backslash

dis = open(r"..\_dis\activation_tool.txt", encoding="utf-8").read()


def manual_unescape(body: str) -> str:
    out = []
    i = 0
    mapping = {"n": "\n", "t": "\t", "r": "\r", BS: BS, '"': '"', "'": "'"}
    while i < len(body):
        ch = body[i]
        if ch == BS and i + 1 < len(body) and body[i + 1] in mapping:
            out.append(mapping[body[i + 1]])
            i += 2
        else:
            out.append(ch)
            i += 1
    return "".join(out)


scripts = []
for line in dis.splitlines():
    if not line.startswith("CONSTS: "):
        continue
    lit = line[8:]
    lit = re.sub(r"<code object[^>]*>", "None", lit)
    consts = ast.literal_eval(lit)
    for c in consts:
        if isinstance(c, str) and "Add-Type" in c:
            assert c[0] == '"' and c[-1] == '"', (c[:10], c[-10:])
            body = c[1:-1]
            s = manual_unescape(body)
            # s is now the repr of the real string (probably double-quoted or single-quoted)
            try:
                real = ast.literal_eval(s)
            except Exception as e:
                print("outer eval fail:", e, "| delimiters:", s[0], s[-1])
                real = s
            scripts.append(real)

print("extracted", len(scripts), "scripts")

# Compare against my reconstruction
src = open("activation_tool.py", encoding="utf-8").read()
tree = ast.parse(src)
mine = [
    n.value
    for n in ast.walk(tree)
    if isinstance(n, ast.Constant) and isinstance(n.value, str) and "Add-Type" in n.value
]
print("mine has", len(mine), "PS scripts")

for idx, s in enumerate(scripts):
    match = s in mine
    print(f"dis script {idx}: exact match in reconstruction = {match}")
    if not match:
        for m in mine:
            n = min(len(s), len(m))
            for i in range(n):
                if s[i] != m[i]:
                    print("  diff at", i)
                    print("  dis:", repr(s[max(0, i - 40): i + 40]))
                    print("  mine:", repr(m[max(0, i - 40): i + 40]))
                    break
            else:
                print("  prefix equal, len dis:", len(s), "mine:", len(m))

# Also docstrings: extract module docstring and function docstrings the same way
docstrings = []
for line in dis.splitlines():
    if not line.startswith("CONSTS: "):
        continue
    lit = line[8:]
    lit = re.sub(r"<code object[^>]*>", "None", lit)
    consts = ast.literal_eval(lit)
    for c in consts:
        if isinstance(c, str) and c.startswith('"') and c.endswith('"') and len(c) > 60 and "Add-Type" not in c:
            body = c[1:-1]
            s = manual_unescape(body)
            try:
                real = ast.literal_eval(s)
            except Exception as e:
                print("docstring outer eval fail:", e, "| delimiters:", s[0], s[-1])
                real = None
            if real:
                docstrings.append(real)

my_docs = [
    n.value
    for n in ast.walk(tree)
    if isinstance(n, ast.Constant) and isinstance(n.value, str) and len(n.value) > 60 and "Add-Type" not in n.value
]
for idx, d in enumerate(docstrings):
    print(f"dis docstring {idx}: exact match = {d in my_docs}")
    if d not in my_docs:
        print("  dis:", repr(d[:200]))
        print("  tail:", repr(d[-80:]))
