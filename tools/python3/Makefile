include $(TOPDIR)/rules.mk

PKG_NAME:=python3
PKG_VERSION:=3.10.7
PKG_RELEASE:=1

PIP_VERSION:=25.3
SETUP_VERSION:=80.9.0
WHEEL_VERSION:=0.45.1
CRYPT_VERSION:=46.0.3

PKG_CPE_ID:=cpe:/a:python:python

PKG_SOURCE:=Python-$(PKG_VERSION).tar.xz
PKG_SOURCE_URL:=https://www.python.org/ftp/python/$(PKG_VERSION)
PKG_HASH:=6eed8415b7516fb2f260906db5d48dd4c06acc0cb24a7d6cc15296a604dcdc48

HOST_BUILD_PARALLEL:=1

PKG_PROGRAMS = \
	python3 \
	python3-config \
	pydoc3 \
	pip3 \
	idle3 \
	2to3 \
	python3.$(word 2,$(subst ., ,$(PKG_VERSION))) \
	python3.$(word 2,$(subst ., ,$(PKG_VERSION)))-config \
	pydoc3.$(word 2,$(subst ., ,$(PKG_VERSION))) \
	pip3.$(word 2,$(subst ., ,$(PKG_VERSION))) \
	idle3.$(word 2,$(subst ., ,$(PKG_VERSION))) \
	2to3-3.$(word 2,$(subst ., ,$(PKG_VERSION))) \
	python \
	pip

ifeq ($(CONFIG_HOST_PYTHON_MODULES),y)
  PKG_PROGRAMS += wheel
endif

HOST_BUILD_DIR:=$(BUILD_DIR_HOST)/Python-$(PKG_VERSION)

include $(INCLUDE_DIR)/host-build.mk

HOSTCC := $(HOSTCC_NOCACHE)

HOST_CONFIGURE_ARGS += \
	--with-system-expat=$(STAGING_DIR_HOST) \
	--disable-shared \
	--disable-test-modules

HOST_PIP_ARGS = \
	--prefix $(STAGING_DIR_HOST) \
	--cache-dir $(TMP_DIR) \
	--progress-bar off \
	--upgrade \
	--verbose

define Host/Install
	$(call Host/Install/Default)
	$(LN) python3 $(STAGING_DIR_HOST)/bin/python
	$(LN) pip3 $(STAGING_DIR_HOST)/bin/pip
endef

ifeq ($(CONFIG_HOST_PYTHON_MODULES),y)
  Hooks/HostInstall/Post += Install/Modules
endif

define Install/Modules
	-$(STAGING_DIR_HOST)/bin/pip install $(HOST_PIP_ARGS) --no-binary :all: pip==$(PIP_VERSION)
	-$(STAGING_DIR_HOST)/bin/pip install $(HOST_PIP_ARGS) --no-binary :all: setuptools==$(SETUP_VERSION)
	-$(STAGING_DIR_HOST)/bin/pip install $(HOST_PIP_ARGS) --no-binary :all: wheel==$(WHEEL_VERSION)
	-$(STAGING_DIR_HOST)/bin/pip install $(HOST_PIP_ARGS) --no-binary :all: cryptography==$(CRYPT_VERSION)
endef

define Host/Uninstall
	$(foreach prog,$(PKG_PROGRAMS),rm -f $(STAGING_DIR_HOST)/bin/$(prog);)
	rm -rf $(STAGING_DIR_HOST)/lib/$(PKG_NAME)*
	rm -f $(STAGING_DIR_HOST)/lib/lib$(PKG_NAME)*
	rm -f $(STAGING_DIR_HOST)/lib/pkgconfig/python*
	rm -rf $(STAGING_DIR_HOST)/include/$(PKG_NAME)*
	rm -rf $(STAGING_DIR_HOST)/share/man/man1/$(PKG_NAME)*
	rm -rf $(TMP_DIR)/wheels
endef

$(eval $(call HostBuild))
