# SPDX-License-Identifier: GPL-2.0-only

# The Nokia G-140W-C's bootloader loads a second stage ("bootram", the one
# with the bldr> prompt) from the firmware slot it boots. It sits after the
# kernel and rootfs in the slot's TRX, and its length is in the TRX header.
# Without it the bootloader crashes, and it only falls back to the other
# slot after nine failed boots. The bootram is Nokia's code, so the image
# can't carry it: copy it from the router, from slot A or else from the stock
# firmware in slot B, and put it after the kernel.
G140WC_TRX_MAGIC=$((0x32524448))	# "2RDH"
G140WC_BOOTRAM_OFS=$((0x3e0000))
G140WC_BOOTRAM_MAX=$((0x20000))

# <file> <offset>: big-endian u32 at <offset>, a multiple of 4
g140wc_get32() {
	local v=$(dd if="$1" bs=4 skip=$(($2 / 4)) count=1 2>/dev/null | \
		hexdump -v -e '1/1 "%02x"')

	echo $((0x${v:-0}))
}

# <file> <offset> <value>: store a big-endian u32 at <offset>
g140wc_put32() {
	printf "$(printf '\\%03o\\%03o\\%03o\\%03o' \
		$(($3 >> 24 & 255)) $(($3 >> 16 & 255)) \
		$(($3 >> 8 & 255)) $(($3 & 255)))" | \
		dd of="$1" bs=4 seek=$(($2 / 4)) conv=notrunc 2>/dev/null
}

g140wc_mtd_size() {
	local size=$(grep "\"$1\"" /proc/mtd | cut -d' ' -f2)

	[ -n "$size" ] && echo $((0x$size))
}

# <partition> <offset> <length> <file>: copy bytes out of an MTD partition
g140wc_mtd_read() {
	local mtd=$(find_mtd_index "$1")
	local ofs=$(($2 % 4096))
	local tmp=/tmp/g140wc.chunk

	[ -n "$mtd" ] || return 1
	dd if=/dev/mtd$mtd of=$tmp bs=4096 skip=$(($2 / 4096)) \
		count=$(((ofs + $3 + 4095) / 4096)) 2>/dev/null || return 1
	tail -c +$((ofs + 1)) $tmp > $tmp.2
	dd if=$tmp.2 of="$4" bs=$3 count=1 2>/dev/null
	rm -f $tmp $tmp.2
	[ "$(wc -c < "$4")" -eq "$3" ]
}

# <partition> <next partition> <file>: copy the bootram of the TRX at the
# start of <partition>. It may lie in <next partition>, which follows it on
# flash.
g140wc_copy_bootram() {
	local part="$1" next="$2" out="$3"
	local hdr=/tmp/g140wc.hdr
	local size len ofs

	size=$(g140wc_mtd_size "$part")
	[ -n "$size" ] || return 1
	g140wc_mtd_read "$part" 0 256 $hdr || return 1
	[ "$(g140wc_get32 $hdr 0)" = "$G140WC_TRX_MAGIC" ] || return 1
	len=$(g140wc_get32 $hdr 148)
	ofs=$((256 + $(g140wc_get32 $hdr 80) + $(g140wc_get32 $hdr 84)))
	rm -f $hdr
	[ "$len" -gt 0 ] && [ "$len" -le "$G140WC_BOOTRAM_MAX" ] || return 1

	if [ $((ofs + len)) -gt "$size" ]; then
		[ -n "$next" ] && [ "$ofs" -ge "$size" ] || return 1
		part="$next"
		ofs=$((ofs - size))
	fi
	g140wc_mtd_read "$part" "$ofs" "$len" "$out" || return 1

	# LZMA, with the properties byte all known bootrams have
	[ "$(dd if="$out" bs=1 count=1 2>/dev/null | hexdump -v -e '1/1 "%02x"')" = "5d" ]
}

g140wc_get_bootram() {
	g140wc_copy_bootram tclinux_kernel "" "$1" ||
		g140wc_copy_bootram kernel_slave rootfs_slave "$1"
}

