#!/bin/sh
# SPDX-License-Identifier: GPL-2.0-or-later
#
# Copyright (C) 2024 OpenWrt.org
#
# This script creates a tar file for the Linksys switches of the LGS3xxC/LGS3xxMPC
# series. It contains not only the OpenWrt firmware but additional scripts that
# are needed for the upgrade.
#
# ./linksys-image.py <ImageFile> <ImageFileOut>

# The check script that verifies if the images matches the hardware model
gen_imagecheck() {
	echo '#!/bin/sh'
	echo 'supperflag=`cat supportlist.txt | grep -i $1 | awk '"'"'{print$2}'"'"'`'
	echo 'if [ "${supperflag}" != "" ]; then'
	echo 'echo 0'
	echo 'else'
	echo 'echo 1'
	echo 'fi'
}

# A file with a list of supported devices. Might contain multiple lines.
gen_supportlist() {
	echo 'LGS310C MagicNumber 0x60402060 8380_2fib_1g 3955,1000,201,3,10,1'
}

# Generic attributes
gen_fwinfo() {
	echo 'FW_VERSION=1.01.100\nBOOT_VERSION=01.00.01'
}

# The central upgrade script
gen_imageupgrade() {
	echo '#!/bin/sh'
	echo 'flash_bank=65536'
	echo 'filesize=`ls -la ./series_vmlinux.bix | awk '"'"'{print$5}'"'"'`'
	echo 'abc=`expr ${filesize} / ${flash_bank} + 1`'
	echo 'num_bank=${abc}'
	echo 'abc=`expr ${num_bank} \* ${flash_bank}`'
	echo 'filesize_bank=${abc}'
	echo 'case $1 in'
	echo '1)'
	echo 'mtd_debug erase $2 0 ${filesize_bank} >/dev/null 2>&1'
	echo 'mtd_debug write $2 0 ${filesize} ./series_vmlinux.bix >/dev/null 2>&1'
	echo 'mtd_debug read $2 0 100 image1.img >/dev/null 2>&1'
	echo 'CreateImage -r ./image1.img > /tmp/app/image1.txt'
	echo 'echo 0'
	echo ';;'
	echo '*)'
	echo 'echo 1'
	echo 'esac'
}

tmpdir="$( mktemp -d 2> /dev/null )"
imgdir=$tmpdir/image
mkdir $imgdir

gen_imagecheck > $imgdir/iss_imagecheck.sh
gen_imageupgrade > $imgdir/iss_imageupgrade.sh
gen_supportlist > $imgdir/supportlist.txt
gen_fwinfo > $imgdir/firmware_information.txt

chmod +x $imgdir/iss_imagecheck.sh
chmod +x $imgdir/iss_imageupgrade.sh

cp $1 $imgdir/series_vmlinux.bix

tar cf $2 -C $tmpdir image/

rm -rf $tmpdir
