FSMENU:=Filesystems

define KernelPackage/fs-cifs
  TITLE:=CIFS support
  DESCRIPTION:=Kernel module for CIFS support
  KCONFIG:=CONFIG_CIFS
  SUBMENU:=$(FSMENU)
  AUTOLOAD:=$(call AutoLoad,30,cifs)
  FILES:=$(LINUX_DIR)/fs/cifs/cifs.$(LINUX_KMOD_SUFFIX)
endef

define KernelPackage/fs-cifs/2.6
  DEPENDS:=+kmod-nls-base
endef
$(eval $(call KernelPackage,fs-cifs))

define KernelPackage/fs-minix
  TITLE:=Minix filesystem support
  DESCRIPTION:=Kernel module for Minix filesystem support
  KCONFIG:=CONFIG_MINIX_FS
  SUBMENU:=$(FSMENU)
  AUTOLOAD:=$(call AutoLoad,30,minix)
  FILES:=$(LINUX_DIR)/fs/minix/minix.$(LINUX_KMOD_SUFFIX)
endef
$(eval $(call KernelPackage,fs-minix))

define KernelPackage/fs-ext2
  TITLE:=EXT2 filesystem support
  DESCRIPTION:=Kernel module for EXT2 filesystem support
  KCONFIG:=CONFIG_EXT2_FS
  SUBMENU:=$(FSMENU)
  AUTOLOAD:=$(call AutoLoad,30,ext2)
  FILES:=$(LINUX_DIR)/fs/ext2/ext2.$(LINUX_KMOD_SUFFIX)
endef
$(eval $(call KernelPackage,fs-ext2))

define KernelPackage/fs-ext3
  TITLE:=EXT3 filesystem support
  DESCRIPTION:=Kernel module for EXT3 filesystem support
  KCONFIG:=CONFIG_EXT3_FS
  SUBMENU:=$(FSMENU)
  AUTOLOAD:=$(call AutoLoad,30,jbd ext3)
  FILES:= \
	$(LINUX_DIR)/fs/jbd/jbd.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/fs/ext3/ext3.$(LINUX_KMOD_SUFFIX)
endef
$(eval $(call KernelPackage,fs-ext3))

define KernelPackage/fs-hfs
  TITLE:=HFS+ filesystem support
  DESCRIPTION:=Kernel module for HFS filesystem support
  KCONFIG:=CONFIG_HFS_FS
  DEPENDS:=+kmod-nls-base
  SUBMENU:=$(FSMENU)
  AUTOLOAD:=$(call AutoLoad,30,hfs)
  FILES:=$(LINUX_DIR)/fs/hfs/hfs.$(LINUX_KMOD_SUFFIX)
endef
define KernelPackage/fs-hfs/2.4
  DEPENDS:=
endef
$(eval $(call KernelPackage,fs-hfs))


define KernelPackage/fs-hfsplus
  TITLE:=HFS+ filesystem support
  DESCRIPTION:=Kernel module for HFS+ filesystem support
  KCONFIG:=CONFIG_HFSPLUS_FS
  DEPENDS:=+kmod-nls-base
  SUBMENU:=$(FSMENU)
  AUTOLOAD:=$(call AutoLoad,30,hfsplus)
  FILES:=$(LINUX_DIR)/fs/hfsplus/hfsplus.$(LINUX_KMOD_SUFFIX)
endef

define KernelPackage/fs-hfsplus/2.4
  DEPENDS:=
endef
$(eval $(call KernelPackage,fs-hfsplus))

define KernelPackage/fs-isofs
  TITLE:=ISO9660 filesystem support
  DESCRIPTION:=Kernel module for ISO9660 filesystem support
  KCONFIG:=CONFIG_ISO9660_FS
  SUBMENU:=$(FSMENU)
  AUTOLOAD:=$(call AutoLoad,30,isofs)
  FILES:=$(LINUX_DIR)/fs/isofs/isofs.$(LINUX_KMOD_SUFFIX)
endef

define Kernel/Package/fs-isofs/2.6
  DEPENDS:=+kmod-nls-base
endef
$(eval $(call KernelPackage,fs-isofs))

