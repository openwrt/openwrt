#!/bin/sh
set -eu

[ $# -eq 4 ] || { echo "Usage: $0 <host-bin-dir> <bootfs.img> <rootfs.img> <output.img.gz>"; exit 1; }

hostbin="$1"
bootfs_img="$2"
rootfs_img="$3"
output_gz="$4"
workdir="/tmp/sdimage_work.$$"
output_raw="$workdir/t30w-openwrt.img"

cleanup() { rm -rf "$workdir"; }
trap cleanup EXIT INT TERM

mkdir -p "$workdir"

# Optimal 4-partition layout with expandable rootfs_data:
# p1: 1MB placeholder (reserved for future use)
# p2: 32MB squashfs root - kernel mounts as root=/dev/mmcblk0p2
# p3: 32MB boot - U-Boot loads kernel from mmc 0:3 (REQUIRED)
# p4: 1GB ext4 rootfs_data - LAST PARTITION = EXPANDABLE!

p1_start=2048
p1_sectors=2048  # 1MB placeholder
p2_start=$((p1_start + p1_sectors))
p2_sectors=65536  # 32MB squashfs (plenty of headroom)
p3_start=$((p2_start + p2_sectors))
p3_sectors=65536  # 32MB boot
p4_start=$((p3_start + p3_sectors))
p4_sectors=2097152  # 1GB rootfs_data (expandable!)

total_sectors=$((p4_start + p4_sectors))

# Create small placeholder - 1MB
"$hostbin/mkfs.ext4" -q -F "$workdir/p1.img" 512

# Create rootfs_data in place - skip intermediate file, write directly
# Use truncate to create sparse output file first
truncate -s $((total_sectors * 512)) "$output_raw"

# Create MBR with 4 primary partitions
/usr/sbin/sfdisk --no-reread "$output_raw" << EOF
label: dos
start=${p1_start}, size=${p1_sectors}, type=83
start=${p2_start}, size=${p2_sectors}, type=83
start=${p3_start}, size=${p3_sectors}, type=83
start=${p4_start}, size=${p4_sectors}, type=83
EOF

# Write p1 placeholder
dd if="$workdir/p1.img" of="$output_raw" bs=512 seek=$p1_start conv=notrunc status=none

# Write p2 squashfs
dd if="$rootfs_img" of="$output_raw" bs=512 seek=$p2_start conv=notrunc status=none

# Write p3 boot partition
# Extract boot files from bootfs.img
"$hostbin/debugfs" -R "dump /uImage $workdir/uImage" "$bootfs_img"
"$hostbin/debugfs" -R "dump /uImage_t30 $workdir/uImage_t30" "$bootfs_img"
"$hostbin/debugfs" -R "dump /t30.dtb $workdir/t30.dtb" "$bootfs_img"

# Create p3 boot partition
"$hostbin/mkfs.ext2" -q -F "$workdir/p3.img" 32768
"$hostbin/debugfs" -w -R "write $workdir/uImage /uImage" "$workdir/p3.img"
"$hostbin/debugfs" -w -R "write $workdir/uImage_t30 /uImage_t30" "$workdir/p3.img"
"$hostbin/debugfs" -w -R "write $workdir/t30.dtb /t30.dtb" "$workdir/p3.img"

# Write p3
dd if="$workdir/p3.img" of="$output_raw" bs=512 seek=$p3_start conv=notrunc status=none

# Create p4 rootfs_data directly in the image at the correct offset
# Use mke2fs with -d to write directly to a device/file at offset
# Actually, just use losetup or write via dd with seek
# Simplest: create ext4 image and write it
# But we need to avoid the space issue...

# Alternative: Create smaller rootfs_data initially (100MB) which can be expanded later
p4_size_mb=100
"$hostbin/mkfs.ext4" -q -F -L rootfs_data "$workdir/p4.img" $((p4_size_mb * 1024))
dd if="$workdir/p4.img" of="$output_raw" bs=1M seek=$((p4_start / 2048)) conv=notrunc status=none

gzip -c "$output_raw" > "$output_gz"
sha256sum "$output_gz" > "$output_gz.sha256"
echo "Done: $output_gz"
echo "Layout: p1=1MB(placeholder), p2=32MB(squashfs), p3=32MB(boot), p4=1GB(rootfs_data,expandable)"
echo "NOTE: Kernel must boot with root=/dev/mmcblk0p2"
echo "NOTE: rootfs_data is initially 100MB but can be expanded with: resize2fs /dev/mmcblk0p4"
echo "To expand p4 on larger SD card: fdisk /dev/mmcblk0, delete p4, recreate with larger size, resize2fs /dev/mmcblk0p4"
