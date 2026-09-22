# nand2tetris

Working through [*The Elements of Computing Systems*](https://www.nand2tetris.org/) —
building a computer from NAND gates up to a compiler and OS. Hardware is written in the
course's HDL, software in C++20. This is my first C++ project, so the software half is
also where I am learning the language.

## Layers

| # | Layer | What is built | Implementation |
| --- | --- | --- | --- |
| 01 | Boolean logic | Not, And, Or, Xor, Mux, DMux, and their 16-bit and multi-way forms | [`projects/01`](projects/01) |
| 02 | Boolean arithmetic | HalfAdder, FullAdder, Add16, Inc16, ALU | [`projects/02`](projects/02) |
| 03 | Memory | Bit, Register, RAM8 to RAM16K, PC | [`projects/03`](projects/03) |
| 04 | Machine language | Mult and Fill in Hack assembly | [`projects/04`](projects/04) |
| 05 | Computer architecture | Memory, CPU, Computer | [`projects/05`](projects/05) |
| 06 | Assembler | Two-pass Hack assembler with a symbol table | [`projects/06/assembler`](projects/06/assembler) |
| 07–08 | VM translator | VM code to Hack assembly | not started |
| 09 | High-level language | A program in Jack | not started |
| 10–11 | Compiler | Jack to VM code | not started |
| 12 | Operating system | The Jack OS libraries | not started |

## Setup

Only authored files are tracked. The official Java tool suite and the course's test
scripts are fetched:

```sh
brew install --cask temurin@21   # the 2014 tool suite breaks on newer JDKs
brew install cmake ninja clang-format

curl -L -o /tmp/n2t.zip \
  https://github.com/itoshkov/nand2tetris-emu/releases/download/original-2.6/nand2tetris.zip
echo "f4ea1e104c9296f98d5b3ebd30acc283a876fedcf289f7679ad80885902bebe4  /tmp/n2t.zip" | shasum -a 256 -c
unzip -q /tmp/n2t.zip -d /tmp/n2t && cp -R /tmp/n2t/nand2tetris/tools . && chmod +x tools/*.sh

git clone --depth 1 https://github.com/nand2tetris/projects.git /tmp/n2t-projects
cp -Rn /tmp/n2t-projects/projects/. projects/   # -n never overwrites authored files
```

`make test` runs every project's tests, and `make help` lists the other targets.

## License

MIT for everything committed here. The tool suite and course material are by Noam Nisan
and Shimon Schocken, are not redistributed, and remain under their own terms; the tool
suite is GPL ([nand2tetris.org](https://www.nand2tetris.org/software)).
