#!/usr/bin/env python3
"""Translate a VM program with a VM translator and run the CPU Emulator test on the result.

The translator is run as `translator Prog.vm`, which must write Prog.asm next to the source,
or as `translator Dir`, which must write Dir/Dir.asm from every .vm file in Dir.
"""

import argparse
import shutil
import subprocess
import sys
import tempfile
from pathlib import Path

from run_tst import LOAD_DIRECTIVE, PASS, SKIP, TOOLS_DIR, directives, kill_group, run_one


class TranslationError(Exception):
    pass


def resolve(target: Path) -> tuple[Path, Path]:
    """The translator input and the .tst for a .tst, a .vm file, or a program directory."""
    if target.is_dir():
        return target, target / f"{target.name}.tst"
    if target.suffix == ".vm":
        return target, target.with_suffix(".tst")
    if target.suffix == ".tst":
        loaded = next(
            (m.group(1) for m in map(LOAD_DIRECTIVE.match, directives(target)) if m), ""
        )
        source = target.parent / Path(loaded).with_suffix(".vm").name
        # A program split across several .vm files is translated as a directory.
        return (source if source.exists() else target.parent), target
    raise TranslationError(f"expected a .tst, a .vm file, or a directory: {target}")


def output_of(source: Path) -> Path:
    if source.is_dir():
        return source / f"{source.name}.asm"
    return source.with_suffix(".asm")


def translate(translator: Path, source: Path, timeout: int) -> Path:
    """Run the translator on source and return the .asm it wrote."""
    asm = output_of(source)
    asm.unlink(missing_ok=True)

    proc = subprocess.Popen(
        [str(translator), str(source)],
        cwd=str(source if source.is_dir() else source.parent),
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
        raise TranslationError(f"timed out after {timeout}s")

    if proc.returncode != 0 or not asm.exists():
        output = " ".join((stdout + stderr).split())
        missing = "" if asm.exists() else f", no {asm.name} written"
        raise TranslationError(f"exit {proc.returncode}{missing}: {output[:160]}")
    return asm


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("translator", help="executable run as `translator Prog.vm | Dir`")
    parser.add_argument("target", help="a .tst, a .vm file, or a program directory")
    parser.add_argument("--in-place", action="store_true",
                        help="translate next to the source and keep the .asm (default: a "
                             "temporary copy of the program directory)")
    parser.add_argument("--timeout", type=int, default=120,
                        help="seconds to allow the translator and the test (default: 120)")
    args = parser.parse_args()

    if not TOOLS_DIR.exists():
        print("tools/ is missing - see Setup in README.md", file=sys.stderr)
        return 2

    translator = Path(args.translator).resolve()
    try:
        source, tst = resolve(Path(args.target).resolve())
    except TranslationError as error:
        print(error, file=sys.stderr)
        return 2
    name = tst.stem

    with tempfile.TemporaryDirectory() as tmp:
        if not args.in_place:
            program_dir = source if source.is_dir() else source.parent
            copy = Path(tmp) / program_dir.name
            shutil.copytree(program_dir, copy)
            source = copy if source.is_dir() else copy / source.name
            tst = copy / tst.name

        try:
            asm = translate(translator, source, args.timeout)
        except TranslationError as error:
            print(f"{name}: {translator.name} failed: {error}")
            return 1
        if args.in_place:
            print(f"wrote {asm}")

        if not tst.exists():
            print(f"{name}: no {tst.name} to run")
            return 0
        status, detail = run_one(tst, args.timeout)

    if status == SKIP:
        print(f"{name}: skipped, {detail}")
        return 0
    if status != PASS:
        print(f"{name}: {detail}")
        return 1
    print(f"{name}: passed on the CPU Emulator")
    return 0


if __name__ == "__main__":
    sys.exit(main())
