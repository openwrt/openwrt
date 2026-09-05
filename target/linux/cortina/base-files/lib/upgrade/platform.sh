REQUIRE_IMAGE_METADATA=1

CSMI_SB_MAGIC=0x44113761
CSMI_SB_SIZE=3942

csmi_u32() {
	printf "\\$(printf %03o $(($1 & 255)))\\$(printf %03o $((($1 >> 8) & 255)))\\$(printf %03o $((($1 >> 16) & 255)))\\$(printf %03o $((($1 >> 24) & 255)))"
}

csmi_zero() {
	[ "$1" -gt 0 ] && dd if=/dev/zero bs="$1" count=1 2>/dev/null
}

csmi_str() {
	printf '%s' "$1"
	csmi_zero $(($2 - ${#1}))
}

csmi_hex() {
	local h="$1"

	while [ -n "$h" ]; do
		printf "\\$(printf %03o 0x${h%${h#??}})"
		h=${h#??}
	done
}

csmi_entry() {
	local idx="$1" name="$2" md5="$3" size="$4" sizestr="$5" offstr="$6"
	local type="$7" typename="$8" flag="$9" mount="${10}"

	[ "$idx" = 0 ] && csmi_u32 0 || csmi_u32 $((0x66 + idx * 0x100))
	csmi_str "$name" 32
	csmi_hex "${md5:-00000000000000000000000000000000}"
	csmi_u32 "${size:-0}"
	csmi_str "$sizestr" 32
	csmi_str "$offstr" 32
	csmi_u32 "$type"
	csmi_str "$typename" 32
	csmi_u32 "${flag:-0}"
	csmi_str "$mount" 96
}

almondplus_sb_body() {
	csmi_u32 5
	csmi_zero 64
	printf '\017'
	csmi_u32 0x66
	csmi_zero 1
	csmi_entry 0 uboot-env0 "$SB_ENV0_MD5" "$SB_ENV0_SIZE" 1024K 0x00100000 4 data
	csmi_entry 1 uboot-env1 "$SB_ENV1_MD5" "$SB_ENV1_SIZE" 1024K 0x00300000 4 data
	csmi_entry 2 sb0 "" "" 1024K 0x00400000 4 data
	csmi_entry 3 sb1 "" "" 1024K 0x00500000 4 data
	csmi_entry 4 kernel "$SB_KERNEL_MD5" "$SB_KERNEL_SIZE" 6144K 0x03A00000 1 uimage 1
	csmi_entry 5 rootfs "$SB_ROOTFS_MD5" "$SB_ROOTFS_SIZE" 47104K 0x04000000 2 uimage
	csmi_entry 6 kernel_standby "$SB_KERNEL_STANDBY_MD5" "$SB_KERNEL_STANDBY_SIZE" 6144K 0x00600000 1 uimage
	csmi_entry 7 rootfs_standby "$SB_ROOTFS_STANDBY_MD5" "$SB_ROOTFS_STANDBY_SIZE" 47104K 0x00C00000 2 uimage
	csmi_entry 8 rootfs_data "" "" 16384K 0x6E00000 4 data
	csmi_entry 9 log "" "" 1024K 0x07E00000 4 data 0 /log
	csmi_entry 10 cal "" "" 1024K 0x07F00000 4 data 0 /cal
	csmi_entry 11 man "" "" 1024K 0x08000000 4 data 0 /man
	csmi_entry 12 Almond_Data "" "" 5120K 0x08100000 4 data 0 /data
	csmi_entry 13 uboot0 "$SB_UBOOT0_MD5" "$SB_UBOOT0_SIZE" 896K 0x20000 4 data
	csmi_entry 14 uboot1 "$SB_UBOOT1_MD5" "$SB_UBOOT1_SIZE" 1024K 0x00200000 4 data
	csmi_zero 4
}

almondplus_sb_image() {
	local body="$1.body"
	local md5

	almondplus_sb_body > "$body"
	md5=$(md5sum "$body" | cut -d' ' -f1)
	{
		csmi_u32 $CSMI_SB_MAGIC
		csmi_u32 $CSMI_SB_SIZE
		csmi_hex "$md5"
		cat "$body"
	} > "$1"
	rm -f "$body"
}

almondplus_kernel_md5() {
	local mtd="$1" size="$2"
	local pages=$((size / 2048)) rest=$((size % 2048))

	{
		dd if=/dev/mtd"$mtd" bs=2048 count="$pages" 2>/dev/null
		[ "$rest" -gt 0 ] && dd if=/dev/mtd"$mtd" bs=1 skip=$((pages * 2048)) count="$rest" 2>/dev/null
	} | md5sum | cut -d' ' -f1
}

almondplus_fix_sb0() {
	local mtd sb=/tmp/sysupgrade-sb0.bin magic flag md5

	mtd=$(find_mtd_index sb0)
	[ -n "$mtd" ] || return 0
	dd if=/dev/mtd"$mtd" of="$sb" bs=$CSMI_SB_SIZE count=1 2>/dev/null || return 0
	magic=$(dd if="$sb" bs=4 count=1 2>/dev/null | hexdump -e '1/4 "%08x"')
	[ "$magic" = "44113761" ] || return 0
	flag=$(dd if="$sb" bs=1 skip=24 count=1 2>/dev/null | hexdump -e '1/1 "%d"')
	[ $((flag & 4)) -ne 0 ] || return 0
	flag=$(((flag & ~4) | 2))
	{
		dd if="$sb" bs=1 skip=25 count=$((CSMI_SB_SIZE - 25)) 2>/dev/null
	} > "$sb.tail"
	{
		printf "\\$(printf %03o $flag)"
		cat "$sb.tail"
	} > "$sb.body"
	md5=$(md5sum "$sb.body" | cut -d' ' -f1)
	{
		csmi_u32 $CSMI_SB_MAGIC
		csmi_u32 $CSMI_SB_SIZE
		csmi_hex "$md5"
		cat "$sb.body"
	} > "$sb.new"
	rm -f "$sb" "$sb.tail" "$sb.body"
	mtd write "$sb.new" sb0
}

almondplus_write_kernel_sb() {
	local tar_file="$1"
	local board_dir kernel mtd md5

	board_dir=$(tar tf "$tar_file" | grep -m 1 '^sysupgrade-.*/$')
	board_dir=${board_dir%/}
	[ -n "$board_dir" ] || return 1

	kernel=/tmp/sysupgrade-kernel.bin
	tar xf "$tar_file" "$board_dir/kernel" -O > "$kernel" || return 1
	SB_KERNEL_MD5=$(md5sum "$kernel" | cut -d' ' -f1)
	SB_KERNEL_SIZE=$(wc -c < "$kernel")

	mtd=$(find_mtd_index kernel)
	[ -n "$mtd" ] || return 1
	mtd write "$kernel" kernel || return 1
	rm -f "$kernel"
	md5=$(almondplus_kernel_md5 "$mtd" "$SB_KERNEL_SIZE")
	if [ "$md5" != "$SB_KERNEL_MD5" ]; then
		echo "kernel readback mismatch (bad blocks in the kernel partition?), not updating the superblock"
		return 1
	fi

	almondplus_sb_image /tmp/sysupgrade-sb1.bin
	mtd write /tmp/sysupgrade-sb1.bin sb1 || return 1
	almondplus_fix_sb0
}

platform_check_image() {
	local board_dir kernel size

	case "$(board_name)" in
	securifi,almondplus)
		board_dir=$(tar tf "$1" | grep -m 1 '^sysupgrade-.*/$')
		board_dir=${board_dir%/}
		[ -n "$board_dir" ] || {
			echo "not a sysupgrade tar image"
			return 1
		}
		size=$(tar xf "$1" "$board_dir/kernel" -O | wc -c)
		[ "$size" -gt 0 ] && [ "$size" -le $((6144 * 1024)) ] || {
			echo "kernel missing or larger than the 6 MiB kernel partition"
			return 1
		}
		;;
	esac
	return 0
}

platform_do_upgrade() {
	case "$(board_name)" in
	securifi,almondplus)
		almondplus_write_kernel_sb "$1" || return 1
		nand_do_upgrade "$1"
		;;
	*)
		default_do_upgrade "$1"
		;;
	esac
}
