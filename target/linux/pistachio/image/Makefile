#
# Copyright (c) 2016, Imagination Technologies Limited and/or its affiliated group companies.
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification, are permitted
# provided that the following conditions are met:
# 1. Redistributions of source code must retain the above copyright notice, this list of conditions
#    and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright notice, this list of
#    conditions and the following disclaimer in the documentation and/or other materials
#    provided with the distribution.
# 3. Neither the name of the copyright holder nor the names of its contributors may be used to
#    endorse or promote products derived from this software without specific prior written
#    permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
# FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
# ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

DTS_DIR:=$(LINUX_DIR)

define CompressGzip
	gzip -9 -c $(1) > $(2)
endef

define MkDTB
	$(call Image/BuildDTB,../dts/$(1).dts,$(1).dtb)
	$(CP) $(1).dtb $(BIN_DIR)/$(1).dtb
	$(CP) $(1).dtb $(KDIR)/$(1).dtb
	$(RM) $(1).dtb
endef

define MkImageGzip
	$(CP) $(KDIR)/vmlinux$(2) $(KDIR)/vmlinux$(2)-$(1)
	$(call CompressGzip,$(KDIR)/vmlinux$(2)-$(1),$(KDIR)/vmlinux$(2)-$(1).gz)
	mkimage -A mips -O linux -T kernel -a 0x84000000 -C gzip \
			-e 0x84000000 -n 'MIPS OpenWrt Linux-$(LINUX_VERSION)' \
			-d $(KDIR)/vmlinux$(2)-$(1).gz $(KDIR)/uImage-$(1)$(2)
endef

define Image/Build/ubifs

ifneq ($($(PROFILE)_UBIFS_OPTS),)
	$(CP) $(KDIR)/root.ubifs $(BIN_DIR)/$(IMG_PREFIX)-$(PROFILE)-rootfs.ubifs
endif

endef

define Image/Build/ubi

ifneq ($($(PROFILE)_UBI_OPTS),)
	$(CP) $(KDIR)/root.ubi $(BIN_DIR)/$(IMG_PREFIX)-$(PROFILE)-rootfs.ubi
endif

endef

define Image/BuildNAND/ubifs

ifneq ($($(PROFILE)_UBIFS_OPTS),)
ifneq ($($(PROFILE)_UBI_OPTS),)
	$(call Image/Build/UbinizeImage,$(PROFILE),,ubifs,$($(PROFILE)_UBI_OPTS))
	$(CP) $(KDIR)/$(IMG_PREFIX)-$(PROFILE)-ubifs-ubinized.bin $(BIN_DIR)/$(IMG_PREFIX)-$(PROFILE)-ubifs.img
endif
endif

endef

define Image/Build
	$(call Image/BuildNAND/$(1),$(1),$(PROFILE))
endef

define Image/BuildKernel/uImage
	$(call MkImageGzip,$(1))
	$(CP) $(KDIR)/uImage-$(1) $(BIN_DIR)/$(IMG_PREFIX)-$(1)-uImage
ifneq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),)
	$(call MkImageGzip,$(1),-initramfs)
	$(CP) $(KDIR)/uImage-$(1)-initramfs $(BIN_DIR)/$(IMG_PREFIX)-$(1)-uImage-initramfs
endif
endef

define Image/BuildKernel
	$(call MkDTB,$($(PROFILE)_DEVICE_DTS))
	$(call Image/BuildKernel/uImage,$(PROFILE))
endef

define Image/InstallKernel/copy
#UImage and DTB must always be in FS
#ifneq ($(CONFIG_TARGET_ROOTFS_INCLUDE_KERNEL),)
	$(INSTALL_DIR) $(TARGET_DIR)/boot

#ifneq ($(CONFIG_TARGET_ROOTFS_INCLUDE_UIMAGE),)
	$(CP) $(BIN_DIR)/$(IMG_PREFIX)-$(1)-uImage $(TARGET_DIR)/boot/
	ln -sf boot/$(IMG_PREFIX)-$(1)-uImage $(TARGET_DIR)/uImage
#endif

#endif

#ifneq ($(CONFIG_TARGET_ROOTFS_INCLUDE_DTB),)
	$(INSTALL_DIR) $(TARGET_DIR)/boot

ifneq ($(1),)
	$(CP) $(KDIR)/$($(PROFILE)_DEVICE_DTS).dtb $(TARGET_DIR)/boot/
	ln -sf boot/$($(PROFILE)_DEVICE_DTS).dtb $(TARGET_DIR)/pistachio_$(BOARDNAME).dtb
endif
#endif

endef

define Image/InstallKernel
        $(call Image/InstallKernel/copy,$(PROFILE))
endef

$(eval $(call BuildImage))
