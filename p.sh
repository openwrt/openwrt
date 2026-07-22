#!/bin/sh
# Hybrid: OpenWrt airoha/an7581 kernel + modules (+ EN8811 FW) into alpine rootfs.
# Device: nokia_xg-040g-md
#
# Naming matches the 6.12 workflow:
#   kernel.6.18.itb
#   rootfs-6.18/lib/modules/<full-kver>/   e.g. 6.18.38
#   rootfs-6.18/lib/firmware/airoha/
set -e

TARGET_ROOT="${TARGET_ROOT:-$HOME/wrt/alpine}"
STAGING_ROOT="staging_dir/target-aarch64_cortex-a53_musl/root-airoha"
BUILD_LINUX="build_dir/target-aarch64_cortex-a53_musl/linux-airoha_an7581"

KERNEL_ITB=$(ls -1 "$BUILD_LINUX"/nokia_xg-040g-md-uImage.itb 2>/dev/null | head -1)
if [ -z "$KERNEL_ITB" ]; then
	KERNEL_ITB=$(ls -1 "$BUILD_LINUX"/nokia_xg-040g-md*-uImage.itb 2>/dev/null | head -1)
fi
if [ -z "$KERNEL_ITB" ]; then
	echo "error: no nokia_xg-040g-md FIT image under $BUILD_LINUX" >&2
	echo "build with: make -j\$(nproc) target/linux/install" >&2
	exit 1
fi

MODULE_DIR=$(ls -d "$STAGING_ROOT"/lib/modules/* 2>/dev/null | head -1)
if [ -z "$MODULE_DIR" ]; then
	echo "error: no modules under $STAGING_ROOT/lib/modules" >&2
	exit 1
fi

# Full module/kernel version, e.g. 6.18.38 (must match running kernel)
KVER=$(basename "$MODULE_DIR")
# Short major.minor for alpine layout, e.g. 6.18 (same style as 6.12)
KSERIES=$(echo "$KVER" | cut -d. -f1-2)

ALPINE_ROOTFS="$TARGET_ROOT/rootfs-$KSERIES"
KERNEL_DST="$TARGET_ROOT/kernel.$KSERIES.itb"

echo "kernel:  $KERNEL_ITB"
echo "         -> $KERNEL_DST"
echo "modules: $MODULE_DIR"
echo "         -> $ALPINE_ROOTFS/lib/modules/$KVER"
echo "fw:      $STAGING_ROOT/lib/firmware/airoha (if present)"
echo "         -> $ALPINE_ROOTFS/lib/firmware/airoha"

cp -f "$KERNEL_ITB" "$KERNEL_DST"

# Alpine trees are often root-owned (same as old 6.12 p.sh).
if [ "$(id -u)" -eq 0 ]; then
	SUDO=
else
	SUDO=sudo
fi

$SUDO mkdir -p "$ALPINE_ROOTFS/lib/modules"
# Clear only this series' module tree content (keep rest of alpine rootfs)
$SUDO rm -rf "$ALPINE_ROOTFS/lib/modules/"*
$SUDO cp -a "$MODULE_DIR" "$ALPINE_ROOTFS/lib/modules/"

if [ -d "$STAGING_ROOT/lib/firmware/airoha" ]; then
	$SUDO mkdir -p "$ALPINE_ROOTFS/lib/firmware"
	$SUDO rm -rf "$ALPINE_ROOTFS/lib/firmware/airoha"
	$SUDO cp -a "$STAGING_ROOT/lib/firmware/airoha" "$ALPINE_ROOTFS/lib/firmware/"
fi

echo "done. series=$KSERIES full=$KVER"
