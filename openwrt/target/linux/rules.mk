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
	mkdir -p $$(I_$(1))/lib/modules/$(LINUX_VERSION)
	$(SCRIPT_DIR)/make-ipkg-dir.sh $$(I_$(1)) ../control/kmod-$(2).control $(LINUX_VERSION)-$(BOARD)-$(PKG_RELEASE) $(ARCH)
	echo "Depends: $$(IDEPEND_$(1))" >> $$(I_$(1))/CONTROL/control
	cp $(3) $$(I_$(1))/lib/modules/$(LINUX_VERSION)
	$(6)
	$(IPKG_BUILD) $$(I_$(1)) $(PACKAGE_DIR)

endef
