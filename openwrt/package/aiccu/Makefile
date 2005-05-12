include $(TOPDIR)/rules.mk

PKG_NAME:=aiccu
PKG_VERSION:=2005.01.31
PKG_RELEASE:=1
PKG_MD5SUM:=7c3da5feab3d59fb5a99a45203e0ca56

PKG_SOURCE_URL:=http://www.sixxs.net/archive/sixxs/aiccu/unix
PKG_SOURCE:=$(PKG_NAME)_$(PKG_VERSION).tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)
PKG_CAT:=zcat

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,AICCU,aiccu,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.built:
	$(SED) "s,strip,$(STRIP)," $(PKG_BUILD_DIR)/unix-console/Makefile
	$(MAKE) CC=$(TARGET_CC) CFLAGS="$(TARGET_CFLAGS) -D_GNU_SOURCE" DEBUG=0 -C $(PKG_BUILD_DIR)
	touch $@

$(IPKG_AICCU):
	mkdir -p $(IDIR_AICCU)/usr/sbin $(IDIR_AICCU)/etc/init.d
	install -m 755 $(PKG_BUILD_DIR)/unix-console/$(PKG_NAME) $(IDIR_AICCU)/usr/sbin/
	install -m 755 ./files/S51aiccu $(IDIR_AICCU)/etc/init.d/
	install -m 644 $(PKG_BUILD_DIR)/doc/aiccu.conf $(IDIR_AICCU)/etc/aiccu.conf
	$(STRIP) $(IDIR_AICCU)/usr/sbin/*
	$(IPKG_BUILD) $(IDIR_AICCU) $(PACKAGE_DIR)

