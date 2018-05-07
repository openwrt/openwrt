#!/usr/bin/env bash
#
# Licensed under the terms of the GNU GPL License version 2 or later.
#
# Author: Peter Tyser <ptyser@xes-inc.com>
# Editor: Johann Neuhauser <jneuhauser@dh-electronics.de>
#
# U-Boot firmware supports the booting of images in the Flattened Image
# Tree (FIT) format.  The FIT format uses a device tree structure to
# describe a kernel image, device tree blob, ramdisk, etc.  This script
# creates an Image Tree Source (.its file) which can be passed to the
# 'mkimage' utility to generate an Image Tree Blob (.itb file).  The .itb
# file can then be booted by U-Boot (or other bootloaders which support
# FIT images).  See doc/uImage.FIT/howto.txt and doc/uImage.FIT/multi.its 
# in U-Boot source code for additional information on FIT images.
#

usage() {
	echo "Usage: `basename $0` -A arch -C comp -a addr -e entry" \
		"-v version -k kernel [-D name] -p path -d dtbs -o its_file"
	echo -e "\t-A ==> set architecture to 'arch'"
	echo -e "\t-C ==> set compression type 'comp'"
	echo -e "\t-a ==> set load address to 'addr' (hex)"
	echo -e "\t-e ==> set entry point to 'entry' (hex)"
	echo -e "\t-v ==> set kernel version to 'version'"
	echo -e "\t-k ==> include kernel image 'kernel'"
	echo -e "\t-D ==> human friendly Device Tree Blob 'name'"
	echo -e "\t-p ==> path to Device Tree Blobs"
	echo -e "\t-d ==> include Device Tree Blobs 'dtbs'"
	echo -e "\t-o ==> create output file 'its_file'"
	exit 1
}

while getopts ":A:a:c:C:D:p:d:e:k:o:v:" OPTION
do
	case $OPTION in
		A ) ARCH=$OPTARG;;
		a ) LOAD_ADDR=$OPTARG;;
		C ) COMPRESS=$OPTARG;;
		D ) DEVICE=$OPTARG;;
		p ) PATH_DTB=$OPTARG;;
		d ) DTB=$OPTARG;;
		e ) ENTRY_ADDR=$OPTARG;;
		k ) KERNEL=$OPTARG;;
		o ) OUTPUT=$OPTARG;;
		v ) VERSION=$OPTARG;;
		* ) echo "Invalid option passed to '$0' (options:$@)"
		usage;;
	esac
done

# Make sure user entered all required parameters
if [ -z "${ARCH}" ] || [ -z "${COMPRESS}" ] || [ -z "${LOAD_ADDR}" ] || \
	[ -z "${ENTRY_ADDR}" ] || [ -z "${VERSION}" ] || [ -z "${KERNEL}" ] || \
	[ -z "${OUTPUT}" ] || [ -z "${DTB}" ] || [ -z "${PATH_DTB}" ]; then
	usage
fi

ARCH_UPPER=`echo $ARCH | tr '[:lower:]' '[:upper:]'`

# Conditionally create fdt information
i=1
for dtb in ${DTB}; do
	# Ensure not .dtb is appended
	dtb=${dtb%.dtb}
	FDT="${FDT}
		fdt@${i} {
			description = \"${ARCH_UPPER} OpenWrt ${DEVICE} ${dtb}\";
			data = /incbin/(\"${PATH_DTB}/${dtb}.dtb\");
			type = \"flat_dt\";
			arch = \"${ARCH}\";
			compression = \"none\";
			hash@1 {
				algo = \"crc32\";
			};
			hash@2 {
				algo = \"sha1\";
			};
		};
"
	let i=${i}+1
done

# Conditionally create config information
i=1
for dtb in ${DTB}; do
	# Truncate to name only
	dtb=${dtb%.dtb}
	dtb=${dtb##*/}
	CFG="${CFG}
		${dtb} {
			description = \"OpenWrt ${dtb}\";
			kernel = \"kernel@1\";
			fdt = \"fdt@${i}\";
		};
"
	let i=${i}+1
done

# Create a default, fully populated DTS file
DATA="/dts-v1/;

/ {
	description = \"${ARCH_UPPER} OpenWrt Multi FIT (Flattened Image Tree)\";
	#address-cells = <1>;

	images {
	
		kernel@1 {
			description = \"${ARCH_UPPER} OpenWrt Linux-${VERSION}\";
			data = /incbin/(\"${KERNEL}\");
			type = \"kernel\";
			arch = \"${ARCH}\";
			os = \"linux\";
			compression = \"${COMPRESS}\";
			load = <${LOAD_ADDR}>;
			entry = <${ENTRY_ADDR}>;
			hash@1 {
				algo = \"crc32\";
			};
			hash@2 {
				algo = \"sha1\";
			};
		};
${FDT}
	};

	configurations {
${CFG}
	};
};"

# Write .its file to disk
echo "$DATA" > ${OUTPUT}
