. /lib/functions/system.sh

preinit_set_mac_address() {
	case $(board_name) in
	acer,predator-w6|\
	acer,predator-w6d)
		$(mmc_get_mac_ascii u-boot-env WANMAC)
		$(mmc_get_mac_ascii u-boot-env LANMAC)
		ip link set dev lan1 address "$lan_mac"
		ip link set dev lan2 address "$lan_mac"
		ip link set dev lan3 address "$lan_mac"
		ip link set dev game address "$lan_mac"
		ip link set dev eth1 address "$wan_mac"
		;;
	acer,vero-w6m)
		wan_mac=$(mmc_get_mac_ascii u-boot-env WANMAC)
		lan_mac=$(mmc_get_mac_ascii u-boot-env LANMAC)
		ip link set dev lan1 address "$lan_mac"
		ip link set dev lan2 address "$lan_mac"
		ip link set dev lan3 address "$lan_mac"
		ip link set dev internet address "$wan_mac"
		;;
	asus,tuf-ax4200|\
	asus,tuf-ax6000)
		CI_UBIPART="UBI_DEV"
		addr=$(mtd_get_mac_binary_ubi "Factory" 0x4)
		ip link set dev eth0 address "$addr"
		ip link set dev eth1 address "$addr"
		;;
	mercusys,mr90x-v1|\
	tplink,re6000xd)
		addr=$(get_mac_binary "/tmp/tp_data/default-mac" 0)
		ip link set dev eth1 address "$(macaddr_add $addr 1)"
		;;
	xiangsi,hg3000-emmc)
		base_mac=$(mmc_get_mac_binary factory 0x04)
		lan1_mac=$(macaddr_add $base_mac 1)
		lan2_mac=$(macaddr_add $base_mac 2)
		lan3_mac=$(macaddr_add $base_mac 3)
		lan4_mac=$(macaddr_add $base_mac 4)
		eth1_mac=$(macaddr_add $base_mac 6)
		ip link set dev lan1 address "$lan1_mac"
		ip link set dev lan2 address "$lan2_mac"
		ip link set dev lan3 address "$lan3_mac"
		ip link set dev lan4 address "$lan4_mac"
		ip link set dev eth1 address "$eth1_mac"
		;;
	xiangsi,hg3000-nand)
		# base_mac=$(mtd_get_mac_binary factory 0x04)
		base_mac=$(get_mac_binary "/proc/device-tree/soc/ethernet@15100000/mac@0/mac-address" 0)
		lan1_mac=$(macaddr_add $base_mac 1)
		lan2_mac=$(macaddr_add $base_mac 2)
		lan3_mac=$(macaddr_add $base_mac 3)
		lan4_mac=$(macaddr_add $base_mac 4)
		eth1_mac=$(macaddr_add $base_mac 6)
		ip link set dev lan1 address "$lan1_mac"
		ip link set dev lan2 address "$lan2_mac"
		ip link set dev lan3 address "$lan3_mac"
		ip link set dev lan4 address "$lan4_mac"
		ip link set dev eth1 address "$eth1_mac"
		;;
	*)
		;;
	esac
}

boot_hook_add preinit_main preinit_set_mac_address
