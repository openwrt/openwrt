# 
# Copyright (C) 2006,2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define BuildIPKGVariable
  $(call shexport,Package/$(1)/$(2))
  $(1)_COMMANDS += var2file "$(call shvar,Package/$(1)/$(2))" $(2);
endef

ifeq ($(DUMP),)
  define BuildIPKG
    IPKG_$(1):=$(PACKAGE_DIR)/$(1)_$(VERSION)_$(PKGARCH).ipk
    IDIR_$(1):=$(PKG_BUILD_DIR)/ipkg/$(1)
    INFO_$(1):=$(IPKG_STATE_DIR)/info/$(1).list

    ifdef Package/$(1)/install
      ifneq ($(CONFIG_PACKAGE_$(1)),)
        compile: $$(IPKG_$(1))

        ifeq ($(CONFIG_PACKAGE_$(1)),y)
          install: $$(INFO_$(1))
        endif

        ifneq ($(MAKECMDGOALS),prereq)
          ifneq ($$(shell $(SCRIPT_DIR)/timestamp.pl -p -x ipkg -x ipkg-install '$$(IPKG_$(1))' '$(PKG_BUILD_DIR)'),$$(IPKG_$(1)))
            $(PKG_BUILD_DIR)/.built: package-rebuild
            $$(info Rebuilding $(subst $(TOPDIR)/,,$$(IPKG_$(1))))
          endif
        endif

      else
        compile: $(1)-disabled
        $(1)-disabled:
		@echo "WARNING: skipping $(1) -- package not selected"
      endif
    endif

    IDEPEND_$(1):=$$(strip $$(DEPENDS))
  
    $(eval $(call BuildIPKGVariable,$(1),conffiles))
    $(eval $(call BuildIPKGVariable,$(1),preinst))
    $(eval $(call BuildIPKGVariable,$(1),postinst))
    $(eval $(call BuildIPKGVariable,$(1),prerm))
    $(eval $(call BuildIPKGVariable,$(1),postrm))
    $$(IDIR_$(1))/CONTROL/control: $(PKG_BUILD_DIR)/.version-$(1)_$(VERSION)_$(PKGARCH)
	@rm -f $(PACKAGE_DIR)/$(1)_*
	mkdir -p $$(IDIR_$(1))/CONTROL
	echo "Package: $(1)" > $$(IDIR_$(1))/CONTROL/control
	echo "Version: $(VERSION)" >> $$(IDIR_$(1))/CONTROL/control
	( \
		DEPENDS='$(EXTRA_DEPENDS)'; \
		for depend in $$(filter-out @%,$$(IDEPEND_$(1))); do \
			DEPENDS=$$$${DEPENDS:+$$$$DEPENDS, }$$$${depend##+}; \
		done; \
		echo "Depends: $$$$DEPENDS"; \
		echo "Source: $(SOURCE)"; \
		echo "Section: $(SECTION)"; \
		echo "Priority: $(PRIORITY)"; \
		echo "Maintainer: $(MAINTAINER)"; \
		echo "Architecture: $(PKGARCH)"; \
		echo -n "Description: "; getvar $(call shvar,Package/$(1)/description) | sed -e 's,^[[:space:]]*, ,g'; \
 	) >> $$(IDIR_$(1))/CONTROL/control
	chmod 644 $$(IDIR_$(1))/CONTROL/control
	(cd $$(IDIR_$(1))/CONTROL; \
		$($(1)_COMMANDS) \
	)

    $$(IPKG_$(1)): $(PKG_BUILD_DIR)/.built $$(IDIR_$(1))/CONTROL/control
	$(call Package/$(1)/install,$$(IDIR_$(1)))
	mkdir -p $(PACKAGE_DIR)
	-find $$(IDIR_$(1)) -name CVS   | xargs rm -rf
	-find $$(IDIR_$(1)) -name .svn  | xargs rm -rf
	-find $$(IDIR_$(1)) -name '.#*' | xargs rm -f
	$(RSTRIP) $$(IDIR_$(1))
	$(IPKG_BUILD) $$(IDIR_$(1)) $(PACKAGE_DIR)
	@[ -f $$(IPKG_$(1)) ] || false 

    $$(INFO_$(1)): $$(IPKG_$(1))
	$(IPKG) install $$(IPKG_$(1))

    $(1)-clean:
	rm -f $(PACKAGE_DIR)/$(1)_*

    clean: $(1)-clean

    $(PKG_BUILD_DIR)/.version-$(1)_$(VERSION)_$(PKGARCH): $(PKG_BUILD_DIR)/.prepared
	-@rm -f $(PKG_BUILD_DIR)/.version-$(1)_* 2>/dev/null
	@touch $$@

    $$(eval $$(call Build/DefaultTargets,$(1)))

  endef
endif
