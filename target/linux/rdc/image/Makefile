#
# Copyright (C) 2006-2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

define Image/Prepare
	$(CP) $(LINUX_DIR)/arch/x86/boot/bzImage $(KDIR)/bzImage
endef

define trxalign/jffs2-128k
bs=128k
endef
define trxalign/jffs2-64k
bs=64k
endef
define trxalign/squashfs
bs=1024
endef

define Image/Prepare/squashfs
	$(call prepare_generic_squashfs,$(KDIR)/root.squashfs)
endef

define Image/Build/ar525w
	touch $(BIN_DIR)/openwrt-$(BOARD)-$(1)-$(2).img
	touch $(BIN_DIR)/openwrt-$(BOARD)-$(1)-$(2)-web.img
	dd if=$(KDIR)/root.$(1) of=$(KDIR)/root.tmp $(call trxalign/$(1)) conv=sync
	mv $(KDIR)/root.tmp $(KDIR)/root.$(1)
	$(STAGING_DIR_HOST)/bin/airlink -b 1 -j $(shell bash -c 'echo $$[$(3)]') $(KDIR)/bzImage $(KDIR)/root.$(1) $(BIN_DIR)/openwrt-$(BOARD)-$(1)-$(2).img
	$(STAGING_DIR_HOST)/bin/airlink -e -b 1 -j $(shell bash -c 'echo $$[$(3)]') $(KDIR)/bzImage $(KDIR)/root.$(1) $(BIN_DIR)/openwrt-$(BOARD)-$(1)-$(2)-web.img
endef

define Image/Prepare/sitecom/squashfs
	dd if=/dev/null of=$(KDIR)/root.tmp seek=1 bs=32k
	echo -n "0123456789abcdef0123" >> $(KDIR)/root.tmp
	cat $(KDIR)/bzImage $(KDIR)/root.squashfs >> $(KDIR)/root.tmp
	dd if=$(KDIR)/root.tmp of=$(KDIR)/root2.tmp bs=64k conv=sync
	dd if=$(KDIR)/root2.tmp of=$(KDIR)/root.squashfs bs=1 skip=$$$$(perl -we 'print((-s"$(KDIR)/bzImage")+0x8014)')
	$(RM) $(KDIR)/root.tmp $(KDIR)/root2.tmp
	$(call add_jffs2_mark,$(KDIR)/root.squashfs)
endef

define Image/Build/sitecom
	echo -ne "\0\0" >> $(KDIR)/bzImage
	dd if=$(KDIR)/bzImage of=$(KDIR)/bzImage.tmp bs=4 conv=sync
	dd if=$(KDIR)/bzImage.tmp of=$(KDIR)/bzImage bs=1 count=$$$$(perl -we 'print((-s"$(KDIR)/bzImage.tmp")-2)')
	perl -we 'while(<>){$$$$i.=$$$$_}print pack"v",-(unpack"%v*",$$$$i)' < $(KDIR)/bzImage.tmp >> $(KDIR)/bzImage
	$(RM) $(KDIR)/bzImage.tmp
	$(call Image/Prepare/sitecom/$(1))
	perl -we 'print "CSYS",pack("V",-s"$(KDIR)/bzImage"),pack("V",-s"$(KDIR)/root.$(1)"),"WRRM",pack("V",-s"$(KDIR)/bzImage")' > $(BIN_DIR)/openwrt-$(BOARD)-$(1)-$(2).img
	cat $(KDIR)/bzImage $(KDIR)/root.$(1) >> $(BIN_DIR)/openwrt-$(BOARD)-$(1)-$(2).img
	if [ `perl -we 'print -s"$(BIN_DIR)/openwrt-$(BOARD)-$(1)-$(2).img"'` -gt $$$$((0xffff0000-0xffe08000)) ]; then mv $(BIN_DIR)/openwrt-$(BOARD)-$(1)-$(2).img $(BIN_DIR)/openwrt-$(BOARD)-$(1)-$(2).img.too_big; echo "#ERR image too big"; fi 
endef

