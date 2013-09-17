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
endef

define KernelPackage/lib-crc7/description
 Kernel module for CRC7 support
endef

$(eval $(call KernelPackage,lib-crc7))


define KernelPackage/lib-crc8
  SUBMENU:=$(LIB_MENU)
  TITLE:=CRC8 support
  KCONFIG:=CONFIG_CRC8
  FILES:=$(LINUX_DIR)/lib/crc8.ko
endef

define KernelPackage/lib-crc8/description
 Kernel module for CRC8 support
endef

$(eval $(call KernelPackage,lib-crc8))


define KernelPackage/lib-crc16
  SUBMENU:=$(LIB_MENU)
  TITLE:=CRC16 support
  KCONFIG:=CONFIG_CRC16
  FILES:=$(LINUX_DIR)/lib/crc16.ko
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
endef

define KernelPackage/lib-lzo/description
 Kernel module for LZO compression/decompression support
endef

$(eval $(call KernelPackage,lib-lzo))


define KernelPackage/lib-raid6
  SUBMENU:=$(LIB_MENU)
  TITLE:=RAID6 algorithm support
  HIDDEN:=1
  KCONFIG:=CONFIG_RAID6_PQ
  FILES:=$(LINUX_DIR)/lib/raid6/raid6_pq.ko
endef

define KernelPackage/lib-raid6/description
 Kernel module for RAID6 algorithms
endef

$(eval $(call KernelPackage,lib-raid6))


define KernelPackage/lib-xor
  SUBMENU:=$(LIB_MENU)
  TITLE:=XOR blocks algorithm support
  HIDDEN:=1
  KCONFIG:=CONFIG_XOR_BLOCKS
  FILES:=$(LINUX_DIR)/crypto/xor.ko
endef

define KernelPackage/lib-xor/description
 Kernel module for XOR blocks algorithms
endef

$(eval $(call KernelPackage,lib-xor))


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
endef

$(eval $(call KernelPackage,lib-textsearch))


define KernelPackage/lib-zlib
  SUBMENU:=$(LIB_MENU)
  TITLE:=Zlib support
  KCONFIG:= \
    CONFIG_ZLIB_DEFLATE \
    CONFIG_ZLIB_INFLATE
  FILES:= \
    $(LINUX_DIR)/lib/zlib_deflate/zlib_deflate.ko \
    $(LINUX_DIR)/lib/zlib_inflate/zlib_inflate.ko
endef

$(eval $(call KernelPackage,lib-zlib))


define KernelPackage/lib-cordic
  SUBMENU:=$(LIB_MENU)
  TITLE:=Cordic function support
  KCONFIG:=CONFIG_CORDIC
  FILES:=$(LINUX_DIR)/lib/cordic.ko
endef

define KernelPackage/lib-cordic/description
 Kernel module for Cordic function support
endef

$(eval $(call KernelPackage,lib-cordic))
