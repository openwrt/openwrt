
define image_template

$(BIN_DIR)/openwrt-$(3)-$(2)-$(1).trx:
	$$(call $(1)_template,$(BUILD_DIR)/linux-$(2)-$(3))

ifeq ($(BR2_LINUX_$(4)),y)
install: $(BIN_DIR)/openwrt-$(3)-$(2)-$(1).trx
endif

endef

$(eval $(foreach fs,$(FILESYSTEMS),$(call image_template,$(fs),2.4,brcm,2_4_BRCM)))