define Image/Build/dir450
	touch $(BIN_DIR)/openwrt-$(BOARD)-$(1)-$(2).img
	dd if=$(KDIR)/root.$(1) of=$(KDIR)/root.tmp $(call trxalign/$(1)) conv=sync
	mv $(KDIR)/root.tmp $(KDIR)/root.$(1)
	$(CP) $(KDIR)/root.$(1) $(BIN_DIR)/onlyrootfs-$(BOARD)-$(1)-$(2).img
endef

define Image/Build/g570s
	$(call Image/Prepare/$(1))
	mv $(KDIR)/root.$(1) $(KDIR)/root.tmp
	dd of=$(KDIR)/root.$(1) if=$(KDIR)/root.tmp $(call trxalign/$(1)) conv=sync
	$(CP) $(KDIR)/bzImage $(BIN_DIR)/openwrt-tftp-$(BOARD)-$(1)-$(2).img
	dd of=$(BIN_DIR)/openwrt-tftp-$(BOARD)-$(1)-$(2).img if=/dev/null bs=1024 seek=1024
	cat $(KDIR)/root.$(1) >> $(BIN_DIR)/openwrt-tftp-$(BOARD)-$(1)-$(2).img
	if [ ! -x $(STAGING_DIR_HOST)/bin/zyxel_combine ]; then echo "#ERR Please copy the ZyXEL \"combine\" tool as $(STAGING_DIR_HOST)/bin/zyxel_combine to build."; fi
	sh -c "mkdir $(KDIR)/zyxel.tmp && cd $(KDIR)/zyxel.tmp && mv ../root.tmp initrd.bin && cp ../bzImage kernel.bin && $(STAGING_DIR_HOST)/bin/zyxel_combine -mcf kernel.bin initrd.bin && mv image $(BIN_DIR)/openwrt-$(BOARD)-$(1)-$(2).rmt" || :
	$(RM) -r $(KDIR)/zyxel.tmp
endef

define Image/Prepare/amit/squashfs
	echo -n "0123456789abcdef" > $(KDIR)/root.tmp
	cat $(KDIR)/root.squashfs >> $(KDIR)/root.tmp
	dd if=$(KDIR)/root.tmp of=$(KDIR)/root2.tmp bs=64k conv=sync
	dd if=$(KDIR)/root2.tmp of=$(KDIR)/root.squashfs bs=1 skip=16
	$(RM) $(KDIR)/root.tmp $(KDIR)/root2.tmp
	$(call add_jffs2_mark,$(KDIR)/root.squashfs)
endef

define Image/Build/amit
	$(call Image/Prepare/amit/$(1))
	if [ ! -x $(STAGING_DIR_HOST)/bin/amit_makebin ]; then echo "#ERR Please copy the AMIT \"makebin\" tool from http://mgb111.pradnik.net/ as $(STAGING_DIR_HOST)/bin/amit_makebin to build."; fi
	sh -c "mkdir -p $(KDIR)/amit.tmp/linux_src/arch/i386/boot $(KDIR)/amit.tmp/rom_disk && cd $(KDIR)/amit.tmp && ln -s ../../root.$(1) rom_disk/fs.img && ln -s ../../../../../bzImage linux_src/arch/i386/boot && $(STAGING_DIR_HOST)/bin/amit_makebin && mv upgrade.img $(BIN_DIR)/openwrt-$(BOARD)-$(1)-anas350.bin" || :
	$(RM) -r $(KDIR)/amit.tmp
	$(STAGING_DIR_HOST)/bin/makeamitbin -o $(BIN_DIR)/openwrt-$(BOARD)-$(1)-ar360w3g.bin -1 ALK_ATG001 -2 Atropos linux3g $(KDIR)/bzImage ramdisk3g $(KDIR)/root.$(1)
endef

define Image/Build/Initramfs
	$(CP) $(KDIR)/bzImage $(BIN_DIR)/openwrt-$(BOARD)-ramfs.bzImage
endef

define Image/Build
	$(CP) $(KDIR)/bzImage $(BIN_DIR)/openwrt-$(BOARD).bzImage
	$(call Image/Build/$(SUBTARGET),$(1),$(SUBTARGET),$(patsubst jffs2-%k,%,$(1)))
ifeq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),y)
	$(call Image/Build/Initramfs)
endif
endef

$(eval $(call BuildImage))
