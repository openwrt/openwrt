# $Id$

include $(TOPDIR)/rules.mk
include ./linux.mk
include ./linux.config

PKG_BUILD_DIR := $(BUILD_DIR)/linux-modules
PKG_RELEASE := 1

TARGETS := 
INSTALL_TARGETS := 


define KMOD_template
ifeq ($$(strip $(4)),)
KDEPEND_$(1):=m
else
KDEPEND_$(1):=$($(4))
endif

PKG_$(1) := $(PACKAGE_DIR)/kmod-$(2)_$(LINUX_VERSION)-$(PKG_RELEASE)_$(ARCH).ipk
I_$(1) := $(PKG_BUILD_DIR)/ipkg/$(2)

ifeq ($$(KDEPEND_$(1)),m)
ifneq ($(BR2_PACKAGE_KMOD_$(1)),)
TARGETS += $$(PKG_$(1))
endif
ifeq ($(BR2_PACKAGE_KMOD_$(1)),y)
INSTALL_TARGETS += $$(PKG_$(1))
endif
endif

$$(PKG_$(1)): $(LINUX_DIR)/.modules_done
	mkdir -p $$(I_$(1))/lib/modules/$(LINUX_VERSION)
	$(SCRIPT_DIR)/make-ipkg-dir.sh $$(I_$(1)) control/kmod-$(2).control $(LINUX_VERSION)-$(PKG_RELEASE) $(ARCH)
	cp $(3) $$(I_$(1))/lib/modules/$(LINUX_VERSION)
	$(IPKG_BUILD) $$(I_$(1)) $(PACKAGE_DIR)

endef

$(eval $(call KMOD_template,ARPT,arptables,\
	$(MODULES_DIR)/kernel/net/ipv4/netfilter/arp*.o \
,CONFIG_IP_NF_ARPTABLES))
$(eval $(call KMOD_template,EBT,ebtables,\
	$(MODULES_DIR)/kernel/net/bridge/netfilter/*.o \
,CONFIG_BRIDGE_NF_EBTABLES))
$(eval $(call KMOD_template,IPT4,iptables-extra,\
	$(MODULES_DIR)/kernel/net/ipv4/netfilter/ip*.o \
))
$(eval $(call KMOD_template,IPT6,ip6tables,\
	$(MODULES_DIR)/kernel/net/ipv6/netfilter/ip*.o \
,CONFIG_IP6_NF_IPTABLES))
$(eval $(call KMOD_template,IPV6,ipv6,\
	$(MODULES_DIR)/kernel/net/ipv6/ipv6.o \
,CONFIG_IPV6))
$(eval $(call KMOD_template,SMBFS,smbfs,\
	$(MODULES_DIR)/kernel/fs/smbfs/smbfs.o \
,CONFIG_SMB_FS))
$(eval $(call KMOD_template,NFS,nfs,\
	$(MODULES_DIR)/kernel/fs/lockd/*.o \
	$(MODULES_DIR)/kernel/fs/nfs/*.o \
	$(MODULES_DIR)/kernel/net/sunrpc/*.o \
,CONFIG_NFS_FS))
$(eval $(call KMOD_template,USB,usb,\
	$(MODULES_DIR)/kernel/drivers/scsi/*.o \
	$(MODULES_DIR)/kernel/drivers/usb/*.o \
	$(MODULES_DIR)/kernel/drivers/usb/*/*.o \
,CONFIG_USB))
$(eval $(call KMOD_template,LP,lp,\
	$(MODULES_DIR)/kernel/drivers/parport/parport.o \
	$(MODULES_DIR)/kernel/drivers/parport/parport_splink.o \
	$(MODULES_DIR)/kernel/drivers/char/lp.o \
,CONFIG_PARPORT))
$(eval $(call KMOD_template,VFAT,vfat,\
	$(MODULES_DIR)/kernel/fs/vfat/vfat.o \
	$(MODULES_DIR)/kernel/fs/fat/fat.o \
	$(MODULES_DIR)/kernel/fs/msdos/msdos.o \
,CONFIG_VFAT_FS))
$(eval $(call KMOD_template,CRYPTO,crypto,\
	$(MODULES_DIR)/kernel/crypto/*.o \
))
$(eval $(call KMOD_template,PPP,ppp,\
	$(MODULES_DIR)/kernel/drivers/net/ppp_generic.o \
	$(MODULES_DIR)/kernel/drivers/net/slhc.o \
,CONFIG_PPP))
$(eval $(call KMOD_template,MPPE,mppe,\
	$(MODULES_DIR)/kernel/drivers/net/ppp_mppe_mppc.o \
,CONFIG_PPP_MPPE_MPPC))
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

source: linux-source
prepare: $(LINUX_DIR)/.configured
compile: $(LINUX_DIR)/.modules_done $(TARGETS)

install: compile $(TARGET_MODULES_DIR)
	@[ "$(INSTALL_TARGETS)" != "" ] && $(IPKG) install $(INSTALL_TARGETS) || true

clean: linux-dirclean
	rm -f $(TARGETS)
