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

$(LINUX_BUILD_DIR)/zImage.flash.srec: $(LINUX_DIR)/vmlinux
	$(CP) $(LINUX_DIR)/arch/mips/boot/compressed/images/zImage.flash.srec $(LINUX_DIR)/zImage.flash.srec
	$(CP) $(LINUX_DIR)/arch/mips/boot/compressed/images/zImage.srec $(LINUX_DIR)/zImage.srec
	touch $@

compile: $(LINUX_BUILD_DIR)/zImage.flash.srec
