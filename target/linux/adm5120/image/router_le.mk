#
# Copyright (C) 2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Image/Build/Compex
	$(call Image/Build/Loader,$(2),gz,0x80500000,0,y,$(2))
	$(call Image/Build/TRX,$(call imgname,$(1),$(2)).trx,$(1),$(KDIR)/loader-$(2).gz)
endef

define Image/Build/Edimax
	$(call Image/Build/Loader,$(2),gz,0x80500000,0x6D8,y,$(2))
	$(call Image/Build/TRXNoloader,$(call imgname,$(1),$(2)).trx,$(1))
	$(STAGING_DIR_HOST)/bin/mkcsysimg -B $(2) -d -w \
		-r $(KDIR)/loader-$(2).gz \
		-x $(call imgname,$(1),$(2)).trx \
		$(call imgname,$(1),$(2))-webui.bin
	$(STAGING_DIR_HOST)/bin/mkcsysimg -B $(2) -d \
		-r $(KDIR)/loader-$(2).gz \
		-x $(call imgname,$(1),$(2)).trx \
		$(call imgname,$(1),$(2))-xmodem.bin
endef

define Image/Build/Infineon
	$(call Image/Build/Loader,$(2),gz,0x80500000,0x6D8,y,$(2))
	$(call Image/Build/TRXNoloader,$(call imgname,$(1),$(2)).trx,$(1))
	dd if=$(KDIR)/loader-$(2).gz of=$(call imgname,$(1),$(2)).img bs=64k conv=sync
	cat $(call imgname,$(1),$(2)).trx >> $(call imgname,$(1),$(2)).img
endef

define Image/Build/MyLoader
	$(call Image/Build/Loader,$(2),gz,0x80500000,0)
	$(call Image/Build/TRXNoloader,$(call imgname,$(1),$(2)).trx,$(1))
	$(STAGING_DIR_HOST)/bin/mkmylofw -B $(2) \
		-p0x20000:0x10000:ahp:0x80001000 \
		-p0x30000:0 \
		-b0x20000:0x10000:h:$(KDIR)/loader-$(2).gz \
		-b0x30000:0::$(call imgname,$(1),$(2)).trx \
		$(call imgname,$(1),$(2)).bin
endef

#
# Cellvision CAS-771
#
define Image/Build/Board/CAS771/Initramfs
	$(call Image/Build/LZMAKernel/Admboot,cas-771,gz)
endef

#
# Cellvision CAS-771W
#
define Image/Build/Board/CAS771W/Initramfs
	$(call Image/Build/LZMAKernel/Admboot,cas-771w,gz)
endef

#
# Compex NP27G
#
define Image/Build/Board/NP27G
	$(call Image/Build/MyLoader,$(1),np27g)
endef

define Image/Build/Board/NP27G/squashfs
	$(call Image/Build/Board/NP27G,squashfs)
endef

define Image/Build/Board/NP27G/jffs2-64k
	$(call Image/Build/Board/NP27G,jffs2-64k)
endef

define Image/Build/Board/NP27G/Initramfs
	$(call Image/Build/LZMAKernel/Generic,np27g,bin)
endef

#
# Compex NP28G
#
define Image/Build/Board/NP28G
	$(call Image/Build/MyLoader,$(1),np28g)
endef

define Image/Build/Board/NP28G/squashfs
	$(call Image/Build/Board/NP28G,squashfs)
endef

define Image/Build/Board/NP28G/jffs2-64k
	$(call Image/Build/Board/NP28G,jffs2-64k)
endef

define Image/Build/Board/NP28G/Initramfs
	$(call Image/Build/LZMAKernel/Generic,np28g,bin)
endef

#
# Compex WP54G
#
define Image/Build/Board/WP54G
	$(call Image/Build/MyLoader,$(1),wp54g)
endef

define Image/Build/Board/WP54G/squashfs
	$(call Image/Build/Board/WP54G,squashfs)
endef

define Image/Build/Board/WP54G/jffs2-64k
	$(call Image/Build/Board/WP54G,jffs2-64k)
endef

define Image/Build/Board/WP54G/Initramfs
	$(call Image/Build/LZMAKernel/Generic,wp54g,bin)
endef

#
# Compex WP54AG
#
define Image/Build/Board/WP54AG
	$(call Image/Build/MyLoader,$(1),wp54ag)
endef

define Image/Build/Board/WP54AG/squashfs
	$(call Image/Build/Board/WP54AG,squashfs)
endef

define Image/Build/Board/WP54AG/jffs2-64k
	$(call Image/Build/Board/WP54AG,jffs2-64k)
endef

define Image/Build/Board/WP54AG/Initramfs
	$(call Image/Build/LZMAKernel/Generic,wp54ag,bin)
endef

#
# Compex WPP54G
#
define Image/Build/Board/WPP54G
	$(call Image/Build/MyLoader,$(1),wpp54g)
endef

define Image/Build/Board/WPP54G/squashfs
	$(call Image/Build/Board/WPP54G,squashfs)
endef

define Image/Build/Board/WPP54G/jffs2-64k
	$(call Image/Build/Board/WPP54G,jffs2-64k)
endef

define Image/Build/Board/WPP54G/Initramfs
	$(call Image/Build/LZMAKernel/Generic,wpp54g,bin)
endef

#
# Compex WPP54AG
#
define Image/Build/Board/WPP54AG
	$(call Image/Build/MyLoader,$(1),wpp54ag)
endef

define Image/Build/Board/WPP54AG/squashfs
	$(call Image/Build/Board/WPP54AG,squashfs)
endef

define Image/Build/Board/WPP54AG/jffs2-64k
	$(call Image/Build/Board/WPP54AG,jffs2-64k)
