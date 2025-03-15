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
	printf "\n\t-s ==> path to uboot flash script '</path/to/device_name>.scr'"
	printf "\n\t-f ==> path to firmware file: </path/to/firmware> )'"
	printf "\n\t-o ==> **Optional**, set the output file name 'Default: flash.its'\n\n"
	exit 1
}

REFERENCE_CHAR='-'
SCRIPTNUM=1
FIRMNUM=1

while getopts ":s:f:o:" OPTION
do
	case $OPTION in
		s ) SCRIPT=$OPTARG;;
		f ) FIRM=$OPTARG;;
		o ) OUTPUT=$OPTARG;;
		* ) echo "Invalid option passed to '$0' (options:$*)"
		usage;;
	esac
done

# Make sure user entered required parameter
if [ -z "${FIRM}" ]; then usage; fi

# use default if no output file name provided
if [ -z "${OUTPUT}" ]; then OUTPUT="flash.its"; fi

# create uboot script node
if [ -n "${SCRIPT}" ]; then
SCRIPT_NODE="
		script {
			description = \"GL.iNET UBOOT UPGRADE V2\";
			data = /incbin/(\"$(basename ${SCRIPT})\");
			type = \"script\";
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
	description = \"Flashing nand 800 20000\";
	#address-cells = <1>;

	images {
		${SCRIPT_NODE}
		ubi {
			description = \"$(basename ${FIRM})\";
			data = /incbin/(\"$(basename ${FIRM})\");
			type = \"firmware\";
			arch = \"arm\";
			os = \"linux\";
			compression = \"none\";
			hash${REFERENCE_CHAR}1 {
				algo = \"crc32\";
			};

		};

	};

};"

# Write .its file to disk
echo "$DATA" > "${OUTPUT}"
