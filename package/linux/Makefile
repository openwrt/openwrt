include $(TOPDIR)/rules.mk
include ./linux.mk

PKG_BUILD_DIR := $(BUILD_DIR)/linux-modules
PKG_RELEASE := 1

TARGETS := 
INSTALL_TARGETS := 

define KMOD_template
PKG_$(1) := $(PACKAGE_DIR)/kmod-$(2)_$(LINUX_VERSION)-$(PKG_RELEASE)_$(ARCH).ipk
I_$(1) := $(PKG_BUILD_DIR)/ipkg/$(2)
ifneq ($(BR2_PACKAGE_KMOD_$(1)),)
TARGETS += $$(PKG_$(1))
endif
ifeq ($(BR2_PACKAGE_KMOD_$(1)),y)
INSTALL_TARGETS += $$(PKG_$(1))
endif

$$(PKG_$(1)): $(LINUX_DIR)/.modules_done
	mkdir -p $$(I_$(1))/lib/modules/$(LINUX_VERSION)
	$(SCRIPT_DIR)/make-ipkg-dir.sh $$(I_$(1)) control/kmod-$(2).control $(LINUX_VERSION)-$(PKG_RELEASE) $(ARCH)
	cp $(3) $$(I_$(1))/lib/modules/$(LINUX_VERSION)
	$(IPKG_BUILD) $$(I_$(1)) $(PACKAGE_DIR)

endef

$(eval $(call KMOD_template,ARPT,arptables,\
	$(MODULES_DIR)/kernel/net/ipv4/netfilter/arp*.o \
))
$(eval $(call KMOD_template,EBT,ebtables,\
	$(MODULES_DIR)/kernel/net/bridge/netfilter/*.o \
))
$(eval $(call KMOD_template,IPT4,iptables-extra,\
	$(MODULES_DIR)/kernel/net/ipv4/netfilter/ip*.o \
))
$(eval $(call KMOD_template,IPT6,ip6tables,\
	$(MODULES_DIR)/kernel/net/ipv6/netfilter/ip*.o \
))
$(eval $(call KMOD_template,IPV6,ipv6,\
	$(MODULES_DIR)/kernel/net/ipv6/ipv6.o \
))
$(eval $(call KMOD_template,NFS,nfs,\
	$(MODULES_DIR)/kernel/fs/lockd/*.o \
	$(MODULES_DIR)/kernel/fs/nfs/*.o \
	$(MODULES_DIR)/kernel/net/sunrpc/*.o \
))
$(eval $(call KMOD_template,USB,usb,\
	$(MODULES_DIR)/kernel/drivers/scsi/*.o \
	$(MODULES_DIR)/kernel/drivers/usb/*.o \
	$(MODULES_DIR)/kernel/drivers/usb/*/*.o \
))
$(eval $(call KMOD_template,CRYPTO,crypto,\
	$(MODULES_DIR)/kernel/crypto/*.o \
))
$(eval $(call KMOD_template,MPPE,mppe,\
	$(MODULES_DIR)/kernel/drivers/net/ppp_mppe_mppc.o \
))
$(eval $(call KMOD_template,TUN,tun,\
	$(MODULES_DIR)/kernel/drivers/net/tun.o \
))
$(eval $(call KMOD_template,GRE,gre,\
	$(MODULES_DIR)/kernel/net/ipv4/ip_gre.o \
))
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
