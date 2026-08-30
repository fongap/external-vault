import ast

dis_lines = open(r"..\_dis\activation_tool.txt", encoding="utf-8").read().splitlines()
tree = ast.parse(open("activation_tool.py", encoding="utf-8").read())
mine = ast.get_docstring(tree)

for line in dis_lines:
    idx = line.find("LOAD_CONST")
    if idx == -1 or "赋码工具" not in line:
        continue
    lit = line[line.index("(") + 1 : line.rindex(")")]
    if not (lit.startswith("'") and lit.endswith("'")):
        continue
    try:
        doc = ast.literal_eval(lit)
    except Exception as e:
        print("eval fail:", e)
        continue
    if not isinstance(doc, str):
        continue
    for i, (a, b) in enumerate(zip(doc, mine)):
        if a != b:
            print("diff at", i)
            print("dis :", repr(doc[max(0, i - 30): i + 60]))
            print("mine:", repr(mine[max(0, i - 30): i + 60]))
            break
    else:
        print("prefix equal; len dis", len(doc), "mine", len(mine))
    break
