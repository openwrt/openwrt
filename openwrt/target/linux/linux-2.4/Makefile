#############################################################
# $Id$
#
# Linux 2.4 kernel target for the OpenWRT project
#
# patches are sorted by numbers
# 0xx	needed patches for board, cpu, filesystem
# 1xx	addon patches for new features (netfilter, ebtables, cifs,..)
# 2xx	compile fixes, driver fixes
#
#############################################################

include $(TOPDIR)/rules.mk
include ../rules.mk

# for testing
ifeq ($(BOARD),)
BOARD:=brcm
endif

LINUX_VERSION:=2.4.30
MODULES_SUBDIR:=lib/modules/$(LINUX_VERSION)
LINUX_KCONFIG:=./config/$(BOARD)
LINUX_BUILD_DIR:=$(BUILD_DIR)/linux-2.4-$(BOARD)
LINUX_TARGET_DIR:=$(LINUX_BUILD_DIR)/root
LINUX_KERNEL:=$(LINUX_BUILD_DIR)/vmlinux
LINUX_KARCH:=$(shell echo $(ARCH) | sed -e 's/i[3-9]86/i386/' \
	-e 's/mipsel/mips/' \
	-e 's/powerpc/ppc/' \
	-e 's/sh[234]/sh/' \
	)

LINUX_BINARY_DRIVER_SITE := http://openwrt.org/downloads/sources/
LINUX_SOURCE := linux-$(LINUX_VERSION).tar.bz2
LINUX_SOURCE_DIR := $(LINUX_BUILD_DIR)/linux-$(LINUX_VERSION)
MODULES_DIR := $(LINUX_BUILD_DIR)/modules/$(MODULES_SUBDIR)
TARGET_MODULES_DIR := $(TARGET_DIR)/lib/modules/$(LINUX_VERSION)
IPKG_KERNEL:=IPKG_TMP=$(BUILD_DIR)/tmp IPKG_INSTROOT=$(LINUX_TARGET_DIR) IPKG_CONF_DIR=$(LINUX_BUILD_DIR) $(SCRIPT_DIR)/ipkg -force-defaults -force-depends

PKG_BUILD_DIR := $(LINUX_BUILD_DIR)/linux-modules
PKG_RELEASE := 1

KERNEL_IPKG:=$(LINUX_BUILD_DIR)/kernel_$(LINUX_VERSION)-$(BOARD)-$(PKG_RELEASE)_$(ARCH).ipk
KERNEL_IDIR:=$(LINUX_BUILD_DIR)/kernel-ipkg

include $(LINUX_KCONFIG)

INSTALL_TARGETS := $(KERNEL_IPKG)
TARGETS := 

ifeq ($(BOARD),brcm)
include ./broadcom.mk
endif

