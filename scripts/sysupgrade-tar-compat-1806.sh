#!/bin/sh

board=""
board1806=""
kernel=""
rootfs=""
outfile=""
err=""

while [ "$1" ]; do
	case "$1" in
	"--board")
		board="$2"
		shift
		shift
		continue
		;;
	"--kernel")
		kernel="$2"
		shift
		shift
		continue
		;;
	"--rootfs")
		rootfs="$2"
		shift
		shift
		continue
		;;
	*)
		if [ ! "$outfile" ]; then
			outfile=$1
			shift
			continue
		fi
		;;
	esac
done

if [ ! -n "$board" -o ! -r "$kernel" -a  ! -r "$rootfs" -o ! "$outfile" ]; then
	echo "syntax: $0 [--board boardname] [--kernel kernelimage] [--rootfs rootfs] out"
	exit 1
fi

tmpdir="$( mktemp -d 2> /dev/null )"
if [ -z "$tmpdir" ]; then
	# try OSX signature
	tmpdir="$( mktemp -t 'ubitmp' -d )"
fi

if [ -z "$tmpdir" ]; then
	exit 1
fi

echo "$tmpdir ########################################################"

board1806="$(echo "$board"|cut -d '_' -f 2|awk -F '-' '{print $1 "-" $2}')"
mkdir -p "${tmpdir}/sysupgrade-${board}"
echo "BOARD=${board}" > "${tmpdir}/sysupgrade-${board}/CONTROL"
mkdir -p "${tmpdir}/sysupgrade-${board1806}"
echo "BOARD=${board1806}" > "${tmpdir}/sysupgrade-${board1806}/CONTROL"
[ -z "${rootfs}" ] || cp "${rootfs}" "${tmpdir}/sysupgrade-${board}/root"
[ -z "${kernel}" ] || cp "${kernel}" "${tmpdir}/sysupgrade-${board}/kernel"

mtime=""
if [ -n "$SOURCE_DATE_EPOCH" ]; then
	mtime="--mtime=@${SOURCE_DATE_EPOCH}"
fi

(cd "$tmpdir"; tar --sort=name --owner=0 --group=0 --numeric-owner -cvf sysupgrade.tar sysupgrade-${board} sysupgrade-${board1806} ${mtime})
err="$?"
if [ -e "$tmpdir/sysupgrade.tar" ]; then
	cp "$tmpdir/sysupgrade.tar" "$outfile"
else
	err=2
fi
rm -rf "$tmpdir"

exit $err
