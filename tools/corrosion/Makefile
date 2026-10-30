# SPDX-License-Identifier: GPL-2.0-only

include $(TOPDIR)/rules.mk

PKG_NAME:=corrosion
PKG_VERSION:=0.5.1
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://codeload.github.com/corrosion-rs/corrosion/tar.gz/v$(PKG_VERSION)?
PKG_HASH:=843334a9f0f5efbc225dccfa88031fe0f2ec6fd787ca1e7d55ed27b2c25d9c97

CMAKE_BINARY_SUBDIR:=build

include $(INCLUDE_DIR)/host-build.mk
include $(INCLUDE_DIR)/cmake.mk
include $(INCLUDE_DIR)/rust-values.mk

# FindRust runs cargo during cmake configure.
export CARGO_HOME := $(CARGO_HOME)

CMAKE_HOST_OPTIONS += \
	-DCORROSION_BUILD_TESTS=OFF \
	-DCARGO_EXECUTABLE=$(CARGO) \
	-DRust_COMPILER=$(RUSTC)

define Host/Install
	cmake --install $(HOST_CMAKE_BINARY_DIR) --prefix $(STAGING_DIR_HOST)
endef

$(eval $(call HostBuild))
