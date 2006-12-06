define Build/Compile
	$(call Build/Compile/Default)
	$(TARGET_CC) -o $(PKG_BUILD_DIR)/patch-cmdline $(PLATFORM_DIR)/src/patch-cmdline.c
endef

define Package/base-files/install-target
	mkdir -p $(1)/sbin
	$(CP) $(PKG_BUILD_DIR)/patch-cmdline $(1)/sbin
endef


