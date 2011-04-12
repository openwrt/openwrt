#
# Copyright (C) 2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

LIB_MENU:=Libraries

define KernelPackage/crc-ccitt
  SUBMENU:=$(LIB_MENU)
  TITLE:=CRC-CCITT support
  KCONFIG:=CONFIG_CRC_CCITT
  FILES:=$(LINUX_DIR)/lib/crc-ccitt.ko
  AUTOLOAD:=$(call AutoLoad,20,crc-ccitt)
endef

define KernelPackage/crc-ccitt/description
 Kernel module for CRC-CCITT support
endef

$(eval $(call KernelPackage,crc-ccitt))


define KernelPackage/crc-itu-t
  SUBMENU:=$(LIB_MENU)
  TITLE:=CRC ITU-T V.41 support
  KCONFIG:=CONFIG_CRC_ITU_T
  FILES:=$(LINUX_DIR)/lib/crc-itu-t.ko
  AUTOLOAD:=$(call AutoLoad,20,crc-itu-t)
endef

define KernelPackage/crc-itu-t/description
 Kernel module for CRC ITU-T V.41 support
endef

$(eval $(call KernelPackage,crc-itu-t))


define KernelPackage/crc7
  SUBMENU:=$(LIB_MENU)
  TITLE:=CRC7 support
  KCONFIG:=CONFIG_CRC7
  FILES:=$(LINUX_DIR)/lib/crc7.ko
  AUTOLOAD:=$(call AutoLoad,20,crc7)
endef

define KernelPackage/crc7/description
 Kernel module for CRC7 support
endef

$(eval $(call KernelPackage,crc7))


define KernelPackage/crc16
  SUBMENU:=$(LIB_MENU)
  TITLE:=CRC16 support
  KCONFIG:=CONFIG_CRC16
  FILES:=$(LINUX_DIR)/lib/crc16.ko
  AUTOLOAD:=$(call AutoLoad,20,crc16,1)
  $(call SetDepends/crc16)
endef

define KernelPackage/crc16/description
 Kernel module for CRC16 support
endef

$(eval $(call KernelPackage,crc16))


define KernelPackage/libcrc32c
  SUBMENU:=$(LIB_MENU)
  TITLE:=CRC32 library support
  KCONFIG:=CONFIG_LIBCRC32C
  DEPENDS:=+kmod-crypto-core +kmod-crypto-misc
  FILES:=$(LINUX_DIR)/lib/libcrc32c.ko
  AUTOLOAD:=$(call AutoLoad,20,crc32c libcrc32c,1)
endef

define KernelPackage/libcrc32c/description
 Kernel module for CRC32 support
endef

$(eval $(call KernelPackage,libcrc32c))


define KernelPackage/textsearch
SUBMENU:=$(LIB_MENU)
  TITLE:=Textsearch support is selected if needed
  KCONFIG:= \
    CONFIG_TEXTSEARCH=y \
    CONFIG_TEXTSEARCH_KMP \
    CONFIG_TEXTSEARCH_BM \
    CONFIG_TEXTSEARCH_FSM
  FILES:= \
    $(LINUX_DIR)/lib/ts_kmp.ko \
    $(LINUX_DIR)/lib/ts_bm.ko \
    $(LINUX_DIR)/lib/ts_fsm.ko
  AUTOLOAD:=$(call AutoLoad,20,ts_kmp ts_bm ts_fsm)
endef

$(eval $(call KernelPackage,textsearch))


define KernelPackage/zlib
  SUBMENU:=$(LIB_MENU)
  TITLE:=zlib support is selected if needed
  KCONFIG:=CONFIG_ZLIB_DEFLATE
  FILES:=$(LINUX_DIR)/lib/zlib_deflate/zlib_deflate.ko
  AUTOLOAD:=$(call AutoLoad,08,zlib_deflate,1)
endef

$(eval $(call KernelPackage,zlib))
