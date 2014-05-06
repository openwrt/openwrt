#!/bin/sh
. /lib/functions.sh

FW="/tmp/Firmware_Speedport_W921V_1.20.000.bin"
URL="http://hilfe.telekom.de/dlp/eki/downloads/Speedport/Speedport%20W%20921V/Firmware_Speedport_W921V_1.20.000.bin"
FW_TAPI="vr9_tapi_fw.bin"
FW_DSL="vr9_dsl_fw_annex_b.bin"
MD5_FW="4d812f2c3476dadd738b022c4767c491"
MD5_TAPI="06b6ab3481b8d3eb7e8bf6131f7f6b7f"
MD5_DSL="59dd9dc81195c6854433c691b163f757"

[ -f /lib/firmware/vdsl.bin] && exit 0

[ -z "$1" ] || URL=$1

[ -f "${FW}" ] || {
	echo "${FW} does not exist. Try to Download it ? (y/N)"
	read -n 1 R
	echo ""
	[ "$R" = "y" ] || {
		echo "Please manually download the firmware from ${URL} and copy the file to ${FW}"
		exit 1
	}
	echo "Download w921v Firmware"
	wget "${URL}" -O "${FW}"
	[ $? -eq 0 -a -f "${FW}" ] || exit 1
}

F=`md5sum -b ${FW} | cut -d" " -f1`
[ "$F" = "${MD5_FW}" ] || {
	echo "Failed to verify Firmware MD5"
	exit 1
}

cd /tmp
echo "Unpack and decompress w921v Firmware"

w921v_fw_cutter
[ $? -eq 0 ] || exit 1

T=`md5sum -b ${FW_TAPI} | cut -d" " -f1`
D=`md5sum -b ${FW_DSL} | cut -d" " -f1`

[ "$T" = "${MD5_TAPI}" -a "$D" = "${MD5_DSL}" ] || {
	echo "Failed to verify MD5"
	exit 1
}

MTD=$(find_mtd_index dsl_fw)
if [ "$MTD" -gt 0 -a -e "/dev/mtd$MTD" ]; then
	echo "Storing firmware in flash"
	tar cvz ${FW_TAPI} ${FW_DSL} > "/dev/mtd$MTD"
	/etc/init.d/dsl_fs boot
else
	cp ${FW_TAPI} ${FW_DSL} /lib/firmware/
	ln -s /lib/firmware/vr9_dsl_fw_annex_b.bin /lib/firmware/vdsl.bin
fi
