# SPDX-License-Identifier: GPL-2.0-only
# Build a TP-Link v3 kernel blob (512-byte tag + FIT) for U-Boot tpimg.
# Mirrors scripts/mk_tp_v3_fit.py for stage2 ramdisk (no Python).

XB432V_TP_V3_TAG_LEN=512
XB432V_TP_V3_TAG_VERSION=50331651

xb432v_be32_to_dec() {
	local hex="$1"
	local b0 b1 b2 b3
	b0=$((0x${hex:0:2}))
	b1=$((0x${hex:2:2}))
	b2=$((0x${hex:4:2}))
	b3=$((0x${hex:6:2}))
	echo $((b0 * 16777216 + b1 * 65536 + b2 * 256 + b3))
}

xb432v_fit_payload_len() {
	local file="$1"
	local magic hex len

	magic=$(xb432v_bb dd if="$file" bs=4 count=1 2>/dev/null | \
		xb432v_bb hexdump -v -n 4 -e '1/1 "%02x"')
	[ "$magic" = "d00dfeed" ] || return 1
	hex=$(xb432v_bb dd if="$file" bs=1 skip=4 count=4 2>/dev/null | \
		xb432v_bb hexdump -v -n 4 -e '1/1 "%02x"')
	len=$(xb432v_be32_to_dec "$hex")
	[ "$len" -ge 64 ] || return 1
	echo "$len"
}

xb432v_crc32_file() {
	local file="$1"
	xb432v_bb hexdump -v -e '1/1 "%u\n"' "$file" 2>/dev/null | xb432v_bb awk '
BEGIN { crc = 0xffffffff }
{
	b = $1 + 0
	crc = xor(crc, b)
	for (i = 0; i < 8; i++) {
		if (and(crc, 1)) {
			crc = xor(rshift(crc, 1), 0xedb88320)
		} else {
			crc = rshift(crc, 1)
		}
		crc = and(crc, 0xffffffff)
	}
}
END { printf "%u", and(xor(crc, 0xffffffff), 0xffffffff) }
'
}

xb432v_write_le32() {
	local file="$1" offset="$2" val="$3"
	local b0 b1 b2 b3
	b0=$((val & 255))
	b1=$(((val >> 8) & 255))
	b2=$(((val >> 16) & 255))
	b3=$(((val >> 24) & 255))
	# Busybox printf lacks %b; emit four raw bytes for little-endian u32.
	printf "\\$(printf '%03o' "$b0")\\$(printf '%03o' "$b1")\\$(printf '%03o' "$b2")\\$(printf '%03o' "$b3")" | \
		xb432v_bb dd of="$file" bs=1 seek="$offset" count=4 conv=notrunc 2>/dev/null
}

xb432v_build_tpv3_kernel() {
	local file="$1"
	local fit_len fit_file tag_file out_file kernel_crc image_len
	local loadaddr=$((${XB432V_LOADADDR:-0x80088000}))

	fit_len=$(xb432v_fit_payload_len "$file") || return 1
	fit_file="$(mktemp)"
	tag_file="$(mktemp)"
	out_file="$(mktemp)"
	trap 'rm -f "$fit_file" "$tag_file" "$out_file"' RETURN

	xb432v_bb dd if="$file" bs=1 count="$fit_len" 2>/dev/null >"$fit_file" || return 1

	kernel_crc=$(xb432v_crc32_file "$fit_file")
	[ -n "$kernel_crc" ] || return 1
	image_len=$((XB432V_TP_V3_TAG_LEN + fit_len))

	xb432v_bb dd if=/dev/zero bs="$XB432V_TP_V3_TAG_LEN" count=1 2>/dev/null >"$tag_file" || return 1
	xb432v_write_le32 "$tag_file" 0 "$XB432V_TP_V3_TAG_VERSION"
	xb432v_write_le32 "$tag_file" 104 "$loadaddr"
	xb432v_write_le32 "$tag_file" 108 "$loadaddr"
	xb432v_write_le32 "$tag_file" 112 "$image_len"
	xb432v_write_le32 "$tag_file" 116 "$XB432V_TP_V3_TAG_LEN"
	xb432v_write_le32 "$tag_file" 120 "$fit_len"
	xb432v_write_le32 "$tag_file" 152 "$kernel_crc"
	printf '\125\252\125\252\361\342\323\304\345\246\152\136\114\075\056\037\252\125\252\125' | \
		xb432v_bb dd of="$tag_file" bs=1 seek=84 count=20 conv=notrunc 2>/dev/null

	cat "$tag_file" "$fit_file" >"$out_file"
	cat "$out_file"
	return 0
}
