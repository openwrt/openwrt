
include $(TOPDIR)/rules.mk

PKG_NAME:=robocfg
PKG_VERSION:=0.01
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/robocfg

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,ROBOCFG,$(PKG_NAME),$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.prepared:
	mkdir -p $@

$(PKG_BUILD_DIR)/.built:
	$(TARGET_CC) -o $(PKG_BUILD_DIR)/robocfg robocfg.c
	$(STRIP) $(PKG_BUILD_DIR)/robocfg
	touch $@

$(IPKG_ROBOCFG):
	mkdir -p $(IDIR_ROBOCFG)/sbin
	install -m 0755 $(PKG_BUILD_DIR)/robocfg $(IDIR_ROBOCFG)/sbin/robocfg
	$(IPKG_BUILD) $(IDIR_ROBOCFG) $(PACKAGE_DIR)
