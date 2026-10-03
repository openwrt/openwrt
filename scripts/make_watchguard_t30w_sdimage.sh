#!/bin/sh
# SPDX-License-Identifier: GPL-2.0-only
#
# Build a complete WatchGuard T30/T30-W SD card image.
#
# SD card layout:
#   p1: 1MB placeholder (reserved)
#   p2: 128MB squashfs root - kernel mounts as root=/dev/mmcblk0p2
#   p3: 32MB boot - U-Boot loads kernel from mmc 0:3 (REQUIRED)
#   p4: 2GB rootfs_data

set -eu

[ $# -eq 4 ] || { echo "Usage: $0 <host-bin-dir> <bootfs.img> <rootfs.img> <output.img.gz>" >&2; exit 1; }

hostbin="$1"
bootfs_img="$2"
rootfs_img="$3"
output_gz="$4"

# Use mktemp for safe temp directory (avoids racy /tmp/$$)
workdir="$(mktemp -d)"
output_raw="$workdir/t30w-openwrt.img"

cleanup() { rm -rf "$workdir"; }
trap cleanup EXIT INT TERM

mkdir -p "$workdir"

p1_size_mb=1
p2_size_mb=128
p3_size_mb=32
p4_size_mb=2048

# Create p1 placeholder (1024 x 1KiB blocks = 1MB, must match p1_size_mb above)
"$hostbin/mkfs.ext4" -q -F "$workdir/p1.img" 1024

# Create p3 boot partition directly from the bootfs image (already correctly-sized ext2)
cp "$bootfs_img" "$workdir/p3.img"

# Create MBR with 4 primary partitions; ptgen captures the actual byte
# offsets/sizes it chooses after CHS/alignment adjustments.
set -- $("$hostbin/ptgen" -o "$output_raw" -h 16 -s 63 -l 1024 \
	-t 83 -p "${p1_size_mb}M" \
	-t 83 -p "${p2_size_mb}M" \
	-t 83 -p "${p3_size_mb}M" \
	-t 83 -p "${p4_size_mb}M")

p1_offset="$1"
p1_size="$2"
p2_offset="$3"
p2_size="$4"
p3_offset="$5"
p3_size="$6"
p4_offset="$7"
p4_size="$8"

# Ensure the sparse image reflects the full declared partition table size.
truncate -s $((p4_offset + p4_size)) "$output_raw"

# Write p1 placeholder
dd if="$workdir/p1.img" of="$output_raw" bs=512 seek=$((p1_offset / 512)) conv=notrunc status=none

# Write p2 squashfs (check it fits within p2 partition first)
rootfs_size="$(wc -c < "$rootfs_img")"
if [ "$rootfs_size" -gt "$p2_size" ]; then
	echo "ERROR: rootfs image ($rootfs_size bytes) exceeds p2 partition ($p2_size bytes)" >&2
	exit 1
fi
dd if="$rootfs_img" of="$output_raw" bs=512 seek=$((p2_offset / 512)) conv=notrunc status=none

# Write p3 boot partition (check it fits)
bootfs_size="$(wc -c < "$bootfs_img")"
if [ "$bootfs_size" -gt "$p3_size" ]; then
	echo "ERROR: bootfs image ($bootfs_size bytes) exceeds p3 partition ($p3_size bytes)" >&2
	exit 1
fi
dd if="$workdir/p3.img" of="$output_raw" bs=512 seek=$((p3_offset / 512)) \
	conv=notrunc count=$((p3_size / 512)) status=none

# p4 rootfs_data: create at 2GB.
p4_actual_kb=$((p4_size / 1024))
"$hostbin/mkfs.ext4" -q -F -L rootfs_data "$workdir/p4.img" "$p4_actual_kb"
dd if="$workdir/p4.img" of="$output_raw" bs=512 seek=$((p4_offset / 512)) conv=notrunc status=none

gzip -c "$output_raw" > "$output_gz"
sha256sum "$output_gz" > "$output_gz.sha256"
echo "Done: $output_gz"
echo "Layout: p1=1MB(placeholder), p2=128MB(squashfs), p3=32MB(boot), p4=2GB(rootfs_data)"
