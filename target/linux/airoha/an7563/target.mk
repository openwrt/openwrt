ARCH:=arm
SUBTARGET:=an7563
BOARDNAME:=AN7563
CPU_TYPE:=cortex-a7
KERNELNAME:=Image dtbs
FEATURES+=source-only
DEPENDS+=@+BUSYBOX_DEFAULT_DEVMEM \
	@+KERNEL_DEVMEM \
	@+PACKAGE_rpcd \
	@+PACKAGE_rpcd-mod-file \
	@+PACKAGE_rpcd-mod-luci \
	@+PACKAGE_rpcd-mod-rrdns \
	@+PACKAGE_rpcd-mod-ucode \
	@+PACKAGE_uhttpd \
	@+PACKAGE_uhttpd-mod-ubus \
	@+PACKAGE_uhttpd-mod-ucode \
	@+PACKAGE_ucode-mod-html \
	@+PACKAGE_ucode-mod-log \
	@+PACKAGE_ucode-mod-math

define Target/Description
	Build firmware images for Airoha an7563 (ARMv8 Cortex-A53 running
	in AArch32 mode) based boards.
endef
