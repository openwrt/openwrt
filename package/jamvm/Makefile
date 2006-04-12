# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=jamvm
PKG_VERSION:=1.4.1
PKG_RELEASE:=1
PKG_MD5SUM:=ed79d6b60e50b007614831f8c28321ac

PKG_SOURCE_URL:=@SF/jamvm

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,JAMVM,$(PKG_NAME),$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured: $(PKG_BUILD_DIR)/.prepared
	(cd $(PKG_BUILD_DIR); \
         $(TARGET_CONFIGURE_OPTS) \
         CFLAGS="$(TARGET_CFLAGS)" \
         CPPFLAGS="-I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/include" \
         LDFLAGS="-L$(STAGING_DIR)/usr/lib -L$(STAGING_DIR)/lib" \
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
                  --enable-ffi \
        );
	touch $@


$(PKG_BUILD_DIR)/.built:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)/usr/bin
	mkdir -p $(PKG_INSTALL_DIR)/usr/share/jamvm
	$(MAKE) -C $(PKG_BUILD_DIR)/src \
              $(TARGET_CONFIGURE_OPTS)
	$(CP) $(PKG_BUILD_DIR)/src/jamvm $(PKG_INSTALL_DIR)/usr/bin
	$(MAKE) -C $(PKG_BUILD_DIR)/lib
	$(CP) $(PKG_BUILD_DIR)/lib/classes.zip $(PKG_INSTALL_DIR)/usr/share/jamvm
	touch $@

$(IPKG_JAMVM):
	install -d -m0755 $(IDIR_JAMVM)/usr/bin
	install -d -m0755 $(IDIR_JAMVM)/usr/share/jamvm
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/jamvm $(IDIR_JAMVM)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/share/jamvm/classes.zip $(IDIR_JAMVM)/usr/share/jamvm
	$(RSTRIP) $(IDIR_JAMVM)
	$(IPKG_BUILD) $(IDIR_JAMVM) $(PACKAGE_DIR)

mostlyclean:
	make -C $(PKG_BUILD_DIR) clean
	rm $(PKG_BUILD_DIR)/.built 
