#!/bin/sh
# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2026 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.

set -eu

SCRIPT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
MODULES="$SCRIPT_DIR/../package/kernel/linux/modules/usb.mk"

awk '
/^define KernelPackage\/usb-net-rtl8152$/ {
	in_block = 1
	next
}

in_block && /^endef$/ {
	if (!boot_autoload)
		exit 1
	exit 0
}

in_block && /AUTOLOAD:=\$\(call AutoProbe,r8152,1\)/ {
	boot_autoload = 1
}
' "$MODULES"
