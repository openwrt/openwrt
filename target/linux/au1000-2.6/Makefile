include $(TOPDIR)/rules.mk

LINUX_VERSION:=2.6.16.4
LINUX_RELEASE:=1
LINUX_KERNEL_MD5SUM:=cb675279c9711237a06ebb8379a4da27

include ../rules.mk
include ./config

include ../generic-$(KERNEL)/modules.mk
include ../kernel.mk


$(LINUX_DIR)/.patched: $(LINUX_DIR)/.unpacked
	[ -d ../generic-$(KERNEL)/patches ] && $(PATCH) $(LINUX_DIR) ../generic-$(KERNEL)/patches
	[ -d ./patches ] && $(PATCH) $(LINUX_DIR) ./patches
	@$(CP) config $(LINUX_DIR)/.config
	touch $@

$(LINUX_BUILD_DIR)/bzImage: $(LINUX_DIR)/vmlinux
	$(MAKE) -C $(LINUX_DIR) V=1 CROSS_COMPILE=$(TARGET_CROSS) ARCH=$(LINUX_KARCH) PATH=$(TARGET_PATH) bzImage
	$(CP) $(LINUX_DIR)/vmlinux $@
	touch $@

compile: $(LINUX_DIR)/vmlinux
