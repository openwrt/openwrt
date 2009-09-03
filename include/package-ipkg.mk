# 
# Copyright (C) 2006,2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

# where to build (and put) .ipk packages
IPKG:= \
  IPKG_TMP=$(TMP_DIR)/ipkg \
  IPKG_INSTROOT=$(TARGET_DIR) \
  IPKG_CONF_DIR=$(STAGING_DIR)/etc \
  IPKG_OFFLINE_ROOT=$(TARGET_DIR) \
  $(SCRIPT_DIR)/ipkg -force-defaults -force-depends

# invoke ipkg-build with some default options
IPKG_BUILD:= \
  ipkg-build -c -o 0 -g 0

IPKG_STATE_DIR:=$(TARGET_DIR)/usr/lib/opkg

define BuildIPKGVariable
  $(call shexport,Package/$(1)/$(2))
  $(1)_COMMANDS += var2file "$(call shvar,Package/$(1)/$(2))" $(2);
endef

dep_split=$(subst :,$(space),$(1))
dep_confvar=CONFIG_$(word 1,$(call dep_split,$(1)))
dep_val=$(word 2,$(call dep_split,$(1)))
strip_deps=$(strip $(subst +,,$(filter-out @%,$(1))))
filter_deps=$(foreach dep,$(call strip_deps,$(1)),$(if $(findstring :,$(dep)),$(if $($(call dep_confvar,$(dep))),$(call dep_val,$(dep))),$(dep)))

ifeq ($(DUMP),)
  define BuildTarget/ipkg
    IPKG_$(1):=$(PACKAGE_DIR)/$(1)_$(VERSION)_$(PKGARCH).ipk
    IDIR_$(1):=$(PKG_BUILD_DIR)/ipkg-$(PKGARCH)/$(1)
    INFO_$(1):=$(IPKG_STATE_DIR)/info/$(1).list

    ifdef Package/$(1)/install
      ifneq ($(CONFIG_PACKAGE_$(1))$(SDK)$(DEVELOPER),)
        compile: $$(IPKG_$(1)) $(STAGING_DIR_ROOT)/stamp/.$(1)_installed

        ifeq ($(CONFIG_PACKAGE_$(1)),y)
          install: $$(INFO_$(1))
        endif
      else
        compile: $(1)-disabled
        $(1)-disabled:
		@echo "WARNING: skipping $(1) -- package not selected"
      endif
    endif

    IDEPEND_$(1):=$$(call filter_deps,$$(DEPENDS))
  
    $(eval $(call BuildIPKGVariable,$(1),conffiles))
    $(eval $(call BuildIPKGVariable,$(1),preinst))
    $(eval $(call BuildIPKGVariable,$(1),postinst))
    $(eval $(call BuildIPKGVariable,$(1),prerm))
    $(eval $(call BuildIPKGVariable,$(1),postrm))

    $(STAGING_DIR_ROOT)/stamp/.$(1)_installed: $(STAMP_BUILT)
	rm -rf $(STAGING_DIR_ROOT)/tmp-$(1)
	mkdir -p $(STAGING_DIR_ROOT)/stamp $(STAGING_DIR_ROOT)/tmp-$(1)
	$(call Package/$(1)/install,$(STAGING_DIR_ROOT)/tmp-$(1))
	$(call Package/$(1)/install_lib,$(STAGING_DIR_ROOT)/tmp-$(1))
	$(CP) $(STAGING_DIR_ROOT)/tmp-$(1)/* $(STAGING_DIR_ROOT)/
	rm -rf $(STAGING_DIR_ROOT)/tmp-$(1)
	touch $$@

    $$(IPKG_$(1)): $(STAGING_DIR)/etc/ipkg.conf $(STAMP_BUILT)
	@rm -f $(PACKAGE_DIR)/$(1)_*
	rm -rf $$(IDIR_$(1))
	mkdir -p $$(IDIR_$(1))/CONTROL
	echo "Package: $(1)" > $$(IDIR_$(1))/CONTROL/control
	echo "Version: $(VERSION)" >> $$(IDIR_$(1))/CONTROL/control
	( \
		DEPENDS='$(EXTRA_DEPENDS)'; \
		for depend in $$(filter-out @%,$$(IDEPEND_$(1))); do \
			DEPENDS=$$$${DEPENDS:+$$$$DEPENDS, }$$$${depend##+}; \
		done; \
		echo "Depends: $$$$DEPENDS"; \
		echo "Provides: $(PROVIDES)"; \
		echo "Source: $(SOURCE)"; \
		echo "Section: $(SECTION)"; \
		echo "Priority: $(PRIORITY)"; \
		echo "Maintainer: $(MAINTAINER)"; \
		echo "Architecture: $(PKGARCH)"; \
		echo "Installed-Size: 1"; \
		echo -n "Description: "; getvar $(call shvar,Package/$(1)/description) | sed -e 's,^[[:space:]]*, ,g'; \
 	) >> $$(IDIR_$(1))/CONTROL/control
	chmod 644 $$(IDIR_$(1))/CONTROL/control
	(cd $$(IDIR_$(1))/CONTROL; \
		$($(1)_COMMANDS) \
	)
	$(call Package/$(1)/install,$$(IDIR_$(1)))
	mkdir -p $(PACKAGE_DIR)
	-find $$(IDIR_$(1)) -name 'CVS' -o -name '.svn' -o -name '.#*' | $(XARGS) rm -rf
	$(RSTRIP) $$(IDIR_$(1))
	SIZE=`cd $$(IDIR_$(1)); du -bs --exclude=./CONTROL . 2>/dev/null | cut -f1`; \
	$(SED) "s|^\(Installed-Size:\).*|\1 $$$$SIZE|g" $$(IDIR_$(1))/CONTROL/control
	$(IPKG_BUILD) $$(IDIR_$(1)) $(PACKAGE_DIR)
	@[ -f $$(IPKG_$(1)) ] || false 

    $$(INFO_$(1)): $$(IPKG_$(1))
	$(IPKG) install $$(IPKG_$(1))

    $(1)-clean:
	rm -f $(PACKAGE_DIR)/$(1)_*

    clean: $(1)-clean

  endef

  $(STAGING_DIR)/etc/ipkg.conf:
	mkdir -p $(STAGING_DIR)/etc
	echo "dest root /" > $(STAGING_DIR)/etc/ipkg.conf
	echo "option offline_root $(TARGET_DIR)" >> $(STAGING_DIR)/etc/ipkg.conf

endif
