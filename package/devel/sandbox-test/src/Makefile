# Fake "upstream" Makefile that attempts to escape the build sandbox.
# Each target tries a different attack vector.
# With CONFIG_BUILD_SANDBOX=y, all write/network attempts should fail.

DESTDIR ?=

.PHONY: all install clean test-write test-network test-staging test-toolchain

all: test-write test-network test-staging test-toolchain
	@echo "=== sandbox-test: compile finished ==="

# 1. Write outside PKG_BUILD_DIR (to its parent directory)
test-write:
	@echo ">>> Attempting to write to parent of build dir..."
	@if echo "ESCAPED" > $(CURDIR)/../sandbox-escape.txt 2>/dev/null; then \
		echo "FAIL: wrote to $(CURDIR)/../sandbox-escape.txt (sandbox not active?)"; \
		rm -f $(CURDIR)/../sandbox-escape.txt; \
	else \
		echo "PASS: blocked write to parent of build dir"; \
	fi

# 2. Attempt network access
test-network:
	@echo ">>> Attempting network access..."
	@if curl -s --max-time 3 http://example.com >/dev/null 2>&1; then \
		echo "FAIL: network access succeeded (sandbox not active?)"; \
	elif wget -q --timeout=3 -O /dev/null http://example.com 2>/dev/null; then \
		echo "FAIL: network access succeeded via wget"; \
	else \
		echo "PASS: network access blocked"; \
	fi

# 3. Try to tamper with the staging directory
test-staging:
	@echo ">>> Attempting to write to staging dir..."
	@STAGING="$$(echo $(CURDIR) | sed 's|/build_dir/.*|/staging_dir|')"; \
	if [ -d "$$STAGING" ]; then \
		if echo "TAMPERED" > "$$STAGING/sandbox-escape.txt" 2>/dev/null; then \
			echo "FAIL: wrote to $$STAGING (sandbox not active?)"; \
			rm -f "$$STAGING/sandbox-escape.txt"; \
		else \
			echo "PASS: blocked write to staging dir"; \
		fi; \
	else \
		echo "SKIP: staging dir not found at $$STAGING"; \
	fi

# 4. Try to modify the host toolchain
test-toolchain:
	@echo ">>> Attempting to write to toolchain..."
	@TOOLCHAIN="$$(echo $(CURDIR) | sed 's|/build_dir/.*|/staging_dir/host/bin|')"; \
	if [ -d "$$TOOLCHAIN" ]; then \
		if cp /bin/sh "$$TOOLCHAIN/sandbox-backdoor" 2>/dev/null; then \
			echo "FAIL: planted backdoor in $$TOOLCHAIN (sandbox not active?)"; \
			rm -f "$$TOOLCHAIN/sandbox-backdoor"; \
		else \
			echo "PASS: blocked write to toolchain"; \
		fi; \
	else \
		echo "SKIP: toolchain dir not found at $$TOOLCHAIN"; \
	fi

install:
	@echo "=== sandbox-test: install finished ==="

clean:
	@true
