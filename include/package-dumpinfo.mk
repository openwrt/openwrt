# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2006-2020 OpenWrt.org

ifneq ($(DUMP),)


define SOURCE_INFO
$(if $(PKG_BUILD_DEPENDS),Build-Depends: $(PKG_BUILD_DEPENDS)
)$(if $(HOST_BUILD_DEPENDS),Build-Depends/host: $(HOST_BUILD_DEPENDS)
)$(if $(BUILD_TYPES),Build-Types: $(BUILD_TYPES)
)$(if $(PKG_PARALLEL_VARIANTS),Parallel-Variants: 1
)

endef

define Dumpinfo/Package
$(eval Package/$(1)/DEFAULT_VARIANT := $(DEFAULT_VARIANT))
$(eval define Package/$(1)/dump
$(if $(MENU),Menu: $(MENU)
)$(if $(SUBMENU),Submenu: $(SUBMENU)
)$(if $(SUBMENUDEP),Submenu-Depends: $(SUBMENUDEP)
)$(if $(DEFAULT),Default: $(DEFAULT)
)$(if $(findstring $(PREREQ_CHECK),1),Prereq-Check: 1
)Version: $(VERSION)
Depends: $(call PKG_FIXUP_DEPENDS,$(1),$(DEPENDS))
Menu-Depends: $(MDEPENDS)
Provides: $(PROVIDES)
$(if $(VARIANT),Build-Variant: $(VARIANT)
)$(if $(DEFAULT_VARIANT),Default-Variant: $(VARIANT)
)Section: $(SECTION)
Category: $(CATEGORY)
$(if $(filter nonshared,$(PKGFLAGS)),,Repository: $(if $(FEED),$(FEED),base)
)Title: $(TITLE)
Maintainer: $(MAINTAINER)
$(if $(USERID),Require-User: $(USERID)
)Source: $(PKG_SOURCE)
$(if $(LICENSE),License: $(LICENSE)
)$(if $(LICENSE_FILES),LicenseFiles: $(LICENSE_FILES)
)$(if $(PKG_CPE_ID),CPE-ID: $(PKG_CPE_ID)
)$(if $(URL),URL: $(URL)
)$(if $(ABI_VERSION),ABI-Version: $(ABI_VERSION)
)Type: $(if $(Package/$(1)/targets),$(Package/$(1)/targets),$(if $(PKG_TARGETS),$(PKG_TARGETS),ipkg))
$(if $(KCONFIG),Kernel-Config: $(KCONFIG)
)$(if $(BUILDONLY),Build-Only: $(BUILDONLY)
)$(if $(HIDDEN),Hidden: $(HIDDEN)
)Description: $(if $(Package/$(1)/description),$(Package/$(1)/description),$(TITLE))
@@
$(if $(Package/$(1)/config),Config:
$(Package/$(1)/config)
@@
)
endef)
endef

define Dumpinfo/ValidateProviders
$(foreach provide,$(sort $(foreach pkg,$(BUILD_PACKAGES),$(Package/$(pkg)/provides))), \
  $(if $(word 2,$(PROVIDERS/$(provide))), \
    $(if $(filter 1,$(words $(foreach pkg,$(PROVIDERS/$(provide)),$(if $(Package/$(pkg)/DEFAULT_VARIANT),$(pkg))))),, \
      $(warning $(CURDIR): provide '$(provide)' needs exactly one DEFAULT_VARIANT (providers: $(PROVIDERS/$(provide)))))))
endef

define Dumpinfo/Print
$(info Package: $(1)
Conflicts: $(Package/$(1)/conflicts)
Inferred-Conflicts: $(filter-out $(Package/$(1)/conflicts),$(call PackageConflicts,$(1)))
$(value Package/$(1)/dump))
endef

dumpinfo: FORCE
	$(if $(CHECK),,$(Dumpinfo/ValidateProviders))
	$(if $(SOURCE_INFO),$(info $(SOURCE_INFO)))
	$(if $(CHECK),,$(foreach pkg,$(BUILD_PACKAGES),$(call Dumpinfo/Print,$(pkg))))

endif
