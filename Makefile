CXX ?= g++
PYTHON ?= python
CXXFLAGS ?= -std=c++17 -O2
BUILD_DIR := build/tests

TEST_NAMES := differential_test gcd_reference_test generator_test oracle_test \
              test_real_d1 test_real_d2 test_scale_d2 test_shrink_real_d2
TEST_HEADERS := $(wildcard oracle/*.hpp src/structA/*.hpp src/structB/*.hpp adversary/*.hpp)

ifeq ($(OS),Windows_NT)
EXEEXT := .exe
endif

TEST_BINS := $(addprefix $(BUILD_DIR)/,$(addsuffix $(EXEEXT),$(TEST_NAMES)))

.PHONY: test

test: $(TEST_BINS)
	$(PYTHON) -c "import pathlib,subprocess,sys; [subprocess.run([str(pathlib.Path(p).resolve())], check=True) for p in sys.argv[1:]]; print('ALL TESTS PASSED')" $(TEST_BINS)

$(BUILD_DIR):
	$(PYTHON) -c "from pathlib import Path; Path('$(BUILD_DIR)').mkdir(parents=True, exist_ok=True)"

$(BUILD_DIR)/%$(EXEEXT): tests/%.cpp $(TEST_HEADERS) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -Ioracle $< -o $@

