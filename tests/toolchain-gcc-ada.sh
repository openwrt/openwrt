#!/bin/sh
# SPDX-License-Identifier: GPL-2.0-only
#
# Verify that the GCC configure invocation disables the Ada compiler probe.
#
# The probe can recursively invoke the compiler when Ada support is absent,
# exhausting the host process table during an otherwise serial toolchain
# build.  Keep this test independent of a downloaded GCC source archive by
# replacing configure with a small executable that checks its environment.

set -eu

TOPDIR=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
TMPDIR=$(mktemp -d)
trap 'rm -rf "$TMPDIR"' EXIT HUP INT TERM

cat > "$TMPDIR/configure" <<'EOF'
#!/bin/sh

test "${acx_cv_cc_gcc_supports_ada-}" = no
EOF
chmod +x "$TMPDIR/configure"

make -f - --no-print-directory \
	TOPDIR="$TOPDIR" \
	GCC_VARIANT=minimal \
	HOST_BUILD_DIR="$TMPDIR" \
	CONFIG_GCC_VERSION=15.3.0 \
	REAL_GNU_TARGET_NAME=x86_64-openwrt-linux \
	GNU_HOST_NAME=x86_64-pc-linux-gnu \
	all <<'EOF'

include $(TOPDIR)/toolchain/gcc/common.mk

all:
	@$(GCC_CONFIGURE)
EOF
