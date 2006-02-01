# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=lua
PKG_VERSION:=5.0.2
PKG_RELEASE:=1
PKG_MD5SUM:= dea74646b7e5c621fef7174df83c34b1

PKG_SOURCE_URL:=http://www.lua.org/ftp/ \
	http://ftp.gwdg.de/pub/languages/lua/ \
	http://mirrors.dotsrc.org/lua/ \
	http://www.tecgraf.puc-rio.br/lua/ftp/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,LIBLUA,liblua,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,LUA,lua,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,LUAC,luac,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,LUA_EXAMPLES,lua-examples,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built: $(PKG_BUILD_DIR)/.configured
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC="$(TARGET_CROSS)gcc" \
		LD="$(TARGET_CROSS)ld" \
		AR="$(TARGET_CROSS)ar rcu" \
		RANLIB="$(TARGET_CROSS)ranlib" \
		INSTALL_ROOT=/usr \
		MYCFLAGS="-I$(STAGING_DIR)/usr/include $(TARGET_CFLAGS)" \
		MYLDFLAGS="-L$(STAGING_DIR)/usr/lib" \
		all so
	# remove statically linked binaries, so that they will get linked against shlib this time
	rm -f $(PKG_BUILD_DIR)/bin/lua{,c}
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC="$(TARGET_CROSS)gcc" \
		LD="$(TARGET_CROSS)ld" \
		AR="$(TARGET_CROSS)ar rcu" \
		RANLIB="$(TARGET_CROSS)ranlib" \
		INSTALL_ROOT=/usr \
		MYCFLAGS="-I$(STAGING_DIR)/usr/include $(TARGET_CFLAGS)" \
		MYLDFLAGS="-L$(STAGING_DIR)/usr/lib" \
		all
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		INSTALL_ROOT="$(PKG_INSTALL_DIR)/usr" \
		install soinstall
	touch $@

$(IPKG_LIBLUA):
	install -d -m0755 $(IDIR_LIBLUA)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/liblua{,lib}.so.* $(IDIR_LIBLUA)/usr/lib/
	$(RSTRIP) $(IDIR_LIBLUA)
	$(IPKG_BUILD) $(IDIR_LIBLUA) $(PACKAGE_DIR)

$(IPKG_LUA):
	install -d -m0755 $(IDIR_LUA)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/lua $(IDIR_LUA)/usr/bin/
	$(RSTRIP) $(IDIR_LUA)
	$(IPKG_BUILD) $(IDIR_LUA) $(PACKAGE_DIR)

$(IPKG_LUAC):
	install -d -m0755 $(IDIR_LUAC)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/luac $(IDIR_LUAC)/usr/bin/
	$(RSTRIP) $(IDIR_LUAC)
	$(IPKG_BUILD) $(IDIR_LUAC) $(PACKAGE_DIR)

$(IPKG_LUA_EXAMPLES):
	install -d -m0755 $(IDIR_LUA_EXAMPLES)/usr/share/lua/examples
	install -m0644 $(PKG_BUILD_DIR)/test/*.lua \
		$(IDIR_LUA_EXAMPLES)/usr/share/lua/examples/
	$(RSTRIP) $(IDIR_LUA_EXAMPLES)
	$(IPKG_BUILD) $(IDIR_LUA_EXAMPLES) $(PACKAGE_DIR)

$(STAGING_DIR)/usr/lib/liblua.so: $(PKG_BUILD_DIR)/.built
	mkdir -p $(STAGING_DIR)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/lua{,lib}.h $(STAGING_DIR)/usr/include/
	$(CP) $(PKG_INSTALL_DIR)/usr/include/lauxlib.h $(STAGING_DIR)/usr/include/
	mkdir -p $(STAGING_DIR)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/liblua{,lib}.{a,so*} $(STAGING_DIR)/usr/lib/
	touch $@

install-dev: $(STAGING_DIR)/usr/lib/liblua.so

uninstall-dev:
	rm -rf \
		$(STAGING_DIR)/usr/include/lua{,lib}.h \
		$(STAGING_DIR)/usr/include/lauxlib.h \
		$(STAGING_DIR)/usr/lib/liblua{,lib}.{a,so*} \

compile-targets: install-dev
clean-targets: uninstall-dev
