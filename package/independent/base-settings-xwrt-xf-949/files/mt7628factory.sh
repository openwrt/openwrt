#!/bin/sh

. /lib/functions.sh
. /lib/functions/uci-defaults.sh
. /lib/functions/system.sh

gen_mac_sn_ft()
{
	local mac1 mac2 sn

	#sn=100249ar9b38ccdf78d38dc3857ed16c
	#mac1=7ca7b05fd778

	mac1=$1
	sn=$2
	mac2=${mac1:0:2}:${mac1:2:2}:${mac1:4:2}:${mac1:6:2}:${mac1:8:2}:${mac1:10:2}
	mac2=$(macaddr_add "$mac2" 1)
	mac2=${mac2:0:2}${mac2:3:2}${mac2:6:2}${mac2:9:2}${mac2:12:2}${mac2:15:2}

	dd if=/tmp/ft.img bs=1 count=$((0x4)) 2>/dev/null

	#mac1
	echo -ne "\x${mac1:0:2}\x${mac1:2:2}\x${mac1:4:2}\x${mac1:6:2}\x${mac1:8:2}\x${mac1:10:2}"

	dd if=/tmp/ft.img bs=1 skip=$((0x4+6)) count=$((0x28-0x4-6)) 2>/dev/null

	#mac2
	echo -ne "\x${mac2:0:2}\x${mac2:2:2}\x${mac2:4:2}\x${mac2:6:2}\x${mac2:8:2}\x${mac2:10:2}"

	dd if=/tmp/ft.img bs=1 skip=$((0x28+6)) count=$((0x1000-0x28-6)) 2>/dev/null
	dd if=/tmp/ft.img bs=$((0x1000)) skip=1 count=$((0xf-1)) 2>/dev/null
	dd if=/tmp/ft.img bs=1 skip=$((0xf000)) count=$((0xff00-0xf000)) 2>/dev/null

	#sn
	echo -n "$sn"

	dd if=/tmp/ft.img bs=1 skip=$((0xff00+32)) count=$((0x10000-0xff00-32)) 2>/dev/null
}

#gen_mac_sn_ft 7ca7b05fd778 100249ar9b38ccdf78d38dc3857ed16c

case "$1" in
	set_mac_sn)
		mac="$2"
		sn="$3"
		test -n "$mac" && test -n "$sn" && \
		[ "$(echo -n "$mac" | wc -c)" = "12" ] && \
		[ "$(echo -n "$sn" | wc -c)" = "32" ] && {
			sncheck -m X-WRT -a $mac -s $sn || {
				echo invalid sn/mac
				exit 1
			}
			cat /dev/mtd2 >/tmp/ft.img && \
			gen_mac_sn_ft $mac $sn >/tmp/ft.img.new && \
			mtd write /tmp/ft.img.new factory && \
			rm -f /tmp/ft.img /tmp/ft.img.new && \
			echo ok
			exit $?
		}
		echo invalid
		exit 1
	;;
	get_mac)
		hexdump -v -n 6 -s $((0x4)) -e '6/1 "%02x"' /dev/mtd2 2>/dev/null
	;;
	get_sn)
		hexdump -v -n 32 -s $((0xff00)) -e '"%c"' /dev/mtd2 2>/dev/null
	;;
	check)
		mac="$(hexdump -v -n 6 -s $((0x4)) -e '6/1 "%02x"' /dev/mtd2 2>/dev/null)"
		sn="$(hexdump -v -n 32 -s $((0xff00)) -e '"%c"' /dev/mtd2 2>/dev/null)"
		sncheck -m X-WRT -a $mac -s $sn
	;;
	*)
		echo "usage:"
		echo "    $0 set_mac_sn <mac> <sn>"
		echo "    $0 get_mac"
		echo "    $0 get_sn"
		echo "    $0 check"
	;;
esac
