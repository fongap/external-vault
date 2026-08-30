import ast
import re

RECON = r"activation_tool.py"
DIS = r"..\_dis\activation_tool.txt"

src = open(RECON, encoding="utf-8").read()
tree = ast.parse(src)
mine = {
    n.value
    for n in ast.walk(tree)
    if isinstance(n, ast.Constant) and isinstance(n.value, str) and "Add-Type" in n.value
}

dis_lines = open(DIS, encoding="utf-8").read().splitlines()

# Full PS scripts appear in LOAD_CONST instruction lines
ok = 0
for line in dis_lines:
    m = re.search(r"LOAD_CONST\s+\d+\s+\((\"(?:[^\"\\]|\\.)*\")\)$", line)
    if not m:
        continue
    val = ast.literal_eval(m.group(1))
    if "Add-Type" in val:
        print("script len", len(val), "exact match:", val in mine)
        if val not in mine:
            for mv in mine:
                n = min(len(val), len(mv))
                for i in range(n):
                    if val[i] != mv[i]:
                        print("  diff at", i)
                        print("  dis :", repr(val[max(0, i - 50): i + 50]))
                        print("  mine:", repr(mv[max(0, i - 50): i + 50]))
                        break
                else:
                    print("  prefix equal; len dis", len(val), "mine", len(mv))
        else:
            ok += 1

print("matched:", ok, "of 3")

# Module docstring full text from LOAD_CONST line
for line in dis_lines:
    m = re.search(r"LOAD_CONST\s+0\s+\('((?:[^'\\]|\\.)*)'\)$", line)
    if m and "赋码工具" in m.group(1):
        doc = ast.literal_eval("'" + m.group(1) + "'")
        mod_doc = ast.get_docstring(tree)
        print("module docstring exact match:", doc == mod_doc)
        if doc != mod_doc:
            print(" dis:", repr(doc[:120]))
            print(" mine:", repr((mod_doc or "")[:120]))
        break
