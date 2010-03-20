#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

FS_MENU:=Filesystems

define KernelPackage/nls/Depends
  DEPENDS:= +!LINUX_2_4:kmod-nls-base
endef

define KernelPackage/fs-cifs
  SUBMENU:=$(FS_MENU)
  TITLE:=CIFS support
  KCONFIG:=CONFIG_CIFS
  FILES:=$(LINUX_DIR)/fs/cifs/cifs.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,30,cifs)
$(call KernelPackage/nls/Depends)
endef


define KernelPackage/fs-cifs/description
 Kernel module for CIFS support
endef

$(eval $(call KernelPackage,fs-cifs))


define KernelPackage/fs-minix
  SUBMENU:=$(FS_MENU)
  TITLE:=Minix filesystem support
  KCONFIG:=CONFIG_MINIX_FS
  FILES:=$(LINUX_DIR)/fs/minix/minix.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,30,minix)
endef

define KernelPackage/fs-minix/description
 Kernel module for Minix filesystem support
endef

$(eval $(call KernelPackage,fs-minix))


define KernelPackage/fs-ntfs
  SUBMENU:=$(FS_MENU)
  TITLE:=NTFS filesystem support
  KCONFIG:=CONFIG_NTFS_FS
  FILES:=$(LINUX_DIR)/fs/ntfs/ntfs.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,30,ntfs)
$(call KernelPackage/nls/Depends)
endef

define KernelPackage/fs-ntfs/description
 Kernel module for NTFS filesystem support
endef

$(eval $(call KernelPackage,fs-ntfs))


define KernelPackage/fs-mbcache
  SUBMENU:=$(FS_MENU)
  TITLE:=mbcache (used by ext2/ext3/ext4)
  KCONFIG:=CONFIG_FS_MBCACHE
  ifneq ($(CONFIG_FS_MBCACHE),)
    FILES:=$(LINUX_DIR)/fs/mbcache.$(LINUX_KMOD_SUFFIX)
    AUTOLOAD:=$(call AutoLoad,20,mbcache,1)
  endif
endef

define KernelPackage/fs-ext2/description
 Meta Block cache used by ext2/ext3
 This package will only be installed if extended attributes 
 are enabled for ext2/ext3
endef
$(eval $(call KernelPackage,fs-mbcache))

define KernelPackage/fs-ext2
  SUBMENU:=$(FS_MENU)
  TITLE:=EXT2 filesystem support
  KCONFIG:=CONFIG_EXT2_FS
  DEPENDS:=$(if $(DUMP)$(CONFIG_FS_MBCACHE),+kmod-fs-mbcache)
  FILES:=$(LINUX_DIR)/fs/ext2/ext2.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,32,ext2,1)
endef

define KernelPackage/fs-ext2/description
 Kernel module for EXT2 filesystem support
endef

$(eval $(call KernelPackage,fs-ext2,1))


define KernelPackage/fs-ext3
  SUBMENU:=$(FS_MENU)
  TITLE:=EXT3 filesystem support
  KCONFIG:= \
	CONFIG_EXT3_FS \
	CONFIG_JBD
  DEPENDS:=$(if $(DUMP)$(CONFIG_FS_MBCACHE),+kmod-fs-mbcache)
  FILES:= \
	$(LINUX_DIR)/fs/ext3/ext3.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/fs/jbd/jbd.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,31,jbd ext3,1)
endef

define KernelPackage/fs-ext3/description
 Kernel module for EXT3 filesystem support
endef

$(eval $(call KernelPackage,fs-ext3))

define KernelPackage/fs-ext4
  SUBMENU:=$(FS_MENU)
  TITLE:=EXT4 filesystem support
  KCONFIG:= \
	CONFIG_EXT4_FS_XATTR=y \
	CONFIG_EXT4_FS_POSIX_ACL=y \
	CONFIG_EXT4_FS_SECURITY=y \
	CONFIG_EXT4_FS \
	CONFIG_JBD2
  DEPENDS:= @LINUX_2_6 +kmod-crc16 $(if $(DUMP)$(CONFIG_FS_MBCACHE),+kmod-fs-mbcache)
  FILES:= \
	$(LINUX_DIR)/fs/ext4/ext4.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/fs/jbd2/jbd2.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,30,jbd2 ext4,1)
