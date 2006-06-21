include $(TOPDIR)/rules.mk

LINUX_VERSION:=2.4.32
LINUX_RELEASE:=1
LINUX_KERNEL_MD5SUM:=38f4d0830e95a20f4bfed17622d5557c

include ./config
include $(TOPDIR)/include/kernel.mk
include $(TOPDIR)/include/kernel-build.mk

$(LINUX_DIR)/.patched: $(LINUX_DIR)/.unpacked
	[ -d ../generic-$(KERNEL)/patches ] && $(PATCH) $(LINUX_DIR) ../generic-$(KERNEL)/patches
	[ -d ./patches ] && $(PATCH) $(LINUX_DIR) ./patches
	@$(CP) config $(LINUX_DIR)/.config
	touch $@

