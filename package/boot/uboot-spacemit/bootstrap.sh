#!/bin/sh -e

# bootstrap SpacemiT K1 boards
# * hold the FDL (Firmware Download) button while powering on
# * run this script without arguments to upload u-boot and drop to its prompt
# * pass the filename of a FIT (*.itb) image to boot it:
#   ./u-boot-spacemit_k1/bootstrap.sh openwrt-spacemit-k1-generic-initramfs.itb

dir="$(dirname "$(readlink -f "$0")")"
stage1="${dir}/FSBL.bin"
stage2="${dir}/u-boot.itb"
stage3="${1}"

die() {
	echo "$(basename "$0"): error: $*"
	exit 1
}

which fastboot 2>/dev/null 1>&2 || die "the fastboot tool is required"
test -f "${stage1}" || die "missing file: ${stage1}"
test -f "${stage2}" || die "missing file: ${stage2}"
test -n "${stage3}" -a ! -f "${stage3}" && die "not a file: ${stage3}"

fastboot getvar version-brom
echo "uploading stage1..."
fastboot stage "${stage1}"
fastboot continue
sleep 1

echo "uploading stage2..."
fastboot stage "${stage2}"
fastboot continue
sleep 1

if test -z "${stage3}"; then
	echo "dropping to prompt..."
	fastboot oem env:set bootdelay:-1
	fastboot continue
	exit 0
fi

echo "uploading stage3..."
fastboot oem env:set bootargs:earlycon=sbi console=ttyS0,115200 root=/dev/ram
fastboot stage "${stage3}"
fastboot oem env:set bootdelay:0
fastboot continue
