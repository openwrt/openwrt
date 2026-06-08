#!/bin/sh
#
# Create a Lantiq VRX200 NAND TPL (Tertiary Program Loader) boot image.
#
# Image layout:
#   0x000-0x00B: NVB DWNLD header (12 bytes)
#   0x00C-0x137: NAND bootstrap (300 bytes, loaded by BootROM to 0xBE220000)
#   0x138-0x143: NVB START header (12 bytes)
#   0x144-0x7FF: 0xFF padding (rest of first NAND page)
#   0x800-0x3FFF: SPL binary (linked at 0xBE220800, loaded by bootstrap)
#   0x4000+: Compressed U-Boot image (loaded by SPL)
#
# Usage: create_nandtpl.sh <bootstrap> <spl> <uboot> <output>
#

set -e

BOOTSTRAP="$1"
SPL="$2"
UBOOT="$3"
OUTPUT="$4"

if [ -z "$BOOTSTRAP" ] || [ -z "$SPL" ] || [ -z "$UBOOT" ] || [ -z "$OUTPUT" ]; then
	echo "Usage: $0 <bootstrap> <spl> <uboot> <output>" >&2
	exit 1
fi

BOOTSTRAP_SIZE=$(wc -c < "$BOOTSTRAP")
UBOOT_OFFSET=16384  # 0x4000
PAGE_SIZE=2048       # 0x800

# Calculate NVB DWNLD size field (bootstrap_size + 4 for address word)
NVB_SIZE=$((BOOTSTRAP_SIZE + 4))

# Write a 32-bit big-endian value as 4 binary bytes
write_be32() {
	printf "$(printf '\\%03o\\%03o\\%03o\\%03o' \
		$(( ($1 >> 24) & 0xFF )) \
		$(( ($1 >> 16) & 0xFF )) \
		$(( ($1 >> 8) & 0xFF )) \
		$(( $1 & 0xFF )))"
}

# Create the output image
{
	# NVB DWNLD header (12 bytes)
	# Command: 0x5509AAF6 (NVB_CMD_DWNLD with START|SDBG flags)
	write_be32 $((0x5509AAF6))
	# Size: bootstrap_size + 4 (includes address word)
	write_be32 "$NVB_SIZE"
	# Entry address: 0xBE220000
	write_be32 $((0xBE220000))

	# Bootstrap binary
	cat "$BOOTSTRAP"

	# NVB START header (12 bytes)
	# Command: 0x770888F7 (NVB_CMD_START with SDBG flag)
	write_be32 $((0x770888F7))
	# Size: 4 (just the address word)
	write_be32 4
	# Entry address: 0xBE220000
	write_be32 $((0xBE220000))
} > "$OUTPUT.tmp"

# Calculate header total size
HEADER_SIZE=$(wc -c < "$OUTPUT.tmp")

# Pad page 0 to PAGE_SIZE with 0xFF
PAD_SIZE=$((PAGE_SIZE - HEADER_SIZE))
if [ "$PAD_SIZE" -gt 0 ]; then
	dd if=/dev/zero bs=1 count="$PAD_SIZE" 2>/dev/null | tr '\000' '\377' >> "$OUTPUT.tmp"
fi

# Append SPL binary
cat "$SPL" >> "$OUTPUT.tmp"

# Calculate current size and pad to UBOOT_OFFSET
CUR_SIZE=$(wc -c < "$OUTPUT.tmp")
if [ "$CUR_SIZE" -lt "$UBOOT_OFFSET" ]; then
	PAD_SIZE=$((UBOOT_OFFSET - CUR_SIZE))
	dd if=/dev/zero bs=1 count="$PAD_SIZE" 2>/dev/null | tr '\000' '\377' >> "$OUTPUT.tmp"
elif [ "$CUR_SIZE" -gt "$UBOOT_OFFSET" ]; then
	echo "Error: SPL too large, exceeds U-Boot offset 0x4000" >&2
	rm -f "$OUTPUT.tmp"
	exit 1
fi

# Append U-Boot image
cat "$UBOOT" >> "$OUTPUT.tmp"

mv "$OUTPUT.tmp" "$OUTPUT"

echo "Created NANDTPL image: $OUTPUT ($(wc -c < "$OUTPUT") bytes)"
