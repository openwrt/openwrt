# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2006-2022 OpenWrt.org

ifndef DUMP
  include $(INCLUDE_DIR)/feeds.mk
endif

IPKG_STATE_DIR:=$(TARGET_DIR)/usr/lib/opkg

define description_escape
$(subst `,\`,$(subst $$,\$$,$(subst ",\",$(subst \,\\,$(1)))))
endef

# Generates a make statement to return a wildcard for candidate ipkg files
# 1: package name
define gen_package_wildcard
  $(1)$$(if $$(filter -%,$$(ABIV_$(1))),,[^a-z-])*
endef

# 1: package name
# 2: candidate ipk files
define remove_ipkg_files
  $(if $(strip $(2)),$(SCRIPT_DIR)/ipkg-remove $(1) $(2))
endef

# 1: package name
# 2: variable name
# 3: variable suffix
# 4: file is a script
define BuildPackVariable
ifdef Package/$(1)/$(2)
  $$(PACK_$(1)) : VAR_$(2)$(3)=$$(Package/$(1)/$(2))
  $(call shexport,Package/$(1)/$(2))
  $(1)_COMMANDS += echo "$$$$$$$$$(call shvar,Package/$(1)/$(2))" > $(2)$(3); $(if $(4),chmod 0755 $(2)$(3);)
endif
endef

PARENL :=(
PARENR :=)

dep_split=$(subst :,$(space),$(1))
dep_rem=$(subst !,,$(subst $(strip $(PARENL)),,$(subst $(strip $(PARENR)),,$(word 1,$(call dep_split,$(1))))))
dep_and=dep_and_res:=$$(and $(subst $(space),$(comma),$(foreach cond,$(subst &&, ,$(1)),$$(CONFIG_$(cond)))))
dep_confvar=$(strip $(foreach cond,$(subst ||, ,$(call dep_rem,$(1))),$(eval $(call dep_and,$(cond)))$(dep_and_res)))
dep_pos=$(if $(call dep_confvar,$(1)),$(call dep_val,$(1)))
dep_neg=$(if $(call dep_confvar,$(1)),,$(call dep_val,$(1)))
dep_if=$(if $(findstring !,$(1)),$(call dep_neg,$(1)),$(call dep_pos,$(1)))
dep_val=$(word 2,$(call dep_split,$(1)))
strip_deps=$(strip $(subst +,,$(filter-out @%,$(1))))
filter_deps=$(foreach dep,$(call strip_deps,$(1)),$(if $(findstring :,$(dep)),$(call dep_if,$(dep)),$(dep)))

define AddDependency
  $$(if $(1),$$(if $(2),$$(foreach pkg,$(1),$$(PACK_$$(pkg))): $$(foreach pkg,$(2),$$(PACK_$$(pkg)))))
endef

define FixupReverseDependencies
  DEPS := $$(filter %:$(1),$$(IDEPEND))
  DEPS := $$(patsubst %:$(1),%,$$(DEPS))
  DEPS := $$(filter $$(DEPS),$$(IPKGS))
  $(call AddDependency,$$(DEPS),$(1))
endef

define FixupDependencies
  DEPS := $$(filter $(1):%,$$(IDEPEND))
  DEPS := $$(patsubst $(1):%,%,$$(DEPS))
  DEPS := $$(filter $$(DEPS),$$(IPKGS))
  $(call AddDependency,$(1),$$(DEPS))
endef

ifneq ($(PKG_NAME),toolchain)
  define CheckDependencies
	@( \
		rm -f $(PKG_INFO_DIR)/$(1).missing; \
		( \
			export \
				READELF=$(TARGET_CROSS)readelf \
				OBJCOPY=$(TARGET_CROSS)objcopy \
				XARGS="$(XARGS)"; \
			$(SCRIPT_DIR)/gen-dependencies.sh "$$(IDIR_$(1))"; \
		) | while read FILE; do \
			grep -qxF "$$$$FILE" $(PKG_INFO_DIR)/$(1).provides || \
				echo "$$$$FILE" >> $(PKG_INFO_DIR)/$(1).missing; \
		done; \
		if [ -f "$(PKG_INFO_DIR)/$(1).missing" ]; then \
			echo "Package $(1) is missing dependencies for the following libraries:" >&2; \
			cat "$(PKG_INFO_DIR)/$(1).missing" >&2; \
			false; \
		fi; \
	)
  endef
endif

_addsep=$(word 1,$(1))$(foreach w,$(wordlist 2,$(words $(1)),$(1)),$(strip $(2) $(w)))
_cleansep=$(subst $(space)$(2)$(space),$(2)$(space),$(1))
mergelist=$(call _cleansep,$(call _addsep,$(1),$(comma)),$(comma))
addfield=$(if $(strip $(2)),$(1): $(2))
_define=define
_endef=endef

ifeq ($(DUMP),)
  define BuildTarget/ipkg
    ABIV_$(1):=$(call FormatABISuffix,$(1),$(ABI_VERSION))
    PDIR_$(1):=$(call FeedPackageDir,$(1))
ifeq ($(CONFIG_USE_APK),)
    PACK_$(1):=$$(PDIR_$(1))/$(1)$$(ABIV_$(1))_$(VERSION)_$(PKGARCH).ipk
else
    PACK_$(1):=$$(PDIR_$(1))/$(1)$$(ABIV_$(1))-$(VERSION).apk
endif
    IDIR_$(1):=$(PKG_BUILD_DIR)/ipkg-$(PKGARCH)/$(1)
    ADIR_$(1):=$(PKG_BUILD_DIR)/apk-$(PKGARCH)/$(1)
    KEEP_$(1):=$(strip $(call Package/$(1)/conffiles))

    APK_SCRIPTS_$(1):=

    ifdef Package/$(1)/preinst
      APK_SCRIPTS_$(1)+=--script "pre-install:$$(ADIR_$(1))/preinst"
    endif
    APK_SCRIPTS_$(1)+=--script "post-install:$$(ADIR_$(1))/post-install"

    ifdef Package/$(1)/preinst
      APK_SCRIPTS_$(1)+=--script "pre-upgrade:$$(ADIR_$(1))/pre-upgrade"
    endif
    APK_SCRIPTS_$(1)+=--script "post-upgrade:$$(ADIR_$(1))/post-upgrade"

    APK_SCRIPTS_$(1)+=--script "pre-deinstall:$$(ADIR_$(1))/pre-deinstall"
    ifdef Package/$(1)/postrm
      APK_SCRIPTS_$(1)+=--script "post-deinstall:$$(ADIR_$(1))/postrm"
    endif

    TARGET_VARIANT:=$$(if $(ALL_VARIANTS),$$(if $$(VARIANT),$$(filter-out *,$$(VARIANT)),$(firstword $(ALL_VARIANTS))))
    ifeq ($(BUILD_VARIANT),$$(if $$(TARGET_VARIANT),$$(TARGET_VARIANT),$(BUILD_VARIANT)))
    do_install=
    ifdef Package/$(1)/install
      do_install=yes
    endif
    ifdef Package/$(1)/install-overlay
      do_install=yes
    endif
    ifdef do_install
      ifneq ($(CONFIG_PACKAGE_$(1))$(DEVELOPER),)
        IPKGS += $(1)
        $(_pkg_target)compile: $$(PACK_$(1)) $(PKG_INFO_DIR)/$(1).provides $(PKG_BUILD_DIR)/.pkgdir/$(1).installed
        prepare-package-install: $$(PACK_$(1))
        compile: $(STAGING_DIR_ROOT)/stamp/.$(1)_installed
      else
        $(if $(CONFIG_PACKAGE_$(1)),$$(info WARNING: skipping $(1) -- package not selected))
      endif

      .PHONY: $(PKG_INSTALL_STAMP).$(1)
      ifeq ($(CONFIG_PACKAGE_$(1)),y)
        compile: $(PKG_INSTALL_STAMP).$(1)
      endif
      $(PKG_INSTALL_STAMP).$(1): prepare-package-install
		echo "$(1)" >> $(PKG_INSTALL_STAMP)
    else
      $(if $(CONFIG_PACKAGE_$(1)),$$(warning WARNING: skipping $(1) -- package has no install section))
    endif
    endif

    DEPENDS:=$(call PKG_FIXUP_DEPENDS,$(1),$(DEPENDS))
    IDEPEND_$(1):=$$(call filter_deps,$$(DEPENDS))
    IDEPEND += $$(patsubst %,$(1):%,$$(IDEPEND_$(1)))
    $(FixupDependencies)
    $(FixupReverseDependencies)

    $(eval $(call BuildPackVariable,$(1),conffiles))
    $(eval $(call BuildPackVariable,$(1),preinst,,1))
    $(eval $(call BuildPackVariable,$(1),postinst,-pkg,1))
    $(eval $(call BuildPackVariable,$(1),prerm,-pkg,1))
    $(eval $(call BuildPackVariable,$(1),postrm,,1))

    $(PKG_BUILD_DIR)/.pkgdir/$(1).installed : export PATH=$$(TARGET_PATH_PKG)
    $(PKG_BUILD_DIR)/.pkgdir/$(1).installed: $(STAMP_BUILT)
	rm -rf $$@ $(PKG_BUILD_DIR)/.pkgdir/$(1)
	mkdir -p $(PKG_BUILD_DIR)/.pkgdir/$(1)
	$(call Package/$(1)/install,$(PKG_BUILD_DIR)/.pkgdir/$(1))
	$(call Package/$(1)/install_lib,$(PKG_BUILD_DIR)/.pkgdir/$(1))
	touch $$@

    $(STAGING_DIR_ROOT)/stamp/.$(1)_installed: $(PKG_BUILD_DIR)/.pkgdir/$(1).installed
	mkdir -p $(STAGING_DIR_ROOT)/stamp
	$(if $(ABI_VERSION),echo '$(ABI_VERSION)' | cmp -s - $(PKG_INFO_DIR)/$(1).version || { \
		echo '$(ABI_VERSION)' > $(PKG_INFO_DIR)/$(1).version; \
		$(foreach pkg,$(filter-out $(1),$(PROVIDES)), \
			cp $(PKG_INFO_DIR)/$(1).version $(PKG_INFO_DIR)/$(pkg).version; \
		) \
	} )
	$(call locked,$(CP) $(PKG_BUILD_DIR)/.pkgdir/$(1)/. $(STAGING_DIR_ROOT)/,root-copy)
	touch $$@

    Package/$(1)/DEPENDS := $$(call mergelist,$$(foreach dep,$$(filter-out @%,$$(IDEPEND_$(1))),$$(dep)$$(call GetABISuffix,$$(dep))))
    ifneq ($$(EXTRA_DEPENDS),)
      Package/$(1)/DEPENDS := $$(EXTRA_DEPENDS)$$(if $$(Package/$(1)/DEPENDS),$$(comma) $$(Package/$(1)/DEPENDS))
    endif

$(_define) Package/$(1)/CONTROL
Package: $(1)$$(ABIV_$(1))
Version: $(VERSION)
$$(call addfield,Depends,$$(Package/$(1)/DEPENDS)
)$$(call addfield,Conflicts,$$(call mergelist,$(CONFLICTS))
)$$(call addfield,Provides,$$(call mergelist,$$(filter-out $(1)$$(ABIV_$(1)),$(PROVIDES)$$(if $$(ABIV_$(1)), $(1) $(foreach provide,$(PROVIDES),$(provide)$$(ABIV_$(1))))))
)$$(call addfield,Alternatives,$$(call mergelist,$(ALTERNATIVES))
)$$(call addfield,Source,$(SOURCE)
)$$(call addfield,SourceName,$(PKG_NAME)
)$$(call addfield,License,$(LICENSE)
)$$(call addfield,LicenseFiles,$(LICENSE_FILES)
)$$(call addfield,Section,$(SECTION)
)$$(call addfield,Require-User,$(USERID)
)$$(call addfield,SourceDateEpoch,$(PKG_SOURCE_DATE_EPOCH)
)$$(call addfield,URL,$(URL)
)$$(if $$(ABIV_$(1)),ABIVersion: $$(ABIV_$(1))
)$(if $(PKG_CPE_ID),CPE-ID: $(PKG_CPE_ID)
)$(if $(filter hold,$(PKG_FLAGS)),Status: unknown hold not-installed
)$(if $(filter essential,$(PKG_FLAGS)),Essential: yes
)$(if $(MAINTAINER),Maintainer: $(MAINTAINER)
)Architecture: $(PKGARCH)
Installed-Size: 0
$(_endef)

    $$(PACK_$(1)) : export CONTROL=$$(Package/$(1)/CONTROL)
    $$(PACK_$(1)) : export DESCRIPTION=$$(Package/$(1)/description)
    $$(PACK_$(1)) : export PATH=$$(TARGET_PATH_PKG)
    $$(PACK_$(1)) : export PKG_SOURCE_DATE_EPOCH:=$(PKG_SOURCE_DATE_EPOCH)
    $(PKG_INFO_DIR)/$(1).provides $$(PACK_$(1)): $(STAMP_BUILT) $(INCLUDE_DIR)/package-pack.mk
	rm -rf $$(IDIR_$(1))
ifeq ($$(CONFIG_USE_APK),)
	$$(call remove_ipkg_files,$(1),$$(call opkg_package_files,$(call gen_package_wildcard,$(1))))
else
	$$(call remove_ipkg_files,$(1),$$(call apk_package_files,$(call gen_package_wildcard,$(1))))
endif
	mkdir -p $(PACKAGE_DIR) $$(IDIR_$(1)) $(PKG_INFO_DIR)
	$(call Package/$(1)/install,$$(IDIR_$(1)))
	$(if $(Package/$(1)/install-overlay),mkdir -p $(PACKAGE_DIR) $$(IDIR_$(1))/rootfs-overlay)
	$(call Package/$(1)/install-overlay,$$(IDIR_$(1))/rootfs-overlay)
	-find $$(IDIR_$(1)) -name 'CVS' -o -name '.svn' -o -name '.#*' -o -name '*~'| $(XARGS) rm -rf
	@( \
		find $$(IDIR_$(1)) -name lib\*.so\* -or -name \*.ko | awk -F/ '{ print $$$$NF }'; \
		for file in $$(patsubst %,$(PKG_INFO_DIR)/%.provides,$$(IDEPEND_$(1))); do \
			if [ -f "$$$$file" ]; then \
				cat $$$$file; \
			fi; \
		done; $(Package/$(1)/extra_provides) \
	) | sort -u > $(PKG_INFO_DIR)/$(1).provides
	$(if $(PROVIDES),@for pkg in $(filter-out $(1),$(PROVIDES)); do cp $(PKG_INFO_DIR)/$(1).provides $(PKG_INFO_DIR)/$$$$pkg.provides; done)
	$(CheckDependencies)

	$(RSTRIP) $$(IDIR_$(1))

    ifneq ($$(CONFIG_IPK_FILES_CHECKSUMS),)
	(cd $$(IDIR_$(1)); \
		( \
			find . -type f \! -path ./CONTROL/\* -exec $(MKHASH) sha256 -n \{\} \; 2> /dev/null | \
			sed 's|\([[:blank:]]\)\./| \1/|' > $$(IDIR_$(1))/CONTROL/files-sha256sum \
		) || true \
	)
    endif

    ifneq ($$(KEEP_$(1)),)
		@( \
			keepfiles=""; \
			for x in $$(KEEP_$(1)); do \
				[ -f "$$(IDIR_$(1))/$$$$x" ] || keepfiles="$$$${keepfiles:+$$$$keepfiles }$$$$x"; \
			done; \
			[ -z "$$$$keepfiles" ] || { \
				mkdir -p $$(IDIR_$(1))/lib/upgrade/keep.d; \
				for x in $$$$keepfiles; do echo $$$$x >> $$(IDIR_$(1))/lib/upgrade/keep.d/$(1); done; \
			}; \
		)
    endif

	$(INSTALL_DIR) $$(PDIR_$(1))/tmp

ifeq ($(CONFIG_USE_APK),)
	mkdir -p $$(IDIR_$(1))/CONTROL
	(cd $$(IDIR_$(1))/CONTROL; \
		( \
			echo "$$$$CONTROL"; \
			printf "Description: "; echo "$$$$DESCRIPTION" | sed -e 's,^[[:space:]]*, ,g'; \
		) > control; \
		chmod 644 control; \
		( \
			echo "#!/bin/sh"; \
			echo "[ \"\$$$${IPKG_NO_SCRIPT}\" = \"1\" ] && exit 0"; \
			echo "[ -s "\$$$${IPKG_INSTROOT}/lib/functions.sh" ] || exit 0"; \
			echo ". \$$$${IPKG_INSTROOT}/lib/functions.sh"; \
			echo "default_postinst \$$$$0 \$$$$@"; \
		) > postinst; \
		( \
			echo "#!/bin/sh"; \
			echo "[ -s "\$$$${IPKG_INSTROOT}/lib/functions.sh" ] || exit 0"; \
			echo ". \$$$${IPKG_INSTROOT}/lib/functions.sh"; \
			echo "default_prerm \$$$$0 \$$$$@"; \
		) > prerm; \
		chmod 0755 postinst prerm; \
		$($(1)_COMMANDS) \
	)

	$(FAKEROOT) $(STAGING_DIR_HOST)/bin/bash $(SCRIPT_DIR)/ipkg-build -m "$(FILE_MODES)" $$(IDIR_$(1)) $$(PDIR_$(1))
else
	mkdir -p $$(ADIR_$(1))/
	mkdir -p $$(IDIR_$(1))/lib/apk/packages/

	(cd $$(ADIR_$(1)); $($(1)_COMMANDS))

	( \
		echo "#!/bin/sh"; \
		echo "[ \"\$$$${IPKG_NO_SCRIPT}\" = \"1\" ] && exit 0"; \
		echo "[ -s "\$$$${IPKG_INSTROOT}/lib/functions.sh" ] || exit 0"; \
		echo ". \$$$${IPKG_INSTROOT}/lib/functions.sh"; \
		echo 'export root="$$$${IPKG_INSTROOT}"'; \
		echo 'export pkgname="$(1)"'; \
		echo "add_group_and_user"; \
		echo "default_postinst"; \
		[ ! -f $$(ADIR_$(1))/postinst-pkg ] || sed -z 's/^\s*#!/#!/' "$$(ADIR_$(1))/postinst-pkg"; \
	) > $$(ADIR_$(1))/post-install;

    ifdef Package/$(1)/preinst
	( \
		echo "#!/bin/sh"; \
		echo 'export PKG_UPGRADE=1'; \
		[ ! -f $$(ADIR_$(1))/preinst ] || sed -z 's/^\s*#!/#!/' "$$(ADIR_$(1))/preinst"; \
	) > $$(ADIR_$(1))/pre-upgrade;
    endif

	( \
		echo "#!/bin/sh"; \
		echo 'export PKG_UPGRADE=1'; \
		[ ! -f $$(ADIR_$(1))/post-install ] || sed -z 's/^\s*#!/#!/' "$$(ADIR_$(1))/post-install"; \
	) > $$(ADIR_$(1))/post-upgrade;

	( \
		echo "#!/bin/sh"; \
		echo "[ -s "\$$$${IPKG_INSTROOT}/lib/functions.sh" ] || exit 0"; \
		echo ". \$$$${IPKG_INSTROOT}/lib/functions.sh"; \
		echo 'export root="$$$${IPKG_INSTROOT}"'; \
		echo 'export pkgname="$(1)"'; \
		echo "default_prerm"; \
		[ ! -f $$(ADIR_$(1))/prerm-pkg ] || sed -z 's/^\s*#!/#!/' "$$(ADIR_$(1))/prerm-pkg"; \
	) > $$(ADIR_$(1))/pre-deinstall;

	[ ! -f $$(ADIR_$(1))/postrm ] || sed -zi 's/^\s*#!/#!/' "$$(ADIR_$(1))/postrm";

	if [ -n "$(USERID)" ]; then echo $(USERID) > $$(IDIR_$(1))/lib/apk/packages/$(1).rusers; fi;
	if [ -n "$(ALTERNATIVES)" ]; then echo $(ALTERNATIVES) > $$(IDIR_$(1))/lib/apk/packages/$(1).alternatives; fi;
	(cd $$(IDIR_$(1)) && find . -type f,l -printf "/%P\n" > $$(IDIR_$(1))/lib/apk/packages/$(1).list)
	# Move conffiles to IDIR and build conffiles_static with csums
	if [ -f $$(ADIR_$(1))/conffiles ]; then \
		mv -f $$(ADIR_$(1))/conffiles $$(IDIR_$(1))/lib/apk/packages/$(1).conffiles; \
		for file in $$$$(cat $$(IDIR_$(1))/lib/apk/packages/$(1).conffiles); do \
			[ -f $$(IDIR_$(1))/$$$$file ] || continue; \
			csum=$$$$($(MKHASH) sha256 $$(IDIR_$(1))/$$$$file); \
			echo $$$$file $$$$csum >> $$(IDIR_$(1))/lib/apk/packages/$(1).conffiles_static; \
		done; \
	fi

	# Some package (base-files) manually append stuff to conffiles
	# Append stuff from it and delete the CONTROL directory since everything else should be migrated
	if [ -f $$(IDIR_$(1))/CONTROL/conffiles ]; then \
		echo $$$$(IDIR_$(1))/CONTROL/conffiles >> $$(IDIR_$(1))/lib/apk/packages/$(1).conffiles; \
		for file in $$$$(cat $$(IDIR_$(1))/CONTROL/conffiles); do \
			[ -f $$(IDIR_$(1))/$$$$file ] || continue; \
			csum=$$$$($(MKHASH) sha256 $$(IDIR_$(1))/$$$$file); \
			echo $$$$file $$$$csum >> $$(IDIR_$(1))/lib/apk/packages/$(1).conffiles_static; \
		done; \
		rm -rf $$(IDIR_$(1))/CONTROL/conffiles; \
	fi

	if [ -z "$$$$(ls -A $$(IDIR_$(1))/CONTROL 2>/dev/null)" ]; then \
		rm -rf $$(IDIR_$(1))/CONTROL; \
	else \
		echo "CONTROL directory $$(IDIR_$(1))/CONTROL is not empty! This is not right and should be checked!" >&2; \
		exit 1; \
	fi

	$(FAKEROOT) $(STAGING_DIR_HOST)/bin/apk mkpkg \
	  --info "name:$(1)$$(ABIV_$(1))" \
	  --info "version:$(VERSION)" \
	  --info "description:$$(call description_escape,$$(strip $$(Package/$(1)/description)))" \
	  $(if $(findstring all,$(PKGARCH)),--info "arch:noarch",--info "arch:$(PKGARCH)") \
	  --info "license:$(LICENSE)" \
	  --info "origin:$(SOURCE)" \
	  --info "url:$(URL)" \
	  --info "maintainer:$(MAINTAINER)" \
	  --info "provides:$$(foreach prov,\
			$$(filter-out $(1)$$(ABIV_$(1)), \
			$(PROVIDES)$$(if $$(ABIV_$(1)), \
				$(1)=$(VERSION) $(foreach provide, \
					$(PROVIDES), \
					$(provide)$$(ABIV_$(1))=$(VERSION) \
				) \
			) \
		), \
		$$(prov) )" \
	  $(if $(DEFAULT_VARIANT),--info "provider-priority:100",$(if $(PROVIDES),--info "provider-priority:1")) \
	  $$(APK_SCRIPTS_$(1)) \
	  --info "depends:$$(foreach depends,$$(subst $$(comma),$$(space),$$(subst $$(space),,$$(subst $$(paren_right),,$$(subst $$(paren_left),,$$(Package/$(1)/DEPENDS))))),$$(depends))" \
	  --files "$$(IDIR_$(1))" \
	  --output "$$(PACK_$(1))" \
	  --sign "$(BUILD_KEY_APK_SEC)"
endif

	@[ -f $$(PACK_$(1)) ]

    $(1)-clean:
ifeq ($(CONFIG_USE_APK),)
	$$(call remove_ipkg_files,$(1),$$(call opkg_package_files,$(call gen_package_wildcard,$(1))))
else
	$$(call remove_ipkg_files,$(1),$$(call apk_package_files,$(call gen_package_wildcard,$(1))))
endif


    clean: $(1)-clean

  endef
endif
