PROJECT_SOURCE_DIR := $(abspath ./)
BUILD_DIR ?= $(PROJECT_SOURCE_DIR)/build

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S), Linux)
    NUM_JOB := $(shell nproc)
else ifeq ($(UNAME_S), Darwin)
    NUM_JOB := $(shell sysctl -n hw.ncpu)
else
    NUM_JOB := 1
endif

COMPILER_PATH ?=
COMPILER_FLAGS ?=

clean:
	rm -rf $(BUILD_DIR) && rm -rf third_party/*build
.PHONY: clean

test:
	cmake -S . -B $(BUILD_DIR) -DCIEL_VECTOR_BUILD_TESTS=ON $(if $(COMPILER_PATH),-DCMAKE_CXX_COMPILER=$(COMPILER_PATH)) $(if $(COMPILER_FLAGS),-DCMAKE_CXX_FLAGS="$(COMPILER_FLAGS)")
	cmake --build $(BUILD_DIR) -j $(NUM_JOB)
	cmake --build $(BUILD_DIR) --target run_all_tests -j $(NUM_JOB)
.PHONY: test

benchmark:
	cmake -S . -B $(BUILD_DIR) -DCIEL_VECTOR_BUILD_BENCHMARKS=ON $(if $(COMPILER_PATH),-DCMAKE_CXX_COMPILER=$(COMPILER_PATH)) $(if $(COMPILER_FLAGS),-DCMAKE_CXX_FLAGS="$(COMPILER_FLAGS)")
	cmake --build $(BUILD_DIR) --target ciel_vector_benchmark -j $(NUM_JOB)
	$(BUILD_DIR)/benchmark/ciel_vector_benchmark
.PHONY: benchmark

format:
	./format.sh run $(PROJECT_SOURCE_DIR)/include $(PROJECT_SOURCE_DIR)/test $(PROJECT_SOURCE_DIR)/benchmark
.PHONY: format

check_format:
	./format.sh check $(PROJECT_SOURCE_DIR)/include $(PROJECT_SOURCE_DIR)/test $(PROJECT_SOURCE_DIR)/benchmark
.PHONY: check_format
