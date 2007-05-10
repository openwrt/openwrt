define Build/Compile
	$(call Build/Compile/Default)
endef

define Package/base-files/install-target
	rm -f $(1)/etc/config/network
endef


