. /lib/functions.sh

PART_SIZE=20
NAME_SIZE=16
MAX_NUM_PARTS=16

MAGIC_START_HEX="a0 a1 a2 a3"
MAGIC_START_TRY_HEX="a1 a1 a2 a3"
MAGIC_END_HEX="b0 b1 b2 b3"

validate_bootconfig_magic() {
	local file=$1
	magic_start=$(hexdump -v -n 4 -e '4/1 "%02x "' "$file")
	magic_end=$(hexdump -v -s 332 -n 4 -e '4/1 "%02x "' "$file")
	
	if [ "$magic_start" != "$MAGIC_START_HEX" ] && \
	   [ "$magic_start" != "$MAGIC_START_TRY_HEX" ]; then
		echo "Not a valid bootconfig file, start magic does not match" >&2
		return 1
	fi

	if [ "$magic_end" != "$MAGIC_END_HEX" ]; then
		echo "Not a valid bootconfig file, end magic does not match" >&2
		return 1
	fi
	return 0
}

get_bootconfig_numparts() {
	local file=$1
	numpartshex=$(hexdump -v -s 8 -n 4 -e '4/1 "%02x "' "$file")
	numparts=$(( 0x$(echo $numpartshex | awk '{print $4$3$2$1}') ))
	echo ${numparts}
}

get_bootconfig_partidx() {
	local file=$1
	local partname=$2
	local numparts=$(get_bootconfig_numparts "$file")

	if [ -z "$numparts" ]; then
		echo "Could not get number of partitions" >&2
		return
	fi

	if [ $numparts -gt $MAX_NUM_PARTS ]; then
		numparts=$MAX_NUM_PARTS
	fi

	for i in $(seq 0 $((numparts -1))); do
		nameoffset=$((12 + i * $PART_SIZE))
		nameraw=$(dd if="$file" bs=1 skip="$nameoffset" count=12 2>/dev/null)
		name=${nameraw//S'\x00'/}
		if [ "$partname" = "$name" ]; then
			echo $i
		fi
	done
}

get_bootconfig_primaryboot() {
	local file=$1
	local partname=$2

	local partidx=$(get_bootconfig_partidx "$file" "$partname")
	if ! echo "$partidx" | grep -Eq '^[0-9]+$'; then
		echo "Could not get partition index for $partname in $file" >&2
		return
	fi

	if [ "$partidx" -ge 0 ] && [ "$partidx" -lt $MAX_NUM_PARTS ]; then
		offset=$((12 + $partidx * $PART_SIZE + $NAME_SIZE))
		primaryboothex=$(hexdump -v -s "$offset" -n 4 -e '4/1 "%02x "' $file)
		primaryboot=$(( 0x$(echo $primaryboothex | awk '{print $4$3$2$1}') ))
		echo $primaryboot
	fi
}

_set_bootconfig_primaryboot() {
	local file=$1
	local partname=$2
	local primaryboot=$3
	local primaryboothex
	local partidx
	local primarybootoffset

	partidx=$(get_bootconfig_partidx "$file" "$partname")
	if ! echo "$partidx" | grep -Eq '^[0-9]+$'; then
		echo "Could not get partition index for $2" >&2
		return 1
	fi
	primarybootoffset=$((12 + $partidx * $PART_SIZE + $NAME_SIZE))

	case "$primaryboot" in
		0)
			printf "\x00\x00\x00\x00" | dd of="$file" seek="$primarybootoffset" bs=1 count=4 conv=notrunc 2>/dev/null
			;;
		1)
			printf "\x01\x00\x00\x00" | dd of="$file" seek="$primarybootoffset" bs=1 count=4 conv=notrunc 2>/dev/null
			;;
		*)
			echo "invalid argument: primaryboot must be 0 or 1" >&2
			return 1
			;;
	esac
}

set_bootconfig_primaryboot() {
	local file=$1
	local partname=$2
	local primaryboot=$3

	[ -z "$file" ] || [ -z "$partname" ] || [ -z "$primaryboot" ] && {
		echo "usage: $0 <file> <partition name> <0|1>"
		return 1
	}

	[ ! -e "$file" ] && {
		echo "file $file not found" >&2
		return 1
	}

	[ ! -w $file ] && {
		echo "file $file not writable" >&2
		return 1
	}

	validate_bootconfig_magic "$file"
	[ $? -ne 0 ] && return 1

	_set_bootconfig_primaryboot $file $partname $primaryboot
	[ $? -ne 0 ] && return 1

	return 0
}

toggle_bootconfig_primaryboot() {
	local file=$1
	local partname=$2
	local primaryboot
	
	[ -z "$file" ] || [ -z "$partname" ] && {
		echo "usage: $0 <file> <partition name>"
		return 1
	}

	[ ! -e "$file" ] && {
		echo "file $file not found" >&2
		return 1
	}

	[ ! -w $file ] && {
		echo "file $file not writable" >&2
		return 1
	}

	validate_bootconfig_magic "$file"
	[ $? -ne 0 ] && return 1
	
	primaryboot=$(get_bootconfig_primaryboot "$1" "$2")

	case "$primaryboot" in
		0)
			_set_bootconfig_primaryboot "$1" "$2" 1
			;;
		1)
			_set_bootconfig_primaryboot "$1" "$2" 0
			;;
		*)
			echo "invalid value: primaryboot must be 0 or 1" >&2
			return 1
			;;
	esac

	[ $? -ne 0 ] && return 1

	return 0
}
