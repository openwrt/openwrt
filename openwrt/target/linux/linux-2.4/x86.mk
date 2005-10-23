# $Id$

# x86 specific kmod packages

$(eval $(call KMOD_template,WD1100,wd1100,\
	$(MODULES_DIR)/kernel/drivers/char/wd1100.o \
,CONFIG_WD1100))


$(LINUX_BUILD_DIR)/bzImage: $(LINUX_DIR)/vmlinux
	$(MAKE) -C $(LINUX_DIR) TOPDIR=$(LINUX_DIR) ARCH=$(LINUX_KARCH) PATH=$(TARGET_PATH) bzImage $(MAKE_TRACE)
	cp -f $(LINUX_DIR)/arch/i386/boot/bzImage $@
	touch $@

compile: $(LINUX_BUILD_DIR)/bzImage
