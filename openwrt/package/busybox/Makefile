include $(TOPDIR)/rules.mk

include ./busybox.mk

source: busybox-source
prepare: $(BUSYBOX_DIR)/.unpacked
compile: $(BUSYBOX_DIR)/busybox
install: busybox
clean: busybox-dirclean
