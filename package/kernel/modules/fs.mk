FSMENU:=Filesystems

define KernelPackage/fs-cifs
  SUBMENU:=$(FSMENU)
  TITLE:=CIFS support
  DESCRIPTION:=Kernel module for CIFS support
  DEPENDS:=+kmod-nls-base
  KCONFIG:=CONFIG_CIFS
  FILES:=$(LINUX_DIR)/fs/cifs/cifs.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,30,cifs)
endef

define KernelPackage/fs-cifs/2.4
  DEPENDS:=
endef

$(eval $(call KernelPackage,fs-cifs))


define KernelPackage/fs-minix
  SUBMENU:=$(FSMENU)
  TITLE:=Minix filesystem support
  DESCRIPTION:=Kernel module for Minix filesystem support
  KCONFIG:=CONFIG_MINIX_FS
  FILES:=$(LINUX_DIR)/fs/minix/minix.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,30,minix)
endef

$(eval $(call KernelPackage,fs-minix))


define KernelPackage/fs-ntfs
  SUBMENU:=$(FSMENU)
  TITLE:=NTFS filesystem support
  DESCRIPTION:=Kernel module for NTFS filesystem support
  KCONFIG:=CONFIG_NTFS_FS
  FILES:=$(LINUX_DIR)/fs/ntfs/ntfs.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,30,ntfs)
endef

$(eval $(call KernelPackage,fs-ntfs))


define KernelPackage/fs-ext2
  SUBMENU:=$(FSMENU)
  TITLE:=EXT2 filesystem support
  DESCRIPTION:=Kernel module for EXT2 filesystem support
  KCONFIG:=CONFIG_EXT2_FS
  FILES:=$(LINUX_DIR)/fs/ext2/ext2.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,30,ext2)
endef

$(eval $(call KernelPackage,fs-ext2))


define KernelPackage/fs-ext3
  SUBMENU:=$(FSMENU)
  TITLE:=EXT3 filesystem support
  DESCRIPTION:=Kernel module for EXT3 filesystem support
  KCONFIG:=CONFIG_EXT3_FS
  FILES:= \
	$(LINUX_DIR)/fs/jbd/jbd.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/fs/ext3/ext3.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,30,jbd ext3)
endef

$(eval $(call KernelPackage,fs-ext3))


define KernelPackage/fs-hfs
  SUBMENU:=$(FSMENU)
  TITLE:=HFS+ filesystem support
  DESCRIPTION:=Kernel module for HFS filesystem support
  DEPENDS:=+kmod-nls-base
  KCONFIG:=CONFIG_HFS_FS
  FILES:=$(LINUX_DIR)/fs/hfs/hfs.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,30,hfs)
endef

define KernelPackage/fs-hfs/2.4
  DEPENDS:=
endef

$(eval $(call KernelPackage,fs-hfs))


define KernelPackage/fs-hfsplus
  SUBMENU:=$(FSMENU)
  TITLE:=HFS+ filesystem support
  DESCRIPTION:=Kernel module for HFS+ filesystem support
  DEPENDS:=+kmod-nls-base
  KCONFIG:=CONFIG_HFSPLUS_FS
  FILES:=$(LINUX_DIR)/fs/hfsplus/hfsplus.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,30,hfsplus)
endef

define KernelPackage/fs-hfsplus/2.4
  DEPENDS:=
endef

$(eval $(call KernelPackage,fs-hfsplus))


define KernelPackage/fs-isofs
  SUBMENU:=$(FSMENU)
  TITLE:=ISO9660 filesystem support
  DESCRIPTION:=Kernel module for ISO9660 filesystem support
  DEPENDS:=+kmod-nls-base
  KCONFIG:=CONFIG_ISO9660_FS
  FILES:=$(LINUX_DIR)/fs/isofs/isofs.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,30,isofs)
endef

define Kernel/Package/fs-isofs/2.4
  DEPENDS:=
endef

$(eval $(call KernelPackage,fs-isofs))


define KernelPackage/fs-udf
  SUBMENU:=$(FSMENU)
  TITLE:=UDF filesystem support
  DESCRIPTION:=Kernel module for UDF filesystem support
  DEPENDS:=+kmod-nls-base
  KCONFIG:=CONFIG_UDF_FS
  FILES:=$(LINUX_DIR)/fs/udf/udf.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,30,udf)
