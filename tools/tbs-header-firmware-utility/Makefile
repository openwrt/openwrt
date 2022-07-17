include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/host-build.mk
include $(INCLUDE_DIR)/kernel.mk
PKG_NAME:=tbs-header-firmware-utility

define cc
	$(HOSTCC) $(HOST_CFLAGS) -include endian.h $(HOST_LDFLAGS) -o $(HOST_BUILD_DIR)/bin/$(firstword $(1)) $(foreach src,$(1),src/$(src).c) $(2)
endef
define Host/Compile
	mkdir -p $(HOST_BUILD_DIR)/bin
	$(call cc,tbs_dlink)
endef
define Host/Install
	$(INSTALL_BIN) $(HOST_BUILD_DIR)/bin/* $(STAGING_DIR_HOST)/bin/
endef

$(eval $(call HostBuild))