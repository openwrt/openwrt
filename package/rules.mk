# default target
ifneq ($(DUMP),)
all: dumpinfo
else
all: compile
endif

define Build/DefaultTargets
$(PKG_BUILD_DIR)/.prepared:
	rm -rf $(PKG_BUILD_DIR)
	mkdir -p $(PKG_BUILD_DIR)
	$(call Build/Prepare)
	touch $$@

$(PKG_BUILD_DIR)/.configured:
	$(call Build/Configure)
	touch $$@

$(PKG_BUILD_DIR)/.built:
	$(call Build/Compile)
	touch $$@

define Build/DefaultTargets
endef
endef


define BuildPackage
CONFIGFILE:=
SECTION:=opt
CATEGORY:=Extra packages
DEPENDS:=
MAINTAINER:=OpenWrt Developers Team <openwrt-devel@openwrt.org>
SOURCE:=$(patsubst $(TOPDIR)/%,%,${shell pwd})
VERSION:=$(PKG_VERSION)-$(PKG_RELEASE)
PKGARCH:=$(ARCH)
PRIORITY:=optional
TITLE:=
DESCRIPTION:=

$$(eval $$(call Package/$(1)))

ifeq ($$(TITLE),)
$$(error Package $(1) has no TITLE)
endif
ifeq ($$(CATEGORY),)
$$(error Package $(1) has no CATEGORY)
endif
ifeq ($$(PRIORITY),)
$$(error Package $(1) has no PRIORITY)
endif
ifeq ($$(VERSION),)
$$(error Package $(1) has no VERSION)
endif
ifeq ($$(PKGARCH),)
PKGARCH:=$(ARCH)
endif

IPKG_$(1):=$(PACKAGE_DIR)/$(1)_$(VERSION)_$(PKGARCH).ipk
IDIR_$(1):=$(PKG_BUILD_DIR)/ipkg/$(1)
INFO_$(1):=$(IPKG_STATE_DIR)/info/$(1).list

ifneq ($(PACKAGE_$(1)),)
compile-targets: $$(IPKG_$(1))
endif
ifneq ($(DEVELOPER),)
compile-targets: $$(IPKG_$(1))
endif
ifeq ($(PACKAGE_$(1)),y)
install-targets: $$(INFO_$(1))
endif

IDEPEND_$(1):=$$(strip $$(DEPENDS))

$$(IDIR_$(1))/CONTROL/control: $(PKG_BUILD_DIR)/.prepared
	mkdir -p $$(IDIR_$(1))/CONTROL
	echo "Package: $(1)" > $$(IDIR_$(1))/CONTROL/control
	echo "Version: $(VERSION)" >> $$(IDIR_$(1))/CONTROL/control
	echo "Depends: $$(IDEPEND_$(1))" >> $$(IDIR_$(1))/CONTROL/control
	echo "Source: $(SOURCE)" >> $$(IDIR_$(1))/CONTROL/control
	echo "Section: $(SECTION)" >> $$(IDIR_$(1))/CONTROL/control
	echo "Priority: $(PRIORITY)" >> $$(IDIR_$(1))/CONTROL/control
	echo "Maintainer: $(MAINTAINER)" >> $$(IDIR_$(1))/CONTROL/control
	echo "Architecture: $(PKGARCH)" >> $$(IDIR_$(1))/CONTROL/control
	echo "Description: $(TITLE)" >> $$(IDIR_$(1))/CONTROL/control
	echo "$(DESCRIPTION)" | sed -e 's,\\,\n ,g' >> $$(IDIR_$(1))/CONTROL/control
	chmod 644 $$(IDIR_$(1))/CONTROL/control
	for file in conffiles preinst postinst prerm postrm; do \
		[ -f ./ipkg/$(1).$$$$file ] && cp ./ipkg/$(1).$$$$file $$(IDIR_$(1))/CONTROL/$$$$file || true; \
	done

