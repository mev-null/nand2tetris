#!/usr/bin/env python3
"""Run nand2tetris .tst scripts through the Java tool suite and report pass/fail."""

import argparse
import os
import re
import signal
import subprocess
import sys
from collections import OrderedDict
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parent.parent
TOOLS_DIR = REPO_ROOT / "tools"
PROJECTS_DIR = REPO_ROOT / "projects"

SUCCESS_MARKER = "End of script - Comparison ended successfully"
LOAD_DIRECTIVE = re.compile(r"^\s*load\s*([^,;\s]*)\s*[,;]")
COMPARE_DIRECTIVE = re.compile(r"^\s*compare-to\s", re.IGNORECASE)
WHILE_DIRECTIVE = re.compile(r"^\s*while\s", re.IGNORECASE)
BLOCK_COMMENT = re.compile(r"/\*.*?\*/", re.DOTALL)

TOOL_BY_SUFFIX = {
    ".hdl": "HardwareSimulator.sh",
    ".asm": "CPUEmulator.sh",
    ".hack": "CPUEmulator.sh",
    ".vm": "VMEmulator.sh",
}
DEFAULT_TOOL = "VMEmulator.sh"

PASS, FAIL, SKIP = "PASS", "FAIL", "SKIP"


def directives(tst: Path) -> list[str]:
    text = BLOCK_COMMENT.sub("", tst.read_text(errors="replace"))
    return [line for line in (raw.split("//")[0].strip() for raw in text.splitlines()) if line]


def load_suffix(lines: list[str]) -> str:
    """Suffix of the file the script loads, or '' when it loads a directory."""
    for line in lines:
        match = LOAD_DIRECTIVE.match(line)
        if match:
            return Path(match.group(1)).suffix.lower()
    return ""


def has_comparison(lines: list[str]) -> bool:
    return any(COMPARE_DIRECTIVE.match(line) for line in lines)


def waits_for_input(lines: list[str]) -> bool:
    """True when the script loops on a condition only GUI input can satisfy."""
    return any(WHILE_DIRECTIVE.match(line) for line in lines)


def kill_group(proc: "subprocess.Popen[str]") -> None:
    """Kill the process group led by proc and reap it."""
    try:
        os.killpg(proc.pid, signal.SIGKILL)
    except (ProcessLookupError, PermissionError):
        proc.kill()
    proc.communicate()


def run_one(tst: Path, timeout: int) -> tuple[str, str]:
    lines = directives(tst)
    if not has_comparison(lines):
        return SKIP, "interactive script, no compare-to"
    if waits_for_input(lines):
        return SKIP, "interactive script, waits on the GUI keyboard"

    tool = TOOLS_DIR / TOOL_BY_SUFFIX.get(load_suffix(lines), DEFAULT_TOOL)
    if not tool.exists():
        return FAIL, f"missing tool {tool.name}"

    proc = subprocess.Popen(
        [str(tool), str(tst)],
        cwd=str(tst.parent),
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True,
        start_new_session=True,
    )
    try:
        stdout, stderr = proc.communicate(timeout=timeout)
    except subprocess.TimeoutExpired:
        kill_group(proc)
        return FAIL, f"timed out after {timeout}s"

    output = stdout + stderr
    if proc.returncode == 0 and SUCCESS_MARKER in output:
        return PASS, tool.name
    detail = " ".join(output.split()) or f"exit {proc.returncode}"
    return FAIL, detail[:160]


def group_of(tst: Path) -> str:
    """Name of the numbered project directory a test belongs to."""
    try:
        return tst.relative_to(PROJECTS_DIR).parts[0]
    except ValueError:
        return tst.parent.name


def display(tst: Path) -> str:
    try:
        return str(tst.relative_to(REPO_ROOT))
    except ValueError:
        return str(tst)


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("target", nargs="?", default=str(PROJECTS_DIR),
                        help="a .tst file, or a directory to search recursively")
    parser.add_argument("--summary", action="store_true",
                        help="print only the per-project totals")
    parser.add_argument("--timeout", type=int, default=120,
                        help="seconds to allow each test (default: 120)")
    args = parser.parse_args()

    if not TOOLS_DIR.is_dir():
        print("tools/ is missing - see Setup in README.md", file=sys.stderr)
        return 2

    target = Path(args.target).resolve()
    if not target.exists():
        print(f"no such path: {target}", file=sys.stderr)
        return 2

    tests = [target] if target.is_file() else sorted(target.rglob("*.tst"))
    if not tests:
        print(f"no .tst files under {target}", file=sys.stderr)
        print("fetch the course material - see Setup in README.md", file=sys.stderr)
        return 2

    totals: "OrderedDict[str, dict]" = OrderedDict()
    for tst in tests:
        status, detail = run_one(tst, args.timeout)
        counts = totals.setdefault(group_of(tst), {PASS: 0, FAIL: 0, SKIP: 0})
        counts[status] += 1
        if not args.summary:
            suffix = f"  {detail}" if status != PASS else ""
            print(f"{status}  {display(tst)}{suffix}")

    failures = 0
    for group, counts in totals.items():
        failures += counts[FAIL]
        ran = counts[PASS] + counts[FAIL]
        skipped = f" ({counts[SKIP]} skipped)" if counts[SKIP] else ""
        print(f"{group}: {counts[PASS]}/{ran} passed{skipped}")

    return 1 if failures else 0


if __name__ == "__main__":
    sys.exit(main())
