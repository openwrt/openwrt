REQUIRE_IMAGE_METADATA=1
RAMFS_COPY_BIN='fw_printenv fw_setenv blockdev'
RAMFS_COPY_DATA='/etc/fw_env.config /var/lock/fw_printenv.lock'

get_cmdline_var() {
	local var=$1
	local cmdlinevar tmp
	local cmdline="$(cat /proc/cmdline)"

	for cmdlinevar in $cmdline; do
		tmp=${cmdlinevar##${var}}
		[ "=" = "${tmp:0:1}" ] && echo ${tmp:1}
	done
}

get_rootdev() {
	local rootvol rootdev
	rootvol=$(get_cmdline_var root)
	rootvol=$(basename $rootvol)
	[ -e /sys/class/block/$rootvol ] || {
		rootvol=${rootvol%%[0-9]}
		[ -e /sys/class/block/$rootvol ] && echo $rootvol
		rootvol=${rootvol%%p}
		[ -e /sys/class/block/$rootvol ] && echo $rootvol
		return
	}
	[ -e /sys/class/block/$rootvol/partition ] || {
		echo $rootvol
		return
	}
	rootdev=$(busybox readlink -f /sys/class/block/$rootvol)
	rootdev=$(basename ${rootdev%%/${rootvol}})
	[ -e /sys/class/block/$rootdev ] && echo $rootdev
}

get_partition() {
	for partname in /sys/class/block/$1/*/name; do
		[ "$(cat ${partname})" = "$2" ] && {
			basename ${partname%%/name}
			break
		}
	done
}

platform_do_upgrade() {
	local board=$(board_name)
	local file_type=$(identify $1)

	case "$board" in
	bananapi,bpi-r64)
		local rootdev=$(get_rootdev)
		local fitpart=$(get_partition $rootdev production)
		[ "$fitpart" ] || exit 1
		dd if=/dev/zero of=/dev/$fitpart bs=4096 count=1 2>/dev/null
		blockdev --rereadpt /dev/$rootdev
		get_image "$1" | dd of=/dev/$fitpart
		echo $rootdev > /tmp/sysupgrade.rootdev
		;;
	linksys,e8450-ubi|\
	mediatek,mt7622,ubi)
		CI_KERNPART="fit"
		nand_do_upgrade "$1"
		;;
	linksys,e8450)
		if grep -q mtdparts=slave /proc/cmdline; then
			PART_NAME=firmware2
		else
			PART_NAME=firmware1
		fi
		default_do_upgrade "$1"
		;;
	*)
		default_do_upgrade "$1"
		;;
	esac
}

PART_NAME=firmware

platform_check_image() {
	local board=$(board_name)
	local magic="$(get_magic_long "$1")"

	[ "$#" -gt 1 ] && return 1

	case "$board" in
	*)
		[ "$magic" != "d00dfeed" ] && {
			echo "Invalid image type."
			return 1
		}
		return 0
		;;
	esac

	return 0
}

platform_copy_config_mmc() {
	[ -e "$UPGRADE_BACKUP" ] || return
	local rootdev=$(cat /tmp/sysupgrade.rootdev)
	blockdev --rereadpt /dev/$rootdev
	local datadev=$(get_partition $rootdev rootfs_data)
	[ "$datadev" ] || echo "no rootfs_data partition, cannot keep configuration." >&2
	dd if="$UPGRADE_BACKUP" of=/dev/$datadev
	sync
}

platform_copy_config() {
	case "$(board_name)" in
	bananapi,bpi-r64)
		platform_copy_config_mmc
		;;
	esac
}
