KMOD-IPV6:=kmod-ipv6
KMOD-IPV6_IPK:=$(KMOD-IPV6)_2.4.20-1_mipsel.ipk
KMOD-IPV6_IPK_DIR:=$(BUILD_DIR)/$(KMOD-IPV6)-2.4.20-ipk

$(BUILD_DIR)/$(KMOD-IPV6_IPK):
	mkdir -p $(KMOD-IPV6_IPK_DIR)/CONTROL
	mkdir -p $(KMOD-IPV6_IPK_DIR)/lib/modules/2.4.20/kernel/net/ipv6
	install -m 644 $(OPENWRT_IPK_DIR)/$(KMOD-IPV6)/$(KMOD-IPV6).control $(KMOD-IPV6_IPK_DIR)/CONTROL/control
	install -m 644 $(BUILD_DIR)/linux/modules/lib/modules/2.4.20/kernel/net/ipv6/ipv6.o $(KMOD-IPV6_IPK_DIR)/lib/modules/2.4.20/kernel/net/ipv6
	cd $(BUILD_DIR); $(STAGING_DIR)/bin/ipkg-build -c -o root -g root $(KMOD-IPV6_IPK_DIR)

kmod-ipv6-ipk:	$(BUILD_DIR)/$(KMOD-IPV6_IPK)

kmod-ipv6-distclean:
	rm -rf $(KMOD-IPV6_IPK_DIR)
