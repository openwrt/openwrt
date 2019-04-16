REQUIRE_IMAGE_SIGNATURE="${REQUIRE_IMAGE_SIGNATURE:-$(uci get system.@system[-1].require_image_signature)}"

fwtool_check_signature() {
	[ $# -gt 1 ] && return 1

	[ "$REQUIRE_IMAGE_SIGNATURE" = 1 ] || return 0

	if ! fwtool -q -s /tmp/sysupgrade.ucert "$1"; then
		echo "Image signature not found"
		[ "$REQUIRE_IMAGE_SIGNATURE" = 1 -a "$FORCE" != 1 ] && {
			echo "Use sysupgrade -F to override this check when downgrading or flashing to vendor firmware"
		}
		[ "$REQUIRE_IMAGE_SIGNATURE" = 1 ] && return 1
		return 0
	fi

	fwtool -q -T -s /dev/null "$1" | \
		ucert -V -m - -c "/tmp/sysupgrade.ucert" -P /etc/opkg/keys

	return $?
}

fwtool_check_image() {
	[ $# -gt 1 ] && return 1

	. /usr/share/libubox/jshn.sh

	if ! fwtool -q -i /tmp/sysupgrade.meta "$1"; then
		echo "Image metadata not found"
		[ "$REQUIRE_IMAGE_METADATA" = 1 -a "$FORCE" != 1 ] && {
			echo "Use sysupgrade -F to override this check when downgrading or flashing to vendor firmware"
		}
		[ "$REQUIRE_IMAGE_METADATA" = 1 ] && return 1
		return 0
	fi

	json_load "$(cat /tmp/sysupgrade.meta)" || {
		echo "Invalid image metadata"
		return 1
	}

	device="$(cat /tmp/sysinfo/board_name)"

	json_select supported_devices || return 1

	json_get_keys dev_keys
	for k in $dev_keys; do
		json_get_var dev "$k"
		[ "$dev" = "$device" ] && return 0
	done

	echo "Device $device not supported by this image"
	echo -n "Supported devices:"
	for k in $dev_keys; do
		json_get_var dev "$k"
		echo -n " $dev"
	done
	echo

	return 1
}
