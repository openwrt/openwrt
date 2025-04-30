fwtool_check_signature() {
	[ $# -gt 1 ] && return 1

	[ ! -x /usr/bin/ucert ] && {
		if [ "$REQUIRE_IMAGE_SIGNATURE" = 1 ]; then
			return 1
		else
			return 0
		fi
	}

	if ! fwtool -q -s /tmp/sysupgrade.ucert "$1"; then
		v "Image signature not present"
		[ "$REQUIRE_IMAGE_SIGNATURE" = 1 -a "$FORCE" != 1 ] && {
			v "Use sysupgrade -F to override this check when downgrading or flashing to vendor firmware"
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
		v "Image metadata not present"
		[ "$REQUIRE_IMAGE_METADATA" = 1 -a "$FORCE" != 1 ] && {
			v "Use sysupgrade -F to override this check when downgrading or flashing to vendor firmware"
		}
		[ "$REQUIRE_IMAGE_METADATA" = 1 ] && return 1
		return 0
	fi

	json_load "$(cat /tmp/sysupgrade.meta)" || {
		v "Invalid image metadata"
		return 1
	}
	# Step 1. check if oem_name file exist and is not empty
	# If the above is true store the contents (b3000) in $oem value for later
	[ -s /tmp/sysinfo/oem_name ] && oem="$(cat /tmp/sysinfo/oem_name)"
	device="$(cat /tmp/sysinfo/board_name)"
	devicecompat="$(uci -q get system.@system[0].compat_version)"
	[ -n "$devicecompat" ] || devicecompat="1.0"

	json_get_var imagecompat compat_version
	json_get_var compatmessage compat_message
	[ -n "$imagecompat" ] || imagecompat="1.0"

	# select correct supported list based on compat_version
	# (using this ensures that compatibility check works for devices
	#  not knowing about compat-version)
	local supported=supported_devices
	[ "$imagecompat" != "1.0" ] && supported=new_supported_devices
	json_select $supported || return 1

	json_get_keys dev_keys
	for k in $dev_keys; do
		json_get_var dev "$k"
		# Step 2.
		# lets start with the original case [ "$dev" = "$device" ]
		# if the evaluated firmware is vanila openwrt, this evals as true -ie
		# [ ("$dev" == "glinet.gl-b3000") == ("$device" == "glinet,gl-b3000") ]
		# however if the firmware is oem then $dev = b3000 and the above check fails resulting 
		# in the erroneous warnings.
		# so we add the secondary check [ "$dev" = "$oem" ];
		# If in Step 1 the oem_file was found and valid, the $oem == "b3000" so
		# [ ("$dev" == "b3000) == ("$oem" == "b3000") ] so firmware is valid oem
		if [ "$dev" = "$device" ] || [ "$dev" = "$oem" ]; then
			# major compat version -> no sysupgrade
			if [ "${devicecompat%.*}" != "${imagecompat%.*}" ]; then
				v "The device is supported, but this image is incompatible for sysupgrade based on the image version ($devicecompat->$imagecompat)."
				[ -n "$compatmessage" ] && v "$compatmessage"
				return 1
			fi

			# minor compat version -> sysupgrade with -n required
			# Step 3.
			# here we must check if $dev == $oem to use this native compatability check
			# so we add the check for [ "$dev" = "$oem" ]
			if (([ "${devicecompat#.*}" != "${imagecompat#.*}" ] || [ "$dev" = "$oem" ])) && [ "$SAVE_CONFIG" = "1" ]; then
				# Step 4.
				# here we have to gaurd against the default case, oem may exsist and default will pass 
				# the original check [ "${devicecompat#.*}" != "${imagecompat#.*}" ] so we must
				# explicitly check $dev == $oem, if it is we update(reuse) the $devicecompat and imagecompat
				# variable to reflect the case - ( Openwrt -> OEM )
				[ "$dev" = "$oem" ] && devicecompat="Openwrt " && imagecompat=" OEM"
				[ "$IGNORE_MINOR_COMPAT" = 1 ] && return 0
				v "The device is supported, but the config is incompatible to the new image ($devicecompat->$imagecompat). Please upgrade without keeping config (sysupgrade -n)."
				[ -n "$compatmessage" ] && v "$compatmessage"
				return 1
			fi

			return 0
		fi
	done

	v "Device $device not supported by this image"
	local devices="Supported devices:"
	for k in $dev_keys; do
		json_get_var dev "$k"
		devices="$devices $dev"
	done
	v "$devices"

	return 1
}
