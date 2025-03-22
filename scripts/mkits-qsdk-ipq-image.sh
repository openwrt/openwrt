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

node_type="firmware"
has_script=

usage() {

	echo ""; echo "** QSDK format (Default) ** "
	echo "Usage: `basename $0` output img0_name img0_file [[img1_name img1_file] ...]"
	echo ""; echo "** QSDK format with booscript (UBOOT_SCRIPT) **"
	echo "Usage: `basename $0` output_name img_name img_file [ device_name ].bootscript"
	exit 1
}

# We need at least 3 arguments
[ "$#" -lt 3 ] && usage

# if $# === 4 check for valid bootscript
if [ "$#" -eq 4 ] && [ "${4##*.}" = "bootscript" ]; then
	has_script=true && node_type="script"
elif [ "$#" -eq 4 ] && [ "${4##*.}" != "bootscript" ]; then
	usage
fi

# Target output file
OUTPUT="$1"; shift

# Create a default, fully populated DTS file
echo "\
/dts-v1/;

/ {
	description = \"OpenWrt factory image\";
	#address-cells = <1>;

	images {" > ${OUTPUT}

while [ -n "$1" -a -n "$2" ] || [ $has_script ]; do
	[ -f "$2" ] || [ $has_script ] || usage

	case "$node_type" in
		script)
			name="$node_type"
			file="$3"
			desc="${3%.*} uboot ${3##*.}"
			type="$node_type"
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

	if [ "$node_type" = "script" ]; then
		node_type="firmware"
		has_script=
	fi
done

echo \
"	};
};" >> ${OUTPUT}
