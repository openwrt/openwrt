include $(TOPDIR)/rules.mk

LINUX_VERSION:=2.6.17
LINUX_RELEASE:=1
LINUX_KERNEL_MD5SUM:=37ddefe96625502161f075b9d907f21e

include ./config
include $(TOPDIR)/include/kernel.mk
include $(TOPDIR)/include/kernel-build.mk

$(LINUX_DIR)/.patched: $(LINUX_DIR)/.unpacked
	[ -d ../generic-$(KERNEL)/patches ] && $(PATCH) $(LINUX_DIR) ../generic-$(KERNEL)/patches
	[ -d ./patches ] && $(PATCH) $(LINUX_DIR) ./patches
	@$(CP) config $(LINUX_DIR)/.config
	touch $@

$(LINUX_BUILD_DIR)/bzImage: $(LINUX_DIR)/vmlinux
	$(MAKE) -C $(LINUX_DIR) V=1 CROSS_COMPILE=$(TARGET_CROSS) ARCH=$(LINUX_KARCH) PATH=$(TARGET_PATH) bzImage
	$(CP) $(LINUX_DIR)/arch/i386/boot/bzImage $@
	touch $@

compile: $(LINUX_BUILD_DIR)/bzImage
