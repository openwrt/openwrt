# $Id$

# x86 specific kmod packages

$(LINUX_BUILD_DIR)/bzImage: $(LINUX_DIR)/vmlinux
	$(MAKE) -C $(LINUX_DIR) V=1 CROSS_COMPILE=$(TARGET_CROSS) ARCH=$(LINUX_KARCH) PATH=$(TARGET_PATH) bzImage $(MAKE_TRACE)
	cp -f $(LINUX_DIR)/arch/i386/boot/bzImage $@
	touch $@

compile: $(LINUX_BUILD_DIR)/bzImage
