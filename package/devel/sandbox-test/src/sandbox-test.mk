# SPDX-License-Identifier: GPL-2.0-only
#
# Fake "upstream" Makefile that attempts to escape the build sandbox.
# Each probe tries a different attack vector; with CONFIG_BUILD_SANDBOX=y
# every write/read/network attempt must fail, otherwise the probe exits 1
# and fails the build.
#
# package.mk sandboxes Build/Configure, Build/Compile and Build/Install
# separately, so each of the configure, all and install targets runs the
# full probe set and labels its output with the step name.

DESTDIR ?=
STAGE ?= compile

PROBES := test-write test-network test-staging test-toolchain \
	test-read-home test-read-shadow test-read-ssh test-passwd-leak

.PHONY: all configure install clean check check-begin $(PROBES)
.NOTPARALLEL:

configure: STAGE := configure
all: STAGE := compile
install: STAGE := install
configure all install: check
	@echo "=== sandbox-test: $(STAGE) step finished ==="

check: check-begin $(PROBES)

check-begin:
	@echo "=== sandbox-test: probing $(STAGE) step ==="

# 1. Write outside PKG_BUILD_DIR (to its parent directory)
test-write:
	@echo ">>> [$(STAGE)] Attempting to write to parent of build dir..."
	@if echo "ESCAPED" > $(CURDIR)/../sandbox-escape.txt 2>/dev/null; then \
		echo "FAIL: wrote to $(CURDIR)/../sandbox-escape.txt (sandbox not active?)"; \
		rm -f $(CURDIR)/../sandbox-escape.txt; \
		exit 1; \
	else \
		echo "PASS: blocked write to parent of build dir"; \
	fi

# 2. Attempt network access
test-network:
	@echo ">>> [$(STAGE)] Attempting network access..."
	@if command -v curl >/dev/null 2>&1; then \
		FETCH="curl -s --max-time 3 -o /dev/null"; \
	elif command -v wget >/dev/null 2>&1; then \
		FETCH="wget -q --timeout=3 -O /dev/null"; \
	else \
		echo "SKIP: neither curl nor wget available"; \
		exit 0; \
	fi; \
	if $$FETCH http://example.com >/dev/null 2>&1; then \
		echo "FAIL: network access succeeded (sandbox not active?)"; \
		exit 1; \
	else \
		echo "PASS: network access blocked"; \
	fi

# 3. Try to tamper with the staging directory
test-staging:
	@echo ">>> [$(STAGE)] Attempting to write to staging dir..."
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
	@echo ">>> [$(STAGE)] Attempting to write to toolchain..."
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

# 5. Try to read files in the user's home directory (outside TOPDIR). When
# TOPDIR lives below it, bwrap creates the home dir as a stub holding only
# the mount point for TOPDIR, so ignore that one entry.
test-read-home:
	@echo ">>> [$(STAGE)] Attempting to read user home outside TOPDIR..."
	@HOME_CANDIDATES="/root /home/$$(id -un 2>/dev/null) /Users/$$(id -un 2>/dev/null)"; \
	TOPDIR="$$(echo $(CURDIR) | sed 's|/build_dir/.*||')"; \
	LEAKED=""; \
	for d in $$HOME_CANDIDATES; do \
		case "$$TOPDIR/" in \
		"$$d"/*) KEEP="$${TOPDIR#$$d/}"; KEEP="$${KEEP%%/*}" ;; \
		*) KEEP="" ;; \
		esac; \
		ENTRIES="$$(ls -A "$$d" 2>/dev/null | grep -vxF "$$KEEP")"; \
		if [ -n "$$ENTRIES" ]; then \
			LEAKED="$$d"; \
			break; \
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
	@echo ">>> [$(STAGE)] Attempting to read /etc/shadow..."
	@if [ -r /etc/shadow ] && cat /etc/shadow >/dev/null 2>&1; then \
		echo "FAIL: /etc/shadow readable (sandbox not active?)"; \
		exit 1; \
	else \
		echo "PASS: /etc/shadow not readable"; \
	fi

# 7. Try to read /etc/ssh host keys. An unmatched glob makes cat fail on
# the literal pattern, so an inaccessible /etc/ssh also counts as PASS.
test-read-ssh:
	@echo ">>> [$(STAGE)] Attempting to read /etc/ssh..."
	@if cat /etc/ssh/ssh_host_*_key >/dev/null 2>&1; then \
		echo "FAIL: /etc/ssh host keys readable (sandbox not active?)"; \
		exit 1; \
	else \
		echo "PASS: /etc/ssh host keys not readable"; \
	fi

# 8. Check that /etc/passwd only contains synthesized entries (no host leak).
# Only the Linux sandbox synthesizes /etc/passwd; macOS keeps real users in
# DirectoryService and /etc/passwd only lists system accounts.
test-passwd-leak:
	@echo ">>> [$(STAGE)] Checking /etc/passwd for host user leak..."
	@if [ "$$(uname -s)" = Darwin ]; then \
		echo "SKIP: /etc/passwd is not synthesized on macOS"; \
	elif [ ! -r /etc/passwd ]; then \
		echo "PASS: /etc/passwd not readable"; \
	else \
		COUNT=$$(grep -cv '^\(root\|nobody\|'"$$(id -un)"'\):' /etc/passwd 2>/dev/null || true); \
		if [ "$$COUNT" -gt 0 ]; then \
			echo "FAIL: /etc/passwd contains $$COUNT non-synthesized entries (host leak?)"; \
			grep -v '^\(root\|nobody\|'"$$(id -un)"'\):' /etc/passwd | head -3; \
			exit 1; \
		else \
			echo "PASS: /etc/passwd contains only synthesized entries"; \
		fi; \
	fi

clean:
	@true
