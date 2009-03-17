# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

ifneq ($(DUMP),)

dumpinfo: FORCE

define Config/template
Preconfig: $(1)
Preconfig-Type: $(2)
Preconfig-Default: $(3)
Preconfig-Label: $(4)

endef

define Config
  Preconfig/$(1) = $$(call Config/template,$(1),$(2),$(3),$(4))
  preconfig_$$(1) += $(1)
endef

define Dumpinfo
$(info Package: $(1)
$(if $(MENU),Menu: $(MENU)
)$(if $(SUBMENU),Submenu: $(SUBMENU)
)$(if $(SUBMENUDEP),Submenu-Depends: $(SUBMENUDEP)
)$(if $(DEFAULT),Default: $(DEFAULT)
)$(if $(findstring $(PREREQ_CHECK),1),Prereq-Check: 1
)Version: $(VERSION)
Depends: $(DEPENDS)
Provides: $(PROVIDES)
$(if $(PKG_BUILD_DEPENDS),Build-Depends: $(PKG_BUILD_DEPENDS)
)$(if $(HOST_BUILD_DEPENDS),Build-Depends/host: $(HOST_BUILD_DEPENDS)
)$(if $(BUILD_TYPES),Build-Types: $(BUILD_TYPES)
)Section: $(SECTION)
Category: $(CATEGORY)
Title: $(TITLE)
Maintainer: $(MAINTAINER)
Source: $(PKG_SOURCE)
Type: $(if $(Package/$(1)/targets),$(Package/$(1)/targets),$(if $(PKG_TARGETS),$(PKG_TARGETS),ipkg))
$(if $(KCONFIG),Kernel-Config: $(KCONFIG)
)$(if $(BUILDONLY),Build-Only: $(BUILDONLY)
)Description: $(if $(Package/$(1)/description),$(Package/$(1)/description),$(TITLE))
$(if $(URL),$(URL)
)@@
$(if $(Package/$(1)/config),Config:
$(Package/$(1)/config)
@@
)$(foreach pc,$(preconfig_$(1)),
$(Preconfig/$(pc))))
  endef
endif
