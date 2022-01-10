#!/bin/sh
#
# Based on the original script without the build stuff
# update_kernel.sh: (c) 2017 Jonas Gorski <jonas.gorski@gmail.com>
# Licensed under the terms of the GNU GPL License version 2

TEST=0
UPDATE=0

KERNEL=
PATCHVER=

while [ $# -gt 0 ]; do
	case $1 in
		-t|--test)
			TEST=1
			;;
		-u|--update)
			UPDATE=1
			;;
		[1-9]*)
			if [ -z "$KERNEL" ]; then
				KERNEL=$1
			elif [ -z "$PATCHVER" ]; then
				PATCHVER=$1
			else
				exit 1
			fi
			;;
		*)
			break
			;;

	esac

	shift
done

if [ -z "$KERNEL" ]; then
	echo "usage: $0 [<options>...] <patchver> [<version>]"
	echo "example: $0 3.18 3.18.30"
	echo "If <version> is not given, it will try to find out the latest from kernel.org"
	echo ""
	echo "valid options:"
	echo "-t|--test         don't do anything, just print what it would do"
	echo "-u|--update       update kernel-files to latest version and hash"
	exit 1
fi

if [ -z "$kernel" ]; then
        echo Missing kernel version. Please provide a version to get the hash.
        exit 0
fi

if [ -z "$PATCHVER" ]; then
	if [ -n "$(command -v curl)" ]; then
		DL_CMD="curl -s "
	fi

	if [ -n "$(command -v wget)" ]; then
		DL_CMD="wget -O - -q "
	fi

	if [ -z "$DL_CMD" ]; then
		echo "Failed to find a suitable download program. Please install either curl or wget." >&2
		exit 1
	fi

	# https://www.kernel.org/feeds/kdist.xml
	# $(curl -s https://www.kernel.org/feeds/kdist.xml | sed -ne 's|^.*"html_url": "\(.*/commit/.*\)",|\1.patch|p')
	# curl -s "https://www.kernel.org/feeds/kdist.xml"
	CURR_VERS=$($DL_CMD "https://www.kernel.org/feeds/kdist.xml" | sed -ne 's|^.*title>\([1-9][^\w]*\): .*|\1|p')

	for ver in $CURR_VERS; do
		case $ver in
			"$KERNEL"*)
				PATCHVER=$ver
				;;
		esac

		if [ -n "$PATCHVER" ]; then
			break
		fi
	done

	if [ -z "$PATCHVER" ]; then
		echo "Failed to find the latest release on kernel.org, please specify the release manually" >&2
		exit 1
	fi
fi

echo "Refreshing Kernel $KERNEL to release $PATCHVER ..."

if [ "$TEST" -eq 1 ]; then
	CMD="echo"
fi

$CMD make target/linux/download KERNEL_PATCHVER=${KERNEL} LINUX_VERSION=${PATCHVER} LINUX_KERNEL_HASH=skip || exit 1

if [ "$UPDATE" -eq 1 ]; then
	NEWVER=${PATCHVER#$KERNEL}
	if [ "$TEST" -eq 1 ]; then
		echo ./staging_dir/host/bin/mkhash sha256 dl/linux-$PATCHVER.tar.xz
	fi

	if [ -f dl/linux-$PATCHVER.tar.xz ]; then
		CHECKSUM=$(./staging_dir/host/bin/mkhash sha256 dl/linux-$PATCHVER.tar.xz)
	fi

	$CMD ./staging_dir/host/bin/sed -i include/kernel-${KERNEL} \
		-e "s|LINUX_VERSION-${KERNEL} =.*|LINUX_VERSION-${KERNEL} = ${NEWVER}|" \
		-e "s|LINUX_KERNEL_HASH-${KERNEL}.*|LINUX_KERNEL_HASH-${PATCHVER} = ${CHECKSUM}|"
fi
