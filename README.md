# nand2tetris

Working through [*The Elements of Computing Systems*](https://www.nand2tetris.org/) —
building a computer from NAND gates up to a compiler and OS.

Projects 1–5 are hardware, written in the course's HDL. Projects 6–12 are software:
an assembler, a VM translator, and a Jack compiler, implemented here in **C++20**.

Tests run headlessly from the command line against the official Java tool suite, so the
whole course is one `make test` away.

## What is in this repository

Only work written for this course is committed. The supplied course material — test
scripts, comparison files, sample programs — and the Java tool suite are fetched by the
setup steps below and are not tracked.

```
projects/             Authored HDL chips, Hack assembly, and the Jack OS
projects/06/assembler Hack assembler: src/, include/, and its GoogleTest suite in tests/
scripts/              run_tst.py - dispatches .tst scripts to the right simulator
                      compare_hack.py - diffs an assembler's output against the official one
tools/                Official Java tool suite (fetched)
```

The software projects are built as one CMake project. The top-level `CMakeLists.txt` holds
only what every project shares — the C++ standard, GoogleTest, and CTest — and adds each
project directory. A project's `CMakeLists.txt` wires its program to the course material
(for project 6, the `.asm` acceptance tests), and the program's own `CMakeLists.txt` defines
only its targets and unit tests.

## Setup

### 1. Toolchain

```sh
brew install --cask temurin@21
brew install cmake ninja clang-format
```

The tool suite's jars were built in 2014. JDK 24 removed `SecurityManager` and newer JDKs
are dropping `java.applet`, so pin to the 21 LTS rather than `brew install --cask temurin`,
which installs a much newer release.

`clang-tidy` is optional and ships with the keg-only `llvm` formula:

```sh
brew install llvm
```

### 2. Java tool suite

```sh
curl -L -o /tmp/nand2tetris.zip \
  https://github.com/itoshkov/nand2tetris-emu/releases/download/original-2.6/nand2tetris.zip
shasum -a 256 /tmp/nand2tetris.zip
# f4ea1e104c9296f98d5b3ebd30acc283a876fedcf289f7679ad80885902bebe4

unzip -q /tmp/nand2tetris.zip -d /tmp/n2t
cp -R /tmp/n2t/nand2tetris/tools .
chmod +x tools/*.sh
```

This is the unmodified official 2.6 suite (728,258 bytes, matching the size published on
nand2tetris.org). The official download is served from Google Drive and cannot be scripted,
so the archive is taken from a mirror that stores it verbatim under the `original-2.6` tag.

### 3. Course material

Required before any test can run — the `.tst` and `.cmp` scripts live here.

```sh
git clone --depth 1 https://github.com/nand2tetris/projects.git /tmp/n2t-projects
cp -Rn /tmp/n2t-projects/projects/. projects/
```

`cp -n` never overwrites, so re-running this is safe: committed work stays untouched and
only missing course files are added.

### 4. Verify

```sh
java -version
./tools/HardwareSimulator.sh --help
make test-hw N=01
```

## Usage

| Command | Description |
| --- | --- |
| `make build` | Configure and build with the debug preset (ASan + UBSan) |
| `make test` | Run every project's tests, one summary line per project |
| `make test-hw N=01` | Run the tests of a single project, with per-test detail |
| `make test-cpp` | Build and run the GoogleTest unit tests; `F=regex` runs only matching tests |
| `make test-asm` | Assemble every `.asm` in `projects/06` with `hack_assembler` and diff it against the official Assembler |
| `make fmt` | Run `clang-format` over the C++ sources under `projects/` |
| `make compdb` | Symlink `compile_commands.json` to the repo root |
| `make clean` | Remove the build directory and generated `.out` files |

`scripts/run_tst.py` picks a simulator from the `load` directive at the top of each `.tst`
file: `.hdl` goes to the hardware simulator, `.asm` and `.hack` to the CPU emulator, and
`.vm` or a bare `load` to the VM emulator. A test counts as passing only when the tool exits
zero *and* prints `End of script - Comparison ended successfully`. Scripts with no
`compare-to` directive are interactive and are reported as skipped.

`make test-asm` runs `hack_assembler Prog.asm`, expects it to write `Prog.hack` next to the
source, and compares the result word by word with what `tools/Assembler.sh` produces. The
first mismatching ROM addresses are reported with the instruction that produced them. Both
`make test-cpp` and `make test-asm` go through CTest, so the same tests also appear in the
VS Code Testing view when the folder is opened with CMake Tools.

For stepping through a chip or watching memory, the browser
[Web IDE](https://nand2tetris.github.io/web-ide/) runs the same simulators with no install.

## Attribution

The MIT license covers everything committed here.

No course material is redistributed by this repository. The files fetched in steps 2 and 3
are the work of Noam Nisan and Shimon Schocken and remain under their own terms: the tool
suite is distributed under the GPL from [nand2tetris.org](https://www.nand2tetris.org/software),
and the project files come from [nand2tetris/projects](https://github.com/nand2tetris/projects).
