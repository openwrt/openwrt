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

$(LINUX_BUILD_DIR)/bzImage: $(LINUX_DIR)/vmlinux
	$(MAKE) -C $(LINUX_DIR) V=1 CROSS_COMPILE=$(TARGET_CROSS) ARCH=$(LINUX_KARCH) PATH=$(TARGET_PATH) bzImage $(MAKE_TRACE)
	$(CP) $(LINUX_DIR)/vmlinux $@
	touch $@

compile: $(LINUX_DIR)/vmlinux
