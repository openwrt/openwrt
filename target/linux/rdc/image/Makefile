# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

ifeq ($(CONFIG_LINUX_2_6_23),y)
ARCH_PATH=i386
else
ARCH_PATH=x86
endif

define Image/Prepare
	$(CP) $(LINUX_DIR)/arch/$(ARCH_PATH)/boot/bzImage $(KDIR)/bzImage
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

define Image/Build/ar525w
	touch $(BIN_DIR)/openwrt-$(BOARD)-$(1)-$(2).img
	touch $(BIN_DIR)/openwrt-$(BOARD)-$(1)-$(2)-web.img
	mv $(KDIR)/root.$(1) $(KDIR)/root.tmp
	dd of=$(KDIR)/root.$(1) if=$(KDIR)/root.tmp $(call trxalign/$(1)) conv=sync
	$(RM) $(KDIR)/root.tmp
	$(STAGING_DIR_HOST)/bin/airlink -b 1 -j $(shell bash -c 'echo $$[$(3)]') $(KDIR)/bzImage $(KDIR)/root.$(1) $(BIN_DIR)/openwrt-$(BOARD)-$(1)-$(2).img
	$(STAGING_DIR_HOST)/bin/airlink -e -b 1 -j $(shell bash -c 'echo $$[$(3)]') $(KDIR)/bzImage $(KDIR)/root.$(1) $(BIN_DIR)/openwrt-$(BOARD)-$(1)-$(2)-web.img
endef

define Image/Build/wl153
	ls -l $(KDIR)/bzImage | sed -r 's/^[^[:blank:]]+[[:blank:]]+[^[:blank:]]+[[:blank:]]+[^[:blank:]]+[[:blank:]]+[^[:blank:]]+[[:blank:]]+([^[:blank:]]+).+$$$$/\1/' | xargs printf '%.8x' > $(KDIR)/bzSize
	gzip -9c $(KDIR)/root.$(1) > $(KDIR)/root.$(1).gz
	ls -l $(KDIR)/root.$(1).gz | sed -r 's/^[^[:blank:]]+[[:blank:]]+[^[:blank:]]+[[:blank:]]+[^[:blank:]]+[[:blank:]]+[^[:blank:]]+[[:blank:]]+([^[:blank:]]+).+$$$$/\1/' | xargs printf '%.8x' > $(KDIR)/rdSize
	echo -ne "\x$$$$(cut -c 7,8 < $(KDIR)/bzSize)\x$$$$(cut -c 5,6 < $(KDIR)/bzSize)\x$$$$(cut -c 3,4 < $(KDIR)/bzSize)\x$$$$(cut -c 1,2 < $(KDIR)/bzSize)" > $(KDIR)/bzSize.tmp
	echo -n 'CSYS' > $(BIN_DIR)/openwrt-$(BOARD)-$(1)-$(2).img
	cat $(KDIR)/bzSize.tmp >> $(BIN_DIR)/openwrt-$(BOARD)-$(1)-$(2).img
	echo -ne "\x$$$$(cut -c 7,8 < $(KDIR)/rdSize)\x$$$$(cut -c 5,6 < $(KDIR)/rdSize)\x$$$$(cut -c 3,4 < $(KDIR)/rdSize)\x$$$$(cut -c 1,2 < $(KDIR)/rdSize)WRRM" >> $(BIN_DIR)/openwrt-$(BOARD)-$(1)-$(2).img
	cat $(KDIR)/bzSize.tmp $(KDIR)/bzImage $(KDIR)/root.$(1).gz >> $(BIN_DIR)/openwrt-$(BOARD)-$(1)-$(2).img
	$(RM) $(KDIR)/bzSize.tmp $(KDIR)/bzSize $(KDIR)/rdSize
	if [ `ls -l $(BIN_DIR)/openwrt-$(BOARD)-$(1)-$(2).img | sed -r 's/^[^[:blank:]]+[[:blank:]]+[^[:blank:]]+[[:blank:]]+[^[:blank:]]+[[:blank:]]+[^[:blank:]]+[[:blank:]]+([^[:blank:]]+).+$$$$/\1/'` -gt $$$$((0xffff0000-0xffe08000)) ]; then mv $(BIN_DIR)/openwrt-$(BOARD)-$(1)-$(2).img $(BIN_DIR)/openwrt-$(BOARD)-$(1)-$(2).img.too_big; echo "#ERR image too big"; fi 
endef

define Image/Build/dir450
	touch $(BIN_DIR)/openwrt-$(BOARD)-$(1)-$(2).img
	mv $(KDIR)/root.$(1) $(KDIR)/root.tmp
	dd of=$(KDIR)/root.$(1) if=$(KDIR)/root.tmp $(call trxalign/$(1)) conv=sync
	$(RM) $(KDIR)/root.tmp
	cp $(KDIR)/root.$(1) $(BIN_DIR)/onlyrootfs-$(BOARD)-$(1)-$(2).img
endef

define Image/Build
	$(CP) $(KDIR)/bzImage $(BIN_DIR)/openwrt-$(BOARD).bzImage
	$(call Image/Build/$(PROFILE),$(1),$(PROFILE),$(patsubst jffs2-%k,%,$(1)))
endef

$(eval $(call BuildImage))
