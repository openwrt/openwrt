define Build/Compile
	$(call Build/Compile/Default)
	$(TARGET_CC) -I $(PLATFORM_DIR)/src -o $(PKG_BUILD_DIR)/jffs2root $(PLATFORM_DIR)/src/jffs2root.c
endef

define Package/base-files/install-target
	rm -f $(1)/etc/config/network
	mkdir -p $(1)/sbin
	$(CP) $(PKG_BUILD_DIR)/jffs2root $(1)/sbin
endef


