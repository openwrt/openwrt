#!/bin/sh

dir="/usr/share/bootloader"
bootinfo_emmc="${dir}/bootinfo_emmc.bin"
stage1="${dir}/FSBL.bin"
stage2="${dir}/u-boot.itb"
nor=
emmc=

die() {
	echo "$(basename "$0"): error: $*"
	exit 1
}

test -f "${bootinfo_emmc}" || die "missing file: ${bootinfo_emmc}"
test -f "${stage1}" || die "missing file: ${stage1}"
test -f "${stage2}" || die "missing file: ${stage2}"

usage() {
	printf "Usage: %s [-e] [-n]" "$(basename "$0")"

	printf "\n\t-e\tinstall first stage bootloader to eMMC"
	printf "\n\t-n\tinstall bootloader to NOR\n"
	exit 1
}

while getopts "en" OPTION
do
	case $OPTION in
		e) emmc=1;;
		n) nor=1;;
		*) echo "Invalid option passed to '$0' (options:$*)"
		usage;;
	esac
done

test -z "${emmc}" -a -z "${nor}" && usage

if test "${emmc}" = "1"; then
	if test -b "/dev/mmcblk0boot0"; then
		dev="mmcblk0boot0"
	elif test -b "/dev/mmcblk1boot0"; then
		dev="mmcblk1boot0"
	else
		die "no eMMC found"
	fi

	echo "installing first stage bootloader to eMMC ${dev}"
	echo 0 > /sys/block/${dev}/force_ro
	sleep 1
	dd if="${bootinfo_emmc}" of="/dev/${dev}" bs=512 conv=notrunc
	dd if="${stage1}" of="/dev/${dev}" bs=512 seek=1 conv=notrunc
	sync
	echo 1 > /sys/block/${dev}/force_ro
fi

check_mtdpart() {
	test -c /dev/mtd${1} || die "/dev/mtd${1} is not a MTD device"
	name=$(cat "/sys/class/mtd/mtd${1}/name")
	test "$name" = "$2" || die "/dev/mtd${1} name mismatch: \"${name}\", expected \"${2}\""
}

if test "${nor}" = "1"; then
	check_mtdpart 2 "fsbl"
	check_mtdpart 3 "env"
	check_mtdpart 4 "opensbi"
	check_mtdpart 5 "uboot"
	echo "installing bootloader to NOR"
	mtd write "${stage1}" /dev/mtd2
	mtd erase /dev/mtd3
	mtd erase /dev/mtd4
	mtd write "${stage2}" /dev/mtd5
fi
