#!/bin/sh
# SPDX-License-Identifier: GPL-2.0

set -e

# This is not necessary, but it makes finding the rootfs easier.
PAD_ROOTFS_OFFSET_TO=4194304

# Constant
HDRLEN=256

die() {
    echo "$1" >&2
    exit 1
}

[ $# -eq 3 ] || die "SYNTAX: $0 <kernel lzma> <rootfs squashfs> <version string>"
kernel=$1
rootfs=$2
version=$3
which zytrx >/dev/null || die "zytrx not found in PATH $PATH"
[ -f "$kernel" ] || die "Kernel file not found: $kernel"
[ -f "$rootfs" ] || die "Rootfs file not found: $rootfs"
[ "$(echo "$version" | wc -c)" -lt 32 ] || die "Version string too long: $version"

kernel_len=$(stat -c '%s' "$kernel")
header_plus_kernel_len=$(($HDRLEN + $kernel_len))
rootfs_len=$(stat -c '%s' "$rootfs")

if [ "$PAD_ROOTFS_OFFSET_TO" -gt "$header_plus_kernel_len" ]; then
    padding_len=$(($PAD_ROOTFS_OFFSET_TO - $header_plus_kernel_len))
else
    padding_len=0
fi

echo "padding_len: $padding_len" >&2

padded_rootfs_len=$(($padding_len + $rootfs_len))

echo "padded_rootfs_len: $padded_rootfs_len" >&2

total_len=$(($header_plus_kernel_len + $padded_rootfs_len))

echo "total_len: $total_len" >&2

padding() {
    head -c $padding_len /dev/zero | tr '\0' '\377'
}

to_hex() {
    hexdump -v -e '1/1 "%02x"'
}

from_hex() {
    perl -pe 's/\s+//g; s/(..)/chr(hex($1))/ge'
}

trx_crc32() {
    tmpfile=$(mktemp)
    outtmpfile=$(mktemp)
    cat "$kernel" > "$tmpfile"
    padding >> "$tmpfile"
    cat "$rootfs" >> "$tmpfile"
    # We just need a CRC-32/JAMCRC of the concatnated files
    # There's no readily available tool for this, but zytrx does create one when
    # creating their TRX header, so we just use that.
    zytrx \
        -B NR7101 \
        -v x \
        -i "$tmpfile" \
        -o "$outtmpfile" >/dev/null
    dd if="$outtmpfile" bs=4 count=1 skip=3 | to_hex
    rm "$tmpfile" "$outtmpfile" >/dev/null
}

tclinux_trx_hdr() {
    # TRX header magic
    printf '2RDH' | to_hex

    # Length of the header
    printf '%08x\n' "$HDRLEN"

    # Length of header + content
    printf '%08x\n' "$total_len"

    # crc32 of the content
    trx_crc32

    # version
    echo "$version" | to_hex
    head -c "$((32 - $(echo "$version" | wc -c)))" /dev/zero | to_hex

    # customer version
    head -c 32 /dev/zero | to_hex

    # kernel length
    printf '%08x\n' "$kernel_len"

    # rootfs length
    printf '%08x\n' "$padded_rootfs_len"

    # romfile length (0)
    printf '00000000\n'

    # "model" (32 bytes of zeros)
    head -c 32 /dev/zero | to_hex

    # Load address (CONFIG_ZBOOT_LOAD_ADDRESS)
    printf '80020000\n'

    # "reserved" 128 bytes of zeros
    head -c 128 /dev/zero | to_hex
}

tclinux_trx_hdr | from_hex
cat "$kernel"
padding
cat "$rootfs"
