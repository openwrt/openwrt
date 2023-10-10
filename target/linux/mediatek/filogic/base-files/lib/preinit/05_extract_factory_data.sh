. /lib/functions/system.sh

predator_w6_factory_extract() {
	local mmc_part

	mmc_part="$(find_mmc_part qcidata)"

	mkdir -p /var/qcidata/data
	mkdir -p /var/qcidata/mount

	mount -r "$mmc_part" /var/qcidata/mount

	cp /var/qcidata/mount/factory/*MAC "/var/qcidata/data/"
	umount "/var/qcidata/mount"
}

preinit_extract_factory() {
	case $(board_name) in
	acer,predator-w6)
		predator_w6_factory_extract
		;;
	esac
}

boot_hook_add preinit_main preinit_extract_factory
