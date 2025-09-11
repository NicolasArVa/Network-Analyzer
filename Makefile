	# Compiler settings
	CC = gcc
	CFLAGS = -Wall -Wextra -std=c99 -pedantic -g -O2
	INCLUDES = -Iinclude
	SRCDIR = src
	BUILDDIR = build
	TESTDIR = tests

	# Find all source files
	SOURCES = $(shell find $(SRCDIR) -name '*.c')
	OBJECTS = $(SOURCES:$(SRCDIR)/%.c=$(BUILDDIR)/%.o)
	TARGET = network_analyzer

	# Test files
	TEST_SOURCES = $(wildcard $(TESTDIR)/*.c)
	TEST_TARGETS = $(TEST_SOURCES:$(TESTDIR)/%.c=$(BUILDDIR)/%)

	.PHONY: all clean test debug release

	# Default target
	all: $(TARGET)

	# Main executable
	$(TARGET): $(OBJECTS)
		@mkdir -p $(dir $@)
		$(CC) $(OBJECTS) -o $@ -lm

	# Object files
	$(BUILDDIR)/%.o: $(SRCDIR)/%.c	
		@mkdir -p $(dir $@)
		@echo "Compiling $<..."
		$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

	# Debug build
	debug: CFLAGS += -DDEBUG -g3 -O0
	debug: $(TARGET)

	# Release build
	release: CFLAGS += -DNDEBUG -O3
	release: $(TARGET)

	# Tests
	test: $(TEST_TARGETS)
		@for test in $(TEST_TARGETS); do \
			echo "Running $$test..."; \
			$$test || exit 1; \
		done

	$(BUILDDIR)/test_%: $(TESTDIR)/test_%.c $(filter-out $(BUILDDIR)/main.o, $(OBJECTS))
		@mkdir -p $(dir $@)
		$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@ -lm

	# Clean build artifacts
	clean:
		rm -rf $(BUILDDIR) $(TARGET)

	# Install (optional)
	install: $(TARGET)
		cp $(TARGET) /usr/local/bin/

	# Help
	help:
		@echo "Available targets:"
		@echo "  all      - Build main executable"
		@echo "  debug    - Build with debug symbols"
		@echo "  release  - Build optimized version"
		@echo "  test     - Run all tests"
		@echo "  clean    - Remove build artifacts"
		@echo "  install  - Install to system"