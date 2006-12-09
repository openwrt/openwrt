FSMENU:=Filesystems

define KernelPackage/fs-cifs
  TITLE:=CIFS support
  DESCRIPTION:=Kernel module for CIFS support
  KCONFIG:=$(CONFIG_CIFS)
  SUBMENU:=$(FSMENU)
  AUTOLOAD:=$(call AutoLoad,30,cifs)
  FILES:=$(MODULES_DIR)/kernel/fs/cifs/cifs.$(LINUX_KMOD_SUFFIX)
endef
$(eval $(call KernelPackage,fs-cifs))

define KernelPackage/fs-minix
  TITLE:=Minix filesystem support
  DESCRIPTION:=Kernel module for Minix filesystem support
  KCONFIG:=$(CONFIG_MINIX_FS)
  SUBMENU:=$(FSMENU)
  AUTOLOAD:=$(call AutoLoad,30,minix)
  FILES:=$(MODULES_DIR)/kernel/fs/minix/minix.$(LINUX_KMOD_SUFFIX)
endef
$(eval $(call KernelPackage,fs-minix))

define KernelPackage/fs-ext2
  TITLE:=EXT2 filesystem support
  DESCRIPTION:=Kernel module for EXT2 filesystem support
  KCONFIG:=$(CONFIG_EXT2_FS)
  SUBMENU:=$(FSMENU)
  AUTOLOAD:=$(call AutoLoad,30,ext2)
  FILES:=$(MODULES_DIR)/kernel/fs/ext2/ext2.$(LINUX_KMOD_SUFFIX)
endef
$(eval $(call KernelPackage,fs-ext2))

define KernelPackage/fs-ext3
  TITLE:=EXT3 filesystem support
  DESCRIPTION:=Kernel module for EXT3 filesystem support
  KCONFIG:=$(CONFIG_EXT3_FS)
  SUBMENU:=$(FSMENU)
  AUTOLOAD:=$(call AutoLoad,30,jbd ext3)
  FILES:= \
	$(MODULES_DIR)/kernel/fs/jbd/jbd.$(LINUX_KMOD_SUFFIX) \
	$(MODULES_DIR)/kernel/fs/ext3/ext3.$(LINUX_KMOD_SUFFIX)
endef
$(eval $(call KernelPackage,fs-ext3))

define KernelPackage/fs-hfsplus
  TITLE:=HFS+ filesystem support
  DESCRIPTION:=Kernel module for HFS+ filesystem support
  DEPENDS:=+kmod-nls-base
  KCONFIG:=$(CONFIG_HFSPLUS_FS)
  SUBMENU:=$(FSMENU)
  AUTOLOAD:=$(call AutoLoad,30,hfsplus)
  FILES:=$(MODULES_DIR)/kernel/fs/hfsplus/hfsplus.$(LINUX_KMOD_SUFFIX)
endef
$(eval $(call KernelPackage,fs-hfsplus))

define KernelPackage/fs-isofs
  TITLE:=ISO9660 filesystem support
  DESCRIPTION:=Kernel module for ISO9660 filesystem support
  DEPENDS:=+kmod-nls-base
  KCONFIG:=$(CONFIG_ISO9660_FS)
  SUBMENU:=$(FSMENU)
  AUTOLOAD:=$(call AutoLoad,30,isofs)
  FILES:=$(MODULES_DIR)/kernel/fs/isofs/isofs.$(LINUX_KMOD_SUFFIX)
endef
$(eval $(call KernelPackage,fs-isofs))

define KernelPackage/fs-udf
  TITLE:=UDF filesystem support
  DESCRIPTION:=Kernel module for UDF filesystem support
  DEPENDS:=+kmod-nls-base
  KCONFIG:=$(CONFIG_UDF_FS)
  SUBMENU:=$(FSMENU)
  AUTOLOAD:=$(call AutoLoad,30,udf)
  FILES:=$(MODULES_DIR)/kernel/fs/udf/udf.$(LINUX_KMOD_SUFFIX)
endef
$(eval $(call KernelPackage,fs-udf))

define KernelPackage/fs-nfs
  TITLE:=NFS filesystem support
  DESCRIPTION:=Kernel module for NFS support
  KCONFIG:=$(CONFIG_NFS_FS)
  SUBMENU:=$(FSMENU)
  AUTOLOAD:=$(call AutoLoad,30,sunrpc lockd nfs)
  FILES:= \
  	$(MODULES_DIR)/kernel/net/sunrpc/sunrpc.$(LINUX_KMOD_SUFFIX) \
  	$(MODULES_DIR)/kernel/fs/lockd/lockd.$(LINUX_KMOD_SUFFIX) \
  	$(MODULES_DIR)/kernel/fs/nfs/nfs.$(LINUX_KMOD_SUFFIX)
endef
$(eval $(call KernelPackage,fs-nfs))

define KernelPackage/fs-msdos
  TITLE:=MSDOS filesystem support
  DESCRIPTION:=Kernel module for MSDOS filesystem support
  DEPENDS:=+kmod-nls-base
  KCONFIG:=$(CONFIG_MSDOS_FS)
  SUBMENU:=$(FSMENU)
  AUTOLOAD:=$(call AutoLoad,30,fat vfat)
  FILES:= \
	$(MODULES_DIR)/kernel/fs/msdos/msdos.$(LINUX_KMOD_SUFFIX)
