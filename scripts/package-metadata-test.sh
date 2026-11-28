#!/bin/sh
# SPDX-License-Identifier: GPL-2.0-or-later
#
# Regression test for virtual package dependencies in package-metadata.pl.
#
# Copyright (C) 2026 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

set -eu

SCRIPTDIR=$(dirname "$0")
BASEDIR=$(cd "$SCRIPTDIR/.." && pwd)
TMPDIR=$(mktemp -d "${TMPDIR:-/tmp}/package-metadata.XXXXXX")
trap 'rm -rf "$TMPDIR"' EXIT HUP INT TERM

cat > "$TMPDIR/packageinfo" <<'EOF'
Source-Makefile: package/test/provider/Makefile

Package: foo-default
Depends:
Provides: foo
Default-Variant: 1
Category: Test
Title: Default foo provider
Description: default
@@

Package: foo-self
Depends: +foo
Provides: foo
Category: Test
Title: Self-dependent foo provider
Description: self
@@

Package: foo-other
Depends:
Provides: foo
Category: Test
Title: Other foo provider
Description: other
@@
EOF

"$BASEDIR/scripts/package-metadata.pl" config "$TMPDIR/packageinfo" > "$TMPDIR/config.in"

if grep -Fq 'PACKAGE_foo-self<PACKAGE_foo-self' "$TMPDIR/config.in"; then
	echo "package-metadata.pl emitted a recursive self-dependency" >&2
	exit 1
fi

grep -Fq 'select PACKAGE_foo-default' "$TMPDIR/config.in"
grep -Fq 'select PACKAGE_foo-default if PACKAGE_foo-other<PACKAGE_foo-self' "$TMPDIR/config.in"

echo "package-metadata.pl virtual package dependency test: ok"
