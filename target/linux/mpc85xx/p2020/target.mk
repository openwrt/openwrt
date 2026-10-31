BOARDNAME:=P2020
KERNEL_IMAGES:=zImage.la3000000
FEATURES += boot-part rootfs-part

define Target/Description
	Build firmware images for Freescale P2020 based boards.
endef
