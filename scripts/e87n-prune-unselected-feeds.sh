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

# The video feed's SDL3 recipe names the libwayland binary package as a build
# dependency. OpenWrt build dependencies are resolved by source provider, whose
# name is "wayland". Correct the downloaded feed before refreshing metadata.
sdl3_makefile='feeds/video/libs/sdl3/Makefile'
if [ -f "$sdl3_makefile" ]; then
	sed -i \
		's/^PKG_BUILD_DEPENDS:=wayland\/host libwayland /PKG_BUILD_DEPENDS:=wayland\/host wayland /' \
		"$sdl3_makefile"
	grep -Fq 'PKG_BUILD_DEPENDS:=wayland/host wayland wayland-protocols libxkbcommon' \
		"$sdl3_makefile" || {
		echo "ERROR: unexpected SDL3 Wayland dependency format: $sdl3_makefile" >&2
		exit 1
	}
fi

# OpenClash's init script is sourced by rc.common while the image builder is
# creating service-enable symlinks. Its helper scripts contain target-absolute
# includes, so loading them with IPKG_INSTROOT set makes the build host look for
# /lib/functions.sh and /usr/share/openclash instead of the staged rootfs. The
# enable action only needs START/STOP from the init script; defer runtime helpers
# until the script is running on the router. Reapply this guard after every feed
# update, and fail loudly if upstream changes the expected source block.
openclash_init='feeds/openclash/luci-app-openclash/root/etc/init.d/openclash'
openclash_guard='# CODEX_OFFLINE_ROOTFS_GUARD'
if [ -f "$openclash_init" ]; then
	if ! grep -Fq "$openclash_guard" "$openclash_init"; then
		grep -Fxq '. $IPKG_INSTROOT/usr/share/openclash/openclash_ps.sh' "$openclash_init"
		grep -Fxq '. $IPKG_INSTROOT/usr/share/openclash/openclash_curl.sh' "$openclash_init"
		openclash_tmp="${openclash_init}.codex-new"
		if awk '
			BEGIN {
				first = ". $IPKG_INSTROOT/usr/share/openclash/openclash_ps.sh"
				last = ". $IPKG_INSTROOT/usr/share/openclash/openclash_curl.sh"
			}
			$0 == first {
				print "# CODEX_OFFLINE_ROOTFS_GUARD"
				print "if [ -z \"${IPKG_INSTROOT:-}\" ]; then"
				opened = 1
			}
			{ print }
			$0 == last {
				print "fi"
				closed = 1
			}
			END { exit !(opened && closed) }
		' "$openclash_init" > "$openclash_tmp"; then
			cat "$openclash_tmp" > "$openclash_init"
			rm -f "$openclash_tmp"
		else
			rm -f "$openclash_tmp"
			echo "ERROR: unexpected OpenClash helper block: $openclash_init" >&2
			exit 1
		fi
	fi

	grep -Fq "$openclash_guard" "$openclash_init"
	grep -Fq 'if [ -z "${IPKG_INSTROOT:-}" ]; then' "$openclash_init"
	grep -Fq '. $IPKG_INSTROOT/usr/share/openclash/uci.sh' "$openclash_init"
	grep -Fq '. $IPKG_INSTROOT/usr/share/openclash/openclash_curl.sh' "$openclash_init"
	awk '
		/# CODEX_OFFLINE_ROOTFS_GUARD/ { guard = 1 }
		guard && /^if \[ -z "\$\{IPKG_INSTROOT:-\}" \]; then$/ { conditional = 1 }
		conditional && /^\. \$IPKG_INSTROOT\/usr\/share\/openclash\/openclash_curl\.sh$/ { helper = 1 }
		helper && /^fi$/ { closed = 1; exit }
		END { exit !(guard && conditional && helper && closed) }
	' "$openclash_init" || {
		echo "ERROR: unexpected OpenClash offline-rootfs guard: $openclash_init" >&2
		exit 1
	}
fi

# These packages are not selected by .config.e87n. Their current feed
# Makefiles reference providers which are absent from the pinned feed set, so
# installing every feed package leaves irrelevant metadata warnings in every
# subsequent make invocation. Keep the video feed itself because it provides
# Mesa, Wayland, Graphene and the SDL2/SDL3 compatibility providers required
# by other feed metadata.
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
