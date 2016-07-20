#!/bin/sh

# based on scripts/sysupgrade-nand.sh

profile=""
dtb=""
dtbname=""
kernel=""
rootfs=""
outfile=""
err=""

while [ "$1" ]; do
	case "$1" in
	"--profile")
		profile="$2"
		shift
		shift
		continue
		;;
	"--dtb")
		dtb="$2"
		shift
		shift
		continue
		;;
	"--dtbname")
		dtbname="$2"
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
		else
			shift
			continue
		fi
		;;
	esac
done

if [ -z "$profile" -o ! -r "$dtb" -o ! -r "$kernel" -o ! -r "$rootfs" -o ! "$outfile" ]; then
	echo "syntax: $0 [--profile profilename] [--dtb dtbimage] [--dtbname dtbname] [--kernel kernelimage] [--rootfs rootfs] out"
	exit 1
fi

tmpdir="$( mktemp -d 2> /dev/null )"
if [ -z "$tmpdir" ]; then
	# try OSX signature
	tmpdir="$( mktemp -t 'roottmp' -d )"
fi

if [ -z "$tmpdir" ]; then
	exit 1
fi

mkdir -p "${tmpdir}/${profile}"
[ -z "${dtb}" ] || cp "${dtb}" "${tmpdir}/${profile}/${dtbname}"
[ -z "${rootfs}" ] || cp "${rootfs}" "${tmpdir}/${profile}/uRamdisk"
[ -z "${kernel}" ] || cp "${kernel}" "${tmpdir}/${profile}/uImage"

mtime=""
if [ -n "$SOURCE_DATE_EPOCH" ]; then
	mtime="--mtime=@${SOURCE_DATE_EPOCH}"
fi

(cd "$tmpdir"; tar cvf ${profile}.tar ${profile} ${mtime})
err="$?"
if [ -e "$tmpdir/${profile}.tar" ]; then
	cp "$tmpdir/${profile}.tar" "$outfile"
else
	err=2
fi
rm -rf "$tmpdir"

exit $err
