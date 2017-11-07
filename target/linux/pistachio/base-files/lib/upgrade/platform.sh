. /lib/functions/pistachio.sh

platform_check_image()
{
	local board=$(pistachio_board_name)
	nand_do_platform_check "$board" "$1"
        return $?
}

platform_pre_upgrade()
{
        export RAMFS_COPY_BIN="${RAMFS_COPY_BIN} /usr/sbin/fw_printenv /usr/sbin/fw_setenv"
        export RAMFS_COPY_BIN="${RAMFS_COPY_BIN} /bin/mkdir /bin/touch"
        export RAMFS_COPY_DATA="${RAMFS_COPY_DATA} /etc/fw_env.config /var/lock/fw_printenv.lock"
}

platform_do_upgrade()
{
	mkdir -p /var/lock
        touch /var/lock/fw_printenv.lock

        boot_partition=`fw_printenv -n boot_partition`
        echo "Current boot partiton " $boot_partition

        if [ "$boot_partition" == "1" ]
        then
                CI_UBIPART="firmware0"
                fw_setenv boot_partition 0
        else
                CI_UBIPART="firmware1"
                fw_setenv boot_partition 1
        fi

        echo "Writing image to " $CI_UBIPART

        nand_upgrade_ubinized $1;
}
