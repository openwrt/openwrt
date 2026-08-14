#!/bin/sh

set -eu

TOPDIR="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
cd "$TOPDIR"

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

# Ruby 4.0 promotes bigdecimal and pstore from their former standard-library
# locations to bundled gems. The pinned packages feed still asks ls(1) for the
# obsolete paths while assembling every Ruby subpackage, which prints fourteen
# misleading "No such file or directory" diagnostics even though the actual
# gem payloads are present and packaged. Remove only those obsolete path
# entries, retain the bundled-gem paths, and omit the Windows-only win32ole gem
# before Ruby's install step so a Linux target does not report it as failed.
ruby_makefile='feeds/packages/lang/ruby/Makefile'
ruby_source_patch='scripts/e87n-feed-patches/999-ruby-4.0-skip-win32ole.patch'
ruby_feed_patch='feeds/packages/lang/ruby/patches/999-ruby-4.0-skip-win32ole.patch'
if [ -f "$ruby_makefile" ]; then
	if grep -Fxq 'PKG_VERSION:=4.0.2' "$ruby_makefile"; then
		ruby_tmp="${ruby_makefile}.codex-new"
		awk '
			$0 == "/usr/lib/ruby/$(PKG_ABI_VERSION)/*/bigdecimal.so" { next }
			$0 == "/usr/lib/ruby/$(PKG_ABI_VERSION)/bigdecimal/" { next }
			$0 == "/usr/lib/ruby/$(PKG_ABI_VERSION)/bigdecimal.rb" { next }
			$0 == "/usr/lib/ruby/gems/$(PKG_ABI_VERSION)/specifications/default/bigdecimal-*.gemspec" { next }
			$0 == "/usr/lib/ruby/gems/$(PKG_ABI_VERSION)/gems/bigdecimal-*/Rakefile" { next }
			$0 == "/usr/lib/ruby/$(PKG_ABI_VERSION)/pstore.rb" { next }
			$0 == "/usr/lib/ruby/$(PKG_ABI_VERSION)/pstore/" { next }
			{ print }
		' "$ruby_makefile" > "$ruby_tmp"
		cat "$ruby_tmp" > "$ruby_makefile"
		rm -f "$ruby_tmp"

		mkdir -p "$(dirname "$ruby_feed_patch")"
		cp "$ruby_source_patch" "$ruby_feed_patch"

		grep -Fxq '/usr/lib/ruby/gems/$(PKG_ABI_VERSION)/gems/bigdecimal-*/' "$ruby_makefile"
		grep -Fxq '/usr/lib/ruby/gems/$(PKG_ABI_VERSION)/extensions/*/$(PKG_ABI_VERSION)/bigdecimal-*/' "$ruby_makefile"
		grep -Fxq '/usr/lib/ruby/gems/$(PKG_ABI_VERSION)/gems/pstore-*/' "$ruby_makefile"
		grep -Fxq '/usr/lib/ruby/gems/$(PKG_ABI_VERSION)/specifications/pstore-*.gemspec' "$ruby_makefile"
		test -s "$ruby_feed_patch"
	else
		# Do not leave a version-specific source patch behind if the feed is
		# deliberately moved away from Ruby 4.0.2 in the future.
		rm -f "$ruby_feed_patch"
	fi
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
ORPHAN_LIST='scripts/e87n-unselected-feed-packages'
test -s "$ORPHAN_LIST"
ORPHAN_PACKAGES="$(sed -e 's/#.*$//' -e '/^[[:space:]]*$/d' "$ORPHAN_LIST")"

# install -a now skips these sources before resolving their absent providers.
# Remove links left by an older checkout directly, without calling
# `scripts/feeds uninstall`, because uninstall refreshes package metadata and
# prints the very dependency warnings this cleanup is designed to prevent.
for package in $ORPHAN_PACKAGES; do
	for link in package/feeds/*/"$package"; do
		[ -L "$link" ] || continue
		rm -f "$link"
	done
done

for package in $ORPHAN_PACKAGES; do
	for link in package/feeds/*/"$package"; do
		[ ! -L "$link" ] || {
			echo "ERROR: failed to remove unselected feed package: $link" >&2
			exit 1
		}
	done
done