# <sysupgrade image> <output tar>: the same image, with the bootram added to
# the kernel at G140WC_BOOTRAM_OFS and the TRX header updated to match
g140wc_add_bootram() {
	local img="$1" out="$2"
	local dir=/tmp/g140wc-upgrade
	local board_dir kernel len

	rm -rf $dir
	mkdir -p $dir || return 1
	g140wc_get_bootram $dir/bootram || {
		echo "Can't find the bootloader's second stage (bootram) in slot A or B"
		return 1
	}
	len=$(wc -c < $dir/bootram)

	tar xf "$img" -C $dir || return 1
	board_dir=$(cd $dir && ls -d sysupgrade-*)
	kernel=$dir/$board_dir/kernel
	[ -d "$dir/$board_dir" ] && [ -f "$kernel" ] && \
	[ "$(g140wc_get32 $kernel 0)" = "$G140WC_TRX_MAGIC" ] || {
		echo "The image has no TRX kernel"
		return 1
	}

	# Everything from G140WC_BOOTRAM_OFS on must be padding
	if dd if=$kernel bs=$G140WC_BOOTRAM_OFS skip=1 2>/dev/null | \
	   hexdump -e '16/1 "%02x" "\n"' | \
	   grep -q -v -e '^f\{32\}$' -e '^\*$'; then
		echo "The kernel doesn't fit in front of the bootram"
		return 1
	fi

	dd if=$kernel of=$kernel.new bs=$G140WC_BOOTRAM_OFS count=1 2>/dev/null
	cat $dir/bootram >> $kernel.new
	rm $dir/bootram
	[ "$(wc -c < $kernel.new)" -eq $((G140WC_BOOTRAM_OFS + len)) ] || return 1

	g140wc_put32 $kernel.new 8 $((G140WC_BOOTRAM_OFS + len))	# total
	g140wc_put32 $kernel.new 80 $((G140WC_BOOTRAM_OFS - 256))	# kernel
	g140wc_put32 $kernel.new 84 0					# rootfs
	g140wc_put32 $kernel.new 148 $len				# bootram

	# CRC-32/JAMCRC of everything after the header. gzip computes the
	# CRC-32 and stores it little-endian in its trailer.
	set -- $(dd if=$kernel.new bs=256 skip=1 2>/dev/null | gzip -1 -c | \
		tail -c 8 | hexdump -n 4 -v -e '4/1 "%02x "')
	[ $# = 4 ] || return 1
	g140wc_put32 $kernel.new 12 $((0x$4$3$2$1 ^ 0xffffffff))
	mv $kernel.new $kernel

	tar cf "$out.tmp" -C $dir "$board_dir" || return 1
	rm -rf $dir
	mv "$out.tmp" "$out"
}

# The stock firmware keeps the Wi-Fi calibration, one EEPROM table per
# radio, as files on its UBIFS "config" partition, which the Wi-Fi driver
# can't read. OpenWrt keeps a copy in a static UBI volume next to its root,
# which the device tree hands to the driver: the 2.4 GHz table at 0x0,
# padded with zeros to the 1 KiB the mt7603 driver reads, and the 5 GHz
# table at 0x400.
G140WC_CAL_VOL=calibration
G140WC_CAL_SIZE=1536

# <file> <offset>: the two bytes at <offset> (an even number) in hex
g140wc_get16() {
	dd if="$1" bs=2 skip=$(($2 / 2)) count=1 2>/dev/null | \
		hexdump -v -e '1/1 "%02x"'
}

# <file>: check a calibration volume image by its size and chip IDs
g140wc_check_cal() {
	[ "$(wc -c < "$1")" -eq $G140WC_CAL_SIZE ] || return 1
	case "$(g140wc_get16 "$1" 0)" in
	9275|0376|0076|2876) ;;
	*) return 1 ;;
	esac
	case "$(g140wc_get16 "$1" 1024)" in
	6276|1276) ;;
	*) return 1 ;;
	esac
}

g140wc_has_cal() {
	local ubi=$(nand_find_ubi "$CI_UBIPART")

	[ -n "$ubi" ] && [ -n "$(nand_find_volume $ubi $G140WC_CAL_VOL)" ]
}

# <file>: build the calibration volume image from the stock settings. The
# "config" partition is read-only in the device tree, so UBI attaches it
# read-only; refuse to attach it otherwise.
g140wc_cal_from_stock() {
	local out="$1"
	local mtd=$(find_mtd_index config)
	local dir=/tmp/g140wc-config
	local ubi ret=1

	[ -n "$mtd" ] || return 1
	[ $(($(cat /sys/class/mtd/mtd$mtd/flags) & 0x400)) -eq 0 ] || return 1
	[ -z "$(nand_find_ubi config)" ] || return 1
	ubiattach -m $mtd > /dev/null 2>&1 || return 1
	ubi=$(nand_find_ubi config)

	mkdir -p $dir
	if [ -n "$ubi" ] && mount -t ubifs -o ro ${ubi}_0 $dir 2>/dev/null; then
		[ -f $dir/RT30xxEEPROM.bin ] && \
		[ -f $dir/bin_ac/RT30xxEEPROM.bin ] && {
			dd if=$dir/RT30xxEEPROM.bin bs=512 count=1
			dd if=/dev/zero bs=512 count=1
			dd if=$dir/bin_ac/RT30xxEEPROM.bin bs=512 count=1
		} 2>/dev/null > "$out" && g140wc_check_cal "$out" && ret=0
		umount $dir
	fi
	rmdir $dir
	ubidetach -m $mtd > /dev/null 2>&1
	[ $ret = 0 ] || rm -f "$out"
	return $ret
}

# <file>: create the calibration volume. This runs on the ramfs, before
# nand_do_upgrade creates rootfs_data from all the free space. If rootfs_data
# is there, remove it first: nand_do_upgrade recreates it, and the settings
# to keep are saved in /tmp. The volume ID is the second-to-last one, so the
# other volumes keep their usual IDs.
g140wc_store_cal() {
	local ubi vol

	ubi=$(nand_attach_ubi "$CI_UBIPART") || return 1
	[ -n "$(nand_find_volume $ubi $G140WC_CAL_VOL)" ] && return 0
	if [ -n "$(nand_find_volume $ubi rootfs_data)" ]; then
		ubirmvol /dev/$ubi -N rootfs_data || return 1
	fi
	ubimkvol /dev/$ubi -N $G140WC_CAL_VOL -t static -s $G140WC_CAL_SIZE \
		-n $(($(cat /sys/class/ubi/$ubi/max_vol_count) - 2)) || return 1
	vol=$(nand_find_volume $ubi $G140WC_CAL_VOL)
	[ -n "$vol" ] && ubiupdatevol /dev/$vol "$1"
}
