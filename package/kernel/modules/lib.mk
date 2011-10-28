#
# Copyright (C) 2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

LIB_MENU:=Libraries

define KernelPackage/lib-crc-ccitt
  SUBMENU:=$(LIB_MENU)
  TITLE:=CRC-CCITT support
  KCONFIG:=CONFIG_CRC_CCITT
  FILES:=$(LINUX_DIR)/lib/crc-ccitt.ko
  AUTOLOAD:=$(call AutoLoad,20,crc-ccitt)
endef

define KernelPackage/lib-crc-ccitt/description
 Kernel module for CRC-CCITT support
endef

$(eval $(call KernelPackage,lib-crc-ccitt))


define KernelPackage/lib-crc-itu-t
  SUBMENU:=$(LIB_MENU)
  TITLE:=CRC ITU-T V.41 support
  KCONFIG:=CONFIG_CRC_ITU_T
  FILES:=$(LINUX_DIR)/lib/crc-itu-t.ko
  AUTOLOAD:=$(call AutoLoad,20,crc-itu-t)
endef

define KernelPackage/lib-crc-itu-t/description
 Kernel module for CRC ITU-T V.41 support
endef

$(eval $(call KernelPackage,lib-crc-itu-t))


define KernelPackage/lib-crc7
  SUBMENU:=$(LIB_MENU)
  TITLE:=CRC7 support
  KCONFIG:=CONFIG_CRC7
  FILES:=$(LINUX_DIR)/lib/crc7.ko
  AUTOLOAD:=$(call AutoLoad,20,crc7)
endef

define KernelPackage/lib-crc7/description
 Kernel module for CRC7 support
endef

$(eval $(call KernelPackage,lib-crc7))


define KernelPackage/lib-crc16
  SUBMENU:=$(LIB_MENU)
  TITLE:=CRC16 support
  KCONFIG:=CONFIG_CRC16
  FILES:=$(LINUX_DIR)/lib/crc16.ko
  AUTOLOAD:=$(call AutoLoad,20,crc16,1)
endef

define KernelPackage/lib-crc16/description
 Kernel module for CRC16 support
endef

$(eval $(call KernelPackage,lib-crc16))


define KernelPackage/lib-crc32c
  SUBMENU:=$(LIB_MENU)
  TITLE:=CRC32 support
  KCONFIG:=CONFIG_LIBCRC32C
  DEPENDS:=+kmod-crypto-crc32c
  FILES:=$(LINUX_DIR)/lib/libcrc32c.ko
  AUTOLOAD:=$(call AutoLoad,20,libcrc32c,1)
endef

define KernelPackage/lib-crc32c/description
 Kernel module for CRC32 support
endef

$(eval $(call KernelPackage,lib-crc32c))


define KernelPackage/lib-lzo
  SUBMENU:=$(LIB_MENU)
  TITLE:=LZO support
  KCONFIG:= \
	CONFIG_LZO_COMPRESS \
	CONFIG_LZO_DECOMPRESS
  FILES:= \
	$(LINUX_DIR)/lib/lzo/lzo_compress.ko \
	$(LINUX_DIR)/lib/lzo/lzo_decompress.ko
  AUTOLOAD:=$(call AutoLoad,20, lzo_compress lzo_decompress,1)
endef

define KernelPackage/lib-lzo/description
 Kernel module for LZO compression/decompression support
endef

$(eval $(call KernelPackage,lib-lzo))


define KernelPackage/lib-textsearch
SUBMENU:=$(LIB_MENU)
  TITLE:=Textsearch support
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

$(eval $(call KernelPackage,lib-textsearch))


define KernelPackage/lib-zlib
  SUBMENU:=$(LIB_MENU)
  TITLE:=Zlib support
  KCONFIG:=CONFIG_ZLIB_DEFLATE
  FILES:=$(LINUX_DIR)/lib/zlib_deflate/zlib_deflate.ko
  AUTOLOAD:=$(call AutoLoad,08,zlib_deflate,1)
endef

$(eval $(call KernelPackage,lib-zlib))
