KMOD-IPT6:=kmod-ipt6
KMOD-IPT6_IPK:=$(KMOD-IPT6)_2.4.20-1_mipsel.ipk
KMOD-IPT6_IPK_DIR:=$(BUILD_DIR)/$(KMOD-IPT6)-2.4.20-ipk

$(BUILD_DIR)/$(KMOD-IPT6_IPK):
	mkdir -p $(KMOD-IPT6_IPK_DIR)/CONTROL
	mkdir -p $(KMOD-IPT6_IPK_DIR)/lib/modules/2.4.20/kernel/net/ipv6/netfilter
	install -m 644 $(OPENWRT_IPK_DIR)/$(KMOD-IPT6)/$(KMOD-IPT6).control $(KMOD-IPT6_IPK_DIR)/CONTROL/control
	install -m 644 $(BUILD_DIR)/linux/modules/lib/modules/2.4.20/kernel/net/ipv6/netfilter/* $(KMOD-IPT6_IPK_DIR)/lib/modules/2.4.20/kernel/net/ipv6/netfilter/
	cd $(BUILD_DIR); $(STAGING_DIR)/bin/ipkg-build -c -o root -g root $(KMOD-IPT6_IPK_DIR)

kmod-ipt6-ipk:	$(BUILD_DIR)/$(KMOD-IPT6_IPK)

kmod-ipt6-distclean:
	rm -rf $(KMOD-IPT6_IPK_DIR)
