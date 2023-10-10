#
# Copyright (C) 2019 OpenWrt.org
#

. /lib/functions.sh

iodata_mstc_prepare_fail() {
	echo "failed to check and prepare the environment, rebooting..."
	umount -a
	reboot -f
}

# read/write 1byte in mtd device
#
# parameters:
#   $1: target mtd device ("/dev/mtdblockN")
#   $2: offset of target value (decimal or hex)
#   $3: value to set (decimal or hex, don't set when reading)
iodata_mstc_rw_byte() {
	local mtd="$1"
	local offset="$2"
	local setval="$3"
	local _val=$(hexdump -s $offset -n 1 -e '"%d"' $mtd)

	if [ -z "$setval" ]; then
		echo $_val
		return 0
	fi

	# decimal or hex -> decimal
	setval=$((setval))
	[ "$_val" = "$setval" ] && return 0
	setval="$(printf '%02x' $setval)"

	if ! (printf "\x$setval" | dd bs=1 seek=$((offset)) conv=notrunc of=$mtd 2>/dev/null); then
		return 1
	fi
}

# set flag in mtd device on I-O DATA devices manufactured by MSTC
# (MitraStar Technology Corp.)
#
# parameters:
#   $1: parameter name
#   $2: mtd name contains target flag
#   $3: offset of flag
#   $4: valid flag values ("n,n,...", ex:"0,1" or "1,2")
#   $5: value to set to the flag
iodata_mstc_set_flag() {
	local name="$1"
	local mtddev="$(find_mtd_part $2)"
	local offset="$3"
	local valid="$4"
	local setval="$5"

	if [ -z "$offset" ]; then
		echo "no $name flag offset provided"
		iodata_mstc_prepare_fail
	fi

	if [ -z "$mtddev" ]; then
		echo "cannot find \"$2\" mtd partition"
		iodata_mstc_prepare_fail
	fi

	local flag=$(iodata_mstc_rw_byte "$mtddev" "$offset")
	local _tmp
	for i in ${valid//,/ }; do
		if [ "$flag" = "$((i))" ]; then
			_tmp=$flag
			break
		fi
	done

	if [ -z "$_tmp" ]; then
		echo "failed to get valid $name flag, please check the value at $offset in $mtddev"
		iodata_mstc_prepare_fail
	fi
	echo "current: $name => $flag"

	if [ "$flag" != "$((setval))" ]; then
		if ! iodata_mstc_rw_byte "$mtddev" "$offset" "$setval"; then
			echo "failed to set \"$name\" flag"
			iodata_mstc_prepare_fail
		fi
		echo " --> set \"$name\" flag to $setval (valid: $valid)"
	fi
}
