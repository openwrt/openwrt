#############################################################
#
# create openwrt images
#
#############################################################

# wrt tools
WRT_DIR:=$(BUILD_DIR)/wrt-tools
WRT_SOURCE=wrt-tools.tar.gz
WRT_SITE=http://openwrt.openbsd-geek.de

$(DL_DIR)/$(WRT_SOURCE):
	$(WGET) -P $(DL_DIR) $(WRT_SITE)/$(WRT_SOURCE)

$(WRT_DIR)/.unpacked: $(DL_DIR)/$(WRT_SOURCE)
	zcat $(DL_DIR)/$(WRT_SOURCE) | tar -C $(BUILD_DIR) -xvf -
	touch $(WRT_DIR)/.unpacked

wrt-tools: $(WRT_DIR)/.unpacked
	$(CC) -o $(BUILD_DIR)/trx $(WRT_DIR)/trx.c
	$(CC) -o $(BUILD_DIR)/addpattern $(WRT_DIR)/addpattern.c

openwrt-linux.trx.squashfs:	wrt-tools
	$(BUILD_DIR)/trx -o openwrt-linux.trx.squashfs $(LINUX_DIR)/$(LINUX_BINLOC) $(IMAGE).squashfs

openwrt-gs-code.bin.squashfs: openwrt-linux.trx.squashfs
	$(BUILD_DIR)/addpattern -2 -i openwrt-linux.trx.squashfs -o openwrt-gs-code.bin.squashfs -g

openwrt-g-code.bin.squashfs: openwrt-gs-code.bin.squashfs
	sed -e "1s,^W54S,W54G," < openwrt-gs-code.bin.squashfs > openwrt-g-code.bin.squashfs

openwrt-linux.trx.jffs2:	wrt-tools
	$(BUILD_DIR)/trx -o openwrt-linux.trx.jffs2 $(LINUX_DIR)/$(LINUX_BINLOC) $(IMAGE).jffs2

openwrt-gs-code.bin.jffs2: openwrt-linux.trx.jffs2
	$(BUILD_DIR)/addpattern -2 -i openwrt-linux.trx.jffs2 -o openwrt-gs-code.bin.jffs2 -g

openwrt-g-code.bin.jffs2: openwrt-gs-code.bin.jffs2
	sed -e "1s,^W54S,W54G," < openwrt-gs-code.bin.jffs2 > openwrt-g-code.bin.jffs2

openwrt-code.bin.squashfs: openwrt-gs-code.bin.squashfs openwrt-g-code.bin.squashfs

openwrt-code.bin.jffs2: openwrt-gs-code.bin.jffs2 openwrt-g-code.bin.jffs2

