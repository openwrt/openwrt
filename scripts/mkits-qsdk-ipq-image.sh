#!/usr/bin/env bash
#
# Licensed under the terms of the GNU GPL License version 2 or later.
# Author: Piotr Dymacz <pepe2k@gmail.com>, based on mkits.sh.
#
# Qualcomm SDK (QSDK) sysupgrade compatible images for IPQ40xx, IPQ806x
# and IPQ807x use FIT format together with 'dumpimage' tool from U-Boot
# for verifying and extracting them. Based on 'images' sections names,
# corresponding mtd partitions are flashed.
# This is a simple script for generating FIT images tree source files,
# compatible with the QSDK sysupgrade format. Resulting images can be
# used for initial (factory -> OpenWrt) installation and would work
# both in CLI and GUI. The script is also universal in a way it allows
# to include as many sections as needed.
#

usage() {
	echo "Usage: `basename $0` output [[device].bootscript] img0_name img0_file [[img1_name img1_file] ...]"
	exit 1
}

# We need at least 3 arguments
[ "$#" -lt 3 ] && usage || node_type="firmware"

# Target output file
OUTPUT="$1"; shift

# check for bootscript
[ "${1##*.}" = "bootscript" ] && has_script=true && node_type="script"

# Create a default, fully populated DTS file
echo "\
/dts-v1/;

/ {
	description = \"OpenWrt factory image\";
	#address-cells = <1>;

	images {" > ${OUTPUT}

while [ -n "$1" -a -n "$2" ] || [ $has_script ]; do
	[ -f "$2" ] || [ $has_script ] && has_script= || usage

	case "$node_type" in
	script)
		name="$node_type"
		file="$1"; shift
		desc="${file%.*} uboot ${file##*.}"
		type="$node_type"
		node_type="firmware"
	;;
	firmware)
		name="$1"; shift
		file="$1"; shift
		desc="$name"
		type="$node_type"
	;;
	esac

	echo \
"		${name} {
			description = \"${desc}\";
			data = /incbin/(\"${file}\");
			type = \"${type}\";
			arch = \"ARM\";
			compression = \"none\";
			hash@1 {
				algo = \"crc32\";
			};
		};" >> ${OUTPUT}
done

echo \
"	};
};" >> ${OUTPUT}
