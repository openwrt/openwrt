. /lib/functions.sh

fortinet_get_active() {
	local fwinfo_mtd="$(find_mtd_part firmware-info)"

	if [ -z "$fwinfo_mtd" ]; then
		echo "WARN: MTD device \"firmware-info\" not found"
		return 1
	fi

	hexdump -n 1 -s $((0x170)) -e '1/1 "%d"' $fwinfo_mtd
}

fortinet_parse_metadata() {
	local key value output

	[ ! -r "/tmp/sysupgrade.meta" ] && \
		return 1

	sed -e 's/, \{1,3\}\"/\n"/g' \
	    -e 's/{ \{1,2\}/\n/g' \
	    -e 's/ \{1,2\}}/\n/g' < /tmp/sysupgrade.meta \
		> /tmp/sysupgrade.meta.tmp

	while read key value; do
		key="${key//\"/}"
		value="${value//\"/}"

		[ -z "$value" ] && continue
		[ -z "$1" ] && break
		if [ "$key" = "${1}:" ]; then
			output="${output:+$output }$value"
			shift
		fi
	done < /tmp/sysupgrade.meta.tmp

	echo "$output"
}

fortinet_align_length() {
	local orig="$1"
	local blksz="$2"
	local align

	align=$((orig / blksz))
	[ $((orig % blksz)) -gt 0 ] && \
		align=$((align + 1))
	align=$((align * blksz))

	echo $align
}

fortinet_check_image() {
	local board_dir="$(tar tf "$1" | grep -m 1 '^sysupgrade-.*/$')"
	local fw_mtd
	local kern_len root_len fwpart_len fwpart_erase
	local tmp ver="1.0" msg
	local active

	board_dir="${board_dir%/}"
	active=$(fortinet_get_active)
	case "$active" in
	0) PART_NAME="firmware" ;;
	1) PART_NAME="firmware2" ;;
	*) echo "ERROR: invalid active partition is set in \"firmware-info\""
	   umount -a
	   reboot -f ;;
	esac

	fw_mtd="$(find_mtd_part $PART_NAME)"
	if [ -z "$fw_mtd" ]; then
		echo "ERROR: MTD device \"$PART_NAME\" not found"
		return 1
	fi

	kern_len=$( (tar xOf "$1" "$board_dir/kernel" | wc -c) 2> /dev/null)
	root_len=$( (tar xOf "$1" "$board_dir/root" | wc -c) 2> /dev/null)
	if [ -z "$kern_len" ] || [ -z "$root_len" ]; then
		echo "ERROR: failed to get kernel/rootfs length of new firmware"
		return 1
	fi

	fwpart_len=$(cat /sys/class/mtd/${fw_mtd//\/dev\/mtdblock/mtd}/size)
	fwpart_erase=$(cat /sys/class/mtd/${fw_mtd//\/dev\/mtdblock/mtd}/erasesize)
	if [ -z "$fwpart_len" ] || [ -z "$fwpart_erase" ]; then
		echo "ERROR: failed to get partition size or erasesize of \"$PART_NAME\" partition"
		return 1
	fi

	ver="$(fortinet_parse_metadata compat_version)"
	msg="$(fortinet_parse_metadata compat_message)"

	# calculate kernel length if the image has "mtdsplit" in
	# compat_version or something of compat_version other than "1.0"
	if [ "$msg" = "mtdsplit" ] || [ "$ver" != "1.0" ]; then
		kern_len="$(fortinet_align_length $kern_len $fwpart_erase)"
	else
		# older firmware supports only the 1st partition ("firmware")
		if [ "$active" = "1" ]; then
			echo "older firmware doesn't support booting from $PART_NAME"
			echo "please switch to the 1st partition on the bootmenu and perform sysupgrade on that image,"
			echo "or perform sysupgrade with '-F' option to flash the firmware to the 1st partition instead of 2nd"
			return 1
		fi
		# for downgrading to older firmware that
		# has fixed kernel/rootfs partitions
		kern_len=0x600000
	fi
	root_len="$(fortinet_align_length $root_len $fwpart_erase)"
	if [ $((kern_len + root_len)) -gt $fwpart_len ]; then
		echo "ERROR: new kernel+rootfs is larger than the current $PART_NAME partition"
		return 1
	fi

	return 0
}

fortinet_bswap32() {
	local val="$(printf %08x $(($1)))"

	# swap and print in hex
	echo "0x${val:6:2}${val:4:2}${val:2:2}${val:0:2}"
}

fortinet_by2bl() {
	local blks="$(($1 / 0x200))"
	[ $(($1 % 0x200)) -gt 0 ] && blks=$((blks + 1))

	printf "0x%08x" $blks
}

fortinet_bl2by() {
	printf "0x%08x" $(($1 * 0x200))
}

fortinet_build_partmap() {
	local new="$1" old="$2"
	local len="${old%%@*}" ofs="${old##*@}"

	case "$new" in
	@*)  ofs="$(fortinet_by2bl ${new##@})" ;;	# "@<offset>"

	*@*) len="$(fortinet_by2bl ${new%%@*})"		# "<length>@<offset>"
	     ofs="$(fortinet_by2bl ${new##*@})" ;;

	"")  ;;						# "" (empty)

	*)   len="$(fortinet_by2bl ${new%%@*})" ;;	# "<length>"
	esac

	# print N blocks of length/offset in dec
	echo "${len}@${ofs}"
}

