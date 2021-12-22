define Device/aerohive_hiveap-330
  DEVICE_VENDOR := Aerohive
  DEVICE_MODEL := HiveAP-330
  DEVICE_PACKAGES := kmod-tpm-i2c-atmel
  BLOCKSIZE := 128k
  KERNEL := kernel-bin | uImage none
  KERNEL_INITRAMFS := kernel-bin | uImage none
  KERNEL_SIZE := 16m
  IMAGES := fdt.bin sysupgrade.bin
  IMAGE/fdt.bin := append-dtb
  IMAGE/sysupgrade.bin := append-dtb | pad-to 256k | append-kernel | \
	append-rootfs | pad-rootfs | check-size | append-metadata
  IMAGE_SIZE = 63m
  DEVICE_COMPAT_VERSION := 2.0
  DEVICE_COMPAT_MESSAGE := The partitioning of the HiveAP 330 has changed. \n$\
    To upgrade to this version, please: \n$\
    1) Be running OpenWrt 21.02, \n$\
    2) Paste and run this script into an SSH shell and follow its instructions: \n$\
\n$\
cat << 'EOF' > /tmp/fix-uboot.sh; chmod +x /tmp/fix-uboot.sh; /tmp/fix-uboot.sh \n$\
$$(shell echo -ne '\x23')!/bin/sh -e \n$\
. /lib/functions.sh \n$\
. /lib/functions/system.sh \n$\
opkg update && opkg install uboot-envtools kmod-mtd-rw || { echo "Cannot install packages; aborting" && false; } \n$\
insmod mtd-rw i_want_a_brick=y \n$\
[ -s "/etc/fw_env.config" ] || echo "/dev/mtd$$$$(find_mtd_index u-boot-env) 0x0 0x20000 0x10000" > "/etc/fw_env.config" \n$\
mtd=/dev/mtd"$$$$(find_mtd_index u-boot)"; \n$\
dd if=$$$$mtd of=/tmp/u-boot \n$\
fw_setenv owrt_boot 'bootm 0xEC040000 - 0xEC000000;' \n$\
cp /tmp/u-boot /tmp/u-boot_patched \n$\
strings -td < /tmp/u-boot | grep '^ *[0-9]* *\\(run owrt_boot\\|setenv bootargs\\).*cp\\.l' | \n$\
        awk '{print $$$$1}' | \n$\
        while read offset; do \n$\
                echo -n "run owrt_boot;            " | dd of=/tmp/u-boot_patched bs=1 seek=$$$${offset} conv=notrunc \n$\
        done; \n$\
mtd write /tmp/u-boot_patched u-boot \n$\
uci set system.@system[0].compat_version=2.0; uci commit; \n$\
echo "Done; you may continue your sysupgrade. Note that if this fails, \n$\
you will need to use the serial console to re-install OpenWrt. \n$\
Note that after this sysupgrade, the AP will be unavailable for 7 \n$\
minutes to reformat flash." \n$\
echo "If you do not want to continue with your sysupgrade, run: \n$\
fw_setenv owrt_boot 'cp.l 0xEC040000 0x2000000 0x500000; bootm 0xEE840000 0x2000000 0xEC000000;'\n$\
uci set system.@system[0].compat_version=1.0; uci commit;"\n$\
EOF\n

endef
TARGET_DEVICES += aerohive_hiveap-330

define Device/enterasys_ws-ap3710i
  DEVICE_VENDOR := Enterasys
  DEVICE_MODEL := WS-AP3710i
  BLOCKSIZE := 128k
  KERNEL = kernel-bin | lzma | fit lzma $(KDIR)/image-$$(DEVICE_DTS).dtb
  IMAGES := sysupgrade.bin
  IMAGE/sysupgrade.bin := append-kernel | append-rootfs | pad-rootfs | append-metadata
endef
TARGET_DEVICES += enterasys_ws-ap3710i

define Device/ocedo_panda
  DEVICE_VENDOR := OCEDO
  DEVICE_MODEL := Panda
  DEVICE_PACKAGES := kmod-rtc-ds1307
  KERNEL = kernel-bin | gzip | fit gzip $(KDIR)/image-$$(DEVICE_DTS).dtb
  PAGESIZE := 2048
  SUBPAGESIZE := 512
  BLOCKSIZE := 128k
  IMAGES := fdt.bin sysupgrade.bin
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
  IMAGE/fdt.bin := append-dtb
endef
TARGET_DEVICES += ocedo_panda

