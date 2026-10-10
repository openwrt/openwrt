#!/bin/sh
# SPDX-License-Identifier: GPL-2.0

set -e

# Maximum kernel size
# NOTE: We ff-pad the kernel and specify the fully padded length as the kernel
#       size in the TRX header. This works around a bug wherein the bootloader
#       reads the kernel length from the kernel in slot A, when it is booting
#       slot B. By including the padding in the length, the kernel length is
#       always the same, and the LZMA decompressor stops when it's done anyway.
#       See: https://econet-linux.pkt.wiki/en/bootloader#a-length-bug
PAD_ROOTFS_OFFSET_TO=$((4 * 1024 * 1024))

# Constant
HDRLEN=256

die() {
    echo "$1" >&2
    exit 1
}

usage() {
    cat >&2 <<EOF
SYNTAX: $0 --kernel <file> --rootfs <file> --version <string> [options]
        $0 --fixup <file>

Options:
  --kernel   Path to kernel lzma file (required)
  --rootfs   Path to rootfs squashfs file
  --version  Version string, max 31 chars (required)
  --endian   Endianness: 'be' for big endian, 'le' for little endian (default: be)
  --model    Model/platform name, max 31 chars (default: empty)
  --loadaddr Address the bootloader decompresses the kernel to
             (default: 0x80020000)
  --fixup    Refresh an existing TRX header after appending UBI
EOF
    exit 1
}

# Defaults
kernel=""
rootfs=""
version=""
endian="be"
model=""
loadaddr="0x80020000"
fixup=""

# Parse named arguments
while [ $# -gt 0 ]; do
    case "$1" in
        --kernel)
            kernel="$2"
            shift 2
            ;;
        --rootfs)
            rootfs="$2"
            shift 2
            ;;
        --version)
            version="$2"
            shift 2
            ;;
        --endian)
            endian="$2"
            shift 2
            ;;
        --model)
            model="$2"
            shift 2
            ;;
        --loadaddr)
            loadaddr="$2"
            shift 2
            ;;
        --fixup)
            fixup="$2"
            shift 2
            ;;
        -h|--help)
            usage
            ;;
        *)
            die "Unknown option: $1"
            ;;
    esac
done

which zytrx >/dev/null || die "zytrx not found in PATH $PATH"

if [ -z "$fixup" ]; then
    # Validate required arguments
    [ -n "$kernel" ] || die "Missing required argument: --kernel"
    [ -n "$version" ] || die "Missing required argument: --version"

    # Validate endianness
    case "$endian" in
        be|BE) endian="be" ;;
        le|LE) endian="le" ;;
        *) die "Invalid endianness: $endian (must be 'be' or 'le')" ;;
    esac

    [ -f "$kernel" ] || die "Kernel file not found: $kernel"
    [ -z "$rootfs" ] || [ -f "$rootfs" ] || die "Rootfs file not found: $rootfs"
    [ "$(echo "$version" | wc -c)" -lt 32 ] || die "Version string too long: $version"
    [ -z "$model" ] || [ "$(printf '%s' "$model" | wc -c)" -lt 32 ] || \
        die "Model string too long: $model"

    kernel_len=$(stat -c '%s' "$kernel")
    header_plus_kernel_len=$(($HDRLEN + $kernel_len))
    rootfs_len=0
    if [ -f "$rootfs" ]; then
        rootfs_len=$(stat -c '%s' "$rootfs")
    fi

    [ "$PAD_ROOTFS_OFFSET_TO" -gt "$header_plus_kernel_len" ] || die "kernel is too large"

    padding_len=$(($PAD_ROOTFS_OFFSET_TO - $header_plus_kernel_len))

    echo "endian: $endian" >&2
    echo "padding_len: $padding_len" >&2

    padded_kernel_len=$(($padding_len + $kernel_len))

    total_len=$(($PAD_ROOTFS_OFFSET_TO + $rootfs_len))

    echo "total_len: $total_len" >&2
else
    [ -f "$fixup" ] || die "TRX file not found: $fixup"
    [ -z "$kernel$rootfs$version$model" ] || die "--fixup cannot be combined with image creation"
fi

padding() {
    head -c $padding_len /dev/zero | tr '\0' '\377'
}

to_hex() {
    hexdump -v -e '1/1 "%02x"'
}

from_hex() {
    perl -pe 's/\s+//g; s/(..)/chr(hex($1))/ge'
}

# Output a 32-bit value in hex with correct endianness
# Usage: hex32 <value>
hex32() {
    val=$(printf '%08x' "$1")
    if [ "$endian" = "le" ]; then
        # Swap bytes for little endian: AABBCCDD -> DDCCBBAA
        echo "$val" | sed 's/\(..\)\(..\)\(..\)\(..\)/\4\3\2\1/'
    else
        echo "$val"
    fi
}

