include $(TOPDIR)/rules.mk

PKG_NAME:=python3
PKG_VERSION:=3.10.7
PKG_RELEASE:=1

PKG_CPE_ID:=cpe:/a:python:python

PKG_SOURCE:=Python-$(PKG_VERSION).tar.xz
PKG_SOURCE_URL:=https://www.python.org/ftp/python/$(PKG_VERSION)
PKG_HASH:=6eed8415b7516fb2f260906db5d48dd4c06acc0cb24a7d6cc15296a604dcdc48

HOST_BUILD_PARALLEL:=1

HOST_BUILD_DIR:=$(BUILD_DIR_HOST)/Python-$(PKG_VERSION)

include $(INCLUDE_DIR)/host-build.mk

HOSTCC := $(HOSTCC_NOCACHE)

HOST_CONFIGURE_ARGS += \
	--with-system-expat=$(STAGING_DIR_HOST) \
	--disable-shared \
	--disable-test-modules

define Host/Install
	$(call Host/Install/Default)
	$(LN) python3 $(STAGING_DIR_HOST)/bin/python
	$(LN) pip3 $(STAGING_DIR_HOST)/bin/pip
endef

ifeq ($(CONFIG_HOST_PYTHON_MODULES),y)
  Hooks/HostInstall/Post += Install/Modules
endif

define Install/Modules
	-$(STAGING_DIR_HOST)/bin/pip install wheel
	-$(STAGING_DIR_HOST)/bin/pip install cryptography
endef

$(eval $(call HostBuild))