endef
$(eval $(call KernelPackage,fs-msdos))

define KernelPackage/fs-vfat
  TITLE:=VFAT filesystem support
  DESCRIPTION:=Kernel module for VFAT filesystem support
  DEPENDS:=+kmod-nls-base
  KCONFIG:=$(CONFIG_VFAT_FS)
  SUBMENU:=$(FSMENU)
  AUTOLOAD:=$(call AutoLoad,30,fat vfat)
  FILES:= \
	$(MODULES_DIR)/kernel/fs/fat/fat.$(LINUX_KMOD_SUFFIX) \
	$(MODULES_DIR)/kernel/fs/vfat/vfat.$(LINUX_KMOD_SUFFIX)
endef
$(eval $(call KernelPackage,fs-vfat))

define KernelPackage/fs-xfs
  TITLE:=XFS filesystem support
  DESCRIPTION:=Kernel module for XFS support
  KCONFIG:=$(CONFIG_XFS_FS)
  SUBMENU:=$(FSMENU)
  AUTOLOAD:=$(call AutoLoad,30,sunrpc lockd xfs)
  FILES:= \
  	$(MODULES_DIR)/kernel/fs/xfs/xfs.$(LINUX_KMOD_SUFFIX)
endef
$(eval $(call KernelPackage,fs-xfs))


define KernelPackage/nls-base
  TITLE:=Native Language Support
  DESCRIPTION:=Kernel module for Native Language Support
  KCONFIG:=$(CONFIG_NLS)
  SUBMENU:=$(FSMENU)
  MENU:=1
  FILES:=$(MODULES_DIR)/kernel/fs/nls/nls_base.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,20,nls_base)
endef
$(eval $(call KernelPackage,nls-base))


define KernelPackage/nls-cp437
  TITLE:=Codepage 437 (United States, Canada)
  DESCRIPTION:=Kernel module for NLS Codepage 437 (United States, Canada)
  DEPENDS:=kmod-nls-base
  KCONFIG:=$(CONFIG_NLS_CODEPAGE_437)
  SUBMENU:=$(FSMENU)
  FILES:=$(MODULES_DIR)/kernel/fs/nls/nls_cp437.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,25,nls_cp437)
endef
$(eval $(call KernelPackage,nls-cp437))


define KernelPackage/nls-cp850
  TITLE:=Codepage 850 (Europe)
  DESCRIPTION:=Kernel module for NLS Codepage 850 (Europe)
  DEPENDS:=kmod-nls-base
  KCONFIG:=$(CONFIG_NLS_CODEPAGE_850)
  SUBMENU:=$(FSMENU)
  FILES:=$(MODULES_DIR)/kernel/fs/nls/nls_cp850.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,25,nls_cp850)
endef
$(eval $(call KernelPackage,nls-cp850))


define KernelPackage/nls-iso8859-1
  TITLE:=ISO 8859-1 (Latin 1; Western European Languages)
  DESCRIPTION:=Kernel module for ISO 8859-1 (Latin 1)
  DEPENDS:=kmod-nls-base
  KCONFIG:=$(CONFIG_NLS_ISO8859_1)
  SUBMENU:=$(FSMENU)
  FILES:=$(MODULES_DIR)/kernel/fs/nls/nls_iso8859-1.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,25,nls_iso8859-1)
endef
$(eval $(call KernelPackage,nls-iso8859-1))

define KernelPackage/nls-iso8859-15
  TITLE:=ISO 8859-15 (Latin 9; Western, with Euro symbol)
  DESCRIPTION:=Kernel module for ISO 8859-15 (Latin 9)
  DEPENDS:=kmod-nls-base
  KCONFIG:=$(CONFIG_NLS_ISO8859_15)
  SUBMENU:=$(FSMENU)
  FILES:=$(MODULES_DIR)/kernel/fs/nls/nls_iso8859-15.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,25,nls_iso8859-15)
endef
$(eval $(call KernelPackage,nls-iso8859-15))

define KernelPackage/nls-koi8r
  TITLE:=KOI8-R (Russian)
  DESCRIPTION:=Kernel module for KOI8-R (Russian)
  DEPENDS:=kmod-nls-base
  KCONFIG:=$(CONFIG_NLS_KOI8_R)
  SUBMENU:=$(FSMENU)
  FILES:=$(MODULES_DIR)/kernel/fs/nls/nls_koi8-r.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,25,nls_koi8-r)
endef
$(eval $(call KernelPackage,nls-koi8r))

define KernelPackage/nls-utf8
  TITLE:=UTF8
  DESCRIPTION:=Kernel module for NLS UTF8
  DEPENDS:=kmod-nls-base
  KCONFIG:=$(CONFIG_NLS_UTF8)
  SUBMENU:=$(FSMENU)
  FILES:=$(MODULES_DIR)/kernel/fs/nls/nls_utf8.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,25,nls_utf8)
endef
$(eval $(call KernelPackage,nls-utf8))


