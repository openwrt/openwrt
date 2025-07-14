PART_NAME=firmware

platform_machine() {
	grep "machine" /proc/cpuinfo | sed "s/.*:[ \t]*//" | sed "s/-.*//g"
}

platform_expected_image() {
	local machine=$(platform_machine)

	case "$machine" in
		# this is from dts
		"sf16a18")	echo "siwifi-1688a"; return;;
		"sf19a28")	echo "siwifi-1688a"; return;;
	esac
}

siwifixx_identify() {
	local magic

	magic=$(get_magic_long "$1")
	case "$magic" in
		"27051956")
			echo "a18"
			return
			;;
		"xxxxxxxx")
			echo "w18"
			return
			;;
	esac

	echo "Sysupgrade is unknown"
}

platform_check_image() {
	[ "$#" -gt 1 ] && return 1

	local file_type=$(siwifixx_identify "$1")
	local magic
	local error=0

	case "$file_type" in
		"a18")
			local dev_siwifi_id=$(platform_expected_image)
			echo "Found A18 image with device siwifi_id $dev_siwifi_id"

			if [ "$dev_siwifi_id" != "siwifi-1688a" ]; then
				echo "Invalid image type."
				error=1
			fi
		;;
		"w18")
			local dev_siwifi_id=$(platform_expected_image)

			if [ "$dev_siwifi_id" != "siwifi-xxxxx" ]; then
				echo "Invalid image type."
				error=1
			fi
		;;
		*)
			echo "Invalid image type. Please use only .trx files"
			error=1
		;;
	esac

	return $error
}

platform_do_upgrade() {
	local siwifi_id=$(platform_expected_image)
	local cmd=""

	case "$siwifi_id" in
		"siwifi-1688a")		cmd=$ARGV;;
		"siwifi-1688b")		cmd=$ARGV;;
	esac

	default_do_upgrade "$cmd"
}

disable_watchdog() {
	killall watchdog
	( ps | grep -v 'grep' | grep '/dev/watchdog' ) && {
	echo 'Could not disable watchdog'
	return 1
	}
}
