#!/usr/bin/env bash
# Copyright (C) 2006 OpenWrt.org

[ $# == 5 ] || {
    echo "SYNTAX: $0 <file> <kernel size> <kernel directory> <rootfs size> <rootfs image>"
    exit 1
}

file="$1"
part1s="$2"
part1d="$3"
part2s="$4"
part2f="$5"

head=16
sect=63
cyl=$(( ($part1s + $part2s) * 1024 * 1024 / ($head * $sect * 512)))

dd if=/dev/zero of="$file" bs=1M count=$(($part1s + $part2s))  2>/dev/null || exit
fdisk -u -C $cyl -H $head -S $sect "$file" > /dev/null 2>/dev/null <<EOF
n
p
1

+$(($part1s * 1024))K
n
p
2


w
q
EOF

block() {
    echo -e 'p\nq' | fdisk -u -C $cyl -H $head -S $sect "$file" | awk -v file="$file$1" -v n="$(($2 + 2))" '
$1 == file {
    print $n * 512
}'
}

start="$(block 1 0)"
end="$(block 1 1)"
blocks="$(( ($end - $start) / 1024 ))"

genext2fs -d "$part1d" -b "$blocks" "$file.kernel"
dd if="$file.kernel" of="$file" bs=512 seek="$(($start / 512))" conv=notrunc
rm -f "$file.kernel"

start="$(block 2 0)"
dd if="$part2f" of="$file" bs=512 seek="$(($start / 512))" conv=notrunc

which chpax >/dev/null && chpax -zp $(which grub)
grub --device-map=/dev/null <<EOF
device (hd0) $file
geometry (hd0) $cyl $head $sect
root (hd0,0)
setup (hd0)
EOF

