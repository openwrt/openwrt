ifneq ($(__meson_mk_inc),1)
__meson_mk_inc=1

include $(TOPDIR)/include/ninja.mk

CROSS_CONF_FILE=$(PKG_BUILD_DIR)/cross_conf.txt
HOST_PYTHON3_BIN ?= python3
HOST_MESON_BIN=$(STAGING_DIR_HOST)/meson/meson.py
MESON_BUILD_DIR ?= builddir

BINPATH=$(TOPDIR)/staging_dir/host/usr/bin:$(PATH)
MESON_ENDIAN := $(if $(CONFIG_BIG_ENDIAN),big,little)

define CreateCrossconf
	echo [binaries] >$(CROSS_CONF_FILE)
	echo c = \'$(TARGET_CC_NOCACHE)\' >>$(CROSS_CONF_FILE)
	echo cpp = \'$(TARGET_CXX_NOCACHE)\'>>$(CROSS_CONF_FILE)
	echo ar = \'$(TARGET_AR)\'>>$(CROSS_CONF_FILE)
	echo strip = \'$(TARGET_CROSS)strip\' >> $(CROSS_CONF_FILE)
	echo nm = \'$(TARGET_NM)\' >> $(CROSS_CONF_FILE)
	echo ld = \'$(TARGET_CROSS)ld\' >> $(CROSS_CONF_FILE)
	echo pkgconfig = \'$(PKG_CONFIG).real\'>>$(CROSS_CONF_FILE)

	echo [host_machine]>>$(CROSS_CONF_FILE)
	echo system = \'linux\'>>$(CROSS_CONF_FILE)
	echo cpu_family = \'$(ARCH)\'>>$(CROSS_CONF_FILE)
	echo cpu = \'$(CONFIG_TARGET_SUBTARGET)\'>>$(CROSS_CONF_FILE)
	echo endian = \'$(MESON_ENDIAN)\' >> $(CROSS_CONF_FILE)
	echo [build_machine]>>$(CROSS_CONF_FILE)
	echo system = \'linux\'>>$(CROSS_CONF_FILE)
	echo cpu_family = \'x86_64\'>>$(CROSS_CONF_FILE)
	echo cpu = \'i686\'>>$(CROSS_CONF_FILE)
	echo endian = \'little\'>>$(CROSS_CONF_FILE)
endef

define Build/Meson/Configure
	$(call CreateCrossconf)
	[ -d "$(PKG_BUILD_DIR)/$(MESON_BUILD_DIR)" ] || mkdir -p "$(PKG_BUILD_DIR)/$(MESON_BUILD_DIR)"
	cd $(PKG_BUILD_DIR) && \
	PATH=$(BINPATH) PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) $(HOST_PYTHON3_BIN) $(HOST_MESON_BIN) $(MESON_BUILD_DIR) --cross-file $(CROSS_CONF_FILE) $(MESON_ARGS)
endef

define Build/Meson/Compile
	$(call Build/Ninja/Compile,$(MESON_BUILD_DIR))
endef

define Build/Meson/Install
	$(call Build/Ninja/Install,$(MESON_BUILD_DIR))
endef

define Build/Configure
	$(call Build/Meson/Configure)
endef

define Build/Compile
	$(call Build/Meson/Compile)
endef

define Build/Install
	$(call Build/Meson/Install)
endef

endif
