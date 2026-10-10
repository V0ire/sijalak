#!/usr/bin/env python3
"""Owned-path verification runner; no installs, Git, shared builds or state writes."""
import datetime
import hashlib
import json
import os
from pathlib import Path
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[2]
OWN = ROOT / "workstreams/oled"


def inputs():
    paths = []
    for relative in ("firmware/components/jalak_logic", "firmware/components/jalak_hal"):
        paths.extend(p for p in (ROOT / relative).rglob("*") if p.is_file())
    return {str(p.relative_to(ROOT)): hashlib.sha256(p.read_bytes()).hexdigest() for p in sorted(paths)}


def main():
    logs = OWN / "evidence"
    logs.mkdir(exist_ok=True)
    (OWN / "tmp").mkdir(exist_ok=True)
    env = dict(os.environ, TMPDIR=str(OWN / "tmp"), PYTHONDONTWRITEBYTECODE="1")
    commands = [
        ["cmake", "-S", "workstreams/oled", "-B", "workstreams/oled/build", "-DCMAKE_BUILD_TYPE=Release"],
        ["cmake", "--build", "workstreams/oled/build", "-j2"],
        ["ctest", "--test-dir", "workstreams/oled/build", "--output-on-failure", "-V"],
        ["cmake", "-S", "workstreams/oled", "-B", "workstreams/oled/build-sanitize", "-DCMAKE_BUILD_TYPE=Debug", "-DOLED_SANITIZERS=ON"],
        ["cmake", "--build", "workstreams/oled/build-sanitize", "-j2"],
        ["ctest", "--test-dir", "workstreams/oled/build-sanitize", "--output-on-failure", "-V"],
        ["workstreams/oled/build/oled_render", "exports/oled"],
        [sys.executable, "workstreams/oled/generate_previews.py"],
        [sys.executable, "workstreams/oled/test_previews.py"],
    ]
    before = inputs()
    record = {"started_utc": datetime.datetime.now(datetime.timezone.utc).isoformat(),
              "scope": "isolated OLED host workstream", "commands": [], "existing_inputs_sha256": before}
    failed = 0
    for i, command in enumerate(commands, 1):
        run = subprocess.run(command, cwd=ROOT, env=env, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
        relative = f"evidence/{i:02d}.log"
        (OWN / relative).write_text(run.stdout)
        record["commands"].append({"argv": command, "exit_code": run.returncode, "log": relative})
        print(f"[{i}/{len(commands)}] exit {run.returncode}: {' '.join(command)}", flush=True)
        if run.returncode:
            print(run.stdout)
            failed = 1
            break
    record["existing_inputs_unchanged_during_run"] = before == inputs()
    if not record["existing_inputs_unchanged_during_run"]:
        print("Shared interface/source inputs changed during verification; rebuild against the new inputs.")
        failed = 1
    record["exit_code"] = failed
    (logs / "commands.json").write_text(json.dumps(record, indent=2) + "\n")
    return failed


if __name__ == "__main__":
    raise SystemExit(main())
