# -*- coding: utf-8 -*-
import json, re
from pathlib import Path

root = Path(r"h:\projects\tgid-app-new\tgid_gis\gid8\gid8")
inv = json.loads(Path(r"h:\projects\tgid-app-new\tgid_gis\docs\schema\legacy_inventory.json").read_text(encoding="utf-8"))
comment_re = re.compile(r"/\*.*?\*/", re.DOTALL)
has_body = {}

for p in root.rglob("*.cpp"):
    text = comment_re.sub("", p.read_text(encoding="utf-8", errors="ignore"))
    for m in re.finditer(r"void\s+GidWidget::(on\w+)\s*\([^;]*?\)\s*(?://[^\n]*)?\s*\{", text):
        name = m.group(1)
        i = m.end() - 1
        depth = 0
        j = i
        while j < len(text):
            if text[j] == "{":
                depth += 1
            elif text[j] == "}":
                depth -= 1
                if depth == 0:
                    body = text[i + 1 : j]
                    body2 = re.sub(r"//.*?$", "", body, flags=re.M)
                    body2 = re.sub(r"\s+", "", body2)
                    if body2:
                        has_body[name] = True
                    else:
                        has_body.setdefault(name, False)
                    break
            j += 1

for a in inv["actions"]:
    if a.get("category") != "other":
        continue
    h = a.get("handler") or ""
    if not h:
        continue  # toggles
    if h not in has_body or not has_body[h]:
        print(f"{a['name']:28} {h:24} {a.get('label','')}")
