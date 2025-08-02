CC = gcc
CFLAGS = -Wall -Wextra -Iinclude -Wformat=2 -g
LDFLAGS = -lcrypto -lz

# main source files
SOURCES := $(wildcard src/**/*.c src/*.c)
OBJECTS := $(patsubst %.c, %.o, $(SOURCES))
TARGET = twig


# test files
TEST_SOURCES := $(wildcard tests/test_*.c)
TEST_BINS := $(patsubst tests/%.c, tests/%, $(TEST_SOURCES))

# exclude main for tests
CORE_OBJECTS := $(filter-out src/main.o, $(OBJECTS))

.PHONY: all clean test

# source binary
all: clean $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# test binaries
tests/%: tests/%.c $(CORE_OBJECTS)
	$(CC) $(CFLAGS) $< $(CORE_OBJECTS) -o $@ $(LDFLAGS)

# run all tests
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
	lcov --capture --directory . --output-file coverage.info
	genhtml coverage.info --output-directory coverage_report
	@echo "Starting Python HTTP server on port 8000..."
	@cd coverage_report && \
		( python3 -m http.server 8000 & \
		  echo "Opening Firefox on Windows..." && \
		  sleep 2 && \
		  powershell.exe start firefox.exe http://localhost:8000/index.html )

clean:
	rm -f $(OBJECTS) $(TARGET) $(TEST_BINS)