trx_crc32() {
    tmpfile=$(mktemp)
    outtmpfile=$(mktemp)
    if [ -n "$fixup" ]; then
        dd if="$fixup" bs=$HDRLEN skip=1 2>/dev/null > "$tmpfile"
    else
        cat "$kernel" > "$tmpfile"
        padding >> "$tmpfile"
        if [ -f "$rootfs" ]; then
            cat "$rootfs" >> "$tmpfile"
        fi
    fi
    # We just need a CRC-32/JAMCRC of the concatnated files
    # There's no readily available tool for this, but zytrx does create one when
    # creating their TRX header, so we just use that.
    zytrx \
        -B NR7101 \
        -v x \
        -i "$tmpfile" \
        -o "$outtmpfile" >/dev/null
    crc_hex=$(dd if="$outtmpfile" bs=4 count=1 skip=3 2>/dev/null | to_hex)
    rm "$tmpfile" "$outtmpfile" >/dev/null
    hex32 "0x$crc_hex"
}

tclinux_trx_hdr() {
    # TRX header magic: "2RDH" for big endian, "HDR2" for little endian
    if [ "$endian" = "le" ]; then
        printf 'HDR2' | to_hex
    else
        printf '2RDH' | to_hex
    fi

    # Length of the header
    hex32 "$HDRLEN"

    # Length of header + content
    hex32 "$total_len"

    # crc32 of the content
    trx_crc32

    # version
    echo "$version" | to_hex
    head -c "$((32 - $(echo "$version" | wc -c)))" /dev/zero | to_hex

    # customer version
    head -c 32 /dev/zero | to_hex

    # kernel length
    hex32 "$padded_kernel_len"

    # rootfs length
    hex32 "$rootfs_len"

    # romfile length (0)
    hex32 0

    # model (32 bytes, zero-padded)
    if [ -n "$model" ]; then
        printf '%s' "$model" | to_hex
        head -c "$((32 - $(printf '%s' "$model" | wc -c)))" /dev/zero | to_hex
    else
        head -c 32 /dev/zero | to_hex
    fi

    # Address the bootloader decompresses to. Note that some bootloaders
    # read this field from the image in slot A even when booting slot B,
    # so both slots need the same value.
    hex32 "$loadaddr"

    # "reserved" 128 bytes of zeros
    head -c 128 /dev/zero | to_hex
}

if [ -n "$fixup" ]; then
    python3 - "$fixup" "$PAD_ROOTFS_OFFSET_TO" <<'EOF'
import struct
import sys
import zlib

path = sys.argv[1]
kernel_size = int(sys.argv[2])
header_size = 256
peb_size = 128 * 1024
page_size = 2048

def die(message):
    sys.exit(message)

def crc(data, initial=0xffffffff):
    return zlib.crc32(data, initial ^ 0xffffffff) ^ 0xffffffff

with open(path, 'rb') as source:
    data = bytearray(source.read())

if len(data) <= kernel_size or (len(data) - kernel_size) % peb_size:
    die("Invalid TRX/UBI image length")

if data[:4] == b'2RDH':
    endian = '>'
elif data[:4] == b'HDR2':
    endian = '<'
else:
    die("Invalid TRX magic")

if struct.unpack_from(endian + 'I', data, 4)[0] != header_size:
    die("Invalid TRX header length")
if struct.unpack_from(endian + 'I', data, 80)[0] != kernel_size - header_size:
    die("Invalid padded TRX kernel length")
if data[kernel_size:kernel_size + 4] != b'UBI#':
    die("Missing appended UBI")
if data[-peb_size:-peb_size + 8] != b'UBI#\x01EOF':
    die("Missing UBI EOF padding eraseblock")

# zloader checks the complete TFTP payload in ATUR, but uses the header's
# total length at boot. Limit the latter to the immutable kernel partition.
# Adjust four padding bytes in the disposable UBI EOF eraseblock so both
# ranges have the same JAMCRC without touching filesystem data.
kernel_crc = crc(data[header_size:kernel_size])
struct.pack_into(endian + 'II', data, 8, kernel_size, kernel_crc)
struct.pack_into(endian + 'I', data, 84, 0)

if crc(data[header_size:]) != kernel_crc:
    if data[-page_size:] != b'\xff' * page_size:
        die("Missing erased UBI padding for CRC adjustment")
    prefix_crc = crc(data[header_size:-4])
    target = kernel_crc ^ crc(bytes(4), prefix_crc)
    columns = [crc((1 << bit).to_bytes(4, 'little'), 0)
               for bit in range(32)]
    rows = [(sum(((column >> bit) & 1) << index
                 for index, column in enumerate(columns)),
             (target >> bit) & 1) for bit in range(32)]

    for bit in range(32):
        pivot = next(index for index in range(bit, 32)
                     if rows[index][0] & (1 << bit))
        rows[bit], rows[pivot] = rows[pivot], rows[bit]
        for index in range(32):
            if index != bit and rows[index][0] & (1 << bit):
                rows[index] = (rows[index][0] ^ rows[bit][0],
                               rows[index][1] ^ rows[bit][1])

    correction = sum(value << bit for bit, (_, value) in enumerate(rows))
    data[-4:] = correction.to_bytes(4, 'little')

if crc(data[header_size:]) != kernel_crc:
    die("Factory TRX CRC adjustment failed")
sys.stdout.buffer.write(data)

EOF
    exit 0
fi

tclinux_trx_hdr | from_hex
cat "$kernel"
padding
if [ -f "$rootfs" ]; then
    cat "$rootfs"
fi
