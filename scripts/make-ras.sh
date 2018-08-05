#!/usr/bin/env bash
#
# --- ZyXEL header format ---
# Original Version by Benjamin Berg <benjamin@sipsolutions.net>
#
# The firmware image prefixed with a header (which is written into the MTD device).
# The header is one erase block (~64KiB) in size, but the checksum only convers the
# first 2KiB. Padding is 0xff. All integers are in big-endian.
#
# The checksum is always a 16-Bit System V checksum (sum -s) stored in a 32-Bit integer.
#
#   4 bytes:  checksum of the rootfs image
#   4 bytes:  length of the contained rootfs image file (big endian)
#  32 bytes:  Firmware Version string (NUL terminated, 0xff padded)
#   4 bytes:  checksum over the header partition (big endian - see below)
#  32 bytes:  Model (e.g. "NBG6617", NUL termiated, 0xff padded)
#   4 bytes:  checksum of the kernel partition
#   4 bytes:  length of the contained kernel image file (big endian)
#      rest: 0xff padding
#
# The checksums are calculated by adding up all bytes and if a 16bit
# overflow occurs, one is added and the sum is masked to 16 bit:
#   csum = csum + databyte; if (csum > 0xffff) { csum += 1; csum &= 0xffff };
# Should the file have an odd number of bytes then the byte len-0x800 is
# used additionally.
#
# The checksum for the header is calculated over the first 2048 bytes with
# the rootfs image checksum as the placeholder during calculation.
#
# The header is padded with 0xff to the erase block size of the device.
#
board=""
version=""
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
	"--version")
		version="$2"
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
	"--rootfssize")
		rootfssize="$2"
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

if [ ! -n "$board" -o ! -n "$version" -o ! -r "$kernel" -o ! -r "$rootfs" -o ! "$rootfssize" -o ! "$outfile" ]; then
	echo "syntax: $0 [--board ras-boardname] [--version ras-version] [--kernel kernelimage] [--rootfs rootfs] out"
	exit 1
fi

rootfs_len=$(wc -c < "$rootfs")

if [ "$rootfs_len" -lt "$rootfssize" ]; then
	dd if=$rootfs of=$rootfs.new bs=$rootfssize conv=sync
	mv $rootfs.new $rootfs
fi

if [ ${#version} -ge 28 ]; then
	echo "version: '$version' is too long"
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

to_be() {
	local val="$1"
	local size="$2"

	case "$size" in
	4)
		echo $(( "$val" >> 24 | ("$val" & 0xff0000) >> 8 | ("$val" & 0xff00) << 8 | ("$val" & 0xff) << 24 ))
		;;
	2)
		echo $(( "$val" >> 8 | ("$val" & 0xff) << 8))
		;;
	esac
}

checksum_file() {
	local file=$1

	# ZyXEL seems to use System V sum mode... Now this is classy, who would have thought?!
	echo $(sum -s ${file} | cut -f1 -d" ")
}

append_bin() {
	local val=$1
	local size=$2
	local file=$3

	while [ "$size" -ne 0 ]; do
		printf \\$(printf %o $(("$val" & 0xff)))  >> "$file"
		val=$(($val >> 8))
		let size-=1
	done
	return
}

tf=${tmpdir}/out
pad=$(printf '%0.1s' $(printf "\xff"){1..64})

rootfs_header_file="$tmpdir/rootfs_header"
rootfs_chksum=$(to_be $(checksum_file ${rootfs}) 4)
rootfs_len=$(to_be $(wc -c < "$rootfs") 4)

versionpadlen=$(( 32 - ( ${#version} + 1) ))

# 4 bytes:  checksum of the rootfs image
append_bin "$rootfs_chksum" 4 "$rootfs_header_file"
# 4 bytes:  length of the contained rootfs image file (big endian)
append_bin "$rootfs_len" 4 "$rootfs_header_file"
# 32 bytes:  Firmware Version string (NUL terminated, 0xff padded)
printf "%s\x00%.*s" "$version" "$versionpadlen" "$pad" >> "$rootfs_header_file"

kernel_header_file="$tmpdir/kernel_header"
kernel_chksum=$(to_be $(checksum_file ${kernel}) 4)
kernel_len=$(to_be $(wc -c < "$kernel") 4)

# 4 bytes:  checksum of the kernel image
append_bin "$kernel_chksum" 4 "$kernel_header_file"
# 4 bytes:  length of the contained kernel image file (big endian)
append_bin "$kernel_len" 4 "$kernel_header_file"

board_header_file="$tmpdir/board_header"
board_file="$tmpdir/board"
boardpadlen=$(( 64 - ( ${#board} + 1) ))
# 32 bytes:  Model (e.g. "NBG6617", NUL termiated, 0xff padded)
printf "%s\x00%.*s" "$board" "$boardpadlen" "$pad" > "$board_file"
cat "$kernel_header_file" >> "$board_file"
printf "%.12s" "$pad" >> "$board_file"
#      rest: 0xff padding
for i in {1..511}; do
	printf "%s%s" "$pad" "$pad" >> "$board_file"
done

tmp_board_file="$tmpdir/tmp_board_file"
cat "$rootfs_header_file" > "$tmp_board_file"

# The checksum for the header is calculated over the first 2048 bytes with
# the rootfs image checksum as the placeholder during calculation.
append_bin "$rootfs_chksum" 4 "$tmp_board_file"
cat "$board_file" >> "$tmp_board_file"

truncate -s 2048 $tmp_board_file
board_chksum=$(to_be $(checksum_file ${tmp_board_file}) 4)

# 4 bytes:  checksum over the header partition (big endian)
append_bin "$board_chksum" 4 "$board_header_file"
cat "$board_file" >> "$board_header_file"

cat "$rootfs_header_file" "$board_header_file" "$rootfs" "$kernel" > "$outfile"

rm -rf "$tmpdir"
