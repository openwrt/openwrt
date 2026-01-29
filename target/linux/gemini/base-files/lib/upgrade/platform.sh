REQUIRE_IMAGE_METADATA=1
MTDSYSFS=/sys/class/mtd

gemini_check_redboot_parts() {
	MTD1OF=`cat ${MTDSYSFS}/mtd1/offset`
	MTD2OF=`cat ${MTDSYSFS}/mtd2/offset`
	MTD3OF=`cat ${MTDSYSFS}/mtd3/offset`
	MTD4OF=`cat ${MTDSYSFS}/mtd4/offset`
	MTD1SZ=$((${MTD2OF} - ${MTD1OF}))
	MTD2SZ=$((${MTD3OF} - ${MTD2OF}))
	MTD3SZ=$((${MTD4OF} - ${MTD3OF}))
	ESZ=`cat ${MTDSYSFS}/mtd1/erasesize`
	if test ${ESZ} == 131072 ; then
		echo "MTD has 128kb EB size"
	else
		echo "MTD has wrong EB size!"
	fi
	KSZ=$(($ESZ * $2))
	RSZ=$(($ESZ * $3))
	ASZ=$(($ESZ * $4))
	NAME=`cat ${MTDSYSFS}/mtd1/name`
	if test "x${NAME}" == "xKern" ; then
		if test ${MTD1SZ} == ${KSZ} ; then
			echo "MTD1 Kern ${MTD1SZ} OK..."
		else
			echo "MTD1 is wrong size, aborting" >&2
			exit 1
		fi
	else
		echo "MTD1 has wrong name, aborting" >&2
		exit 1
	fi
	NAME=`cat ${MTDSYSFS}/mtd2/name`
	if test "x${NAME}" == "xRamdisk" ; then
		if test ${MTD2SZ} == ${RSZ} ; then
			echo "MTD2 Ramdisk ${MTD2SZ} OK..."
		else
			echo "MTD2 is at wrong offset, aborting" >&2
			exit 1
		fi
	else
		echo "MTD2 has wrong name, aborting" >&2
		exit 1
	fi
	NAME=`cat ${MTDSYSFS}/mtd3/name`
	if test "x${NAME}" == "xApplication" ; then
		if test ${MTD3SZ} == ${ASZ} ; then
			echo "MTD3 Application ${MTD3SZ} OK..."
		else
			echo "MTD3 is at wrong offset, aborting" >&2
			exit 1
		fi
	else
		echo "MTD3 has wrong name, aborting" >&2
		exit 1
	fi
}

gemini_do_platform_upgrade() {
	echo "Extract the three firmware parts"
	echo 3 > /proc/sys/vm/drop_caches
	echo "COMMENCING UPGRADE. BE PATIENT, THIS IS NOT FAST!"
	KFSZ=$(tar xfz "$1" zImage -O | wc -c)
	echo "Upgrade Kern partition (kernel part 1, size ${KFSZ})"
	tar xfz "$1" zImage -O | mtd write - Kern
	[ $? -ne 0 ] && exit 1
	RFSZ=$(tar xfz "$1" rd.gz -O | wc -c)
	echo "Upgrade Ramdisk partition (kernel part 2, size ${RFSZ})"
	tar xfz "$1" rd.gz -O | mtd write - Ramdisk
	[ $? -ne 0 ] && exit 1
	AFSZ=$(tar xfz "$1" hddapp.tgz -O | wc -c)
	echo "Upgrade Application partition (rootfs, size ${AFSZ})"
	tar xfz "$1" hddapp.tgz -O | mtd write - Application
	[ $? -ne 0 ] && exit 1
}

platform_check_image() {
	local board=$(board_name)

	case "$board" in
	dlink,dir-685)
		return 0
		;;
	raidsonic,ib-4220-b|\
	itian,sq201|\
	storlink,gemini324)
		return 0
		;;
	esac

	echo "Sysupgrade is not yet supported on $board."
	return 1
}

platform_do_upgrade() {
	local board=$(board_name)

	case "$board" in
	dlink,dir-685)
		PART_NAME=firmware
		default_do_upgrade "$1"
		;;
	itian,sq201|\
	storlink,gemini324)
		gemini_check_redboot_parts "$1" 16 48 48
		gemini_do_platform_upgrade "$1"
		;;
	raidsonic,ib-4220-b)
		gemini_check_redboot_parts "$1" 24 48 48
		gemini_do_platform_upgrade "$1"
		;;
	esac
}
