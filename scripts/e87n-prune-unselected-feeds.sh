#!/bin/sh

set -eu

TOPDIR="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
cd "$TOPDIR"

# These packages are not selected by .config.e87n. Their current feed
# Makefiles reference routing providers which are absent from the pinned feed
# set, so installing every feed package leaves irrelevant metadata warnings in
# every subsequent make invocation.
ORPHAN_PACKAGES='bmx7-dnsupdate
luci-app-babeld
luci-app-bmx7
luci-app-olsr
luci-app-olsr-services
luci-app-olsr-viz
luci-proto-batman-adv
prometheus-node-exporter-lua'

installed=''
for package in $ORPHAN_PACKAGES; do
	for link in package/feeds/*/"$package"; do
		[ -L "$link" ] || continue
		installed="$installed $package"
		break
	done
done

# E87N does not select packages from the video feed. Remove legacy symlinks
# left by an older checkout as a complete set: pruning only libsdl3 leaves its
# unselected consumers behind and merely moves the unresolved warning to the
# next dependency level.
for link in package/feeds/video/*; do
	[ -L "$link" ] || continue
	installed="$installed ${link##*/}"
done

if [ -n "$installed" ]; then
	# The feeds helper removes only feed-owned symlinks and refreshes metadata
	# once after the entire list has been removed.
	./scripts/feeds uninstall $installed
fi

for package in $ORPHAN_PACKAGES; do
	for link in package/feeds/*/"$package"; do
		[ ! -L "$link" ] || {
			echo "ERROR: failed to remove unselected feed package: $link" >&2
			exit 1
		}
	done
done

for link in package/feeds/video/*; do
	[ ! -L "$link" ] || {
		echo "ERROR: failed to remove unused video feed package: $link" >&2
		exit 1
	}
done