endef

define KernelPackage/fs-ext4/description
 Kernel module for EXT4 filesystem support
endef

$(eval $(call KernelPackage,fs-ext4))



define KernelPackage/fs-hfs
  SUBMENU:=$(FS_MENU)
  TITLE:=HFS+ filesystem support
  KCONFIG:=CONFIG_HFS_FS
  FILES:=$(LINUX_DIR)/fs/hfs/hfs.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,30,hfs)
$(call KernelPackage/nls/Depends)
endef

define KernelPackage/fs-hfs/description
 Kernel module for HFS filesystem support
endef

$(eval $(call KernelPackage,fs-hfs))


define KernelPackage/fs-hfsplus
  SUBMENU:=$(FS_MENU)
  TITLE:=HFS+ filesystem support
  KCONFIG:=CONFIG_HFSPLUS_FS
  FILES:=$(LINUX_DIR)/fs/hfsplus/hfsplus.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,30,hfsplus)
$(call KernelPackage/nls/Depends,utf8)
endef


define KernelPackage/fs-hfsplus/description
 Kernel module for HFS+ filesystem support
endef

$(eval $(call KernelPackage,fs-hfsplus))


define KernelPackage/fs-isofs
  SUBMENU:=$(FS_MENU)
  TITLE:=ISO9660 filesystem support
  KCONFIG:=CONFIG_ISO9660_FS CONFIG_JOLIET=y CONFIG_ZISOFS=n
  FILES:=$(LINUX_DIR)/fs/isofs/isofs.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,30,isofs)
$(call KernelPackage/nls/Depends)
endef


define KernelPackage/fs-isofs/description
 Kernel module for ISO9660 filesystem support
endef

$(eval $(call KernelPackage,fs-isofs))


define KernelPackage/fs-udf
  SUBMENU:=$(FS_MENU)
  TITLE:=UDF filesystem support
  KCONFIG:=CONFIG_UDF_FS
  FILES:=$(LINUX_DIR)/fs/udf/udf.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,30,udf)
$(call KernelPackage/nls/Depends)
endef


define KernelPackage/fs-udf/description
 Kernel module for UDF filesystem support
endef

$(eval $(call KernelPackage,fs-udf))

define KernelPackage/fs-nfs-common
  SUBMENU:=$(FS_MENU)
  TITLE:=Common NFS filesystem modules
  KCONFIG:= \
	CONFIG_LOCKD \
	CONFIG_SUNRPC
  FILES:= \
	$(LINUX_DIR)/fs/lockd/lockd.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/net/sunrpc/sunrpc.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,30,sunrpc lockd)
endef

$(eval $(call KernelPackage,fs-nfs-common))


define KernelPackage/fs-nfs-common-v4
  SUBMENU:=$(FS_MENU)
  TITLE:=Common NFS V4 filesystem modules
  KCONFIG+=\
	CONFIG_SUNRPC_GSS\
	CONFIG_NFS_V4=y\
	CONFIG_NFSD_V4=y
  DEPENDS:= @LINUX_2_6 @BROKEN
  FILES+=$(LINUX_DIR)/net/sunrpc/auth_gss/auth_rpcgss.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD=$(call AutoLoad,30,auth_rpcgss)
endef

define KernelPackage/fs-nfs-common-v4/description
 Kernel modules for NFS V4 & NFSD V4 kernel support
endef

$(eval $(call KernelPackage,fs-nfs-common-v4))


define KernelPackage/fs-nfs
  SUBMENU:=$(FS_MENU)
  TITLE:=NFS filesystem support
  DEPENDS:=+kmod-fs-nfs-common
  KCONFIG:= \
	CONFIG_NFS_FS
  FILES:= \
	$(LINUX_DIR)/fs/nfs/nfs.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,40,nfs)
endef

define KernelPackage/fs-nfs/description
 Kernel module for NFS support
endef

$(eval $(call KernelPackage,fs-nfs))


