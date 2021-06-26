# meson uses Ninja. Set BUILD_PARALLEL as there are no issues with Ninja.
PKG_BUILD_PARALLEL:=1
HOST_BUILD_PARALLEL:=1

MESON_DIR:=$(STAGING_DIR_HOST)/lib/meson

MESON_HOST_BUILD_DIR:=$(HOST_BUILD_DIR)/openwrt-build
MESON_HOST_VARS:=
MESON_HOST_ARGS:=

MESON_BUILD_DIR:=$(PKG_BUILD_DIR)/openwrt-build
MESON_VARS:=
MESON_ARGS:=

ifneq ($(findstring i386,$(CONFIG_ARCH)),)
MESON_ARCH:="x86"
else ifneq ($(findstring powerpc64,$(CONFIG_ARCH)),)
MESON_ARCH:="ppc64"
else ifneq ($(findstring powerpc,$(CONFIG_ARCH)),)
MESON_ARCH:="ppc"
else ifneq ($(findstring mips64el,$(CONFIG_ARCH)),)
MESON_ARCH:="mips64"
else ifneq ($(findstring mipsel,$(CONFIG_ARCH)),)
MESON_ARCH:="mips"
else ifneq ($(findstring armeb,$(CONFIG_ARCH)),)
MESON_ARCH:="arm"
else
MESON_ARCH:=$(CONFIG_ARCH)
endif

# this is undefined for just x64_64
ifeq ($(origin CPU_TYPE),undefined)
MESON_CPU:="generic"
else
MESON_CPU:="$(CPU_TYPE)$(if $(CPU_SUBTYPE),+$(CPU_SUBTYPE))"
endif

define Meson
	$(2) $(STAGING_DIR_HOST)/bin/$(PYTHON) $(MESON_DIR)/meson.py $(1)
endef

define Meson/CreateNativeFile
	$(STAGING_DIR_HOST)/bin/sed \
		-e "s|@CC@|$(foreach BIN,$(HOSTCC),'$(BIN)',)|" \
		-e "s|@CXX@|$(foreach BIN,$(HOSTCXX),'$(BIN)',)|" \
		-e "s|@PKGCONFIG@|$(PKG_CONFIG)|" \
		-e "s|@CFLAGS@|$(foreach FLAG,$(HOST_CFLAGS) $(HOST_CPPFLAGS),'$(FLAG)',)|" \
		-e "s|@CXXFLAGS@|$(foreach FLAG,$(HOST_CXXFLAGS) $(HOST_CPPFLAGS),'$(FLAG)',)|" \
		-e "s|@LDFLAGS@|$(foreach FLAG,$(HOST_LDFLAGS),'$(FLAG)',)|" \
		-e "s|@PREFIX@|$(HOST_BUILD_PREFIX)|" \
		< $(MESON_DIR)/openwrt-native.txt.in \
		> $(1)
endef

define Meson/CreateCrossFile
	$(STAGING_DIR_HOST)/bin/sed \
		-e "s|@CC@|$(foreach BIN,$(TARGET_CC),'$(BIN)',)|" \
		-e "s|@CXX@|$(foreach BIN,$(TARGET_CXX),'$(BIN)',)|" \
		-e "s|@AR@|$(TARGET_AR)|" \
		-e "s|@STRIP@|$(TARGET_CROSS)strip|" \
		-e "s|@NM@|$(TARGET_NM)|" \
		-e "s|@PKGCONFIG@|$(PKG_CONFIG)|" \
		-e "s|@CFLAGS@|$(foreach FLAG,$(TARGET_CFLAGS) $(EXTRA_CFLAGS) $(TARGET_CPPFLAGS) $(EXTRA_CPPFLAGS),'$(FLAG)',)|" \
		-e "s|@CXXFLAGS@|$(foreach FLAG,$(TARGET_CXXFLAGS) $(EXTRA_CXXFLAGS) $(TARGET_CPPFLAGS) $(EXTRA_CPPFLAGS),'$(FLAG)',)|" \
		-e "s|@LDFLAGS@|$(foreach FLAG,$(TARGET_LDFLAGS) $(EXTRA_LDFLAGS),'$(FLAG)',)|" \
		-e "s|@ARCH@|$(MESON_ARCH)|" \
		-e "s|@CPU@|$(MESON_CPU)|" \
		-e "s|@ENDIAN@|$(if $(CONFIG_BIG_ENDIAN),big,little)|" \
		< $(MESON_DIR)/openwrt-cross.txt.in \
		> $(1)
endef

define Host/Configure/Meson
	$(call Meson/CreateNativeFile,$(HOST_BUILD_DIR)/openwrt-native.txt)
	$(call Meson, \
		--native-file $(HOST_BUILD_DIR)/openwrt-native.txt \
		$(MESON_HOST_ARGS) \
		$(MESON_HOST_BUILD_DIR) \
		$(HOST_BUILD_DIR), \
		$(MESON_HOST_VARS))
endef

define Host/Compile/Meson
	+$(NINJA) -C $(MESON_HOST_BUILD_DIR) $(1)
endef

define Host/Install/Meson
	+$(NINJA) -C $(MESON_HOST_BUILD_DIR) install
endef

define Host/Uninstall/Meson
	+$(NINJA) -C $(MESON_HOST_BUILD_DIR) uninstall
endef

define Build/Configure/Meson
	$(call Meson/CreateNativeFile,$(PKG_BUILD_DIR)/openwrt-native.txt)
	$(call Meson/CreateCrossFile,$(PKG_BUILD_DIR)/openwrt-cross.txt)
	$(call Meson, \
		--buildtype plain \
		--native-file $(PKG_BUILD_DIR)/openwrt-native.txt \
		--cross-file $(PKG_BUILD_DIR)/openwrt-cross.txt \
		$(MESON_ARGS) \
		$(MESON_BUILD_DIR) \
		$(MESON_BUILD_DIR)/.., \
		$(MESON_VARS))
endef

define Build/Compile/Meson
	+$(NINJA) -C $(MESON_BUILD_DIR) $(1)
endef

define Build/Install/Meson
	+DESTDIR="$(PKG_INSTALL_DIR)" $(NINJA) -C $(MESON_BUILD_DIR) install
endef

Host/Configure=$(call Host/Configure/Meson)
Host/Compile=$(call Host/Compile/Meson)
Host/Install=$(call Host/Install/Meson)
Host/Uninstall=$(call Host/Uninstall/Meson)
Build/Configure=$(call Build/Configure/Meson)
Build/Compile=$(call Build/Compile/Meson)
Build/Install=$(call Build/Install/Meson)
