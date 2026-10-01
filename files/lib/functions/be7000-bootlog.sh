#!/bin/sh
# Shared by /lib/preinit/02_be7000_bootlog and /etc/init.d/be7000-bootlog:
# what goes into the boot log written to the crash_syslog partition.

be7000_bootlog_state() {
	local i
	echo "=== release ==="
	cat /etc/be7000-release 2>/dev/null
	echo "=== flags ==="
	fw_printenv 2>/dev/null | grep -E '^flag_|^fdt|^soc_|^flash_'
	echo "=== mtd/ubi ==="
	cat /proc/mtd 2>/dev/null
	ubinfo -a 2>/dev/null | grep -E '^ubi|Name|Size|Volume ID'
	echo "=== mounts ==="
	grep -E ' /overlay | / |/tmp/|ubifs|squashfs' /proc/mounts 2>/dev/null
	df -h 2>/dev/null | grep -E 'overlay|rootfs|tmpfs|/$'
	echo "=== link ==="
	for i in wan lan1 lan2 lan3; do
		[ -e /sys/class/net/$i ] || continue
		echo "--- $i: $(cat /sys/class/net/$i/operstate 2>/dev/null) carrier=$(cat /sys/class/net/$i/carrier 2>/dev/null) speed=$(cat /sys/class/net/$i/speed 2>/dev/null) duplex=$(cat /sys/class/net/$i/duplex 2>/dev/null)"
		if [ -x /usr/sbin/ethtool ]; then
			ethtool $i 2>/dev/null | grep -E 'Speed|Duplex|Auto-negotiation|Link detected|Advertised link modes|Link partner advertised link modes' -A0
			ethtool -a $i 2>/dev/null | grep -E 'RX|TX'
			ethtool -S $i 2>/dev/null | grep -vE ': 0$' | head -60
		fi
	done
	echo "=== bridge ==="
	ls /sys/class/net/br-lan/brif/ 2>/dev/null | tr '\n' ' '; echo
	cat /proc/net/dev 2>/dev/null
	echo "=== interrupts ==="
	cat /proc/interrupts 2>/dev/null
	echo "=== ip addr ==="
	ip addr 2>&1
	echo "=== ip route ==="
	ip route 2>&1; ip -6 route 2>&1 | head -20
	echo "=== ip neigh ==="
	ip neigh 2>&1
	echo "=== board.json ==="
	ls -la /etc/board.json 2>&1
	head -c 4000 /etc/board.json 2>/dev/null; echo
	echo "=== uci ==="
	for i in network wireless dhcp fstab bigoverlay system; do
		uci -q export $i 2>&1 | grep -vE "option key|option password"
	done
	echo "=== wireless ==="
	iw dev 2>&1
	iw phy 2>/dev/null | grep -E '^Wiphy|Band [0-9]|Available Antennas|Configured Antennas'
	echo "=== nft ==="
	nft list ruleset 2>&1 | head -200
	echo "=== gpio ==="
	cat /sys/kernel/debug/gpio 2>/dev/null | grep -E 'gpio(6|7)[^0-9]|wifi'
	echo "=== ps ==="
	ps w 2>/dev/null
	echo "=== services ==="
	ls /etc/rc.d/ 2>/dev/null | tr '\n' ' '; echo
	logread 2>/dev/null | grep -E 'bigoverlay|romsync|be7000-|wifi-defaults|bootconfirm|AP-ENABLED|AP-DISABLED|debug' | tail -n 40
	echo "=== logread ==="
	logread 2>/dev/null | tail -n 600
}

# The copy taken in preinit, before the overlay is mounted, carries dmesg
# only: it is the one that must never get in the way of the boot. Every later
# copy carries the full state.
be7000_bootlog_write() {
	grep -q '"crash_syslog"' /proc/mtd || return 0
	{
		echo "=== be7000 bootlog: $1 $(cut -d' ' -f1 /proc/uptime)s ==="
		cat /proc/cmdline
		echo "=== dmesg ==="
		dmesg | head -c 200000
		[ "$1" = preinit ] || be7000_bootlog_state
		echo "=== end ==="
	} 2>&1 | head -c 480000 > /tmp/be7000-bootlog.txt
	mtd -q -e crash_syslog write /tmp/be7000-bootlog.txt crash_syslog >/dev/null 2>&1
}
