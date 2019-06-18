PKG_INSTALL:=1

ifneq ($(findstring c,$(OPENWRT_VERBOSE)),)
  MESON_NINJA_ARGS += -v
endif

MESON_CROSS = $(PKG_BUILD_DIR)/meson.cross

MESON_ENDIAN := $(if $(CONFIG_BIG_ENDIAN),big,little)
MESON_C_ARGS := $(foreach F,$(TARGET_CFLAGS),'$(F)', )
MESON_CPP_ARGS := $(foreach F,$(TARGET_CXXFLAGS),'$(F)', )
MESON_C_LINK_ARGS := $(foreach F,$(TARGET_LDFLAGS),'$(F)', )

define create_meson_cross
	echo "[binaries]" > $(MESON_CROSS); \
	echo "c = '$(TARGET_CC)'" >> $(MESON_CROSS); \
	echo "cpp = '$(TARGET_CXX)'" >> $(MESON_CROSS); \
	echo "ar = '$(TARGET_AR)'" >> $(MESON_CROSS); \
	echo "nm = '$(TARGET_NM)'" >> $(MESON_CROSS); \
	echo "ld = '$(TARGET_CROSS)ld'" >> $(MESON_CROSS); \
	echo "strip = '$(TARGET_CROSS)strip'" >> $(MESON_CROSS); \
	echo "pkgconfig = '$(STAGING_DIR_HOST)/bin/pkg-config'" >> $(MESON_CROSS);
	echo "" >> $(MESON_CROSS); \
	echo "[host_machine]" >> $(MESON_CROSS); \
	echo "system = 'linux'" >> $(MESON_CROSS); \
	echo "cpu_family = '$(ARCH)'" >> $(MESON_CROSS); \
	echo "cpu = '$(ARCH)'" >> $(MESON_CROSS); \
	echo "endian = '$(MESON_ENDIAN)'" >> $(MESON_CROSS); \
	echo "" >> $(MESON_CROSS); \
	echo "[properties]" >> $(MESON_CROSS); \
	echo "c_args = [$(MESON_C_ARGS)]" >> $(MESON_CROSS); \
	echo "c_link_args = [$(MESON_CPP_ARGS)]" >> $(MESON_CROSS); \
	echo "cpp_args = [$(MESON_CPP_ARGS)]" >> $(MESON_CROSS); \
	echo "cpp_link_args = [$(MESON_C_LINK_ARGS)]" >> $(MESON_CROSS);
endef

define Build/Configure/Default
	$(call create_meson_cross)
	(cd $(PKG_BUILD_DIR); \
		meson --prefix=/usr --libdir=lib --cross-file meson.cross $(MESON_OPTIONS) build; \
	)
endef

define Build/Compile
	ninja $(MESON_NINJA_ARGS) -C $(PKG_BUILD_DIR)/build
endef

define Build/Install/Default
	DESTDIR="$(PKG_INSTALL_DIR)" ninja $(MESON_NINJA_ARGS) -C $(PKG_BUILD_DIR)/build install
endef

define Build/InstallDev/meson
	$(INSTALL_DIR) $(1)
	$(CP) $(PKG_INSTALL_DIR)/* $(1)/
endef

Build/InstallDev = $(if $(MESON_INSTALL),$(Build/InstallDev/meson))
