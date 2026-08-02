# -*- coding: utf-8 -*-
"""Проверка таблиц сортировки участков в tgid_gis."""
import io
import os
import sys

import psycopg2

sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding="utf-8", errors="replace")
for line in open(".env", encoding="utf-8").read().splitlines():
    line = line.strip()
    if not line or line.startswith("#") or "=" not in line:
        continue
    k, v = line.split("=", 1)
    os.environ.setdefault(k.strip(), v.strip().strip('"').strip("'"))

user = os.environ.get("TGID_USER", "postgres")
pw = os.environ.get("PGPASSWORD") or os.environ.get("TGID_PASSWORD", "")

try:
    db = psycopg2.connect(host="localhost", port=5440, dbname="tgid_gis", user=user, password=pw)
except Exception:
    db = psycopg2.connect(host="localhost", port=5432, dbname="tgid_gis", user=user, password=pw)

c = db.cursor()
c.execute("SELECT table_schema,table_name FROM information_schema.tables "
          "WHERE table_name IN ('uchastok_ms','uchastok_rs','sortnodesforuchastok','sortlinesforuchastok') ORDER BY 1,2")
for r in c.fetchall():
    print(r)
for tn in ("sortnodesforuchastok", "sortlinesforuchastok"):
    c.execute("SELECT column_name FROM information_schema.columns WHERE table_name=%s ORDER BY ordinal_position", (tn,))
    print(tn, [r[0] for r in c.fetchall()])
c.execute("SET search_path TO compat, public, net, ops")
for tn in ("uchastok_ms", "uchastok_rs", "sortnodesforuchastok", "sortlinesforuchastok"):
    c.execute(f"SELECT count(*) FROM {tn}")
    print(tn, c.fetchone()[0])
db.close()
