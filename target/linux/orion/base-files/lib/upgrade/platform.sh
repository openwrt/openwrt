# use default "image" for PART_NAME
# use default for platform_do_upgrade()

platform_check_image() {
	[ "${ARGC}" -gt 1 ] && { echo 'Too many arguments. Only flash file expected.'; return 1; }

	local hardware=`sed -n /Hardware/s/.*:.//p /proc/cpuinfo`
	local magic="$(get_magic_word "$1")"

	case "${hardware}" in
	 # hardware with padded uImage + padded rootfs
	 'Netgear WNR854T' | 'Linksys WRT350N v2')
		[ "${magic}" != '2705' ] && {
			echo "Invalid image type ${magic}."
			return 1
		}
		return 0
		;;
	esac

	echo "Sysupgrade is not yet supported on ${hardware}."
	return 1
}
