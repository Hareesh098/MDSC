# Convenience Makefile for MDSC (flat src layout)
# Targets: release, debug, relwithdebinfo, cuda, test, run, docs, format, clean, distclean

BUILD_DIR ?= build
CMAKE ?= cmake
CTEST ?= ctest

COMMON_OPTS := -S . -B $(BUILD_DIR)

.PHONY: all
all: release

.PHONY: release
release:
	$(CMAKE) $(COMMON_OPTS) -DCMAKE_BUILD_TYPE=Release
	$(CMAKE) --build $(BUILD_DIR) -j

.PHONY: relwithdebinfo
relwithdebinfo:
	$(CMAKE) $(COMMON_OPTS) -DCMAKE_BUILD_TYPE=RelWithDebInfo
	$(CMAKE) --build $(BUILD_DIR) -j

.PHONY: debug
debug:
	$(CMAKE) $(COMMON_OPTS) -DCMAKE_BUILD_TYPE=Debug -DMDSC_ENABLE_SANITIZERS=ON -DMDSC_ENABLE_DEBUG_SYMBOLS=ON
	$(CMAKE) --build $(BUILD_DIR) -j

.PHONY: cuda
cuda:
	$(CMAKE) $(COMMON_OPTS) -DCMAKE_BUILD_TYPE=Release -DUSE_CUDA=ON
	$(CMAKE) --build $(BUILD_DIR) -j

.PHONY: test
test: release
	$(CTEST) --test-dir $(BUILD_DIR) --output-on-failure

IN ?= examples/lj_berendsen/in.mdsc
.PHONY: run
run: release
	./$(BUILD_DIR)/bin/mdsc $(IN)

.PHONY: docs
docs:
	doxygen docs/Doxyfile || true
	@if [ -d docs/build/latex ]; then $(MAKE) -C docs/build/latex || true; fi
	@echo "Docs built under docs/build/html (and PDF under docs/build/latex if LaTeX available)."

.PHONY: format
format:
	@command -v pre-commit >/dev/null 2>&1 && pre-commit run -a || echo "pre-commit not installed; skipping."

.PHONY: clean
clean:
	@rm -rf $(BUILD_DIR)

.PHONY: distclean
distclean: clean
	@rm -rf docs/build
