#!/bin/sh

ubootenv=""
nokernel=""
ubinize_param=""
kernel=""
rootfs=""
outfile=""
err=""

get_magic_word() {
	dd if=$1 bs=2 count=1 2>/dev/null | hexdump -v -n 2 -e '1/1 "%02x"'
}

is_ubifs() {
	if [ "$( get_magic_word $1 )" = "3118" ]; then
		echo "1"
	fi
}

ubivol() {
	volid=$1
	name=$2
	image=$3
	autoresize=$4
	echo "[$name]"
	echo "mode=ubi"
	echo "vol_id=$volid"
	echo "vol_type=dynamic"
	echo "vol_name=$name"
	if [ "$image" ]; then
		echo "image=$image"
	else
		echo "vol_size=1MiB"
	fi
	if [ "$autoresize" ]; then
		echo "vol_flags=autoresize"
	fi
}

ubilayout() {
	local vol_id=0
	local root_is_ubifs="$( is_ubifs "$2" )"
	if [ "$1" = "ubootenv" ]; then
		ubivol $vol_id ubootenv
		vol_id=$(( $vol_id + 1 ))
		ubivol $vol_id ubootenv2
		vol_id=$(( $vol_id + 1 ))
	fi
	if [ "$3" ]; then
		ubivol $vol_id kernel "$3"
		vol_id=$(( $vol_id + 1 ))
	fi
	ubivol $vol_id rootfs "$2" $root_is_ubifs
	vol_id=$(( $vol_id + 1 ))
	[ "$root_is_ubifs" ] || ubivol $vol_id rootfs_data "" 1
}

while [ "$1" ]; do
	case "$1" in
	"--uboot-env")
		ubootenv="ubootenv"
		shift
		continue
		;;
	"--no-kernel")
		nokernel="nokernel"
		shift
		continue
		;;
	"-"*)
		ubinize_param="$@"
		break
		;;
	*)
		if [ ! "$kernel" -a ! "$nokernel" ]; then
			kernel=$1
			shift
			continue
		fi
		if [ ! "$rootfs" ]; then
			rootfs=$1
			shift
			continue
		fi
		if [ ! "$outfile" ]; then
			outfile=$1
			shift
			continue
		fi
		;;
	esac
done

if [ ! -r "$rootfs" -o ! -r "$kernel" -a ! "$nokernel" -o ! "$outfile" ]; then
	echo "syntax: $0 [--no-kernel] [--uboot-env] rootfs [kernel] out [ubinize opts]"
	exit 1
fi

ubinize="$( which ubinize )"
if [ ! -x "$ubinize" ]; then
	echo "ubinize tool not found or not usable"
	exit 1
fi

ubinizecfg="$( mktemp )"
ubilayout "$ubootenv" "$rootfs" "$kernel" > "$ubinizecfg"

cat "$ubinizecfg"
ubinize -o "$outfile" $ubinize_param "$ubinizecfg"
err="$?"
[ ! -e "$outfile" ] && err=2
rm "$ubinizecfg"

exit $err

