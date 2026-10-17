#!/bin/sh
# SPDX-License-Identifier: GPL-2.0-only

set -eu

TOPDIR=$(CDPATH= cd -- "$(dirname "$0")/.." && pwd)
COMMON="$TOPDIR/package/network/config/wifi-scripts/files-ucode/usr/share/ucode/wifi/common.uc"
MAC80211="$TOPDIR/package/network/config/wifi-scripts/files-ucode/lib/netifd/wireless/mac80211.sh"

grep -q '^export function wiphy_radio' "$COMMON" || {
	echo "wiphy radio resolver is missing" >&2
	exit 1
}
grep -q 'wiphy_radio(phy_info, data.config.band, data.config.radio)' "$MAC80211" || {
	echo "mac80211 does not resolve radio by band" >&2
	exit 1
}

# Model the nl80211 radio list returned by a mocked wiphy. The numeric index
# for the 5 GHz radio changes from 1 to 2 between boots, while the frequency
# range remains the stable identity.
radio_for_band() {
	band="$1"
	current="$2"
	radio_list="$3"

	while IFS=: read -r index range; do
		[ "$index" = "$current" ] || continue
		[ "$range" = "$band" ] && {
			echo "$current"
			return
		}
	done < "$radio_list"

	while IFS=: read -r index range; do
		[ "$range" = "$band" ] && {
			echo "$index"
			return
		}
	done < "$radio_list"

	echo "$current"
}

mock_sysfs=$(mktemp -d)
trap 'rm -rf "$mock_sysfs"' EXIT

cat > "$mock_sysfs/boot-a" <<EOF
0:2g
1:5g
2:6g
EOF
cat > "$mock_sysfs/boot-b" <<EOF
0:6g
1:2g
2:5g
EOF

[ "$(radio_for_band 5g 1 "$mock_sysfs/boot-a")" = 1 ]
[ "$(radio_for_band 5g 1 "$mock_sysfs/boot-b")" = 2 ]
[ "$(radio_for_band 2g 0 "$mock_sysfs/boot-b")" = 1 ]
[ "$(radio_for_band 6g 2 "$mock_sysfs/boot-b")" = 0 ]

echo "radio mapping follows band identity across changed phy order"
