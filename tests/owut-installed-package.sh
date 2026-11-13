#!/bin/sh
# SPDX-License-Identifier: GPL-2.0-only
#
# Ensure the EdgeRouter X image manifest retains uboot-envtools, which is a
# target default and may already be installed when owut selects packages.

set -eu

TOPDIR=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
device_file=$TOPDIR/target/linux/ramips/image/mt7621.mk
target_file=$TOPDIR/target/linux/ramips/mt7621/target.mk

device=$(sed -n '/^define Device\/ubnt_edgerouter_common$/,/^endef$/p' "$device_file")
defaults=$(sed -n 's/^DEFAULT_PACKAGES += //p' "$target_file")

# The device must not subtract a package supplied by the target defaults.
test -n "$device"
if printf '%s\n' "$device" | grep -q -- '-uboot-envtools'; then
	exit 1
fi
printf '%s\n' "$defaults" | grep -qw uboot-envtools

# Model owut's installed-package selection against the generated manifest.
manifest="$defaults $(printf '%s\n' "$device" | sed -n 's/.*DEVICE_PACKAGES[^=]*=[[:space:]]*//p')"
printf '%s\n' "$manifest" | grep -qw uboot-envtools