define KernelPackage/fs-exportfs
  SUBMENU:=$(FS_MENU)
  TITLE:=exportfs kernel server support
  KCONFIG:=CONFIG_EXPORTFS
  FILES=$(LINUX_DIR)/fs/exportfs/exportfs.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,20,exportfs)
endef

define KernelPackage/fs-exportfs/description
 Kernel module for exportfs. Needed for some other modules.
endef

$(eval $(call KernelPackage,fs-exportfs))


define KernelPackage/fs-nfsd
  SUBMENU:=$(FS_MENU)
  TITLE:=NFS kernel server support
  DEPENDS:=+kmod-fs-nfs-common +kmod-fs-exportfs
  KCONFIG:=CONFIG_NFSD
  FILES:=$(LINUX_DIR)/fs/nfsd/nfsd.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,40,nfsd)
endef

define KernelPackage/fs-nfsd/description
 Kernel module for NFS kernel server support
endef

$(eval $(call KernelPackage,fs-nfsd))

define KernelPackage/fs-msdos
  SUBMENU:=$(FS_MENU)
  TITLE:=MSDOS filesystem support
  KCONFIG:=CONFIG_MSDOS_FS
  FILES:=$(LINUX_DIR)/fs/fat/msdos.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,40,msdos)
$(call KernelPackage/nls/Depends)
endef

define KernelPackage/fs-msdos/2.4
  FILES:=$(LINUX_DIR)/fs/msdos/msdos.$(LINUX_KMOD_SUFFIX)
endef

define KernelPackage/fs-msdos/description
 Kernel module for MSDOS filesystem support
endef

$(eval $(call KernelPackage,fs-msdos))


define KernelPackage/fs-reiserfs
  SUBMENU:=$(FS_MENU)
  TITLE:=ReiserFS filesystem support
  KCONFIG:=CONFIG_REISERFS_FS
  FILES:=$(LINUX_DIR)/fs/reiserfs/reiserfs.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,30,reiserfs,1)
endef

define KernelPackage/fs-reiserfs/description
 Kernel module for ReiserFS support
endef

$(eval $(call KernelPackage,fs-reiserfs))

define KernelPackage/fs-vfat
  SUBMENU:=$(FS_MENU)
  TITLE:=VFAT filesystem support
  KCONFIG:= \
	CONFIG_FAT_FS \
	CONFIG_VFAT_FS
  FILES:= \
	$(LINUX_DIR)/fs/fat/fat.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/fs/fat/vfat.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,30,fat vfat)
$(call KernelPackage/nls/Depends)
endef

define KernelPackage/fs-vfat/2.4
  FILES:= \
	$(LINUX_DIR)/fs/fat/fat.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/fs/vfat/vfat.$(LINUX_KMOD_SUFFIX)
endef

define KernelPackage/fs-vfat/description
 Kernel module for VFAT filesystem support
endef

$(eval $(call KernelPackage,fs-vfat))


define KernelPackage/fs-xfs
  SUBMENU:=$(FS_MENU)
  TITLE:=XFS filesystem support
  KCONFIG:=CONFIG_XFS_FS
  DEPENDS:= +kmod-fs-exportfs
  FILES:=$(LINUX_DIR)/fs/xfs/xfs.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,30,xfs,1)
endef

define KernelPackage/fs-xfs/description
 Kernel module for XFS support
endef

$(eval $(call KernelPackage,fs-xfs))

define KernelPackage/fs-btrfs
  SUBMENU:=$(FS_MENU)
  TITLE:=BTRFS filesystem support
  KCONFIG:=\
	CONFIG_LIBCRC32C \
	CONFIG_BTRFS_FS \
	CONFIG_BTRFS_FS_POSIX_ACL=n
  # for crc32c
  DEPENDS:=+kmod-crypto-core +kmod-crypto-misc
  FILES:=\
	$(LINUX_DIR)/lib/libcrc32c.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/fs/btrfs/btrfs.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,30,crc32c libcrc32c btrfs,1)
endef

define KernelPackage/fs-btrfs/description
  Kernel module for BTRFS support
endef

$(eval $(call KernelPackage,fs-btrfs))

