#
# Copyright (C) 2011 OpenWrt.org
#

PART_NAME=firmware
REQUIRE_IMAGE_METADATA=0

glinet_led_indicator()          
{                    
        while true;do          
                for i in 1 0;do         
                        for led in $@;do                                  
                                echo $i > /sys/class/leds/$led/brightness;
                                sleep 0.1;
                        done
                done
        done
}

redboot_fis_do_upgrade() {
	local append
	local sysup_file="$1"
	local kern_part="$2"
	local magic=$(get_magic_word "$sysup_file")

	if [ "$magic" = "4349" ]; then
		local kern_length=0x$(dd if="$sysup_file" bs=2 skip=1 count=4 2>/dev/null)

		[ -f "$UPGRADE_BACKUP" ] && append="-j $UPGRADE_BACKUP"
		dd if="$sysup_file" bs=64k skip=1 2>/dev/null | \
			mtd -r $append -F$kern_part:$kern_length:0x80060000,rootfs write - $kern_part:rootfs

	elif [ "$magic" = "7379" ]; then
		local board_dir=$(tar tf $sysup_file | grep -m 1 '^sysupgrade-.*/$')
		local kern_length=$(tar xf $sysup_file ${board_dir}kernel -O | wc -c)

		[ -f "$UPGRADE_BACKUP" ] && append="-j $UPGRADE_BACKUP"
		tar xf $sysup_file ${board_dir}kernel ${board_dir}root -O | \
			mtd -r $append -F$kern_part:$kern_length:0x80060000,rootfs write - $kern_part:rootfs

	else
		echo "Unknown image, aborting!"
		return 1
	fi
}

nand_check_support_device()                                                                                 
{                                                                                                           
        local model=""                                                                                      
        json_load "$(cat /tmp/sysupgrade.meta)" || return 1
        json_select supported_devices || {
                #glinet openwrt 18.06 device                      
                model=`awk -F': ' '/machine/ {print tolower($NF)}' /proc/cpuinfo |cut -d  ' ' -f2`
                nand_do_platform_check "$model"  "$1"
                return $?
	}
        json_get_keys dev_keys                                                 
        for k in $dev_keys; do                                                                              
                json_get_var dev "$k"                                                                       
                model=${dev/,/_}                                                                            
                nand_do_platform_check "$model"  "$1" && return 0                                           
        done                                                                                                
        return 1                                                                                            
}

platform_check_image() {
        local board=$(board_name)

        case "$board" in
        glinet,gl-ar300m-nand|\
        glinet,gl-ar750s-nor-nand|\
        glinet,gl-e750-nor-nand|\
        glinet,gl-x1200-nor-nand|\
        glinet,gl-x300b-nor-nand|\
        glinet,gl-x750-nor-nand|\
        glinet,gl-xe300-iot|\
        glinet,gl-xe300-nor-nand)
                nand_check_support_device  "$1"
                return $?
                ;;
        *)
                return 0
                ;;
        esac
}

platform_do_upgrade() {
	local board=$(board_name)

	case "$board" in
	jjplus,ja76pf2)
		redboot_fis_do_upgrade "$1" linux
		;;
	ubnt,routerstation|\
	ubnt,routerstation-pro)
		redboot_fis_do_upgrade "$1" kernel
		;;
	glinet,gl-ar300m-nand|\
	glinet,gl-ar750s-nor-nand|\
	glinet,gl-e750-nor-nand|\
	glinet,gl-x1200-nor-nand|\
	glinet,gl-x300b-nor-nand|\
	glinet,gl-x750-nor-nand)
		nand_do_upgrade "$1"
		;;
	glinet,gl-xe300-iot|\
	glinet,gl-xe300-nor-nand)
		glinet_led_indicator gl-xe300:green:wan gl-xe300:green:lan gl-xe300:green:wlan gl-xe300:green:lte &
		nand_do_upgrade "$1"
		;;
	*)
		default_do_upgrade "$1"
		;;
	esac
}
