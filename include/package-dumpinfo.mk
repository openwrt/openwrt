# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

ifneq ($(DUMP),)
  define Config
    preconfig_$$(1) += echo "Preconfig: $(1)"; echo "Preconfig-Type: $(2)"; echo "Preconfig-Default: $(3)"; echo "Preconfig-Label: $(4)";
  endef

  define Dumpinfo
    dumpinfo: dumpinfo-$(1)
    .SILENT: dumpinfo-$(1)
    dumpinfo-$(1): FORCE
	  @echo "Package: $(1)" ; \
		$(if $(MENU),echo "Menu: $(MENU)";) \
		$(if $(SUBMENU),echo "Submenu: $(SUBMENU)";) \
		$(if $(SUBMENUDEP),echo "Submenu-Depends: $(SUBMENUDEP)";) \
		$(if $(DEFAULT),echo "Default: $(DEFAULT)";)  \
		if [ "$$$$PREREQ_CHECK" = 1 ]; then echo "Prereq-Check: 1"; fi; \
		echo "Version: $(VERSION)"; \
		echo "Depends: $(DEPENDS)"; \
		echo "Provides: $(PROVIDES)"; \
		echo "Build-Depends: $(PKG_BUILD_DEPENDS)"; \
		echo "Section: $(SECTION)"; \
		echo "Category: $(CATEGORY)"; \
		echo "Title: $(TITLE)"; \
		echo "Maintainer: $(MAINTAINER)"; \
		echo "Type: $(if $(Package/$(1)/targets),$(Package/$(1)/targets),$(if $(PKG_TARGETS),$(PKG_TARGETS),ipkg))"; \
		$(if $(KCONFIG),echo "Kernel-Config: $(KCONFIG)";) \
		$(if $(BUILDONLY),echo "Build-Only: $(BUILDONLY)";) \
		echo -n "Description: "; \
		getvar $(call shvar,Package/$(1)/description); \
		$(if $(URL),echo;echo "$(URL)";) \
		echo "@@" ; \
		$$(if $$(Package/$(1)/config),echo "Config: "; getvar $(call shvar,Package/$(1)/config); echo "@@"; ) \
		$$(if $$(preconfig_$(1)),$$(preconfig_$(1)) echo "")
  endef
endif
