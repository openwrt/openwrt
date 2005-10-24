# $Id$
# Linux 2.6 kernel target for the OpenWRT project

include $(TOPDIR)/rules.mk
include ../rules.mk

KERNEL:=2.6
LINUX_VERSION:=2.6.12.5
LINUX_KERNEL_MD5SUM:=076f0dc714112c764c790fbaef19e228
LINUX_SITE=http://www.fi.kernel.org/pub/linux/kernel/v2.6 \
           http://www.fr.kernel.org/pub/linux/kernel/v2.6 \
           http://www.kernel.org/pub/linux/kernel/v2.6 \
           http://www.de.kernel.org/pub/linux/kernel/v2.6 
MODULES_SUBDIR:=lib/modules/$(LINUX_VERSION)
LINUX_KCONFIG:=./config/$(BOARD)
LINUX_BUILD_DIR:=$(BUILD_DIR)/linux-2.6-$(BOARD)
LINUX_TARGET_DIR:=$(LINUX_BUILD_DIR)/root
LINUX_KERNEL:=$(LINUX_BUILD_DIR)/vmlinux
LINUX_KARCH:=$(shell echo $(ARCH) | sed -e 's/i[3-9]86/i386/' \
	-e 's/mipsel/mips/' \
	-e 's/powerpc/ppc/' \
	-e 's/sh[234]/sh/' \
	)

LINUX_SOURCE := linux-$(LINUX_VERSION).tar.bz2
LINUX_SOURCE_DIR := $(LINUX_BUILD_DIR)/linux-$(LINUX_VERSION)
MODULES_DIR := $(LINUX_BUILD_DIR)/modules/$(MODULES_SUBDIR)
TARGET_MODULES_DIR := $(LINUX_TARGET_DIR)/lib/modules/$(LINUX_VERSION)
IPKG_KERNEL:=IPKG_TMP=$(BUILD_DIR)/tmp IPKG_INSTROOT=$(LINUX_TARGET_DIR) IPKG_CONF_DIR=$(LINUX_BUILD_DIR) $(SCRIPT_DIR)/ipkg -force-defaults -force-depends

PKG_BUILD_DIR := $(LINUX_BUILD_DIR)/linux-modules
PKG_RELEASE := 1

KERNEL_IPKG:=$(LINUX_BUILD_DIR)/kernel-$(LINUX_VERSION)-$(BOARD)_$(PKG_RELEASE)_$(ARCH).ipk
KERNEL_IDIR:=$(LINUX_BUILD_DIR)/kernel-ipkg

KPKG_MAKEOPTS:=	IPKG="$(IPKG_KERNEL)" \
		BOARD="$(BOARD)" \
		TARGET_DIR="$(LINUX_TARGET_DIR)" \
		BUILD_DIR="$(LINUX_BUILD_DIR)" \
		KERNEL_DIR="$(LINUX_SOURCE_DIR)" \
		LINUX_VERSION="$(LINUX_VERSION)" \
		KERNEL_RELEASE="$(PKG_RELEASE)"

include $(LINUX_KCONFIG)

INSTALL_TARGETS := $(KERNEL_IPKG)
TARGETS := 

