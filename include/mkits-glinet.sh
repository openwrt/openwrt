#!/bin/sh
#
# Licensed under the terms of the GNU GPL License version 2 or later.
#
# Author: Peter Tyser <ptyser@xes-inc.com>
#
# U-Boot firmware supports the booting of images in the Flattened Image
# Tree (FIT) format.  The FIT format uses a device tree structure to
# describe a kernel image, device tree blob, ramdisk, etc.  This script
# creates an Image Tree Source (.its file) which can be passed to the
# 'mkimage' utility to generate an Image Tree Blob (.itb file).  The .itb
# file can then be booted by U-Boot (or other bootloaders which support
# FIT images).  See doc/uImage.FIT/howto.txt in U-Boot source code for
# additional information on FIT images.
#
# ^ Original mkits.sh ^
################################################################################
#
# **This script is a Modified Version of mkits.sh**
# It has been moddified to produce legacy MultiFit format its files
# This script is used,in combination with the oem uboot "flash.scr",to enable uboot
# in Openwrt for some new devices.
#
# At this time, this script is intended for use with the GL.iNet GL-B3000 device only.
# If more devices that deploy this method are dicovered, this script can be easily updated
# to accomidate support for these additional devices.

usage() {
	printf "\nUsage: %s -s script -f firmware -d desc -o output" "$(basename "$0")"
	printf "\n\t-s ==> uboot flash script 'flash.scr'"
	printf "\n\t-f ==> set firmware description 'Defualt: <firmware name> )'"
	printf "\n\t-o ==> **Optional**, set output file name 'Default: flash.its'\n\n"
	printf "\n\t-d ==> **Optional**, set dts description field 'Default: Flashing nand 800 20000'\n\n"
	exit 1
}

REFERENCE_CHAR='-'
DTS_DESC_NODE=
SCRIPTNUM=1
FIRMNUM=1
NODENAME="ubi {"
NODETYPE="Firmware"
KADDR=
KENTRY=

while getopts ":s:f:o:d:" OPTION
do
	case $OPTION in
		s ) SCRIPT=$OPTARG;;
		f ) FIRM=$OPTARG;;
		o ) OUTPUT=$OPTARG;;
		d ) DESC=$OPTARG;;
		* ) echo "Invalid option passed to '$0' (options:$*)"
		usage;;
	esac
done

# Make sure user entered required parameter
if [ -z "${FIRM}" ];
then
	usage
fi

NODEDESC="\"$(basename ${FIRM})\""

# use default if no output name provided
if [ -z "${OUTPUT}" ];
then
	OUTPUT="flash.its"
fi

# conditionally, use dts description for main dts
# ** used to trigger uboot NAND mode **
if [ -n "${DESC}" ];
then
	addr=0x41080000
	DTS_DESC_NODE="description = \"GL.iNet GL-B3000 UBOOT MODE: [ NAND ]\";"
	NODEDESC="description =\"Flashing nand 800 20000\";"
	KADDR="load = <$addr>;"
	KENTRY="entry = <${addr}>;"
else
	DTS_DESC_NODE="description = \"GL.iNet GL-B3000 UBOOT MODE: [ UBI ]\";"
	NODEDESC="description = \"Flashing nand 800 20000\";"
fi

# custom uboot script node
if [ -n "${SCRIPT}" ]; then
SCRIPT_NODE="
		script {
			 description = \"GL.iNET uboot upgrade Ver 2.0\";
			 data = /incbin/(\"${SCRIPT}\");
			 type = \"Script\";
			 arch = \"arm\";
			 os = \"linux\";
			 compression = \"none\";

			 hash${REFERENCE_CHAR}1 {
				algo = \"crc32\";
			 };
		};
"
fi

# Create a default, fully populated DTS file
DATA="/dts-v1/;

/ {
	${DTS_DESC_NODE}
	#address-cells = <1>;

	images {
		${SCRIPT_NODE}
		ubi {
			 ${NODEDESC}
			 data = /incbin/(\"${FIRM}\");
			 type = \"Firmware\";
			 arch = \"arm\";
			 os = \"linux\";
			 compression = \"none\";
			 ${KADDR}
			 ${KENTRY}
			 hash${REFERENCE_CHAR}1 {
				algo = \"crc32\";
			 };

		};

	};

};"

# Write .its file to disk
echo "$DATA" > "${OUTPUT}"
