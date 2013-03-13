define Package/base-files/install-target
	rm -f $(1)/etc/config/network
	$(if $(CONFIG_PROCD_INIT),rm -f  $(1)/etc/uci-defaults/00_inittab-console-fixup)
	$(if $(CONFIG_PROCD_INIT),echo "::askconsole:/bin/ash --login" >> $(1)/etc/inittab)
endef


