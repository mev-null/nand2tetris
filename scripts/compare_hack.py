#!/usr/bin/env python3
"""Assemble a .asm file with a Hack assembler and the official one, and compare the .hack output."""

import argparse
import shutil
import subprocess
import sys
import tempfile
from pathlib import Path

from run_tst import TOOLS_DIR, kill_group

OFFICIAL_ASSEMBLER = TOOLS_DIR / "Assembler.sh"


class AssemblyError(Exception):
    pass


def assemble(assembler: Path, asm: Path, workdir: Path, timeout: int) -> list[str]:
    """Run `assembler Prog.asm` on a copy of asm in workdir and return the lines of Prog.hack."""
    workdir.mkdir()
    source = workdir / asm.name
    shutil.copyfile(asm, source)

    proc = subprocess.Popen(
        [str(assembler), str(source)],
        cwd=str(workdir),
        stdin=subprocess.DEVNULL,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True,
        start_new_session=True,
    )
    try:
        stdout, stderr = proc.communicate(timeout=timeout)
    except subprocess.TimeoutExpired:
        kill_group(proc)
        raise AssemblyError(f"timed out after {timeout}s")

    hack = source.with_suffix(".hack")
    if proc.returncode != 0 or not hack.exists():
        output = " ".join((stdout + stderr).split())
        missing = "" if hack.exists() else f", no {hack.name} written"
        raise AssemblyError(f"exit {proc.returncode}{missing}: {output[:160]}")

    lines = [line.strip() for line in hack.read_text().splitlines()]
    while lines and not lines[-1]:
        lines.pop()
    return lines


def instructions(asm: Path) -> list[str]:
    """Source of each ROM word: lines that are not blank, comments, or labels."""
    lines = (raw.split("//")[0].strip() for raw in asm.read_text().splitlines())
    return [line for line in lines if line and not line.startswith("(")]


def compare(asm: Path, expected: list[str], actual: list[str]) -> list[str]:
    """Human-readable differences, empty when the outputs match."""
    source = instructions(asm)
    mismatches = [
        address for address, (want, got) in enumerate(zip(expected, actual)) if want != got
    ]
    problems = []
    if len(expected) != len(actual):
        problems.append(f"expected {len(expected)} words, got {len(actual)}")
    if mismatches:
        problems.append(f"{len(mismatches)} of {len(expected)} words differ")
        for address in mismatches[:5]:
            text = source[address] if address < len(source) else "?"
            problems.append(
                f"  ROM[{address}] {text:<16} expected {expected[address]}  got {actual[address]}"
            )
    return problems


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("assembler", help="executable run as `assembler Prog.asm` -> Prog.hack")
    parser.add_argument("asm", help="the .asm file to assemble")
    parser.add_argument("--timeout", type=int, default=30,
                        help="seconds to allow each assembler (default: 30)")
    args = parser.parse_args()

    if not OFFICIAL_ASSEMBLER.exists():
        print("tools/ is missing - see Setup in README.md", file=sys.stderr)
        return 2

    asm = Path(args.asm).resolve()
    assembler = Path(args.assembler).resolve()

    with tempfile.TemporaryDirectory() as tmp:
        try:
            expected = assemble(OFFICIAL_ASSEMBLER, asm, Path(tmp) / "expected", args.timeout)
        except AssemblyError as error:
            print(f"official assembler failed on {asm.name}: {error}", file=sys.stderr)
            return 2
        try:
            actual = assemble(assembler, asm, Path(tmp) / "actual", args.timeout)
        except AssemblyError as error:
            print(f"{assembler.name} failed on {asm.name}: {error}")
            return 1

    problems = compare(asm, expected, actual)
    if problems:
        print(f"{asm.name}: output differs from the official assembler")
        print("\n".join(problems))
        return 1
    print(f"{asm.name}: {len(expected)} words match")
    return 0


if __name__ == "__main__":
    sys.exit(main())
