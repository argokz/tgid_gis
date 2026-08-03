"""Run sql/tests/*.sql and report pass/fail. Never executes migration DDL outside tests."""
from __future__ import annotations

import os
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def load_env() -> dict[str, str]:
    env = os.environ.copy()
    env_path = ROOT / ".env"
    if env_path.exists():
        for line in env_path.read_text(encoding="utf-8").splitlines():
            line = line.strip()
            if not line or line.startswith("#") or "=" not in line:
                continue
            key, val = line.split("=", 1)
            env.setdefault(key.strip(), val.strip().strip('"').strip("'"))
    if not env.get("PGPASSWORD") and env.get("TGID_PASSWORD"):
        env["PGPASSWORD"] = env["TGID_PASSWORD"]
    return env


def main() -> int:
    env = load_env()
    host = env.get("TGID_HOST", "localhost")
    port = env.get("TGID_PORT", "5440")
    user = env.get("TGID_USER", "postgres")
    db = "tgid_gis"
    tests = sorted((ROOT / "sql" / "tests").glob("test_*.sql"))
    passed = failed = 0
    failures: list[tuple[str, str]] = []
    for path in tests:
        cmd = [
            "psql",
            "-h", host,
            "-p", str(port),
            "-U", user,
            "-d", db,
            "-v", "ON_ERROR_STOP=1",
            "-f", str(path),
        ]
        proc = subprocess.run(
            cmd, env=env, capture_output=True, text=True, encoding="utf-8",
            errors="replace",
        )
        if proc.returncode == 0:
            passed += 1
            print(f"PASS  {path.name}")
        else:
            failed += 1
            err = (proc.stderr or proc.stdout or "").strip().splitlines()
            tail = err[-3:] if err else ["(no output)"]
            msg = " | ".join(tail)[:240]
            failures.append((path.name, msg))
            print(f"FAIL  {path.name}: {msg}")
    print(f"\n{passed} passed, {failed} failed, {len(tests)} total")
    for name, msg in failures:
        print(f"  - {name}: {msg}")
    return 1 if failed else 0


if __name__ == "__main__":
    sys.exit(main())
