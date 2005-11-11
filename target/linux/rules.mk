LINUX_KARCH:=$(shell echo $(ARCH) | sed -e 's/i[3-9]86/i386/' \
	-e 's/mipsel/mips/' \
	-e 's/powerpc/ppc/' \
	-e 's/sh[234]/sh/' \
)

ifeq ($(KERNEL),2.6)
LINUX_KMOD_SUFFIX=ko
else
LINUX_KMOD_SUFFIX=o
endif

define KMOD_template
ifeq ($$(strip $(4)),)
KDEPEND_$(1):=m
else
KDEPEND_$(1):=$($(4))
endif

ifeq ($$(strip $(5)),)
IDEPEND_$(1):=kernel-$(LINUX_VERSION)-$(BOARD) ($(PKG_RELEASE))
else
IDEPEND_$(1):=kernel-$(LINUX_VERSION)-$(BOARD) ($(PKG_RELEASE)), $(5)
endif

PKG_$(1) := $(PACKAGE_DIR)/kmod-$(2)_$(LINUX_VERSION)-$(BOARD)-$(PKG_RELEASE)_$(ARCH).ipk
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
	rm -rf $$(I_$(1))
	$(SCRIPT_DIR)/make-ipkg-dir.sh $$(I_$(1)) ../control/kmod-$(2).control $(LINUX_VERSION)-$(BOARD)-$(PKG_RELEASE) $(ARCH)
	echo "Depends: $$(IDEPEND_$(1))" >> $$(I_$(1))/CONTROL/control
ifneq ($(strip $(3)),)
	mkdir -p $$(I_$(1))/lib/modules/$(LINUX_VERSION)
	cp $(3) $$(I_$(1))/lib/modules/$(LINUX_VERSION)
endif
ifneq ($(6),)
	mkdir -p $$(I_$(1))/etc/modules.d
	for module in $(7); do \
		echo $$$$module >> $$(I_$(1))/etc/modules.d/$(6)-$(2); \
	done
endif
	$(8)
	$(IPKG_BUILD) $$(I_$(1)) $(PACKAGE_DIR) $(MAKE_TRACE)

endef


