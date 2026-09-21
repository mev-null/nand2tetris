PYTHON := python3
RUNNER := $(PYTHON) scripts/run_tst.py

.PHONY: help
help:
	@echo "make build          Configure and build with the debug preset (ASan + UBSan)"
	@echo "make test           Run every project's tests, one summary line per project"
	@echo "make test-hw N=01   Run the tests of a single project, with per-test detail"
	@echo "make test-cpp       Build and run the GoogleTest unit tests (F=regex to filter)"
	@echo "make test-asm       Compare hack_assembler with the official Assembler on projects/06"
	@echo "make fmt            Run clang-format over the C++ sources under projects/"
	@echo "make compdb         Symlink build/debug/compile_commands.json to the repo root"
	@echo "make clean          Remove the build directory and generated .out files"

.PHONY: build
build:
	cmake --preset debug
	cmake --build --preset debug

.PHONY: test-cpp
test-cpp: build
	ctest --preset debug --label-exclude acceptance $(if $(F),--tests-regex '$(F)')

.PHONY: test-asm
test-asm: build
	ctest --preset debug --label-regex acceptance

.PHONY: test
test:
	@$(RUNNER) --summary projects

.PHONY: test-hw
test-hw:
	@test -n "$(N)" || { echo "usage: make test-hw N=01"; exit 2; }
	@$(RUNNER) projects/$(N)

.PHONY: fmt
fmt:
	@find projects -name '*.cpp' -o -name '*.hpp' | xargs -r clang-format -i

.PHONY: compdb
compdb:
	ln -sf build/debug/compile_commands.json compile_commands.json

.PHONY: clean
clean:
	rm -rf build compile_commands.json
	find projects -name '*.out' -delete
