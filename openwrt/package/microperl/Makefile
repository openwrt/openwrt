# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=microperl
PKG_VERSION:=5.8.6
PKG_RELEASE:=1
PKG_MD5SUM:=8d4aa4e061fd5bc0c39e4f0a63267880

PKG_SOURCE_URL:=ftp://ftp.cpan.org/pub/CPAN/src/5.0 \
	        ftp://ftp.mpi-sb.mpg.de/pub/perl/CPAN/src/5.0 \
	   	ftp://ftp.gmd.de/mirrors/CPAN/src/5.0 \
		ftp://ftp.funet.fi/pub/languages/perl/CPAN/src/5.0
PKG_SOURCE:=perl-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/perl-$(PKG_VERSION)

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,MICROPERL,microperl,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR) -f Makefile.micro \
		CC="$(TARGET_CC)" OPTIMIZE="$(TARGET_CFLAGS)"
	touch $@

$(IPKG_MICROPERL):
	install -d -m0755 $(IDIR_MICROPERL)/usr/bin
	install -m0755 $(PKG_BUILD_DIR)/microperl $(IDIR_MICROPERL)/usr/bin/
	$(RSTRIP) $(IDIR_MICROPERL)
	$(IPKG_BUILD) $(IDIR_MICROPERL) $(PACKAGE_DIR)