$(eval $(call KMOD_template,ATM,atm,\
	$(MODULES_DIR)/kernel/net/atm/atm.ko \
	$(MODULES_DIR)/kernel/net/atm/br2684.ko \
,CONFIG_ATM,,50,atm))
$(eval $(call KMOD_template,PPPOATM,pppoatm,\
	$(MODULES_DIR)/kernel/net/atm/pppoatm.ko \
,CONFIG_PPPOATM))
$(eval $(call KMOD_template,ARPT,arptables,\
	$(MODULES_DIR)/kernel/net/ipv4/netfilter/arp*.ko \
,CONFIG_IP_NF_ARPTABLES))
$(eval $(call KMOD_template,EBT,ebtables,\
	$(MODULES_DIR)/kernel/net/bridge/netfilter/*.ko \
,CONFIG_BRIDGE_NF_EBTABLES))
$(eval $(call KMOD_template,IPTABLES_V4_EXTRA,iptables-extra,\
	$(MODULES_DIR)/kernel/net/ipv4/netfilter/ip*.ko \
))
$(eval $(call KMOD_template,IPTABLES_V6,ip6tables,\
	$(MODULES_DIR)/kernel/net/ipv6/netfilter/ip*.ko \
,CONFIG_IP6_NF_IPTABLES,kmod-ipv6))
$(eval $(call KMOD_template,IPV6,ipv6,\
	$(MODULES_DIR)/kernel/net/ipv6/ipv6.ko \
,CONFIG_IPV6,,20,ipv6))
$(eval $(call KMOD_template,IDE,ide,\
	$(MODULES_DIR)/kernel/drivers/ide/*.ko \
	$(MODULES_DIR)/kernel/drivers/ide/*/*.ko \
,CONFIG_IDE))
$(eval $(call KMOD_template,LOOP,loop,\
    $(MODULES_DIR)/kernel/drivers/block/loop.ko \
,CONFIG_BLK_DEV_LOOP,,20,loop))
$(eval $(call KMOD_template,CRYPTO,crypto,\
	$(MODULES_DIR)/kernel/crypto/*.ko \
))
$(eval $(call KMOD_template,PPP,ppp,\
	$(MODULES_DIR)/kernel/drivers/net/ppp_async.ko \
	$(MODULES_DIR)/kernel/drivers/net/ppp_generic.ko \
	$(MODULES_DIR)/kernel/drivers/net/slhc.ko \
,CONFIG_PPP))
$(eval $(call KMOD_template,MPPE,mppe,\
	$(MODULES_DIR)/kernel/drivers/net/ppp_mppe_mppc.ko \
,CONFIG_PPP_MPPE_MPPC))
$(eval $(call KMOD_template,PPPOE,pppoe,\
	$(MODULES_DIR)/kernel/drivers/net/pppoe.ko \
	$(MODULES_DIR)/kernel/drivers/net/pppox.ko \
,CONFIG_PPPOE))
$(eval $(call KMOD_template,TUN,tun,\
	$(MODULES_DIR)/kernel/drivers/net/tun.ko \
,CONFIG_TUN,,20,tun))
$(eval $(call KMOD_template,GRE,gre,\
	$(MODULES_DIR)/kernel/net/ipv4/ip_gre.ko \
,CONFIG_NET_IPGRE))
$(eval $(call KMOD_template,SCHED,sched,\
	$(MODULES_DIR)/kernel/net/sched/*.ko \
))


# Filesystems

$(eval $(call KMOD_template,FS_CIFS,fs-cifs,\
	$(MODULES_DIR)/kernel/fs/cifs/cifs.ko \
,CONFIG_CIFS,kmod-nls-base,30,cifs))

$(eval $(call KMOD_template,FS_EXT2,fs-ext2,\
	$(MODULES_DIR)/kernel/fs/ext2/*.ko \
,CONFIG_EXT2_FS,,30,ext2))

$(eval $(call KMOD_template,FS_EXT3,fs-ext3,\
	$(MODULES_DIR)/kernel/fs/ext3/*.ko \
	$(MODULES_DIR)/kernel/fs/jbd/*.ko \
,CONFIG_EXT3_FS,,30,jbd ext3))

$(eval $(call KMOD_template,FS_HFSPLUS,fs-hfsplus,\
	$(MODULES_DIR)/kernel/fs/hfsplus/*.ko \
,CONFIG_HFSPLUS_FS,kmod-nls-base,30,hfsplus))

$(eval $(call KMOD_template,FS_NFS,fs-nfs,\
	$(MODULES_DIR)/kernel/fs/lockd/*.ko \
	$(MODULES_DIR)/kernel/fs/nfs/*.ko \
	$(MODULES_DIR)/kernel/net/sunrpc/*.ko \
,CONFIG_NFS_FS,,30,sunrpc lockd nfs))

$(eval $(call KMOD_template,FS_VFAT,fs-vfat,\
	$(MODULES_DIR)/kernel/fs/fat/fat.ko \
	$(MODULES_DIR)/kernel/fs/vfat/vfat.ko \
,CONFIG_VFAT_FS,kmod-nls-base,30,fat vfat))

$(eval $(call KMOD_template,FS_XFS,fs-xfs,\
	$(MODULES_DIR)/kernel/fs/xfs/*.ko \
,CONFIG_XFS_FS,,30,xfs))


# NLS

$(eval $(call KMOD_template,NLS_BASE,nls-base,\
	$(MODULES_DIR)/kernel/fs/nls/nls_base.ko \
,CONFIG_NLS,,20,nls_base))

$(eval $(call KMOD_template,NLS_CP437,nls-cp437,\
	$(MODULES_DIR)/kernel/fs/nls/nls_cp437.ko \
,CONFIG_NLS_CODEPAGE_437,,20,nls_cp437))

$(eval $(call KMOD_template,NLS_CP850,nls-cp850,\
	$(MODULES_DIR)/kernel/fs/nls/nls_cp850.ko \
,CONFIG_NLS_CODEPAGE_850,,20,nls_cp850))

$(eval $(call KMOD_template,NLS_ISO8859_1,nls-iso8859-1,\
	$(MODULES_DIR)/kernel/fs/nls/nls_iso8859-1.ko \
,CONFIG_NLS_ISO8859_1,,20,nls_iso8859-1))

$(eval $(call KMOD_template,NLS_ISO8859_15,nls-iso8859-15,\
	$(MODULES_DIR)/kernel/fs/nls/nls_iso8859-15.ko \
,CONFIG_NLS_ISO8859_15,,20,nls_iso8859-15))

$(eval $(call KMOD_template,NLS_UTF8,nls-utf8,\
	$(MODULES_DIR)/kernel/fs/nls/nls_utf8.ko \
,CONFIG_NLS_UTF8,,20,nls_utf8))


# Network devices

$(eval $(call KMOD_template,NET_B44,net-b44,\
	$(MODULES_DIR)/kernel/drivers/net/b44.ko \
,CONFIG_B44))


# USB

$(eval $(call KMOD_template,USB,usb-core,\
	$(MODULES_DIR)/kernel/drivers/usb/core/usbcore.ko \
,CONFIG_USB,,50,usbcore))

$(eval $(call KMOD_template,USB_OHCI,usb-ohci,\
	$(MODULES_DIR)/kernel/drivers/usb/host/ohci-hcd.ko \
,CONFIG_USB_OHCI_HCD,kmod-usb-core,60,ohci-hcd))

$(eval $(call KMOD_template,USB_UHCI,usb-uhci,\
	$(MODULES_DIR)/kernel/drivers/usb/host/uhci.ko \
,CONFIG_USB_UHCI_ALT,kmod-usb-core,60,uhci))

$(eval $(call KMOD_template,USB2,usb2,\
	$(MODULES_DIR)/kernel/drivers/usb/host/ehci-hcd.ko \
,CONFIG_USB_EHCI_HCD,kmod-usb-core,60,ehci-hcd))

$(eval $(call KMOD_template,USB_ACM,usb-acm,\
	$(MODULES_DIR)/kernel/drivers/usb/class/cdc-acm.ko \
,CONFIG_USB_ACM))

$(eval $(call KMOD_template,USB_PRINTER,usb-printer,\
	$(MODULES_DIR)/kernel/drivers/usb/class/usblp.ko \
,CONFIG_USB_PRINTER,kmod-usb-core,60,usblp))

$(eval $(call KMOD_template,USB_STORAGE,usb-storage,\
	$(MODULES_DIR)/kernel/drivers/scsi/*.ko \
	$(MODULES_DIR)/kernel/drivers/usb/storage/*.ko \
,CONFIG_USB_STORAGE,kmod-usb-core,60,scsi_mod sd_mod usb-storage))


# Misc.

$(eval $(call KMOD_template,BLUETOOTH,bluetooth,\
	$(MODULES_DIR)/kernel/net/bluetooth/*.ko \
	$(MODULES_DIR)/kernel/net/bluetooth/rfcomm/*.ko \
	$(MODULES_DIR)/kernel/drivers/bluetooth/*.ko \
,CONFIG_BLUEZ))


$(TARGETS): $(PACKAGE_DIR)

$(PACKAGE_DIR):
	mkdir -p $(PACKAGE_DIR)

$(DL_DIR)/$(LINUX_SOURCE):
	-mkdir -p $(DL_DIR)
	$(SCRIPT_DIR)/download.pl $(DL_DIR) $(LINUX_SOURCE) $(LINUX_KERNEL_MD5SUM) $(LINUX_SITE) $(MAKE_TRACE)

$(LINUX_DIR)/.unpacked: $(DL_DIR)/$(LINUX_SOURCE)
	-mkdir -p $(LINUX_BUILD_DIR)
	bzcat $(DL_DIR)/$(LINUX_SOURCE) | tar -C $(LINUX_BUILD_DIR) $(TAR_OPTIONS) -
	rm -f $(LINUX_DIR)
	ln -s $(LINUX_BUILD_DIR)/linux-$(LINUX_VERSION) $(LINUX_DIR)
	touch $(LINUX_DIR)/.unpacked

$(LINUX_DIR)/.patched: $(LINUX_DIR)/.unpacked
	$(PATCH) $(LINUX_DIR) ./patches/generic $(MAKE_TRACE)
	[ -d ./patches/$(BOARD) ] && $(PATCH) $(LINUX_DIR) ./patches/$(BOARD) $(MAKE_TRACE)
	touch $(LINUX_DIR)/.patched

$(LINUX_DIR)/.configured: $(LINUX_DIR)/.patched
	-cp $(LINUX_KCONFIG) $(LINUX_DIR)/.config
	$(MAKE) -C $(LINUX_DIR) ARCH=$(LINUX_KARCH) oldconfig $(MAKE_TRACE)
	touch $(LINUX_DIR)/.configured

$(LINUX_DIR)/vmlinux: $(LINUX_DIR)/.configured
	$(MAKE) -C $(LINUX_DIR) CROSS_COMPILE=$(TARGET_CROSS) ARCH=$(LINUX_KARCH) PATH=$(TARGET_PATH) $(MAKE_TRACE)

$(LINUX_KERNEL): $(LINUX_DIR)/vmlinux
	$(TARGET_CROSS)objcopy -O binary -R .reginfo -R .note -R .comment -R .mdebug -S $< $@ $(MAKE_TRACE)
	touch -c $(LINUX_KERNEL)
	
$(LINUX_DIR)/.modules_done: $(LINUX_KERNEL) $(LINUX_IMAGE)
	rm -rf $(LINUX_BUILD_DIR)/modules
	$(MAKE) -C $(LINUX_DIR) CROSS_COMPILE=$(TARGET_CROSS) ARCH=$(LINUX_KARCH) PATH=$(TARGET_PATH) modules $(MAKE_TRACE)
	$(MAKE) -C $(LINUX_DIR) CROSS_COMPILE=$(TARGET_CROSS) ARCH=$(LINUX_KARCH) PATH=$(TARGET_PATH) DEPMOD=true INSTALL_MOD_PATH=$(LINUX_BUILD_DIR)/modules modules_install $(MAKE_TRACE)
	touch $(LINUX_DIR)/.modules_done

$(STAGING_DIR)/include/linux/version.h: $(LINUX_DIR)/.configured
	mkdir -p $(STAGING_DIR)/include
	tar -ch -C $(LINUX_DIR)/include -f - linux | tar -xf - -C $(STAGING_DIR)/include/
	tar -ch -C $(LINUX_DIR)/include -f - asm | tar -xf - -C $(STAGING_DIR)/include/

$(TARGET_MODULES_DIR): 
	-mkdir -p $(TARGET_MODULES_DIR)

$(KERNEL_IPKG):
	rm -rf $(KERNEL_IDIR)
	mkdir -p $(KERNEL_IDIR)/etc
	$(SCRIPT_DIR)/make-ipkg-dir.sh $(KERNEL_IDIR) ../control/kernel.control $(PKG_RELEASE) $(ARCH)
	echo 'Package: kernel-$(LINUX_VERSION)-$(BOARD)' >> $(KERNEL_IDIR)/CONTROL/control
	if [ -f ./config/$(BOARD).modules ]; then \
		cp ./config/$(BOARD).modules $(KERNEL_IDIR)/etc/modules; \
	fi
	$(IPKG_BUILD) $(KERNEL_IDIR) $(LINUX_BUILD_DIR) $(MAKE_TRACE)
	
$(STAMP_DIR)/.linux-compile: $(LINUX_DIR)/.modules_done
	@mkdir -p $(STAMP_DIR)
	@$(MAKE) $(TARGETS)
	ln -sf $(LINUX_BUILD_DIR)/linux-$(LINUX_VERSION) $(LINUX_DIR)
	@$(TRACE) target/linux/package
	$(MAKE) -C $(TOPDIR)/target/linux/package \
		$(KPKG_MAKEOPTS) \
		compile
	touch $@

source: $(DL_DIR)/$(LINUX_SOURCE)
prepare: $(LINUX_DIR)/.configured
compile:
	$(MAKE) $(STAMP_DIR)/.linux-compile $(MAKE_TRACE)

install: compile $(TARGET_MODULES_DIR) $(KERNEL_IPKG)
	rm -rf $(LINUX_BUILD_DIR)/root*
	cp -fpR $(BUILD_DIR)/root $(LINUX_BUILD_DIR)/
	echo -e 'dest root /\noption offline_root $(LINUX_BUILD_DIR)/root' > $(LINUX_BUILD_DIR)/ipkg.conf
	$(MAKE) -C $(TOPDIR)/target/linux/package \
		$(KPKG_MAKEOPTS) \
		install
	@{ [ "$(INSTALL_TARGETS)" != "" ] && $(IPKG_KERNEL) install $(INSTALL_TARGETS) || true; } $(MAKE_TRACE)

mostlyclean:
	rm -f $(LINUX_BUILD_DIR)/linux-$(LINUX_VERSION)/.modules_done
	rm -f $(LINUX_BUILD_DIR)/linux-$(LINUX_VERSION)/.drivers-unpacked
	$(MAKE) -C $(LINUX_BUILD_DIR)/linux-$(LINUX_VERSION) clean
	rm -f $(LINUX_KERNEL) $(LINUX_IMAGE)

rebuild:
	-$(MAKE) mostlyclean
	if [ -f $(LINUX_KERNEL) ]; then \
		$(MAKE) clean; \
	fi
	$(MAKE) compile

clean:
	rm -f $(STAMP_DIR)/.linux-compile
	rm -rf $(LINUX_BUILD_DIR)
	rm -f $(TARGETS)
