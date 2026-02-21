include $(TOPDIR)/rules.mk

PKG_NAME:=gnupg
PKG_VERSION:=2.4.9
PKG_SOURCE:=gnupg-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=https://gnupg.org/ftp/gcrypt/gnupg
PKG_HASH:=dd17ab2e9a04fd79d39d853f599cbc852062ddb9ab52a4ddeb4176fd8b302964

HOST_BUILD_DEPENDS:=npth/host libgpg-error/host libgcrypt/host libassuan/host libksba/host

HOST_BUILD_DIR:=$(BUILD_DIR_HOST)/gnupg-$(PKG_VERSION)

include $(INCLUDE_DIR)/host-build.mk

define Host/Configure
    $(call Host/Configure/Default, \
	--disable-card-support \
	--disable-ccid-driver \
	--disable-dependency-tracking \
	--disable-dirmngr \
	--disable-dirmngr-auto-start \
	--disable-doc \
	--disable-exec \
	--disable-gpgsm \
	--disable-gpgtar \
	--disable-gnutls \
	--disable-keyboxd \
	--disable-largefile \
	--disable-ldap \
	--disable-libdns \
	--disable-nls \
	--disable-ntbtls \
	--disable-optimization \
	--disable-photo-viewers \
	--disable-rpath \
	--disable-scdaemon \
	--disable-silent-rules \
	--disable-sqlite \
	--disable-tests \
	--disable-tpm2d \
	--disable-tofu \
	--disable-wks-tools \
	--with-libgpg-error-prefix=$(STAGING_DIR_HOST) \
	--with-libgcrypt-prefix=$(STAGING_DIR_HOST) \
	--with-libassuan-prefix=$(STAGING_DIR_HOST) \
	--with-ksba-prefix=$(STAGING_DIR_HOST) \
	--with-npth-prefix=$(STAGING_DIR_HOST) \
    )
endef

define Host/Install
	$(INSTALL_DIR) $(STAGING_DIR_HOST)/bin
	$(INSTALL_BIN) $(HOST_BUILD_DIR)/g10/gpgv $(STAGING_DIR_HOST)/bin/gpgv
	$(INSTALL_BIN) $(HOST_BUILD_DIR)/g10/gpg $(STAGING_DIR_HOST)/bin/gpg
endef

$(eval $(call HostBuild))
