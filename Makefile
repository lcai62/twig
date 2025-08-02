CC = gcc
CFLAGS = -Wall -Wextra -Iinclude -Wformat=2 -g
LDFLAGS = -lcrypto -lz
BUILD_DIR = build

# main source files
SOURCES := $(wildcard src/**/*.c src/*.c)
OBJECTS := $(patsubst %.c,$(BUILD_DIR)/%.o,$(SOURCES))
TARGET = twig

# test source files
TEST_SOURCES := $(wildcard tests/test_*.c)
TEST_BINS := $(patsubst tests/%.c,$(BUILD_DIR)/tests/%,$(TEST_SOURCES))

# no main for building test files
CORE_OBJECTS := $(filter-out $(BUILD_DIR)/src/main.o,$(OBJECTS))

.PHONY: all clean test coverage

all: clean $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

# compiled sources go to build dir
$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# compiled tests go to build/tests
$(BUILD_DIR)/tests/%: tests/%.c $(CORE_OBJECTS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $< $(CORE_OBJECTS) -o $@ $(LDFLAGS)

test: $(TEST_BINS)
	@echo "====================="
	@echo "Running tests"
	@echo "====================="
	@for test in $(TEST_BINS); do \
		echo "Executing $$test..."; \
		echo "--------------------------"; \
		$$test || exit 1; \
	done
	@echo "All tests PASSED."

coverage: clean
	@echo "Building with coverage flags"
	$(MAKE) CFLAGS="$(CFLAGS) --coverage" LDFLAGS="$(LDFLAGS) --coverage" all
	$(MAKE) CFLAGS="$(CFLAGS) --coverage" LDFLAGS="$(LDFLAGS) --coverage" test
	lcov --capture --directory $(BUILD_DIR) --output-file coverage.info
	genhtml coverage.info --output-directory coverage_report
	@echo "Starting Python HTTP server on port 8000..."
	@cd coverage_report && \
		( python3 -m http.server 8000 & \
		  echo "Opening Firefox on Windows..." && \
		  sleep 2 && \
		  powershell.exe start firefox.exe http://localhost:8000/index.html )

clean:
	rm -rf $(BUILD_DIR) $(TARGET) coverage.info coverage_report
