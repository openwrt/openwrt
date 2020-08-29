
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=libcxxabi
PKG_VERSION:=10.0.0
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).src.tar.xz
PKG_SOURCE_URL:=https://github.com/llvm/llvm-project/releases/download/llvmorg-$(PKG_VERSION)
PKG_HASH:=e71bac75a88c9dde455ad3f2a2b449bf745eafd41d2d8432253b2964e0ca14e1
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION).src

PKG_MAINTAINER:=Rosen Penev <rosenp@gmail.com>
PKG_LICENSE:=MIT
PKG_LICENSE_FILES:=LICENSE.txt

PKG_BUILD_PARALLEL:=1
CMAKE_BINARY_SUBDIR:=build

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/libcxxabi
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=LLVM lib++abi
  URL:=https://libcxxabi.llvm.org/
  DEPENDS:=+libpthread
  BUILDONLY:=1
endef

define Package/libcxxabi/description
  libc++abi is a new implementation of low level support for a standard C++ library.
endef

CMAKE_OPTIONS += \
	-DLIBCXXABI_ENABLE_EXCEPTIONS=ON \
	-DLIBCXXABI_ENABLE_PEDANTIC=OFF \
	-DLIBCXXABI_ENABLE_PIC=ON \
	-DLIBCXXABI_ENABLE_WERROR=OFF \
	-DLIBCXXABI_USE_LLVM_UNWINDER=OFF \
	-DLIBCXXABI_USE_COMPILER_RT=OFF \
	-DLIBCXXABI_ENABLE_THREADS=ON \
	-DLIBCXXABI_HAS_PTHREAD_API=ON \
	-DLIBCXXABI_INCLUDE_TESTS=OFF \
	-DLIBCXXABI_LIBDIR_SUFFIX="" \
	-DLIBCXXABI_INSTALL_LIBRARY=ON \
	-DLIBCXXABI_ENABLE_SHARED=OFF \
	-DLIBCXXABI_LIBCXX_SRC_DIR=$(BUILD_DIR)/libcxx-$(PKG_VERSION).src \
	-DLIBCXXABI_LIBCXX_INCLUDES=$(BUILD_DIR)/libcxx-$(PKG_VERSION).src/include

TARGET_CXXFLAGS += -flto
TARGET_LDFLAGS += -Wl,--gc-sections,--as-needed

define Build/Prepare
	$(MAKE) -C $(TOPDIR)/package/libs/libcxx prepare
	$(call Build/Prepare/Default)
endef

define Build/InstallDev
	$(call Build/InstallDev/cmake,$(1))
	$(INSTALL_DIR) $(1)/usr/include/
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/include/*.h $(1)/usr/include/
endef

$(eval $(call BuildPackage,libcxxabi))
