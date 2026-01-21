REQUIRE_IMAGE_METADATA=1
MTDSYSFS=/sys/class/mtd

gemini_do_platform_upgrade() {
	ESZ=`cat ${MTDSYSFS}/mtd1/erasesize`
	if test ${ESZ} == 131072 ; then
		echo "MTD1 has 128kb EB size..."
	else
		echo "MTD1 has wrong EB size!"
	fi
	NAME=`cat ${MTDSYSFS}/mtd1/name`
	SZ=`cat ${MTDSYSFS}/mtd1/size`
	KSZ=$(($ESZ * $2))
	if test "x${NAME}" == "xKern" ; then
		if test ${SZ} == ${KSZ} ; then
			echo "MTD1 OK..."
		else
			echo "MTD1 is wrong size, aborting" >&2
			exit 1
		fi
	else
		echo "MTD1 has wrong name, aborting" >&2
		exit 1
	fi
	NAME=`cat ${MTDSYSFS}/mtd2/name`
	SZ=`cat ${MTDSYSFS}/mtd2/size`
	RSZ=$(($ESZ * $3))
	if test "x${NAME}" == "xRamdisk" ; then
		if test ${SZ} == ${RSZ} ; then
			echo "MTD2 OK..."
		else
			echo "MTD2 is wrong size, aborting" >&2
			exit 1
		fi
	else
		echo "MTD2 has wrong name, aborting" >&2
		exit 1
	fi
	NAME=`cat ${MTDSYSFS}/mtd3/name`
	SZ=`cat ${MTDSYSFS}/mtd3/size`
	ASZ=$(($ESZ * $4))
	if test "x${NAME}" == "xApplication" ; then
		if test ${SZ} == ${ASZ} ; then
			echo "MTD3 OK..."
		else
			echo "MTD3 is wrong size, aborting" >&2
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
