#!/bin/sh

config_file="$1"
device="$2"

is_yes () {
	grep -qe "^$1=y\$" "$config_file"
}

if [ "$#" -lt 1 ]; then
	echo "ERROR: config is not given. $0 .config [DEVICE]" 1>&2
	exit 1
fi

if is_yes CONFIG_TARGET_PER_DEVICE_ROOTFS; then
	board="$(grep -e '^CONFIG_TARGET_BOARD=' "$config_file" | cut -d '=' -f 2 | tr -d '"')"
	subtarget="$(grep -e '^CONFIG_TARGET_SUBTARGET=' "$config_file" | cut -d '=' -f 2 | tr -d '"')"

	if [ -z "$device" ]; then
		echo "ERROR: CONFIG_TARGET_PER_DEVICE_ROOTFS=y, but device is unset. $0 $1 DEVICE" 1>&2
		exit 1
	elif ! grep -qe "^CONFIG_TARGET_DEVICE_PACKAGES_${board}_${subtarget}_DEVICE_${device}=" "$config_file"; then
		echo "ERROR: CONFIG_TARGET_DEVICE_PACKAGES_${board}_${subtarget}_DEVICE_${device} not found for device \"$device\"." 1>&2
		exit 1
	fi
fi

# Per device packages
if is_yes CONFIG_TARGET_PER_DEVICE_ROOTFS; then
	grep -e "^CONFIG_TARGET_DEVICE_PACKAGES_${board}_${subtarget}_DEVICE_${device}=" "$config_file" | \
		cut -d '=' -f 2 | tr -d '"' | tr -s '\n' ' '
fi

# some CONFIG_PACKAGE_* symbols appear, which are not a package, therefore
# strip everything that begins with uppercase characters.
grep -e '^CONFIG_PACKAGE_[[:lower:]].*=y' "$config_file" | \
	cut -d '_' -f 3- | cut -d '=' -f 1 | \
	tr -s '\n' ' '


# Disable default packages (if necessary)
default_packages="$(
	grep -e '^CONFIG_DEFAULT_[[:lower:]].*=y$' "$config_file" | \
		sed 's/^CONFIG_DEFAULT_\(.*\)=y$/\1/'
)"

for default_package in $default_packages; do
	if ! is_yes "CONFIG_PACKAGE_${default_package}"; then
		echo "-${default_package}"
	fi
done | tr -s '\n' ' '
