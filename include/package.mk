# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
ifneq ($(DUMP),)
  all: dumpinfo
else
  all: compile
endif

include $(INCLUDE_DIR)/prereq.mk

define Build/DefaultTargets
  ifeq ($(DUMP),)
    ifeq ($(CONFIG_AUTOREBUILD),y)
      _INFO:=
      ifneq ($$(shell $(SCRIPT_DIR)/timestamp.pl -p $(PKG_BUILD_DIR) .),$(PKG_BUILD_DIR))
        _INFO+=$(subst $(TOPDIR)/,,$(PKG_BUILD_DIR))
        $(PKG_BUILD_DIR)/.prepared: package-clean
      endif

    endif
  endif

  $(PKG_BUILD_DIR)/.prepared:
	@-rm -rf $(PKG_BUILD_DIR)
	@mkdir -p $(PKG_BUILD_DIR)
	$(call Build/Prepare)
	touch $$@

  $(PKG_BUILD_DIR)/.configured: $(PKG_BUILD_DIR)/.prepared
	$(call Build/Configure)
	touch $$@

  $(PKG_BUILD_DIR)/.built: $(PKG_BUILD_DIR)/.configured
	$(call Build/Compile)
	touch $$@

  ifdef Build/InstallDev
    ifneq ($$(shell $(SCRIPT_DIR)/timestamp.pl -p -x ipkg -x ipkg-install $(STAGING_DIR)/stampfiles/.$(PKG_NAME)-installed $(PKG_BUILD_DIR)),$(STAGING_DIR)/stampfiles/.$(PKG_NAME)-installed)
      $(PKG_BUILD_DIR)/.built: package-rebuild
    endif

    $(STAGING_DIR)/stampfiles/.$(PKG_NAME)-installed: $(PKG_BUILD_DIR)/.built
	mkdir -p $(STAGING_DIR)/stampfiles
	$(call Build/InstallDev)
	touch $$@
	
    compile-targets: $(STAGING_DIR)/stampfiles/.$(PKG_NAME)-installed
  endif

  package-clean: FORCE
	$(call Build/Clean)
	$(call Build/UninstallDev)
	-rm -f $(STAGING_DIR)/stampfiles/.$(PKG_NAME)-installed

  package-rebuild: FORCE
	@-rm -f $(PKG_BUILD_DIR)/.built

  define Build/DefaultTargets
  endef
endef

define Package/Default
  CONFIGFILE:=
  SECTION:=opt
  CATEGORY:=Extra packages
  DEPENDS:=
  EXTRA_DEPENDS:=
  MAINTAINER:=OpenWrt Developers Team <openwrt-devel@openwrt.org>
  SOURCE:=$(patsubst $(TOPDIR)/%,%,${shell pwd})
  ifneq ($(PKG_VERSION),)
    ifneq ($(PKG_RELEASE),)
      VERSION:=$(PKG_VERSION)-$(PKG_RELEASE)
    else
      VERSION:=$(PKG_VERSION)
    endif
  else
    VERSION:=$(PKG_RELEASE)
  endif
  PKGARCH:=$(ARCH)
  PRIORITY:=optional
  DEFAULT:=
  MENU:=
  SUBMENU:=
  SUBMENUDEP:=
  TITLE:=
  DESCRIPTION:=
endef

define BuildIPKGVariable
  pkg_$(subst .,_,$(subst -,_,$(1)))_$(2) = $$(Package/$(1)/$(2))
  export pkg_$(subst .,_,$(subst -,_,$(1))_$(2))
  $(1)_COMMANDS += if [ -n "$$$$$$$$pkg_$(subst .,_,$(subst -,_,$(1)))_$(2)" ]; then echo "$$$$$$$$pkg_$(subst .,_,$(subst -,_,$(1)))_$(2)" > $(2); fi;
endef

