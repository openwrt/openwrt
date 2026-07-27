#!/usr/bin/env bash
#
# SPDX-License-Identifier: GPL-2.0-or-later
#
# Based on OpenWrt scripts/mkits.sh (Author: Peter Tyser <ptyser@xes-inc.com>).
# Copyright (C) 2026 LLC NAGTECH
#
# Build a FIT image .its for the MediaTek CONFIG_MTK_DUAL_BOOT bootloader.
# In addition to the usual kernel(+fdt) FIT, it embeds a top-level /rootfs node
# holding the squashfs size and crc32/sha1 hashes. The OEM U-Boot verifies this
# node at every boot (CONFIG_MTK_DUAL_BOOT_IMAGE_ROOTFS_VERIFY); without it the
# loader rejects the image and falls back to the other boot slot.

usage() {
	echo "Usage: $(basename "$0") -A arch -C comp -a addr -e entry -v version" \
	     "-k kernel [-D name -d dtb] -c config -o its_file [-R rootfs]"
	exit 1
}

while getopts "A:a:C:c:D:d:e:k:o:v:R:" OPTION; do
	case $OPTION in
		A ) ARCH=$OPTARG;;
		a ) LOAD_ADDR=$OPTARG;;
		C ) COMPRESS=$OPTARG;;
		c ) CONFIG=$OPTARG;;
		D ) DEVICE=$OPTARG;;
		d ) DTB=$OPTARG;;
		e ) ENTRY_ADDR=$OPTARG;;
		k ) KERNEL=$OPTARG;;
		o ) OUTPUT=$OPTARG;;
		v ) VERSION=$OPTARG;;
		R ) ROOTFS_FILE=$OPTARG;;
		* ) usage;;
	esac
done

if [ -z "${ARCH}" ] || [ -z "${COMPRESS}" ] || [ -z "${LOAD_ADDR}" ] || \
   [ -z "${ENTRY_ADDR}" ] || [ -z "${VERSION}" ] || [ -z "${KERNEL}" ] || \
   [ -z "${OUTPUT}" ] || [ -z "${CONFIG}" ]; then
	usage
fi

ARCH_UPPER=$(echo "${ARCH}" | tr '[:lower:]' '[:upper:]')

# Optional FDT image + configuration reference
if [ -n "${DTB}" ]; then
	FDT="
			fdt@1 {
				description = \"${ARCH_UPPER} OpenWrt ${DEVICE} device tree blob\";
				data = /incbin/(\"${DTB}\");
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
	FDT_PROP="fdt = \"fdt@1\";"
fi

# Optional top-level /rootfs node with squashfs size + hashes (MediaTek dual-boot)
if [ -f "${ROOTFS_FILE}" ]; then
	ROOTFS_SIZE=$(stat -c %s "${ROOTFS_FILE}")
	ROOTFS_CRC32=$(python3 -c "import sys,zlib;print('%08x'%(zlib.crc32(open(sys.argv[1],'rb').read())&0xffffffff))" "${ROOTFS_FILE}")
	ROOTFS_SHA1=$(sha1sum "${ROOTFS_FILE}" | awk '{print "<0x"substr($0,1,8)" 0x"substr($0,9,8)" 0x"substr($0,17,8)" 0x"substr($0,25,8)" 0x"substr($0,33,8)">"}')

	ROOTFS="
	rootfs {
		size = <${ROOTFS_SIZE}>;

		hash@1 {
			value = <0x${ROOTFS_CRC32}>;
			algo = \"crc32\";
		};

		hash@2 {
			value = ${ROOTFS_SHA1};
			algo = \"sha1\";
		};
	};
"
fi

DATA="/dts-v1/;

/ {
	description = \"${ARCH_UPPER} OpenWrt FIT (Flattened Image Tree)\";
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
${ROOTFS}
	configurations {
		default = \"${CONFIG}\";
		${CONFIG} {
			description = \"OpenWrt\";
			kernel = \"kernel@1\";
			${FDT_PROP}
		};
	};
};"

echo "$DATA" > "${OUTPUT}"