define KernelPackage/fs-autofs4
  SUBMENU:=$(FS_MENU)
  TITLE:=AUTOFS4 filesystem support
  KCONFIG:=CONFIG_AUTOFS4_FS 
  FILES:=$(LINUX_DIR)/fs/autofs4/autofs4.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,30,autofs4)
endef

define KernelPackage/fs-autofs4/description
  Kernel module for AutoFS4 support
endef

$(eval $(call KernelPackage,fs-autofs4))


define KernelPackage/nls-base
  SUBMENU:=$(FS_MENU)
  TITLE:=Native Language Support
  KCONFIG:=CONFIG_NLS
  FILES:=$(LINUX_DIR)/fs/nls/nls_base.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,20,nls_base,1)
endef

define KernelPackage/nls-base/description
 Kernel module for NLS (Native Language Support)
endef

$(eval $(call KernelPackage,nls-base))


define KernelPackage/nls-cp437
  SUBMENU:=$(FS_MENU)
  TITLE:=Codepage 437 (United States, Canada)
  KCONFIG:=CONFIG_NLS_CODEPAGE_437
  FILES:=$(LINUX_DIR)/fs/nls/nls_cp437.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,25,nls_cp437)
$(call KernelPackage/nls/Depends)
endef


define KernelPackage/nls-cp437/description
 Kernel module for NLS Codepage 437 (United States, Canada)
endef

$(eval $(call KernelPackage,nls-cp437))


define KernelPackage/nls-cp850
  SUBMENU:=$(FS_MENU)
  TITLE:=Codepage 850 (Europe)
  KCONFIG:=CONFIG_NLS_CODEPAGE_850
  FILES:=$(LINUX_DIR)/fs/nls/nls_cp850.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,25,nls_cp850)
$(call KernelPackage/nls/Depends)
endef


define KernelPackage/nls-cp850/description
 Kernel module for NLS Codepage 850 (Europe)
endef

$(eval $(call KernelPackage,nls-cp850))


define KernelPackage/nls-cp852
  SUBMENU:=$(FS_MENU)
  TITLE:=Codepage 852 (Europe)
  KCONFIG:=CONFIG_NLS_CODEPAGE_852
  FILES:=$(LINUX_DIR)/fs/nls/nls_cp852.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,25,nls_cp852)
$(call KernelPackage/nls/Depends)
endef


define KernelPackage/nls-cp852/description
 Kernel module for NLS Codepage 852 (Europe)
endef

$(eval $(call KernelPackage,nls-cp852))


define KernelPackage/nls-cp866
  SUBMENU:=$(FS_MENU)
  TITLE:=Codepage 866 (Cyrillic)
  KCONFIG:=CONFIG_NLS_CODEPAGE_866
  FILES:=$(LINUX_DIR)/fs/nls/nls_cp866.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,25,nls_cp866)
$(call KernelPackage/nls/Depends)
endef


define KernelPackage/nls-cp866/description
  Kernel module for NLS Codepage 866 (Cyrillic)
endef

$(eval $(call KernelPackage,nls-cp866))


define KernelPackage/nls-cp1250
  SUBMENU:=$(FS_MENU)
  TITLE:=Codepage 1250 (Eastern Europe)
  KCONFIG:=CONFIG_NLS_CODEPAGE_1250
  FILES:=$(LINUX_DIR)/fs/nls/nls_cp1250.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,25,nls_cp1250)
$(call KernelPackage/nls/Depends)
endef


define KernelPackage/nls-cp1250/description
 Kernel module for NLS Codepage 1250 (Eastern Europe)
endef

$(eval $(call KernelPackage,nls-cp1250))


define KernelPackage/nls-cp1251
  SUBMENU:=$(FS_MENU)
  TITLE:=Codepage 1251 (Russian)
  KCONFIG:=CONFIG_NLS_CODEPAGE_1251
  FILES:=$(LINUX_DIR)/fs/nls/nls_cp1251.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,25,nls_cp1251)
$(call KernelPackage/nls/Depends)
endef


define KernelPackage/nls-cp1251/description
 Kernel module for NLS Codepage 1251 (Russian)
