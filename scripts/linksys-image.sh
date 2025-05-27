#!/bin/sh
# SPDX-License-Identifier: GPL-2.0-or-later
#
# Copyright (C) 2024 OpenWrt.org
#
# This script creates a tar file for the Linksys switches of the LGS3xxC/LGS3xxMPC
# series. It contains not only the OpenWrt firmware but additional scripts that
# are needed for the upgrade.
#
# ./linksys-image.py <ImageFile> <ImageFileOut> <LinksysModel>
#
# Known values for LinksysModel are currently
#
# LGS310MPC		60402010
# LGS310C		60402060
# LGS328PC		60401070
# LGS328PC(RTL8218D)	60401080
# LGS310MPCv2		60402090
# LGS328MPC		60412020
# LGS328C		60412040
# LGS328MPCv2		60412060
# LGS352MPC		60422030
# LGS352C		60422050
# LGS352MPCv2		60422070

# The check script that verifies if the images matches the hardware model
gen_imagecheck() {
	echo '#!/bin/sh'
	echo 'if [ "$1" = "'${1}'" ]; then'
	echo 'echo 0'
	echo 'else'
	echo 'echo 1'
	echo 'fi'
}

# Generic attributes
gen_fwinfo() {
	echo 'FW_VERSION=1.01.100\nBOOT_VERSION=01.00.01'
}

# NOR upgrade script. It allows to install OpenWrt only to first partition.
gen_nor_upgrade() {
	echo '#!/bin/sh'
	echo 'flash_bank=65536'
	echo 'filesize=`stat --format=%s ./series_vmlinux.bix`'
	echo 'num_bank=`expr \( ${filesize} + ${flash_bank} - 1 \) / ${flash_bank}`'
	echo 'filesize_bank=`expr ${num_bank} \* ${flash_bank}`'
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

# NAND upgrade script. It allows to install OpenWrt only to first partition.
gen_nand_upgrade() {
	echo '#!/bin/sh'
	echo 'case $1 in'
	echo '1)'
	echo 'flash_eraseall $2 >/dev/null 2>&1'
	echo 'nandwrite -p $2 ./series_vmlinux.bix >/dev/null 2>&1'
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

gen_imagecheck $3 > $imgdir/iss_imagecheck.sh
gen_nor_upgrade > $imgdir/iss_imageupgrade.sh
gen_nand_upgrade > $imgdir/iss_nand_imageupgrade.sh
gen_fwinfo > $imgdir/firmware_information.txt

chmod +x $imgdir/iss_imagecheck.sh
chmod +x $imgdir/iss_imageupgrade.sh
chmod +x $imgdir/iss_nand_imageupgrade.sh

cp $1 $imgdir/series_vmlinux.bix

tar cf $2 -C $tmpdir image/

rm -rf $tmpdir
