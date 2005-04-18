# $(call PKG_template,EXAMPLE,example,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
define PKG_template

IPKG_$(1):=$(PACKAGE_DIR)/$(2)_$(3)_$(4).ipk
IDIR_$(1):=$(PKG_BUILD_DIR)/ipkg/$(2)
INFO_$(1):=$(IPKG_STATE_DIR)/info/$(2).list

PKG_FILES += $$(IPKG_$(1))
ifneq ($(BR2_PACKAGE_$(1)),)
COMPILE_TARGETS += $$(IPKG_$(1))
endif
ifeq ($(BR2_PACKAGE_$(1)),y)
INSTALL_TARGETS += $$(INFO_$(1))
endif

$$(IDIR_$(1))/CONTROL/control: $(PKG_BUILD_DIR)/.prepared
	$(SCRIPT_DIR)/make-ipkg-dir.sh $$(IDIR_$(1)) ./ipkg/$(2).control $(3) $(4)

$$(INFO_$(1)): $$(IPKG_$(1))
	$(IPKG) install $$(IPKG_$(1))

endef