$$(IPKG_$(1)): $$(IDIR_$(1))/CONTROL/control $(PKG_BUILD_DIR)/.built $(PACKAGE_DIR)
	$(call Package/$(1)/install,$$(IDIR_$(1)))
	$(IPKG_BUILD) $$(IDIR_$(1)) $(PACKAGE_DIR)

$$(INFO_$(1)): $$(IPKG_$(1))
	$(IPKG) install $$(IPKG_$(1))

$(1)-clean:
	rm -f $$(IPKG_$(1))
clean: $(1)-clean

DUMPINFO += \
	echo "Package: $(1)"; \
	echo "Version: $(VERSION)"; \
	echo "Depends: $(IDEPEND_$(1))"; \
	echo "Title: $(TITLE)"; \
	echo "$(DESCRIPTION)" | sed -e 's,\\,\n,g'; \
	echo; \
	echo "$(URL)"; \
	echo "@@";

PACKAGES += $(1)

ifneq ($(__DEFAULT_TARGETS),1)
$(eval $(call Build/DefaultTargets))
endif

endef

ifneq ($(strip $(PKG_SOURCE)),)
$(DL_DIR)/$(PKG_SOURCE):
	@$(CMD_TRACE) "downloading... "
	$(SCRIPT_DIR)/download.pl "$(DL_DIR)" "$(PKG_SOURCE)" "$(PKG_MD5SUM)" $(PKG_SOURCE_URL) $(MAKE_TRACE) 
	
$(PKG_BUILD_DIR)/.prepared: $(DL_DIR)/$(PKG_SOURCE)
endif

ifneq ($(strip $(PKG_CAT)),)
define Build/Prepare/Default
	if [ "$(PKG_CAT)" = "unzip" ]; then \
		unzip -d $(PKG_BUILD_DIR) $(DL_DIR)/$(PKG_SOURCE) ; \
	else \
		$(PKG_CAT) $(DL_DIR)/$(PKG_SOURCE) | tar -C $(PKG_BUILD_DIR)/.. $(TAR_OPTIONS) - ; \
	fi						  
	if [ -d ./patches ]; then \
		$(PATCH) $(PKG_BUILD_DIR) ./patches ; \
	fi
endef
endif

define Build/Prepare
$(call Build/Prepare/Default)
endef

define Build/Configure/Default
# TODO: add configurable default command
endef

define Build/Configure
$(call Build/Configure/Default)
endef

define Build/Compile/Default
# TODO: add configurable default command
endef

define Build/Compile
$(call Build/Compile/Default)
endef

ifneq ($(DUMP),)
dumpinfo:
	$(DUMPINFO)
else

source: $(DL_DIR)/$(PKG_SOURCE)
prepare: source
	@[ -f $(PKG_BUILD_DIR)/.prepared ] || { \
		$(CMD_TRACE) "preparing... "; \
		$(MAKE) $(PKG_BUILD_DIR)/.prepared $(MAKE_TRACE); \
	}

configure: prepare
	@[ -f $(PKG_BUILD_DIR)/.configured ] || { \
		$(CMD_TRACE) "configuring... "; \
		$(MAKE) $(PKG_BUILD_DIR)/.configured $(MAKE_TRACE); \
	}

compile-targets:
compile: configure
	@$(CMD_TRACE) "compiling... " 
	@$(MAKE) compile-targets $(MAKE_TRACE)

install-targets:
install:
	@$(CMD_TRACE) "installing... "
	@$(MAKE) install-targets $(MAKE_TRACE)

mostlyclean:
rebuild:
	$(CMD_TRACE) "rebuilding... "
	@-$(MAKE) mostlyclean 2>&1 >/dev/null
	if [ -f $(PKG_BUILD_DIR)/.built ]; then \
		$(MAKE) clean $(MAKE_TRACE); \
	fi
	$(MAKE) compile $(MAKE_TRACE)

$(PACKAGE_DIR):
	mkdir -p $@

clean-targets:
clean: 
	@$(CMD_TRACE) "cleaning... " 
	@$(MAKE) clean-targets $(MAKE_TRACE)
	rm -rf $(PKG_BUILD_DIR)
endif

.PHONY: all source prepare compile install clean dumpinfo
