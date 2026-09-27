PYTHON := python3
RUNNER := $(PYTHON) scripts/run_tst.py
ASSEMBLER := build/debug/projects/06-assembler/hack_assembler
VM_TRANSLATOR := build/debug/projects/07-08-vm-translator/vm_translator
CLANG_TIDY ?= $(or $(shell command -v clang-tidy),$(wildcard /opt/homebrew/opt/llvm/bin/clang-tidy),clang-tidy)

.PHONY: help
help:
	@echo "make build          Configure and build with the debug preset (ASan + UBSan)"
	@echo "make test           Run every project's tests, one summary line per project"
	@echo "make test-hw N=01   Run the tests of the project directory for chapter N, with per-test detail"
	@echo "make test-cpp       Build and run the GoogleTest unit tests (P=assembler|vm for one project, F=regex to filter)"
	@echo "make test-asm       Compare hack_assembler with the official Assembler on the chapter 6 programs (F=regex to filter)"
	@echo "make asm FILE=...   Assemble one .asm, print the .hack it writes, and compare it with the official Assembler"
	@echo "make test-vm        Translate the chapter 7 and 8 programs with vm_translator and run their tests (F=regex to filter)"
	@echo "make vm FILE=...    Translate one .vm file or program directory in place and run its test"
	@echo "make fmt            Run clang-format over the C++ sources under projects/"
	@echo "make fmt-check      Fail if any C++ source under projects/ is not clang-formatted"
	@echo "make tidy           Run clang-tidy over the C++ sources under projects/"
	@echo "make lint           Run fmt-check and tidy, as the CI lint job does"
	@echo "make ci             Run the CI jobs locally: lint, then build and the tests except Translate.*"
	@echo "make compdb         Symlink build/debug/compile_commands.json to the repo root"
	@echo "make clean          Remove the build directory and the generated .out, .hack, and translated .asm files"

.PHONY: build
build:
	cmake --preset debug
	cmake --build --preset debug

.PHONY: test-cpp
test-cpp: build
	ctest --preset debug --label-regex '^unit$$' $(if $(P),--label-regex '^$(P)$$') $(if $(F),--tests-regex '$(F)')

.PHONY: test-asm
test-asm: build
	ctest --preset debug --label-regex '^acceptance$$' --label-regex '^assembler$$' $(if $(F),--tests-regex '$(F)')

.PHONY: asm
asm: build
	@test -n "$(FILE)" || { echo "usage: make asm FILE=projects/06-assembler/course/add/Add.asm"; exit 2; }
	$(ASSEMBLER) $(FILE)
	@cat $(basename $(FILE)).hack
	@$(PYTHON) scripts/compare_hack.py $(ASSEMBLER) $(FILE)

.PHONY: test-vm
test-vm: build
	ctest --preset debug --label-regex '^acceptance$$' --label-regex '^vm$$' $(if $(F),--tests-regex '$(F)')

.PHONY: vm
vm: build
	@test -n "$(FILE)" || { echo "usage: make vm FILE=projects/07-08-vm-translator/course/StackArithmetic/SimpleAdd/SimpleAdd.vm"; exit 2; }
	@$(PYTHON) scripts/run_vm_test.py --in-place $(VM_TRANSLATOR) $(FILE)

.PHONY: test
test:
	@$(RUNNER) --summary projects

.PHONY: test-hw
test-hw:
	@test -n "$(N)" || { echo "usage: make test-hw N=01"; exit 2; }
	@dir="$(firstword $(wildcard projects/$(N)-* projects/*-$(N)-*))"; \
	test -n "$$dir" || { echo "no project directory for chapter $(N)"; exit 2; }; \
	$(RUNNER) "$$dir"

.PHONY: fmt
fmt:
	@find projects -name '*.cpp' -o -name '*.hpp' | xargs -r clang-format -i

.PHONY: fmt-check
fmt-check:
	@find projects \( -name '*.cpp' -o -name '*.hpp' \) -print0 | xargs -0 clang-format --dry-run --Werror

.PHONY: tidy
tidy:
	cmake --preset debug
	@find projects -name '*.cpp' -print0 | xargs -0 $(CLANG_TIDY) -p build/debug --quiet

.PHONY: lint
lint: fmt-check tidy

.PHONY: ci
ci: lint build
	ctest --preset debug --exclude-regex '^Translate\.'

.PHONY: compdb
compdb:
	ln -sf build/debug/compile_commands.json compile_commands.json

.PHONY: clean
clean:
	rm -rf build compile_commands.json
	find projects -name '*.out' -delete
	find projects/06-assembler -name '*.hack' -delete
	find projects/07-08-vm-translator -name '*.asm' -delete
