include $(INCLUDE_DIR)/image.mk

define Image/Build
ifneq ($(FS),jffs2-64k)
	cp $(KDIR)/root.$(1) $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-$(1).img
	cp $(KDIR)/vmlinux $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-vmlinuz
endif
endef

$(eval $(call BuildImage))
