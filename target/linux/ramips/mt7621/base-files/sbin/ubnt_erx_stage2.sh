#!/bin/sh

. /lib/functions.sh

include /lib/upgrade


# Manually handle kernel partitions
CI_KERNPART="none"

BOARD="$(board_name | sed 's/,/_/g')"
tar_file=$IMAGE
board_dir=$( (tar tf "$tar_file" | grep -m 1 '^sysupgrade-.*/$') 2> /dev/null)
board_dir="${board_dir%/}"

ubnt_update_kernel_flag() {
    local UBNT_ERX_KERNEL_INDEX_OFFSET=160
    local factory_mtd=$(find_mtd_part factory)
    if [ -z "$factory_mtd" ]; then
        echo "cannot find factory partition" >&2
        return 1
    fi

    local kernel_index=$(hexdump -s $UBNT_ERX_KERNEL_INDEX_OFFSET -n 1 -e '/1 "%X "' ${factory_mtd})

    if [ $kernel_index = "0" ]; then
        echo "Kernel flag already set to kernel slot 1" >&2
        return 0
    fi

    if ! (echo -e "\x00" | dd of=${factory_mtd} bs=1 count=1 seek=$UBNT_ERX_KERNEL_INDEX_OFFSET); then
        echo 'Failed to update kernel boot index' >&2
        return 1
    fi
}

prepare_kernel(){
    v "Flashing kernel..."
    local kernel1_mtd=$(find_mtd_part kernel1)
    local kernel2_mtd=$(find_mtd_part kernel2)
    if [ -z "$kernel1_mtd" ] || [ -z "$kernel2_mtd" ]; then
        echo "cannot find kernel1 or kernel2 partition" >&2
        exit 1
    fi

    ubnt_update_kernel_flag || exit 1

    local kernel_file="/tmp/$board_dir/kernel"
    local kernel_length=$( (cat "$kernel_file" | wc -c) 2> /dev/null)
    dd if=$kernel_file bs=1024 count=3072 | mtd write - "kernel1"
    if [ $kernel_length -ge 3145728 ]; then
        dd if=$kernel_file bs=1024 skip=3072 | mtd write - "kernel2"
    fi
}

prepare_rootfs(){
    v "Flashing rootfs..."
    local rootfs_file="/tmp/$board_dir/root"
    local rootfs_length=$( (cat "$rootfs_file" | wc -c) 2> /dev/null)
    local rootfs_type="$(identify_tar "$tar_file" "$board_dir/root" "")"

    nand_upgrade_prepare_ubi "$rootfs_length" "$rootfs_type" "" "0" || return 1

    local ubidev="$( nand_find_ubi "$CI_UBIPART" )"
    local root_ubivol="$( nand_find_volume $ubidev "$CI_ROOTPART" )"
    ubiupdatevol /dev/$root_ubivol -s "$rootfs_length" "$rootfs_file"
}

prepare_kernel
prepare_rootfs

v "Rebooting system..."

umount -a
reboot -f
sleep 5
echo b 2>/dev/null >/proc/sysrq-trigger