# Update firmware information in "firmware-info" partition
#
# parameters:
#   $1: image index (0/1)
#   $2: new image name (up to 32 characters)
#   $3: length and/or offset for kernel (bytes)
#   $4: length and/or offset for rootfs (bytes)
#
# Note: $3 and $4 support multiple formats:
#
# - <length>@<offset>: set <length> and <rootfs>
# - <length>         : set <length> and keep the current offset
# - @<offset>        : set <offset> and keep the current length
# - "" (empty)       : keep the current length and offset
fortinet_update_fwinfo() {
	local fwinfo_mtd="$(find_mtd_part firmware-info)"
	local index="$1"
	local name="$2"
	local offset
	local old_kr
	local old new tmp part pos
	local output

	if [ -z "$fwinfo_mtd" ]; then
		echo "ERROR: MTD device \"firmware-info\" not found"
		return 1
	fi

	# Image Name
	case "$index" in
	0) offset=0x10 ;;
	1) offset=0x30 ;;
	*) echo "ERROR: invalid image index specified!"; return 1 ;;
	esac

	printf "Image Index: %d\n" $index

	old="$(dd bs=16 count=2 skip=$((offset / 16)) if=$fwinfo_mtd 2>/dev/null)"
	printf "Image Name : \"%s\"\n" "$old"
	if [ -n "$name" ]; then
		echo -n "$name" | \
			dd bs=32 count=1 oflag=seek_bytes seek=$((offset)) \
				conv=sync,notrunc of=$fwinfo_mtd 2>/dev/null
		printf "             --> \"%s\"\n\n" "$name"
	else
		printf "\n"
	fi

	# length/offset values of kernel/rootfs
	case "$index" in
	0) offset=0x180 ;;
	1) offset=0x190 ;;
	esac

	# <kernel offset:4><kernel length:4><rootfs offset:4><rootfs length:4>
	old_kr="$(hexdump -n 16 -v -s $((offset)) -e '1/4 "%08x"' $fwinfo_mtd)"

	pos=0
	for part in kernel rootfs; do
		old="0x${old_kr:$((8 + pos)):8}@0x${old_kr:$((0 + pos)):8}"
		new="$(fortinet_build_partmap "$3" "$old")"
		shift

		printf "  %s:\n" $part
		printf "    old: 0x%08x@0x%08x\n" \
			$(fortinet_bl2by ${old%%@*}) $(fortinet_bl2by ${old##*@})
		printf "    new: 0x%08x@0x%08x\n\n" \
			$(fortinet_bl2by ${new%%@*}) $(fortinet_bl2by ${new##*@})

		tmp="$(fortinet_bswap32 ${new%%@*})@$(fortinet_bswap32 ${new##*@})"
		new="$(echo $tmp | sed 's/0x\([0-9a-f]\{8\}\)@0x\([0-9a-f]\{8\}\)/\2\1/')"
		output="${output}${new}"

		pos=$((pos + 16))
	done

	data_2bin "$output" | \
		dd bs=16 count=1 seek=$((offset / 16)) conv=notrunc \
			of=$fwinfo_mtd 2>/dev/null
}

fortinet_do_upgrade() {
	local board_dir="$(tar tf "$1" | grep -m 1 '^sysupgrade-.*/$')"
	local fw_mtd
	local kern_len kern_ofs
	local fwpart_erase
	local imgname ver="1.0" msg
	local active
	local kern_padlen

	# try to load and parse /tmp/sysupgrade.meta
	# for image name and rootfs offset
	imgname="$(fortinet_parse_metadata dist version revision)"
	[ -z "$imgname" ] && imgname="OpenWrt"
	ver="$(fortinet_parse_metadata compat_version)"
	msg="$(fortinet_parse_metadata compat_message)"

	# for downgrading to older firmware that
	# has fixed kernel/rootfs partitions
	if [ "$msg" != "mtdsplit" ] && [ "$ver" = "1.0" ]; then
		# use the 1st partition
		active="0"
		echo -e "\n##### older firmware image detected #####"
		echo -e "using the 1st partition and 0x600000 as rootfs offset"
		echo -e "please switch active partition on the bootmenu when rebooting"
		echo -e "(\"[B]: Boot with backup firmware and set as default.\")\n"
		kern_padlen="0x600000"
	fi

	board_dir="${board_dir%/}"
	[ -z "$active" ] && \
		active=$(fortinet_get_active)
	case "$active" in
	0) PART_NAME="firmware" ;;
	1) PART_NAME="firmware2" ;;
	*) echo "ERROR: invalid active partition is set in \"firmware-info\""
	   umount -a
	   reboot -f ;;
	esac

	fw_mtd="$(find_mtd_part $PART_NAME)"
	if [ -z "$fw_mtd" ]; then
		echo "ERROR: MTD device \"$PART_NAME\" not found"
		umount -a
		reboot -f
	fi

	kern_ofs=$(cat /sys/class/mtd/${fw_mtd//\/dev\/mtdblock/mtd}/offset)
	kern_len=$( (tar xOf "$1" "$board_dir/kernel" | wc -c) 2> /dev/null)
	if [ -z "$kern_ofs" ] || [ -z "$kern_len" ]; then
		echo "ERROR: failed to get offset or length of new kernel"
		umount -a
		reboot -f
	fi

	fwpart_erase=$(cat /sys/class/mtd/${fw_mtd//\/dev\/mtdblock/mtd}/erasesize)

	fortinet_update_fwinfo "$active" "${imgname%% }" \
		"${kern_len}@${kern_ofs}" "" || {
		umount -a
		reboot -f
	}

	tar xOf "$1" "$board_dir/kernel" > "${1}.kernel"
	# pad with the erase size or fixed size
	dd if="${1}.kernel" of="${1}.concat" \
		bs=$((${kern_padlen:-$fwpart_erase})) conv=sync
	tar xOf "$1" "$board_dir/root" >> "${1}.concat"

	default_do_upgrade "${1}.concat"
}
