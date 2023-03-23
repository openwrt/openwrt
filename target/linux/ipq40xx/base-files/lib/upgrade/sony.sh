. /lib/functions.sh

update_bootconfig() {
	local offset=$1
	local index="$2"
	local cfgpart=$(find_mmc_part "0:BOOTCONFIG")
	local cur_index

	if [ -z "$cfgpart" ]; then
		echo "failed to get the partition: \"0:BOOTCONFIG\""
		return 1
	fi

	cur_index=$(dd if=${cfgpart} bs=1 count=1 skip=$offset 2> /dev/null | hexdump -e '"%d"')
	if [ ${index} != ${cur_index} ]; then
		echo "updating \"0:BOOTCONFIG\""
		echo -en "\x0${index}" | \
			dd of=${cfgpart} bs=1 count=1 seek=$offset conv=notrunc 2>/dev/null
	fi

	# also update 0:BOOTCONFIG1 if exists
	cfgpart=$(find_mmc_part "0:BOOTCONFIG1")
	[ -z "$cfgpart" ] && return

	cur_index=$(dd if=${cfgpart} bs=1 count=1 skip=$offset 2> /dev/null | hexdump -e '"%d"')
	if [ ${index} != ${cur_index} ]; then
		echo "updating \"0:BOOTCONFIG1\""
		echo -en "\x0${index}" | \
			dd of=${cfgpart} bs=1 count=1 seek=$offset conv=notrunc 2>/dev/null
	fi
}

### Note ###
# After the commit bad1835f27ec31dbc30060b03cc714212275168a in fstools,
# p17 (label: "rootfs_data") is mounted as a rootfs_data on boot instead
# of the loop device labeled as "rootfs_data" in p15 (label: "rootfs").
#
# cmdline flag is added to avoid mount "rootfs_data" partition by the
# commit 964d1e3af0e111bad6d393f8a3be702e334c2398 in fstools, but
# NCP-HG100 doesn't use it because it has a large (abount 1.6GB)
# "rootfs_data" partition and the advantage is larger than the
# disadvantages, such as overwriting the stock data in "rootfs_data"
# partition.
sony_emmc_do_upgrade() {
	local tar_file=$1
	local kernel_dev
	local rootfs_dev
	local board_dir

	kernel_dev=$(find_mmc_part "0:HLOS")
	rootfs_dev=$(find_mmc_part "rootfs")
	rootfs_data_dev=$(find_mmc_part "rootfs_data")

	if [ -z "$kernel_dev" ] || [ -z "$rootfs_dev" ] || [ -z "$rootfs_data_dev" ]; then
		echo "The partition name for kernel or rootfs or rootfs_data is not specified or failed to get the mmc device."
		exit 1
	fi

	# use first partitions of kernel/rootfs for NCP-HG100
	# - offset  88 (0x58): 0:HLOS (kernel)
	# - offset 108 (0x6c): rootfs
	update_bootconfig 88 0 || exit 1
	update_bootconfig 108 0 || exit 1

	board_dir=$(tar tf $tar_file | grep -m 1 '^sysupgrade-.*/$')
	board_dir=${board_dir%/}

	echo "Flashing kernel to ${kernel_dev}"
	tar xf $tar_file ${board_dir}/kernel -O > $kernel_dev

	echo "Flashing rootfs to ${rootfs_dev}"
	tar xf $tar_file ${board_dir}/root -O > $rootfs_dev

	echo "Format new rootfs_data"
	mkfs.ext4 -F -L rootfs_data $rootfs_data_dev

	if [ -e "$UPGRADE_BACKUP" ]; then
		mkdir /tmp/new_root
		mount -t ext4 $rootfs_data_dev /tmp/new_root && {
			echo "Saving configurations to rootfs_data"
			cp "$UPGRADE_BACKUP" "/tmp/new_root/$BACKUP_FILE"
			umount /tmp/new_root
		}
	fi

	echo "sysupgrade successful"

	sync
	umount -a
	reboot -f
}