define KernelPackage/fs-udf
  TITLE:=UDF filesystem support
  DESCRIPTION:=Kernel module for UDF filesystem support
  KCONFIG:=CONFIG_UDF_FS
  SUBMENU:=$(FSMENU)
  AUTOLOAD:=$(call AutoLoad,30,udf)
  FILES:=$(LINUX_DIR)/fs/udf/udf.$(LINUX_KMOD_SUFFIX)
endef

define KernelPackage/fs-udf/2.6
  DEPENDS:=+kmod-nls-base
endef
$(eval $(call KernelPackage,fs-udf))

define KernelPackage/fs-nfs
  TITLE:=NFS filesystem support
  DESCRIPTION:=Kernel module for NFS support
  KCONFIG:=CONFIG_NFS_FS
  SUBMENU:=$(FSMENU)
  AUTOLOAD:=$(call AutoLoad,30,sunrpc lockd nfs)
  FILES:= \
  	$(LINUX_DIR)/net/sunrpc/sunrpc.$(LINUX_KMOD_SUFFIX) \
  	$(LINUX_DIR)/fs/lockd/lockd.$(LINUX_KMOD_SUFFIX) \
  	$(LINUX_DIR)/fs/nfs/nfs.$(LINUX_KMOD_SUFFIX)
endef
$(eval $(call KernelPackage,fs-nfs))

define KernelPackage/fs-nfsd
  TITLE:=NFS kernel server support
  DESCRIPTION:=Kernel module for NFS kernel server support
  KCONFIG:=CONFIG_NFSD
  SUBMENU:=$(FSMENU)
  FILES:= \
  	$(LINUX_DIR)/fs/nfsd/nfsd.$(LINUX_KMOD_SUFFIX)
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
  TITLE:=MSDOS filesystem support
  DESCRIPTION:=Kernel module for MSDOS filesystem support
  KCONFIG:=CONFIG_MSDOS_FS
  SUBMENU:=$(FSMENU)
  AUTOLOAD:=$(call AutoLoad,40,msdos)
  FILES:= \
	$(LINUX_DIR)/fs/msdos/msdos.$(LINUX_KMOD_SUFFIX)
endef

define KernelPackage/fs-msdos/2.6
  DEPENDS:=+kmod-nls-base
endef
$(eval $(call KernelPackage,fs-msdos))

define KernelPackage/fs-vfat
  TITLE:=VFAT filesystem support
  DESCRIPTION:=Kernel module for VFAT filesystem support
  KCONFIG:=CONFIG_VFAT_FS
  SUBMENU:=$(FSMENU)
  AUTOLOAD:=$(call AutoLoad,30,fat vfat)
  FILES:= \
	$(LINUX_DIR)/fs/fat/fat.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/fs/vfat/vfat.$(LINUX_KMOD_SUFFIX)
endef

define KernelPackage/fs-vfat/2.6
  DEPENDS:=+kmod-nls-base
endef
$(eval $(call KernelPackage,fs-vfat))

define KernelPackage/fs-xfs
  TITLE:=XFS filesystem support
  DESCRIPTION:=Kernel module for XFS support
  KCONFIG:=CONFIG_XFS_FS
  SUBMENU:=$(FSMENU)
  AUTOLOAD:=$(call AutoLoad,30,sunrpc lockd xfs)
  FILES:= \
  	$(LINUX_DIR)/fs/xfs/xfs.$(LINUX_KMOD_SUFFIX)
endef
$(eval $(call KernelPackage,fs-xfs))


define KernelPackage/nls-base
  TITLE:=Native Language Support
  DESCRIPTION:=Kernel module for Native Language Support
  KCONFIG:=CONFIG_NLS
  SUBMENU:=$(FSMENU)
  MENU:=1
  FILES:=$(LINUX_DIR)/fs/nls/nls_base.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,20,nls_base)
endef
$(eval $(call KernelPackage,nls-base))


define KernelPackage/nls-cp437
  TITLE:=Codepage 437 (United States, Canada)
  DESCRIPTION:=Kernel module for NLS Codepage 437 (United States, Canada)
  KCONFIG:=CONFIG_NLS_CODEPAGE_437
  SUBMENU:=$(FSMENU)
  FILES:=$(LINUX_DIR)/fs/nls/nls_cp437.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,25,nls_cp437)
endef

