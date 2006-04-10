include $(TOPDIR)/rules.mk

LINUX_VERSION:=2.6.16.2
LINUX_RELEASE:=1
LINUX_KERNEL_MD5SUM:=d4105a5ba1d9f82acee2c525c65ef969

include ../rules.mk
include ./config

include ../generic-$(KERNEL)/modules.mk
include ../kernel.mk


$(LINUX_DIR)/.patched: $(LINUX_DIR)/.unpacked
	[ -d ../generic-$(KERNEL)/patches ] && $(PATCH) $(LINUX_DIR) ../generic-$(KERNEL)/patches $(MAKE_TRACE)
	[ -d ./patches ] && $(PATCH) $(LINUX_DIR) ./patches $(MAKE_TRACE)
	@$(CP) config $(LINUX_DIR)/.config
	touch $@