endef

define KernelPackage/fs-udf/2.4
  DEPENDS:=
endef

$(eval $(call KernelPackage,fs-udf))


define KernelPackage/fs-nfs
  SUBMENU:=$(FSMENU)
  TITLE:=NFS filesystem support
  DESCRIPTION:=Kernel module for NFS support
  KCONFIG:=CONFIG_NFS_FS
  FILES:= \
  	$(LINUX_DIR)/net/sunrpc/sunrpc.$(LINUX_KMOD_SUFFIX) \
  	$(LINUX_DIR)/fs/lockd/lockd.$(LINUX_KMOD_SUFFIX) \
  	$(LINUX_DIR)/fs/nfs/nfs.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,30,sunrpc lockd nfs)
endef

$(eval $(call KernelPackage,fs-nfs))


define KernelPackage/fs-nfsd
  SUBMENU:=$(FSMENU)
  TITLE:=NFS kernel server support
  DESCRIPTION:=Kernel module for NFS kernel server support
  KCONFIG:=CONFIG_NFSD
  FILES:=$(LINUX_DIR)/fs/nfsd/nfsd.$(LINUX_KMOD_SUFFIX)
endef

define KernelPackage/fs-nfsd/2.4
  AUTOLOAD:=$(call AutoLoad,30,nfsd)
endef

define KernelPackage/fs-nfsd/2.6
  FILES+=$(LINUX_DIR)/fs/exportfs/exportfs.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,30,exportfs nfsd)
endef

$(eval $(call KernelPackage,fs-nfsd))


define KernelPackage/fs-msdos
  SUBMENU:=$(FSMENU)
  TITLE:=MSDOS filesystem support
  DESCRIPTION:=Kernel module for MSDOS filesystem support
  DEPENDS:=+kmod-nls-base
  KCONFIG:=CONFIG_MSDOS_FS
  FILES:=$(LINUX_DIR)/fs/msdos/msdos.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,40,msdos)
endef

define KernelPackage/fs-msdos/2.4
  DEPENDS:=
endef

$(eval $(call KernelPackage,fs-msdos))


define KernelPackage/fs-vfat
  SUBMENU:=$(FSMENU)
  TITLE:=VFAT filesystem support
  DESCRIPTION:=Kernel module for VFAT filesystem support
  DEPENDS:=+kmod-nls-base
  KCONFIG:=CONFIG_VFAT_FS
  FILES:= \
	$(LINUX_DIR)/fs/fat/fat.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/fs/vfat/vfat.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,30,fat vfat)
endef

define KernelPackage/fs-vfat/2.4
  DEPENDS:=
endef

$(eval $(call KernelPackage,fs-vfat))


define KernelPackage/fs-xfs
  SUBMENU:=$(FSMENU)
  TITLE:=XFS filesystem support
  DESCRIPTION:=Kernel module for XFS support
  KCONFIG:=CONFIG_XFS_FS
  FILES:=$(LINUX_DIR)/fs/xfs/xfs.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,30,sunrpc lockd xfs)
endef

$(eval $(call KernelPackage,fs-xfs))


define KernelPackage/nls-base
  SUBMENU:=$(FSMENU)
  TITLE:=Native Language Support
  DESCRIPTION:=Kernel module for Native Language Support
  KCONFIG:=CONFIG_NLS
  FILES:=$(LINUX_DIR)/fs/nls/nls_base.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,20,nls_base)
endef
$(eval $(call KernelPackage,nls-base))


define KernelPackage/nls-cp437
  SUBMENU:=$(FSMENU)
  TITLE:=Codepage 437 (United States, Canada)
  DESCRIPTION:=Kernel module for NLS Codepage 437 (United States, Canada)
  DEPENDS:=+kmod-nls-base
  KCONFIG:=CONFIG_NLS_CODEPAGE_437
  FILES:=$(LINUX_DIR)/fs/nls/nls_cp437.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,25,nls_cp437)
endef

define KernelPackage/nfs-c437/2.4
  DEPENDS:=
endef

$(eval $(call KernelPackage,nls-cp437))


