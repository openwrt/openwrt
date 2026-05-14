# Fake "upstream" Makefile that attempts to escape the build sandbox.
# Each target tries a different attack vector.
# With CONFIG_BUILD_SANDBOX=y, all write/network attempts should fail.

DESTDIR ?=

.PHONY: all install clean test-write test-network test-staging test-toolchain test-read-home test-read-shadow test-read-ssh test-passwd-leak

all: test-write test-network test-staging test-toolchain test-read-home test-read-shadow test-read-ssh test-passwd-leak
	@echo "=== sandbox-test: compile finished ==="

# 1. Write outside PKG_BUILD_DIR (to its parent directory)
test-write:
	@echo ">>> Attempting to write to parent of build dir..."
	@if echo "ESCAPED" > $(CURDIR)/../sandbox-escape.txt 2>/dev/null; then \
		echo "FAIL: wrote to $(CURDIR)/../sandbox-escape.txt (sandbox not active?)"; \
		rm -f $(CURDIR)/../sandbox-escape.txt; \
		exit 1; \
	else \
		echo "PASS: blocked write to parent of build dir"; \
	fi

# 2. Attempt network access
test-network:
	@echo ">>> Attempting network access..."
	@if curl -s --max-time 3 http://example.com >/dev/null 2>&1; then \
		echo "FAIL: network access succeeded (sandbox not active?)"; \
		exit 1; \
	elif wget -q --timeout=3 -O /dev/null http://example.com 2>/dev/null; then \
		echo "FAIL: network access succeeded via wget"; \
		exit 1; \
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
			exit 1; \
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
			exit 1; \
		else \
			echo "PASS: blocked write to toolchain"; \
		fi; \
	else \
		echo "SKIP: toolchain dir not found at $$TOOLCHAIN"; \
	fi

# 5. Try to read files in the user's home directory (outside TOPDIR)
test-read-home:
	@echo ">>> Attempting to read user home outside TOPDIR..."
	@HOME_CANDIDATES="/root /home/$$(id -un 2>/dev/null) /Users/$$(id -un 2>/dev/null)"; \
	TOPDIR_PARENT="$$(echo $(CURDIR) | sed 's|/build_dir/.*||')"; \
	LEAKED=""; \
	for d in $$HOME_CANDIDATES; do \
		if [ -d "$$d" ] && [ "$${d##$$TOPDIR_PARENT}" = "$$d" ]; then \
			if ls "$$d" >/dev/null 2>&1; then \
				LEAKED="$$d"; \
				break; \
			fi; \
		fi; \
	done; \
	if [ -n "$$LEAKED" ]; then \
		echo "FAIL: readable home dir $$LEAKED (sandbox not active?)"; \
		exit 1; \
	else \
		echo "PASS: user home outside TOPDIR not readable"; \
	fi

# 6. Try to read /etc/shadow
test-read-shadow:
	@echo ">>> Attempting to read /etc/shadow..."
	@if [ -r /etc/shadow ] && cat /etc/shadow >/dev/null 2>&1; then \
		echo "FAIL: /etc/shadow readable (sandbox not active?)"; \
		exit 1; \
	else \
		echo "PASS: /etc/shadow not readable"; \
	fi

# 8. Check that /etc/passwd only contains synthesized entries (no host leak)
test-passwd-leak:
	@echo ">>> Checking /etc/passwd for host user leak..."
	@if [ ! -r /etc/passwd ]; then \
		echo "PASS: /etc/passwd not readable"; \
	else \
		COUNT=$$(grep -cv '^\(root\|nobody\|'"$$(id -un)"'\):' /etc/passwd 2>/dev/null || echo 0); \
		if [ "$$COUNT" -gt 0 ]; then \
			echo "FAIL: /etc/passwd contains $$COUNT non-synthesized entries (host leak?)"; \
			grep -v '^\(root\|nobody\|'"$$(id -un)"'\):' /etc/passwd | head -3; \
			exit 1; \
		else \
			echo "PASS: /etc/passwd contains only synthesized entries"; \
		fi; \
	fi

# 7. Try to read /etc/ssh host keys
test-read-ssh:
	@echo ">>> Attempting to read /etc/ssh..."
	@if ls /etc/ssh/ssh_host_*_key 2>/dev/null | head -1 | xargs -r cat >/dev/null 2>&1; then \
		echo "FAIL: /etc/ssh host keys readable (sandbox not active?)"; \
		exit 1; \
	else \
		echo "PASS: /etc/ssh host keys not readable"; \
	fi

install:
	@echo "=== sandbox-test: install finished ==="

clean:
	@true
