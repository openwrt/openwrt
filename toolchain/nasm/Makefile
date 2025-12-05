#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=nasm
PKG_VERSION:=3.01

PKG_SOURCE_URL:=https://www.nasm.us/pub/nasm/releasebuilds/$(PKG_VERSION)/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.xz
PKG_HASH:=b7324cbe86e767b65f26f467ed8b12ad80e124e3ccb89076855c98e43a9eddd4
PKG_CPE_ID:=cpe:/a:nasm:netwide_assembler

HOST_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/toolchain-build.mk

HOST_CONFIGURE_ARGS+= \
		--target=$(REAL_GNU_TARGET_NAME) \
		--with-sysroot=$(TOOLCHAIN_DIR) \
		--disable-lto \
		--disable-werror \
		--disable-gdb \
		$(SOFT_FLOAT_CONFIG_OPTION) \

define Host/Prepare
	$(call Host/Prepare/Default)
	ln -snf $(notdir $(HOST_BUILD_DIR)) $(BUILD_DIR_TOOLCHAIN)/$(PKG_NAME)
	$(CP) $(SCRIPT_DIR)/config.{guess,sub} $(HOST_BUILD_DIR)/
endef

define Host/Configure
	(cd $(HOST_BUILD_DIR); \
		./autogen.sh \
	);
	$(call Host/Configure/Default)
endef

define Host/Install
	$(MAKE) -C $(HOST_BUILD_DIR) \
		$(HOST_MAKE_FLAGS) \
		prefix=$(TOOLCHAIN_DIR) \
		install
endef

define Host/Clean
	rm -rf \
		$(HOST_BUILD_DIR) \
		$(BUILD_DIR_TOOLCHAIN)/$(PKG_NAME)
endef

$(eval $(call HostBuild))