define KernelPackage/nls-cp850
  SUBMENU:=$(FSMENU)
  TITLE:=Codepage 850 (Europe)
  DESCRIPTION:=Kernel module for NLS Codepage 850 (Europe)
  DEPENDS:=+kmod-nls-base
  KCONFIG:=CONFIG_NLS_CODEPAGE_850
  FILES:=$(LINUX_DIR)/fs/nls/nls_cp850.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,25,nls_cp850)
endef

define KernelPackage/nls-cp850/2.4
  DEPENDS:=
endef

$(eval $(call KernelPackage,nls-cp850))


define KernelPackage/nls-cp1250
  SUBMENU:=$(FSMENU)
  TITLE:=Codepage 1250 (Eastern Europe)
  DESCRIPTION:=Kernel module for NLS Codepage 1250 (Eastern Europe)
  DEPENDS:=+kmod-nls-base
  KCONFIG:=CONFIG_NLS_CODEPAGE_1250
  FILES:=$(LINUX_DIR)/fs/nls/nls_cp1250.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,25,nls_cp1250)
endef

define KernelPackage/nls-cp1250/2.4
  DEPENDS:=
endef

$(eval $(call KernelPackage,nls-cp1250))


define KernelPackage/nls-iso8859-1
  SUBMENU:=$(FSMENU)
  TITLE:=ISO 8859-1 (Latin 1; Western European Languages)
  DESCRIPTION:=Kernel module for ISO 8859-1 (Latin 1)
  DEPENDS:=+kmod-nls-base
  KCONFIG:=CONFIG_NLS_ISO8859_1
  FILES:=$(LINUX_DIR)/fs/nls/nls_iso8859-1.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,25,nls_iso8859-1)
endef

define KernelPackage/nls-iso8859-1/2.4
  DEPENDS:=
endef

$(eval $(call KernelPackage,nls-iso8859-1))


define KernelPackage/nls-iso8859-2
  SUBMENU:=$(FSMENU)
  TITLE:=ISO 8859-2 (Latin 2; Central European Languages)
  DESCRIPTION:=Kernel module for ISO 8859-2 (Latin 2)
  DEPENDS:=+kmod-nls-base
  KCONFIG:=CONFIG_NLS_ISO8859_2
  FILES:=$(LINUX_DIR)/fs/nls/nls_iso8859-2.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,25,nls_iso8859-2)
endef

define KernelPackage/nls-iso8859-2/2.4
  DEPENDS:=
endef

$(eval $(call KernelPackage,nls-iso8859-2))


define KernelPackage/nls-iso8859-15
  SUBMENU:=$(FSMENU)
  TITLE:=ISO 8859-15 (Latin 9; Western, with Euro symbol)
  DESCRIPTION:=Kernel module for ISO 8859-15 (Latin 9)
  DEPENDS:=+kmod-nls-base
  KCONFIG:=CONFIG_NLS_ISO8859_15
  FILES:=$(LINUX_DIR)/fs/nls/nls_iso8859-15.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,25,nls_iso8859-15)
endef

define KernelPackage/nls-iso8859-15/2.4
  DEPENDS:=
endef

$(eval $(call KernelPackage,nls-iso8859-15))


define KernelPackage/nls-koi8r
  SUBMENU:=$(FSMENU)
  TITLE:=KOI8-R (Russian)
  DESCRIPTION:=Kernel module for KOI8-R (Russian)
  DEPENDS:=+kmod-nls-base
  KCONFIG:=CONFIG_NLS_KOI8_R
  FILES:=$(LINUX_DIR)/fs/nls/nls_koi8-r.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,25,nls_koi8-r)
endef

define KernelPackage/nls-koi8r/2.4
  DEPENDS:=
endef

$(eval $(call KernelPackage,nls-koi8r))


define KernelPackage/nls-utf8
  SUBMENU:=$(FSMENU)
  TITLE:=UTF8
  DESCRIPTION:=Kernel module for NLS UTF8
  DEPENDS:=+kmod-nls-base
  KCONFIG:=CONFIG_NLS_UTF8
  FILES:=$(LINUX_DIR)/fs/nls/nls_utf8.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,25,nls_utf8)
endef

define KernelPackage/nls-utf8/2.4
  DEPENDS:=
endef

$(eval $(call KernelPackage,nls-utf8))
