openwrt-linux.trx: openwrt-trx
	PATH=$(TARGET_PATH) trx -o openwrt-linux.trx $(LINUX_DIR)/$(LINUX_BINLOC) $(IMAGE).$(ROOTFS)

openwrt-gs-code.bin: openwrt-addpattern openwrt-linux.trx
	PATH=$(TARGET_PATH) addpattern -2 -i  openwrt-linux.trx -o openwrt-gs-code.bin -g

openwrt-g-code.bin: openwrt-gs-code.bin
	sed -e "1s,^W54S,W54G," < openwrt-gs-code.bin > openwrt-g-code.bin

openwrt-image:	openwrt-g-code.bin