endef

define Image/Build/Board/WPP54AG/Initramfs
	$(call Image/Build/LZMAKernel/Generic,wpp54ag,bin)
endef

#
# Compex WP54G-WRT
#
define Image/Build/Board/WP54GWRT
	$(call Image/Build/Compex,$(1),wp54g-wrt)
endef

define Image/Build/Board/WP54GWRT/squashfs
	$(call Image/Build/Board/WP54GWRT,squashfs)
endef

define Image/Build/Board/WP54GWRT/jffs2-64k
	$(call Image/Build/Board/WP54GWRT,jffs2-64k)
endef

define Image/Build/Board/WP54GWRT/Initramfs
	$(call Image/Build/LZMAKernel/KArgs,wp54g-wrt,bin)
endef

#
# Edimax BR-6104K/KP
#
define Image/Build/Board/BR6104K
	$(call Image/Build/Edimax,$(1),br-6104k)
endef

define Image/Build/Board/BR6104K/squashfs
	$(call Image/Build/Board/BR6104K,squashfs)
endef

define Image/Build/Board/BR6104K/Initramfs
	$(call Image/Build/LZMAKernel/Admboot,br-6104k,gz)
endef

#
# Edimax BR-6104WG
#
define Image/Build/Board/BR6104WG
	$(call Image/Build/Edimax,$(1),br-6104wg)
endef

define Image/Build/Board/BR6104WG/squashfs
	$(call Image/Build/Board/BR6104WG,squashfs)
endef

define Image/Build/Board/BR6104WG/Initramfs
	$(call Image/Build/LZMAKernel/Admboot,br-6104wg,gz)
endef

#
# Infineon EASY 83000
#
define Image/Build/Board/EASY83000
	$(call Image/Build/Infineon,$(1),easy-83000)
endef

define Image/Build/Board/EASY83000/squashfs
	$(call Image/Build/Board/EASY83000,squashfs)
endef

define Image/Build/Board/EASY83000/jffs2-64k
	$(call Image/Build/Board/EASY83000,jffs2-64k)
endef

define Image/Build/Board/EASY83000/Initramfs
	$(call Image/Build/LZMAKernel/Admboot,easy-83000,gz)
endef

#
# Mikrotik RB-1xx
#
define Image/Build/Board/RB1xx/Initramfs
	$(CP) $(KDIR)/vmlinux.elf $(call imgname,netboot,rb1xx)
endef

#
# Groups
#
define Image/Build/Group/Cellvision
	$(call Image/Build/Board/CAS771/$(1))
	$(call Image/Build/Board/CAS771W/$(1))
endef

define Image/Build/Group/WP54G
	$(call Image/Build/Board/WP54G/$(1))
	$(call Image/Build/Board/WP54AG/$(1))
	$(call Image/Build/Board/WPP54G/$(1))
	$(call Image/Build/Board/WPP54AG/$(1))
	$(call Image/Build/Board/WP54GWRT/$(1))
endef

define Image/Build/Group/Compex
	$(call Image/Build/Board/NP27G/$(1))
	$(call Image/Build/Board/NP28G/$(1))

	$(call Image/Build/Group/WP54G,$(1))
endef

define Image/Build/Group/Edimax
	$(call Image/Build/Board/BR6104K/$(1))
	$(call Image/Build/Board/BR6104WG/$(1))
endef

define Image/Build/Group/Infineon
	$(call Image/Build/Board/EASY83000/$(1))
endef

define Image/Build/Group/All
	$(call Image/Build/Group/Cellvision,$(1))
	$(call Image/Build/Group/Compex,$(1))
	$(call Image/Build/Group/Edimax,$(1))
	$(call Image/Build/Group/Cellvision,$(1))
	$(call Image/Build/Group/Infineon,$(1))
	$(call Image/Build/Board/RB1xx/$(1))
endef

#
# Profiles
#
define Image/Build/Profile/Generic
	$(call Image/Build/Group/All,$(1))
endef

define Image/Build/Profile/Atheros
	$(call Image/Build/AllBoards,$(1))
endef

define Image/Build/Profile/Texas
	$(call Image/Build/AllBoards,$(1))
endef

define Image/Build/Profile/Ralink
	$(call Image/Build/AllBoards,$(1))
endef

define Image/Build/Profile/WP54G
	$(call Image/Build/Group/WP54G,$(1))
endef

define Image/Build/Profile/NP27G
	$(call Image/Build/Board/NP27G/$(1))
endef

define Image/Build/Profile/NP28G
	$(call Image/Build/Board/NP28G/$(1))
endef

define Image/Build/Profile/CAS771
	$(call Image/Build/Board/CAS771/$(1))
endef

define Image/Build/Profile/CAS771W
	$(call Image/Build/Board/CAS771W/$(1))
endef

define Image/Build/Profile/BR6104KP
	$(call Image/Build/Board/BR6104K/$(1))
endef

define Image/Build/Profile/RouterBoard
	$(call Image/Build/Board/RB1xx/$(1))
endef

ifeq ($(PROFILE),RouterBoard)
  define Image/cmdline/yaffs2
	root=/dev/mtdblock3 rootfstype=yaffs2 init=/etc/preinit
  endef

  define Image/BuildKernel/RouterBoard
	$(CP) $(KDIR)/vmlinux.elf $(call imgname,kernel,rb1xx)
	$(STAGING_DIR_HOST)/bin/patch-cmdline $(call imgname,kernel,rb1xx) \
		'$(strip $(call Image/cmdline/yaffs2))'
  endef

  ifneq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),y)
    define Image/BuildKernel
	$(call Image/BuildKernel/RouterBoard)
    endef
  endif

endif

