include $(TOPDIR)/rules.mk

include ./dnsmasq.mk

source: dnsmasq-source
prepare: $(DNSMASQ_DIR)/.unpacked
compile: $(DNSMASQ_DIR)/src/dnsmasq
install: dnsmasq
clean: dnsmasq-dirclean
