include $(TOPDIR)/rules.mk

include ./openwrt.mk

source: openwrt-source
prepare: $(LINKSYS_SHARED_DIR)/.source
compile: $(TARGET_DIR)/$(LINKSYS_SHARED_TARGET_BINARY)
install: compile openwrt
clean: openwrt-dirclean