$(eval $(call KMOD_template,B44,b44,\
	$(MODULES_DIR)/kernel/drivers/net/b44.o \
,CONFIG_B44))
$(eval $(call KMOD_template,ARPT,arptables,\
	$(MODULES_DIR)/kernel/net/ipv4/netfilter/arp*.o \
,CONFIG_IP_NF_ARPTABLES))
$(eval $(call KMOD_template,EBT,ebtables,\
	$(MODULES_DIR)/kernel/net/bridge/netfilter/*.o \
,CONFIG_BRIDGE_NF_EBTABLES))
$(eval $(call KMOD_template,IPTABLES_V4_EXTRA,iptables-extra,\
	$(MODULES_DIR)/kernel/net/ipv4/netfilter/ip*.o \
))
$(eval $(call KMOD_template,IPTABLES_V6,ip6tables,\
	$(MODULES_DIR)/kernel/net/ipv6/netfilter/ip*.o \
,CONFIG_IP6_NF_IPTABLES,kmod-ipv6))
$(eval $(call KMOD_template,IPV6,ipv6,\
	$(MODULES_DIR)/kernel/net/ipv6/ipv6.o \
,CONFIG_IPV6))
$(eval $(call KMOD_template,CIFS,cifs,\
	$(MODULES_DIR)/kernel/fs/cifs/cifs.o \
,CONFIG_CIFS))
$(eval $(call KMOD_template,NFS,nfs,\
	$(MODULES_DIR)/kernel/fs/lockd/*.o \
	$(MODULES_DIR)/kernel/fs/nfs/*.o \
	$(MODULES_DIR)/kernel/net/sunrpc/*.o \
,CONFIG_NFS_FS))
$(eval $(call KMOD_template,USB,usb-core,\
	$(MODULES_DIR)/kernel/drivers/usb/usbcore.o \
,CONFIG_USB))
$(eval $(call KMOD_template,USB_UHCI,usb-uhci,\
	$(MODULES_DIR)/kernel/drivers/usb/host/uhci.o \
,CONFIG_USB_UHCI_ALT,kmod-usb-core))
$(eval $(call KMOD_template,USB_OHCI,usb-ohci,\
	$(MODULES_DIR)/kernel/drivers/usb/host/usb-ohci.o \
,CONFIG_USB_OHCI,kmod-usb-core))
$(eval $(call KMOD_template,USB2,usb2,\
	$(MODULES_DIR)/kernel/drivers/usb/host/ehci-hcd.o \
,CONFIG_USB_EHCI_HCD,kmod-usb-core))
$(eval $(call KMOD_template,USB_STORAGE,usb-storage,\
	$(MODULES_DIR)/kernel/drivers/scsi/*.o \
	$(MODULES_DIR)/kernel/drivers/usb/storage/*.o \
,CONFIG_USB_STORAGE))
$(eval $(call KMOD_template,USB_PRINTER,usb-printer,\
	$(MODULES_DIR)/kernel/drivers/usb/printer.o \
,CONFIG_USB_PRINTER))
$(eval $(call KMOD_template,IDE,ide,\
	$(MODULES_DIR)/kernel/drivers/ide/*.o \
	$(MODULES_DIR)/kernel/drivers/ide/*/*.o \
,CONFIG_IDE))
$(eval $(call KMOD_template,EXT2,ext2,\
	$(MODULES_DIR)/kernel/fs/ext2/*.o \
,CONFIG_EXT2_FS))
$(eval $(call KMOD_template,EXT3,ext3,\
	$(MODULES_DIR)/kernel/fs/ext3/*.o \
	$(MODULES_DIR)/kernel/fs/jbd/*.o \
,CONFIG_EXT3_FS))
$(eval $(call KMOD_template,XFS,xfs,\
	$(MODULES_DIR)/kernel/fs/xfs/*.o \
,CONFIG_XFS_FS))
$(eval $(call KMOD_template,HFSPLUS,hfsplus,\
	$(MODULES_DIR)/kernel/fs/hfsplus/*.o \
,CONFIG_HFSPLUS_FS))
$(eval $(call KMOD_template,VFAT,vfat,\
	$(MODULES_DIR)/kernel/fs/vfat/vfat.o \
	$(MODULES_DIR)/kernel/fs/fat/fat.o \
,CONFIG_VFAT_FS))
$(eval $(call KMOD_template,CRYPTO,crypto,\
	$(MODULES_DIR)/kernel/crypto/*.o \
))
$(eval $(call KMOD_template,PPP,ppp,\
	$(MODULES_DIR)/kernel/drivers/net/ppp_async.o \
	$(MODULES_DIR)/kernel/drivers/net/ppp_generic.o \
	$(MODULES_DIR)/kernel/drivers/net/slhc.o \
,CONFIG_PPP))
$(eval $(call KMOD_template,MPPE,mppe,\
	$(MODULES_DIR)/kernel/drivers/net/ppp_mppe_mppc.o \
,CONFIG_PPP_MPPE_MPPC))
$(eval $(call KMOD_template,PPPOE,pppoe,\
	$(MODULES_DIR)/kernel/drivers/net/pppoe.o \
	$(MODULES_DIR)/kernel/drivers/net/pppox.o \
,CONFIG_PPPOE))
$(eval $(call KMOD_template,TUN,tun,\
	$(MODULES_DIR)/kernel/drivers/net/tun.o \
,CONFIG_TUN))
$(eval $(call KMOD_template,GRE,gre,\
	$(MODULES_DIR)/kernel/net/ipv4/ip_gre.o \
,CONFIG_NET_IPGRE))
$(eval $(call KMOD_template,SCHED,sched,\
	$(MODULES_DIR)/kernel/net/sched/*.o \
))

$(TARGETS): $(PACKAGE_DIR)

$(PACKAGE_DIR):
	mkdir -p $(PACKAGE_DIR)

$(LINUX_DIR)/.unpacked: $(DL_DIR)/$(LINUX_SOURCE) $(DL_DIR)/$(LINUX_BINARY_WL_DRIVER) $(DL_DIR)/$(LINUX_ET_DRIVER)
	-mkdir -p $(LINUX_BUILD_DIR)
	bzcat $(DL_DIR)/$(LINUX_SOURCE) | tar -C $(LINUX_BUILD_DIR) $(TAR_OPTIONS) -
	rm -f $(LINUX_DIR)
	ln -s $(LINUX_BUILD_DIR)/linux-$(LINUX_VERSION) $(LINUX_DIR)
	touch $(LINUX_DIR)/.unpacked

$(LINUX_DIR)/.patched: $(LINUX_DIR)/.unpacked
	$(PATCH) $(LINUX_DIR) ./patches/generic
	[ -d ./patches/$(BOARD) ] && $(PATCH) $(LINUX_DIR) ./patches/$(BOARD)
	touch $(LINUX_DIR)/.patched

$(LINUX_DIR)/.configured: $(LINUX_DIR)/.patched
	-cp $(LINUX_KCONFIG) $(LINUX_DIR)/.config
	$(SED) "s,^CROSS_COMPILE.*,CROSS_COMPILE=$(KERNEL_CROSS),g;" \
	  $(LINUX_DIR)/Makefile  \
	  $(LINUX_DIR)/arch/mips/Makefile
	$(SED) "s,\-mcpu=,\-mtune=,g;" $(LINUX_DIR)/arch/mips/Makefile
	$(MAKE) -C $(LINUX_DIR) ARCH=$(LINUX_KARCH) oldconfig include/linux/version.h
	touch $(LINUX_DIR)/.configured

$(LINUX_DIR)/.depend_done:  $(LINUX_DIR)/.configured
	$(MAKE) -C $(LINUX_DIR) ARCH=$(LINUX_KARCH) dep
	touch $(LINUX_DIR)/.depend_done

$(LINUX_DIR)/vmlinux: $(LINUX_DIR)/.depend_done
	$(MAKE) -C $(LINUX_DIR) ARCH=$(LINUX_KARCH) PATH=$(TARGET_PATH)

$(LINUX_KERNEL): $(LINUX_DIR)/vmlinux
	$(TARGET_CROSS)objcopy -O binary -R .reginfo -R .note -R .comment -R .mdebug -S $< $@ 
	touch -c $(LINUX_KERNEL)
	
$(LINUX_DIR)/.modules_done: $(LINUX_KERNEL) $(LINUX_IMAGE)
	rm -rf $(LINUX_BUILD_DIR)/modules
	$(MAKE) -C $(LINUX_DIR) ARCH=$(LINUX_KARCH) PATH=$(TARGET_PATH) modules
	$(MAKE) -C $(LINUX_DIR) DEPMOD=true INSTALL_MOD_PATH=$(LINUX_BUILD_DIR)/modules modules_install
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
	$(SCRIPT_DIR)/make-ipkg-dir.sh $(KERNEL_IDIR) ../control/kernel.control $(LINUX_VERSION)-$(BOARD)-$(PKG_RELEASE) $(ARCH)
	if [ -f ./config/$(BOARD).modules ]; then \
		cp ./config/$(BOARD).modules $(KERNEL_IDIR)/etc/modules; \
	fi
	$(IPKG_BUILD) $(KERNEL_IDIR) $(LINUX_BUILD_DIR)

source: $(DL_DIR)/$(LINUX_SOURCE)
prepare: $(LINUX_DIR)/.configured
compile: $(LINUX_DIR)/.modules_done $(TARGETS)
	ln -sf $(LINUX_BUILD_DIR)/linux-$(LINUX_VERSION) $(LINUX_DIR)
	$(MAKE) -C $(TOPDIR)/target/linux/package \
		BOARD="$(BOARD)" \
		TARGET_DIR="$(LINUX_TARGET_DIR)" \
		BUILD_DIR="$(LINUX_BUILD_DIR)" \
		KERNEL_DIR="$(LINUX_SOURCE_DIR)" \
		LINUX_VERSION="$(LINUX_VERSION)" \
		KERNEL_RELEASE="$(PKG_RELEASE)"

install: compile $(TARGET_MODULES_DIR) $(KERNEL_IPKG)
	rm -rf $(LINUX_BUILD_DIR)/root*
	cp -a $(BUILD_DIR)/root $(LINUX_BUILD_DIR)/
	echo -e 'dest root /\noption offline_root $(LINUX_BUILD_DIR)/root' > $(LINUX_BUILD_DIR)/ipkg.conf
	[ "$(INSTALL_TARGETS)" != "" ] && $(IPKG_KERNEL) install $(INSTALL_TARGETS) || true

mostlyclean:
	rm -f $(LINUX_BUILD_DIR)/linux-$(LINUX_VERSION)/.{drivers-unpacked,modules_done}
	$(MAKE) -C $(LINUX_BUILD_DIR)/linux-$(LINUX_VERSION) clean
	rm -f $(LINUX_KERNEL) $(LINUX_IMAGE)

rebuild:
	-$(MAKE) mostlyclean
	if [ -f $(LINUX_KERNEL) ]; then \
		$(MAKE) clean; \
	fi
	$(MAKE) compile

clean:
	rm -rf $(LINUX_BUILD_DIR)
	rm -f $(TARGETS)
