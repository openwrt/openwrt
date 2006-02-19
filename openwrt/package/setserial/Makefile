# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=setserial
PKG_VERSION:=2.17
PKG_RELEASE:=1
PKG_MD5SUM:=c4867d72c41564318e0107745eb7a0f2

PKG_SOURCE_URL:=@SF/setserial
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,SETSERIAL,setserial,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); rm -rf config.{cache,status} ; \
		$(TARGET_CONFIGURE_OPTS) \
		./configure \
		  --target=$(GNU_TARGET_NAME) \
		  --host=$(GNU_TARGET_NAME) \
		  --build=$(GNU_HOST_NAME) \
		  --program-prefix="" \
		  --program-suffix="" \
		  --prefix=/usr \
		  --exec-prefix=/usr \
		  --bindir=/usr/bin \
		  --datadir=/usr/share \
		  --includedir=/usr/include \
		  --infodir=/usr/share/info \
		  --libdir=/usr/lib \
		  --libexecdir=/usr/lib \
		  --localstatedir=/var \
		  --mandir=/usr/share/man \
		  --sbindir=/usr/sbin \
		  --sysconfdir=/etc \
		  $(DISABLE_LARGEFILE) \
		  $(DISABLE_NLS) \
	)
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR) \
		all
	touch $@

$(IPKG_SETSERIAL):
	install -d -m0755 $(IDIR_SETSERIAL)/etc/init.d
	install -m0755 ./files/serial.init $(IDIR_SETSERIAL)/etc/init.d/S15serial
	install -d -m0755 $(IDIR_SETSERIAL)/usr/sbin
	install -m0755 $(PKG_BUILD_DIR)/setserial $(IDIR_SETSERIAL)/usr/sbin/
	$(RSTRIP) $(IDIR_SETSERIAL)
	$(IPKG_BUILD) $(IDIR_SETSERIAL) $(PACKAGE_DIR)

mostlyclean:
	$(MAKE) -C $(PKG_BUILD_DIR) \
		clean
	rm -f $(PKG_BUILD_DIR)/.built

