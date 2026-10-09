// SPDX-License-Identifier: GPL-2.0-or-later

. /lib/functions.sh
#Note: this code also uses some functions from nand.sh, but it is expected to be run by nand.sh, so we are not
#sourcing it explicitly here

tplink_er605_v2_prepare_volume_image_with_header() {
	local content=$1
	local output="/tmp/tp-link-$2"
	local magic=$(echo -ne \\xaa\\x55\\xd9\\x8f\\x04\\xe9\\x55\\xaa)
	# Extra byte for trailing zero
	local length=$(($(echo -n "$content" | wc -c) + 1))
	local length_bin=$(printf '\\x%02x\\x%02x\\x%02x\\x%02x' $((length >> 24 & 255)) $((length >> 16 & 255)) $((length >> 8 & 255)) $((length & 255)))
	# The format is: magic,data-length,4 zero bytes,data with trailing zero
	echo -ne "${magic}${length_bin}\x00\x00\x00\x00${content}\x00" > $output
	echo $output
}

tplink_er605_v2_update_extra_para_volume_if_needed() {
	local volume=$1
	local content=$2
	local ubidev=$( nand_find_ubi "$CI_UBIPART" )
	local ubivol="$( nand_find_volume $ubidev $volume )"
	if [ ! "$ubivol" ]; then
		echo "Cannot find ubifs volume $volume"
		return 1
	fi
	if (cat /dev/$ubivol | grep -q OpenWrt) 2>/dev/nul; then
		echo "Volume $volume doesn't need updating"
		return 0
	fi
	content_file=$(tplink_er605_v2_prepare_volume_image_with_header "$content" $volume)
	ubiupdatevol /dev/$ubivol $content_file
	rm -f $content_file
}

tplink_er605_v2_update_extra_para() {
	content=$(cat <<'EOF'
	{
		"dbootFlag": "1",
		"integerFlag": "1",
		"fwFlag": "GOOD",
		"score": 1,
		"OpenWrt": 1
	}
EOF
				 )
	tplink_er605_v2_update_extra_para_volume_if_needed "extra-para" "$content"

	content_b=$(cat <<'EOF'
	{
		"dbootFlag": "1",
		"integerFlag": "1",
		"fwFlag": "BAD",
		"score": 1,
		"OpenWrt": 1
	}
EOF
					 )
	tplink_er605_v2_update_extra_para_volume_if_needed "extra-para.b" "$content_b"
}

