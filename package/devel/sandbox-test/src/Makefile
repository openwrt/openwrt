# Fake "upstream" Makefile that attempts to escape the build sandbox.
# Each target tries a different attack vector.
# With CONFIG_BUILD_SANDBOX=y, all write/network attempts should fail.

ESCAPE_DIR ?= /tmp/sandbox-escape-test
DESTDIR ?=

.PHONY: all install clean test-write test-network test-staging test-toolchain

all: test-write test-network test-staging test-toolchain
	@echo "=== sandbox-test: compile finished ==="

# 1. Write outside PKG_BUILD_DIR to an arbitrary path
test-write:
	@echo ">>> Attempting to write to $(ESCAPE_DIR)..."
	@if mkdir -p $(ESCAPE_DIR) && echo "ESCAPED" > $(ESCAPE_DIR)/pwned.txt 2>/dev/null; then \
		echo "FAIL: wrote to $(ESCAPE_DIR)/pwned.txt (sandbox not active?)"; \
	else \
		echo "PASS: blocked write to $(ESCAPE_DIR)"; \
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
	@echo ">>> Attempting host filesystem write during install..."
	@if echo "ESCAPED-INSTALL" > /tmp/sandbox-escape-install.txt 2>/dev/null; then \
		echo "FAIL: install step wrote to /tmp (sandbox not active?)"; \
	else \
		echo "PASS: install step blocked from writing to /tmp"; \
	fi
	@echo "=== sandbox-test: install finished ==="

clean:
	@rm -rf $(ESCAPE_DIR)
