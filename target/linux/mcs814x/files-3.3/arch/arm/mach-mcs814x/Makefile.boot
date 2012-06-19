    zreladdr-y := 0x00008000
 params_phys-y := 0x00000008
 initrd_phys-y := 0x00400000

dtb-$(CONFIG_MACH_DLAN_USB_EXT) += dlan-usb-extender.dtb
dtb-$(CONFIG_MACH_RBT_832) += rbt-832.dtb
