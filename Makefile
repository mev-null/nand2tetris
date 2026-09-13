PYTHON := python3
RUNNER := $(PYTHON) scripts/run_tst.py

.PHONY: help
help:
	@echo "make build          Configure and build with the debug preset (ASan + UBSan)"
	@echo "make test           Run every project's tests, one summary line per project"
	@echo "make test-hw N=01   Run the tests of a single project, with per-test detail"
	@echo "make test-cpp       Run the GoogleTest suite"
	@echo "make fmt            Run clang-format over src/ and tests/"
	@echo "make compdb         Symlink build/debug/compile_commands.json to the repo root"
	@echo "make clean          Remove the build directory and generated .out files"

.PHONY: build
build:
	cmake --preset debug
	cmake --build --preset debug

.PHONY: test-cpp
test-cpp: build
	ctest --preset debug

.PHONY: test
test:
	@$(RUNNER) --summary projects

.PHONY: test-hw
test-hw:
	@test -n "$(N)" || { echo "usage: make test-hw N=01"; exit 2; }
	@$(RUNNER) projects/$(N)

.PHONY: fmt
fmt:
	@find src tests -name '*.cpp' -o -name '*.h' | xargs -r clang-format -i

.PHONY: compdb
compdb:
	ln -sf build/debug/compile_commands.json compile_commands.json

.PHONY: clean
clean:
	rm -rf build compile_commands.json
	find projects -name '*.out' -delete
