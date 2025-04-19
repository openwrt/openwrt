. /lib/upgrade/common.sh

glinet_do_fit_upgrade() {
	echo -n "fit: Extract [ FIT IMAGE ] -x-x-> [ ubi.bin ] ... "
	local ubi=/tmp/ubi.bin
	local part=$(dumpimage -l "$1" | grep -o "Image [0-9] (ubi)" | cut -f2 -d" ")

	local ubibin=$( dumpimage -T flat_dt -p ${part} -o "$ubi"  "$1" )
	if [ -s "$ubi" ]; then
		echo "[ OK ]"
		local ubiMd5=$(md5sum "$ubi" | cut -f1 -d" ")
		echo -n "fit-copy: [ ubi.bin ] -c-c-> [ firmware.bin ] ... "
		mv "$ubi" "$1"
		echo "[ OK ]"
		local firmMd5=$(md5sum "$1" | cut -f1 -d" ")
		echo -n "fit-copy: MD5 CHECK: "
		if [ "$ubiMd5" = "$firmMd5" ]; then
			echo "[ OK ]"
			echo "$ubiMd5 <=> $firmMd5"
			echo "fit: Successfully Extracted UBI from FIT IMAGE"
			echo "fit: Proceeding with sysupgrade .."
			nand_do_upgrade "$1"
			return
		fi
		echo "[ FAILED ] !!"
		echo "ERROR: Failed to Copy UBI into firmware.bin !!"
		echo "fit: Terminating sysupgrade .."
		exit 1
	fi
	echo "[ FAILED ] !!"
	echo "ERROR: Failed to Extract UBI from FIT IMAGE !!"
	echo "fit: Terminating sysupgrade .."
	exit 1
}

glinet_do_upgrade() {
	CI_UBIPART="rootfs"
	echo -n "Validating GL.iNet Image ... "
	case "$(identify_magic_long $(get_magic_long "$1"))" in
	fit)
		echo "[ OK ]"
		echo "fit-main: Firmware is Valid: fit"
		echo "fit-main: Upgrading Firmware via [ FIT IMAGE ]"
		glinet_do_fit_upgrade "$1"
		;;
	*)
		echo "[ FAILED ] !!"
		echo "main: GL.iNet Image Validation Failed !!"
		echo "main: Attempting default sysupgrade"
		nand_do_upgrade "$1"
		;;
	esac
}
