#!/bin/sh
echo "rtl8366ub diag: uname=$(uname -r)"
for m in rtl8366ub rtl8366ub_mdio swconfig; do
	echo "=== modinfo $m ==="
	modinfo "$m" 2>/dev/null || echo "missing"
done
echo "=== ko files ==="
ls -la /lib/modules/*/rtl8366ub*.ko 2>/dev/null
echo "=== dmesg rtl8366ub ==="
dmesg | grep -i rtl8366ub | tail -30
