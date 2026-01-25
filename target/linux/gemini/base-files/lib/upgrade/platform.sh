REQUIRE_IMAGE_METADATA=1
MTDSYSFS=/sys/class/mtd

gemini_do_platform_upgrade() {
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
	echo "Extract the three firmware parts"
	tar xvfz "$1"; rm "$1"
	sync
	echo 3 > /proc/sys/vm/drop_caches
	echo "COMMENCING UPGRADE. BE PATIENT, THIS IS NOT FAST!"
	echo "Upgrade Kern partition (kernel part 1, $2 erase blocks)"
	mtd write zImage Kern
	[ $? -ne 0 ] && exit 1
	echo "Upgrade Ramdisk partition (kernel part 2, $3 erase blocks)"
	mtd write rd.gz Ramdisk
	[ $? -ne 0 ] && exit 1
	echo "Upgrade Application partition (rootfs, $4 erase blocks)"
	mtd write hddapp.tgz Application
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
		gemini_do_platform_upgrade "$1" 16 48 48
		;;
	raidsonic,ib-4220-b)
		gemini_do_platform_upgrade "$1" 24 48 48
		;;
	esac
}
