#!/bin/sh

set -eu

TOPDIR="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
cd "$TOPDIR"

# scripts/feeds uninstall refreshes an existing .config as a side effect.
# Preserve the caller's exact configuration so this metadata cleanup cannot
# dirty the checkout or silently rewrite a local E87N build selection.
config_backup=''
restore_config() {
	[ -n "$config_backup" ] || return 0
	cp "$config_backup" .config
	rm -f "$config_backup"
	config_backup=''
}

if [ -f .config ]; then
	config_backup="$(mktemp "${TMPDIR:-/tmp}/e87n-config.XXXXXX")"
	cp .config "$config_backup"
	trap 'restore_config' EXIT HUP INT TERM
fi

# These packages are not selected by .config.e87n. Their current feed
# Makefiles reference providers which are absent from the pinned feed set, so
# installing every feed package leaves irrelevant metadata warnings in every
# subsequent make invocation. Keep the video feed itself because it provides
# Mesa, Wayland and Graphene to the packages feed; only its unused SDL3 chain
# is removed.
ORPHAN_PACKAGES='bmx7-dnsupdate
luci-app-babeld
luci-app-bmx7
luci-app-olsr
luci-app-olsr-services
luci-app-olsr-viz
luci-proto-batman-adv
prometheus-node-exporter-lua
sdl2-compat
sdl3
sdl3-doom
sdl3-image
sdl3-mixer'

installed=''
for package in $ORPHAN_PACKAGES; do
	for link in package/feeds/*/"$package"; do
		[ -L "$link" ] || continue
		installed="$installed $package"
		break
	done
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

restore_config
trap - EXIT HUP INT TERM
