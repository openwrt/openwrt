
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=libcxx
PKG_VERSION:=10.0.0
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).src.tar.xz
PKG_SOURCE_URL:=https://github.com/llvm/llvm-project/releases/download/llvmorg-$(PKG_VERSION)
PKG_HASH:=270f8a3f176f1981b0f6ab8aa556720988872ec2b48ed3b605d0ced8d09156c7
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION).src

PKG_MAINTAINER:=Rosen Penev <rosenp@gmail.com>
PKG_LICENSE:=MIT
PKG_LICENSE_FILES:=LICENSE.txt

PKG_BUILD_PARALLEL:=1
PKG_BUILD_DEPENDS:=libcxxabi
CMAKE_BINARY_SUBDIR:=build

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/libcxx
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=LLVM libstdc++
  URL:=https://libcxx.llvm.org/
  DEPENDS:=+libatomic +libpthread
endef

define Package/libcxx/description
  libc++ is an implementation of the C++ standard library, targeting C++11, C++14 and above.
endef

CMAKE_OPTIONS += \
	-DLIBCXX_CXX_ABI="libcxxabi" \
	-DLIBCXX_ENABLE_ASSERTIONS=OFF \
	-DLIBCXX_ENABLE_EXPERIMENTAL_LIBRARY=OFF \
	-DLIBCXX_INCLUDE_BENCHMARKS=OFF \
	-DLIBCXX_INCLUDE_DOCS=OFF \
	-DLIBCXX_INCLUDE_TESTS=OFF \
	-DLIBCXX_LIBDIR_SUFFIX="" \
	-DLIBCXX_STANDALONE_BUILD=ON \
	-DLIBCXX_HAS_MUSL_LIBC=$(if $(CONFIG_USE_MUSL),ON,OFF)

TARGET_CXXFLAGS += -D_LIBCPP_DISABLE_DEPRECATION_WARNINGS -Wno-attributes -flto
TARGET_LDFLAGS += -Wl,--gc-sections,--as-needed

define Build/InstallDev
	$(call Build/InstallDev/cmake,$(1))
	$(CP) files/g++-libcxx	$(TOOLCHAIN_DIR)/bin/
	$(SED) 's,CXX,$(TARGET_CXX),g' $(TOOLCHAIN_DIR)/bin/g++-libcxx
endef

define Package/libcxx/install
	$(INSTALL_DIR)					$(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libc++.so.*	$(1)/usr/lib/
endef

$(eval $(call BuildPackage,libcxx))
