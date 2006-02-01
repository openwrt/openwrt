include $(TOPDIR)/rules.mk

LINUX_VERSION:=2.4.32
LINUX_RELEASE:=1
LINUX_KERNEL_MD5SUM:=38f4d0830e95a20f4bfed17622d5557c

include ../rules.mk
include ./config

$(eval $(call KMOD_template,WD1100,wd1100,\
	$(MODULES_DIR)/kernel/drivers/char/wd1100.o \
,CONFIG_WD1100))


include ../generic-$(KERNEL)/modules.mk
include ../kernel.mk


$(LINUX_DIR)/.patched: $(LINUX_DIR)/.unpacked
	[ -d ../generic-$(KERNEL)/patches ] && $(PATCH) $(LINUX_DIR) ../generic-$(KERNEL)/patches $(MAKE_TRACE)
	[ -d ./patches ] && $(PATCH) $(LINUX_DIR) ./patches $(MAKE_TRACE)
	@$(CP) config $(LINUX_DIR)/.config
	touch $@

$(LINUX_BUILD_DIR)/bzImage: $(LINUX_DIR)/vmlinux
	$(MAKE) -C $(LINUX_DIR) TOPDIR=$(LINUX_DIR) ARCH=$(LINUX_KARCH) PATH=$(TARGET_PATH) bzImage $(MAKE_TRACE)
	$(CP) $(LINUX_DIR)/arch/i386/boot/bzImage $@
	touch $@

compile: $(LINUX_BUILD_DIR)/bzImage