define BuildPackage
  $(eval $(call Package/Default))
  $(eval $(call Package/$(1)))

  $(foreach FIELD, TITLE CATEGORY PRIORITY VERSION,
    ifeq ($($(FIELD)),)
      $$(error Package/$(1) is missing the $(FIELD) field)
    endif
  )

  ifeq ($(PKGARCH),)
    PKGARCH:=$(ARCH)
  endif

  ifeq ($(DESCRIPTION),)
    $(eval DESCRIPTION:=$(TITLE))
  endif

  IPKG_$(1):=$(PACKAGE_DIR)/$(1)_$(VERSION)_$(PKGARCH).ipk
  IDIR_$(1):=$(PKG_BUILD_DIR)/ipkg/$(1)
  INFO_$(1):=$(IPKG_STATE_DIR)/info/$(1).list

  ifdef Package/$(1)/install
    ifeq ($(CONFIG_PACKAGE_$(1)),y)
      install-targets: $$(INFO_$(1))
    endif

    ifneq ($(CONFIG_PACKAGE_$(1)),)
      compile-targets: $$(IPKG_$(1))
    else
      compile-targets: $(1)-disabled
      $(1)-disabled:
	@echo "WARNING: skipping $(1) -- package not selected"
    endif
  endif

  ifeq ($(FORCEREBUILD),y)
    $$(IPKG_$(1)): FORCE
  endif

  IDEPEND_$(1):=$$(strip $$(DEPENDS))

  ifneq ($(DUMP),)
    DUMPINFO += \
	echo "Package: $(1)"; 

    ifneq ($(MENU),)
      DUMPINFO += \
	echo "Menu: $(MENU)";
    endif

    ifneq ($(SUBMENU),)
      DUMPINFO += \
	echo "Submenu: $(SUBMENU)";
      ifneq ($(SUBMENUDEP),)
        DUMPINFO += \
	  echo "Submenu-Depends: $(SUBMENUDEP)";
      endif
    endif

    ifneq ($(DEFAULT),)
      DUMPINFO += \
	echo "Default: $(DEFAULT)";
    endif

    DUMPINFO += \
	echo "Version: $(VERSION)"; \
	echo "Depends: $$(IDEPEND_$(1))"; \
	echo "Build-Depends: $(PKG_BUILDDEP)"; \
	echo "Category: $(CATEGORY)"; \
	echo "Title: $(TITLE)"; \
	echo "Description: $(DESCRIPTION)" | sed -e 's,\\,\n,g';

    ifneq ($(URL),)
      DUMPINFO += \
	echo; \
	echo "$(URL)";
    endif

    DUMPINFO += \
	echo "@@";

    ifneq ($(CONFIG),)
      DUMPINFO += \
	echo "Config: $(CONFIG)" | sed -e 's,\\,\n,g'; \
	echo "@@";
    endif
  endif

  $(eval $(call BuildIPKGVariable,$(1),conffiles))
  $(eval $(call BuildIPKGVariable,$(1),preinst))
  $(eval $(call BuildIPKGVariable,$(1),postinst))
  $(eval $(call BuildIPKGVariable,$(1),prerm))
  $(eval $(call BuildIPKGVariable,$(1),postrm))
  $$(IDIR_$(1))/CONTROL/control: $(PKG_BUILD_DIR)/.version-$(1)_$(VERSION)_$(PKGARCH)
	mkdir -p $$(IDIR_$(1))/CONTROL
	echo "Package: $(1)" > $$(IDIR_$(1))/CONTROL/control
	echo "Version: $(VERSION)" >> $$(IDIR_$(1))/CONTROL/control
	( \
		DEPENDS=; \
		for depend in $$(filter-out @%,$$(IDEPEND_$(1))); do \
			DEPENDS=$$$${DEPENDS:+$$$$DEPENDS, }$$$${depend##+}; \
		done; \
		echo "Depends: $(EXTRA_DEPENDS) $$$$DEPENDS" >> $$(IDIR_$(1))/CONTROL/control; \
	)
	echo "Source: $(SOURCE)" >> $$(IDIR_$(1))/CONTROL/control
	echo "Section: $(SECTION)" >> $$(IDIR_$(1))/CONTROL/control
	echo "Priority: $(PRIORITY)" >> $$(IDIR_$(1))/CONTROL/control
	echo "Maintainer: $(MAINTAINER)" >> $$(IDIR_$(1))/CONTROL/control
	echo "Architecture: $(PKGARCH)" >> $$(IDIR_$(1))/CONTROL/control
	echo "Description: $(DESCRIPTION)" | sed -e 's,\\,\n ,g' >> $$(IDIR_$(1))/CONTROL/control
	chmod 644 $$(IDIR_$(1))/CONTROL/control
	(cd $$(IDIR_$(1))/CONTROL; \
		$($(1)_COMMANDS) \
	)

  $$(IPKG_$(1)): $(PKG_BUILD_DIR)/.built $$(IDIR_$(1))/CONTROL/control
	$(call Package/$(1)/install,$$(IDIR_$(1)))
	mkdir -p $(PACKAGE_DIR)
	-find $$(IDIR_$(1)) -name CVS | xargs rm -rf
	-find $$(IDIR_$(1)) -name .svn | xargs rm -rf
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
	-@rm $(PKG_BUILD_DIR)/.version-$(1)_* 2>/dev/null
	@touch $$@

  $$(eval $$(call Build/DefaultTargets,$(1)))

  ifneq ($$(CONFIG_PACKAGE_$(1)),)
    ifneq ($(MAKECMDGOALS),prereq)
      ifneq ($(DUMP),1)
        ifneq ($$(shell $(SCRIPT_DIR)/timestamp.pl -p -x ipkg -x ipkg-install '$$(IPKG_$(1))' '$(PKG_BUILD_DIR)'),$$(IPKG_$(1)))
          _INFO+=$(subst $(TOPDIR)/,,$$(IPKG_$(1)))
          $(PKG_BUILD_DIR)/.built: package-rebuild
        endif

        ifneq ($$(_INFO),)
          $$(info Rebuilding $$(_INFO))
        endif
      endif
    endif
  endif
endef

ifneq ($(strip $(PKG_CAT)),)
  ifeq ($(PKG_CAT),unzip)
    UNPACK=unzip -d $(PKG_BUILD_DIR) $(DL_DIR)/$(PKG_SOURCE)
  else
    UNPACK=$(PKG_CAT) $(DL_DIR)/$(PKG_SOURCE) | tar -C $(PKG_BUILD_DIR)/.. $(TAR_OPTIONS) -
  endif
  define Build/Prepare/Default
  	$(UNPACK)
	@if [ -d ./patches ]; then \
		$(PATCH) $(PKG_BUILD_DIR) ./patches; \
	fi
  endef
endif

define Build/Prepare
  $(call Build/Prepare/Default,)
endef

define Build/Configure/Default
	(cd $(PKG_BUILD_DIR)/$(3); \
	if [ -x configure ]; then \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		CXXFLAGS="$(TARGET_CFLAGS)" \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/include" \
		LDFLAGS="-L$(STAGING_DIR)/usr/lib -L$(STAGING_DIR)/lib" \
		PKG_CONFIG_PATH="$(STAGING_DIR)/usr/lib/pkgconfig" \
		$(2) \
		./configure \
		--target=$(GNU_TARGET_NAME) \
		--host=$(GNU_TARGET_NAME) \
		--build=$(GNU_HOST_NAME) \
		--program-prefix="" \
		--program-suffix="" \
		--prefix=/usr \
		--exec-prefix=/usr \
		--bindir=/usr/bin \
		--sbindir=/usr/sbin \
		--libexecdir=/usr/lib \
		--sysconfdir=/etc \
		--datadir=/usr/share \
		--localstatedir=/var \
		--mandir=/usr/man \
		--infodir=/usr/info \
		$(DISABLE_NLS) \
		$(1); \
	fi; \
	)
endef

define Build/Configure
  $(call Build/Configure/Default,)
endef

define Build/Compile/Default
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		CROSS="$(TARGET_CROSS)" \
		EXTRA_CFLAGS="$(TARGET_CFLAGS) -I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/include " \
		EXTRA_LDFLAGS="-L$(STAGING_DIR)/usr/lib -L$(STAGING_DIR)/lib " \
		ARCH="$(ARCH)" \
		$(1);
endef

define Build/Compile
  $(call Build/Compile/Default,)
endef

ifneq ($(DUMP),)
  dumpinfo: FORCE
	@$(DUMPINFO)
else
  $(PACKAGE_DIR):
	mkdir -p $@
		
  ifneq ($(strip $(PKG_SOURCE)),)
    download: $(DL_DIR)/$(PKG_SOURCE)

    $(DL_DIR)/$(PKG_SOURCE):
		mkdir -p $(DL_DIR)
		$(SCRIPT_DIR)/download.pl "$(DL_DIR)" "$(PKG_SOURCE)" "$(PKG_MD5SUM)" $(PKG_SOURCE_URL)

    $(PKG_BUILD_DIR)/.prepared: $(DL_DIR)/$(PKG_SOURCE)
  endif

  download:
  prepare: $(PKG_BUILD_DIR)/.prepared
  configure: $(PKG_BUILD_DIR)/.configured

  compile-targets:
  compile: compile-targets

  install-targets:
  install: install-targets

  clean-targets:
  clean: FORCE
	@$(MAKE) clean-targets
	$(call Build/Clean)
	rm -rf $(PKG_BUILD_DIR)
endif
