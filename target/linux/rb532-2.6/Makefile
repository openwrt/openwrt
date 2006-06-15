include $(TOPDIR)/rules.mk

LINUX_VERSION:=2.6.16.19
LINUX_RELEASE:=1
LINUX_KERNEL_MD5SUM:=b1e3c65992b0049fdbee825eb2a856af

include ../rules.mk
include ./config

include ../generic-$(KERNEL)/modules.mk
include ../kernel.mk

$(LINUX_DIR)/.patched: $(LINUX_DIR)/.unpacked
	[ -d ../generic-$(KERNEL)/patches ] && $(PATCH) $(LINUX_DIR) ../generic-$(KERNEL)/patches
	[ -d ./patches ] && $(PATCH) $(LINUX_DIR) ./patches
	@$(CP) config $(LINUX_DIR)/.config
	touch $@