define KernelPackage/nfs-c437/2.6
  DEPENDS:=+kmod-nls-base
endef
$(eval $(call KernelPackage,nls-cp437))


define KernelPackage/nls-cp850
  TITLE:=Codepage 850 (Europe)
  DESCRIPTION:=Kernel module for NLS Codepage 850 (Europe)
  KCONFIG:=CONFIG_NLS_CODEPAGE_850
  SUBMENU:=$(FSMENU)
  FILES:=$(LINUX_DIR)/fs/nls/nls_cp850.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,25,nls_cp850)
endef

define KernelPackage/nls-cp850/2.6
  DEPENDS:=+kmod-nls-base
endef
$(eval $(call KernelPackage,nls-cp850))

define KernelPackage/nls-cp1250
  TITLE:=Codepage 1250 (Eastern Europe)
  DESCRIPTION:=Kernel module for NLS Codepage 1250 (Eastern Europe)
  KCONFIG:=CONFIG_NLS_CODEPAGE_1250
  SUBMENU:=$(FSMENU)
  FILES:=$(LINUX_DIR)/fs/nls/nls_cp1250.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,25,nls_cp1250)
endef

define KernelPackage/nls-cp1250/2.6
  DEPENDS:=+kmod-nls-base
endef
$(eval $(call KernelPackage,nls-cp1250))

define KernelPackage/nls-iso8859-1
  TITLE:=ISO 8859-1 (Latin 1; Western European Languages)
  DESCRIPTION:=Kernel module for ISO 8859-1 (Latin 1)
  KCONFIG:=CONFIG_NLS_ISO8859_1
  SUBMENU:=$(FSMENU)
  FILES:=$(LINUX_DIR)/fs/nls/nls_iso8859-1.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,25,nls_iso8859-1)
endef

define KernelPackage/nls-iso8859-1/2.6
  DEPENDS:=+kmod-nls-base
endef
$(eval $(call KernelPackage,nls-iso8859-1))

define KernelPackage/nls-iso8859-2
  TITLE:=ISO 8859-2 (Latin 2; Central European Languages)
  DESCRIPTION:=Kernel module for ISO 8859-2 (Latin 2)
  KCONFIG:=CONFIG_NLS_ISO8859_2
  SUBMENU:=$(FSMENU)
  FILES:=$(LINUX_DIR)/fs/nls/nls_iso8859-2.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,25,nls_iso8859-2)
endef

define KernelPackage/nls-iso8859-2/2.6
  DEPENDS:=+kmod-nls-base
endef
$(eval $(call KernelPackage,nls-iso8859-2))

define KernelPackage/nls-iso8859-15
  TITLE:=ISO 8859-15 (Latin 9; Western, with Euro symbol)
  DESCRIPTION:=Kernel module for ISO 8859-15 (Latin 9)
  KCONFIG:=CONFIG_NLS_ISO8859_15
  SUBMENU:=$(FSMENU)
  FILES:=$(LINUX_DIR)/fs/nls/nls_iso8859-15.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,25,nls_iso8859-15)
endef

define KernelPackage/nls-iso8859-15/2.6
  DEPENDS:=+kmod-nls-base
endef
$(eval $(call KernelPackage,nls-iso8859-15))

define KernelPackage/nls-koi8r
  TITLE:=KOI8-R (Russian)
  DESCRIPTION:=Kernel module for KOI8-R (Russian)
  KCONFIG:=CONFIG_NLS_KOI8_R
  SUBMENU:=$(FSMENU)
  FILES:=$(LINUX_DIR)/fs/nls/nls_koi8-r.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,25,nls_koi8-r)
endef

define KernelPackage/nls-koi8r/2.6
  DEPENDS:=+kmod-nls-base
endef
$(eval $(call KernelPackage,nls-koi8r))

define KernelPackage/nls-utf8
  TITLE:=UTF8
  DESCRIPTION:=Kernel module for NLS UTF8
  KCONFIG:=CONFIG_NLS_UTF8
  SUBMENU:=$(FSMENU)
  FILES:=$(LINUX_DIR)/fs/nls/nls_utf8.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,25,nls_utf8)
endef

define KernelPackage/nls-utf8/2.6
  DEPENDS:=+kmod-nls-base
endef
$(eval $(call KernelPackage,nls-utf8))


