#!/bin/sh
# Copyright (C) 2021 OpenWrt.org
#

. /lib/functions.sh
. /lib/functions/system.sh

patch_aerohive_hiveap_370_uboot() {
	tmpd=/tmp/uboot_patch;
	mkdir -p ${tmpd}

	local uboot1=$(find_mtd_index Uboot);
	local uboot2=$(find_mtd_index Uboot-backup);

	for part in Uboot Uboot-backup; do
		local index=$(find_mtd_index ${part});
		[ ! "$index" ] && { echo "No such index for part ${part}; panicking!" >&2 ; exit 1; };

		# Back up
		dd if=/dev/mtd${index} of=${tmpd}/mtd${index};

		# Patch
		cp ${tmpd}/mtd${index} ${tmpd}/mtd${index}_patched;
		strings -td < ${tmpd}/mtd${index} | grep '^ *[0-9]* *\(run owrt_boot\|setenv bootargs\).*nand read' |
			awk '{print $1}' |
			while read offset; do
				echo -n "run owrt_boot_a;          " | dd of=${tmpd}/mtd${index}_patched bs=1 seek=${offset} conv=notrunc
			done;

		# Write
		insmod mtd_rw i_want_a_brick=y
		mtd erase /dev/mtd${index}
		mtd write ${tmpd}/mtd${index}_patched /dev/mtd${index}

		# Verify
		cmp /dev/mtd${index} ${tmpd}/mtd${index}_patched || {
			echo "Warning: written media differs from write source!" >&2;
			return 2;
		}
	done
}
