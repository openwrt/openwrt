#!/bin/sh

[ -e /lib/firmware/$FIRMWARE ] && exit 0

. /lib/functions/caldata.sh

board=$(board_name)

case "$FIRMWARE" in
"ath10k/pre-cal-pci-0000:00:00.0.bin")
	case $board in
	tplink,tl-wpa8630p-v2-int|\
	tplink,tl-wpa8630p-v2.0-eu|\
	tplink,tl-wpa8630p-v2.1-eu)
		caldata_extract "art" 0x5000 0x2f20
		ath10k_patch_mac $(macaddr_add $(mtd_get_mac_binary info 0x8) 1)
		ln -sf /lib/firmware/ath10k/pre-cal-pci-0000\:00\:00.0.bin \
			/lib/firmware/ath10k/QCA9888/hw2.0/board.bin
		;;
	esac
	;;
*)
	exit 1
	;;
esac