endef

$(eval $(call KernelPackage,nls-cp1251))


define KernelPackage/nls-iso8859-1
  SUBMENU:=$(FS_MENU)
  TITLE:=ISO 8859-1 (Latin 1; Western European Languages)
  KCONFIG:=CONFIG_NLS_ISO8859_1
  FILES:=$(LINUX_DIR)/fs/nls/nls_iso8859-1.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,25,nls_iso8859-1)
$(call KernelPackage/nls/Depends)
endef


define KernelPackage/nls-iso8859-1/description
 Kernel module for NLS ISO 8859-1 (Latin 1)
endef

$(eval $(call KernelPackage,nls-iso8859-1))


define KernelPackage/nls-iso8859-2
  SUBMENU:=$(FS_MENU)
  TITLE:=ISO 8859-2 (Latin 2; Central European Languages)
  KCONFIG:=CONFIG_NLS_ISO8859_2
  FILES:=$(LINUX_DIR)/fs/nls/nls_iso8859-2.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,25,nls_iso8859-2)
$(call KernelPackage/nls/Depends)
endef


define KernelPackage/nls-iso8859-2/description
 Kernel module for NLS ISO 8859-2 (Latin 2)
endef

$(eval $(call KernelPackage,nls-iso8859-2))


define KernelPackage/nls-iso8859-15
  SUBMENU:=$(FS_MENU)
  TITLE:=ISO 8859-15 (Latin 9; Western, with Euro symbol)
  KCONFIG:=CONFIG_NLS_ISO8859_15
  FILES:=$(LINUX_DIR)/fs/nls/nls_iso8859-15.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,25,nls_iso8859-15)
$(call KernelPackage/nls/Depends)
endef


define KernelPackage/nls-iso8859-15/description
 Kernel module for NLS ISO 8859-15 (Latin 9)
endef

$(eval $(call KernelPackage,nls-iso8859-15))


define KernelPackage/nls-koi8r
  SUBMENU:=$(FS_MENU)
  TITLE:=KOI8-R (Russian)
  KCONFIG:=CONFIG_NLS_KOI8_R
  FILES:=$(LINUX_DIR)/fs/nls/nls_koi8-r.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,25,nls_koi8-r)
$(call KernelPackage/nls/Depends)
endef


define KernelPackage/nls-koi8r/description
 Kernel module for NLS KOI8-R (Russian)
endef

$(eval $(call KernelPackage,nls-koi8r))


define KernelPackage/nls-utf8
  SUBMENU:=$(FS_MENU)
  TITLE:=UTF-8
  KCONFIG:=CONFIG_NLS_UTF8
  FILES:=$(LINUX_DIR)/fs/nls/nls_utf8.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,25,nls_utf8)
$(call KernelPackage/nls/Depends)
endef

define KernelPackage/nls-utf8/description
 Kernel module for NLS UTF-8
endef

$(eval $(call KernelPackage,nls-utf8))


define KernelPackage/nls-iso8859-13
  SUBMENU:=$(FS_MENU)
  TITLE:=ISO 8859-13 (Latin 7; Baltic)
  KCONFIG:=CONFIG_NLS_ISO8859_13
  FILES:=$(LINUX_DIR)/fs/nls/nls_iso8859-13.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,25,nls_iso8859-13)
$(call KernelPackage/nls/Depends)
endef


define KernelPackage/nls-iso8859-13/description
 Kernel module for NLS ISO 8859-13 (Latin 7; Baltic)
endef

$(eval $(call KernelPackage,nls-iso8859-13))

define KernelPackage/nls-cp775
  SUBMENU:=$(FS_MENU)
  TITLE:=Codepage 775 (Baltic Rim)
  KCONFIG:=CONFIG_NLS_CODEPAGE_775
  FILES:=$(LINUX_DIR)/fs/nls/nls_cp775.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,25,nls_cp775)
$(call KernelPackage/nls/Depends)
endef


define KernelPackage/nls-cp775/description
 Kernel module for NLS Codepage 775 (Baltic Rim)
endef

$(eval $(call KernelPackage,nls-cp775))

