#
# Copyright (C) 2008-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

### DO NOT INDENT LINES CONTAINING $(call xyz) AS THIS MAY CHANGE THE CONTEXT
### OF THE FIRST LINE IN THE CALLED VARIABLE (NOTE: variable!)
### see http://www.gnu.org/software/make/manual/html_node/Call-Function.html#Call-Function
### ACTUALLY IT IS A SIMPLE MACRO EXPANSION

### use round brackets for make variables, and curly brackets for shell variables

define Image/Prepare
### Dummy comment for indented calls of Image/Prepare
	cp $(LINUX_DIR)/arch/arm/boot/uImage $(KDIR)/uImage
endef

define Image/BuildKernel
### Dummy comment for indented calls of Image/BuildKernel
	# Orion Kernel uImages
 # WRT350N v2: mach id 1633 (0x661)
	echo -en "\x06\x1c\xa0\xe3\x61\x10\x81\xe3" > $(KDIR)/wrt350nv2-zImage
	cat $(LINUX_DIR)/arch/arm/boot/zImage >> $(KDIR)/wrt350nv2-zImage
	$(STAGING_DIR_HOST)/bin/mkimage -A arm -O linux -T kernel \
	-C none -a 0x00008000 -e 0x00008000 -n 'Linux-$(LINUX_VERSION)' \
	-d $(KDIR)/wrt350nv2-zImage $(KDIR)/wrt350nv2-uImage
	cp $(KDIR)/wrt350nv2-uImage $(BIN_DIR)/openwrt-wrt350nv2-uImage
 # WNR854T: mach id 1801 (0x709)
	echo -en "\x07\x1c\xa0\xe3\x09\x10\x81\xe3" > $(KDIR)/wnr854t-zImage
	cat $(LINUX_DIR)/arch/arm/boot/zImage >> $(KDIR)/wnr854t-zImage
	$(STAGING_DIR_HOST)/bin/mkimage -A arm -O linux -T kernel \
	-C none -a 0x00008000 -e 0x00008000 -n 'Linux-$(LINUX_VERSION)' \
	-d $(KDIR)/wnr854t-zImage $(KDIR)/wnr854t-uImage
	cp $(KDIR)/wnr854t-uImage $(BIN_DIR)/openwrt-wnr854t-uImage
endef

define Image/Build/Netgear
	# Orion Netgear Images
	mkdir $(KDIR)/netgear_image
	cp $(KDIR)/wnr854t-uImage $(KDIR)/netgear_image/uImage
	$(STAGING_DIR_HOST)/bin/mkfs.jffs2 -m none -p -l -q -e 128KiB -o $(KDIR)/wnr854t-uImage.jffs2 -d $(KDIR)/netgear_image
	rm -rf $(KDIR)/netgear_image
	( \
		dd if=$(KDIR)/wnr854t-uImage.jffs2 bs=1024k conv=sync; \
		dd if=$(KDIR)/root.$(1) bs=128k conv=sync; \
	) > $(BIN_DIR)/openwrt-$(2)-$(1).img
	$(STAGING_DIR_HOST)/bin/add_header $(3) $(BIN_DIR)/openwrt-$(2)-$(1).img $(BIN_DIR)/openwrt-$(2)-$(1)-webupgrade.img
endef

define Image/Build/Linksys
	# Orion Linksys Images
 # sysupgrade image
	( \
		dd if="${KDIR}/$2-uImage" bs=$5 conv=sync; \
		dd if="${KDIR}/root.$1" bs=64k conv=sync; \
	) > "${BIN_DIR}/openwrt-$2-$1.img"
 # recovery image and webupgrade image for stock firmware
	rm -rf "${TMP_DIR}/$2_webupgrade"
	mkdir "${TMP_DIR}/$2_webupgrade"
 # create parameter file
	echo ':image 0 $(BIN_DIR)/openwrt-$(2)-$(1).img' >'$(TMP_DIR)/$(2)_webupgrade/$(2).par'
	[ ! -f "$(STAGING_DIR_HOST)/share/wrt350nv2-builder/u-boot.bin" ] || ( \
		echo ":u-boot 0 $(STAGING_DIR_HOST)/share/wrt350nv2-builder/u-boot.bin" >>"${TMP_DIR}/$2_webupgrade/$2.par"; )
	echo "#version 0x2020" >>"${TMP_DIR}/$2_webupgrade/$2.par"
 # create bin file for recovery and webupgrade image
	( cd "${TMP_DIR}/$2_webupgrade"; \
		"${STAGING_DIR_HOST}/bin/wrt350nv2-builder" \
			-v -b "${TMP_DIR}/$2_webupgrade/$2.par"; \
	)
 # copy bin file as recovery image
	$(CP) "${TMP_DIR}/$2_webupgrade/wrt350n.bin" "$(BIN_DIR)/openwrt-$2-$1-recovery.bin"
 # create webupgrade image for stock firmware update mechanism
	( cd '$(TMP_DIR)/$(2)_webupgrade'; \
		zip 'wrt350n.zip' 'wrt350n.bin'; \
	)
	"${STAGING_DIR_HOST}/bin/wrt350nv2-builder" -v -z "${TMP_DIR}/$2_webupgrade/wrt350n.zip" "${BIN_DIR}/openwrt-$2-$1-webupgrade.img"
	rm -rf "${TMP_DIR}/$2_webupgrade"
endef

define Image/Build
### Dummy comment for indented calls of Image/Build with $(1)
$(call Image/Build/$(1),$(1))
$(call Image/Build/Netgear,$(1),wnr854t,NG_WNR854T,$(1))
 # Leave WRT350Nv2 at last position due to webimage dependency for zip
 # 5th parameter is kernel mtd size, e.g. 0x00100000 = 1048576 or 0x001A0000 = 1703936
$(call Image/Build/Linksys,$(1),wrt350nv2,WNR350Nv2,$(1),1048576)
endef

define Image/Build/squashfs
$(call prepare_generic_squashfs,$(KDIR)/root.squashfs)
	( \
		dd if=$(KDIR)/uImage bs=1024k conv=sync; \
		dd if=$(KDIR)/root.$(1) bs=128k conv=sync; \
	) > $(BIN_DIR)/$(IMG_PREFIX)-$(1).img
endef

# Dependency for WRT350N v2 webupgrade image
$(eval $(call RequireCommand,zip, \
	Please install zip. \
))
