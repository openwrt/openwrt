include $(TOPDIR)/rules.mk

LINUX_VERSION:=2.6.17
LINUX_RELEASE:=1
LINUX_KERNEL_MD5SUM:=37ddefe96625502161f075b9d907f21e

include ../rules.mk
include ./config

include ../generic-$(KERNEL)/modules.mk
include ../kernel.mk

$(LINUX_DIR)/.patched: $(LINUX_DIR)/.unpacked
	[ -d ../generic-$(KERNEL)/patches ] && $(PATCH) $(LINUX_DIR) ../generic-$(KERNEL)/patches
	[ -d ./patches ] && $(PATCH) $(LINUX_DIR) ./patches
	@$(CP) config $(LINUX_DIR)/.config
	touch $@

