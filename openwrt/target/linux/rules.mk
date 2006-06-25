KERNEL:=unknown
ifneq (,$(findstring 2.4.,$(LINUX_VERSION)))
KERNEL:=2.4
endif
ifneq (,$(findstring 2.6.,$(LINUX_VERSION)))
KERNEL:=2.6
endif

MODULES_SUBDIR:=lib/modules/$(LINUX_VERSION)

LINUX_BUILD_DIR:=$(BUILD_DIR)/linux-$(KERNEL)-$(BOARD)
LINUX_DIR := $(LINUX_BUILD_DIR)/linux-$(LINUX_VERSION)
LINUX_KERNEL:=$(LINUX_BUILD_DIR)/vmlinux

LINUX_TARGET_DIR:=$(LINUX_BUILD_DIR)/root
IPKG_KERNEL:=IPKG_TMP=$(BUILD_DIR)/tmp IPKG_INSTROOT=$(LINUX_TARGET_DIR) IPKG_CONF_DIR=$(LINUX_BUILD_DIR) $(SCRIPT_DIR)/ipkg -force-defaults -force-depends

LINUX_KARCH:=$(shell echo $(ARCH) | sed -e 's/i[3-9]86/i386/' \
	-e 's/mipsel/mips/' \
	-e 's/mipseb/mips/' \
	-e 's/powerpc/ppc/' \
	-e 's/sh[234]/sh/' \
	-e 's/armeb/arm/' \
)

KPKG_MAKEOPTS:=	IPKG="$(IPKG_KERNEL)" \
		BOARD="$(BOARD)" \
		KERNEL="$(KERNEL)" \
		TARGET_DIR="$(LINUX_TARGET_DIR)" \
		LINUX_BUILD_DIR="$(LINUX_BUILD_DIR)" \
		LINUX_DIR="$(LINUX_DIR)" \
		LINUX_VERSION="$(LINUX_VERSION)" \
		LINUX_RELEASE="$(LINUX_RELEASE)"

KMOD_BUILD_DIR := $(LINUX_BUILD_DIR)/linux-modules
MODULES_DIR := $(LINUX_BUILD_DIR)/modules/$(MODULES_SUBDIR)
TARGET_MODULES_DIR := $(LINUX_TARGET_DIR)/$(MODULES_SUBDIR)

ifeq ($(KERNEL),2.6)
LINUX_KMOD_SUFFIX=ko
else
LINUX_KMOD_SUFFIX=o
endif

define KMOD_template
ifeq ($$(strip $(4)),)
KDEPEND_$(1):=m
else
KDEPEND_$(1):=$($(4))
endif

IDEPEND_$(1):=kernel ($(LINUX_VERSION)-$(BOARD)-$(LINUX_RELEASE)) $(foreach pkg,$(5),", $(pkg)")

PKG_$(1) := $(PACKAGE_DIR)/kmod-$(2)_$(LINUX_VERSION)-$(BOARD)-$(LINUX_RELEASE)_$(ARCH).ipk
I_$(1) := $(KMOD_BUILD_DIR)/ipkg/$(2)

ifeq ($$(KDEPEND_$(1)),m)
ifneq ($$(BR2_PACKAGE_KMOD_$(1)),)
TARGETS += $$(PKG_$(1))
endif
ifeq ($$(BR2_PACKAGE_KMOD_$(1)),y)
INSTALL_TARGETS += $$(PKG_$(1))
endif
endif

$$(PKG_$(1)): $(LINUX_DIR)/.modules_done
	rm -rf $$(I_$(1))
	$(SCRIPT_DIR)/make-ipkg-dir.sh $$(I_$(1)) ../control/kmod-$(2).control $(LINUX_VERSION)-$(BOARD)-$(LINUX_RELEASE) $(ARCH)
	echo "Depends: $$(IDEPEND_$(1))" >> $$(I_$(1))/CONTROL/control
ifneq ($(strip $(3)),)
	mkdir -p $$(I_$(1))/lib/modules/$(LINUX_VERSION)
	$(CP) $(3) $$(I_$(1))/lib/modules/$(LINUX_VERSION)
endif
ifneq ($(6),)
	mkdir -p $$(I_$(1))/etc/modules.d
	for module in $(7); do \
		echo $$$$module >> $$(I_$(1))/etc/modules.d/$(6)-$(2); \
	done
	echo "#!/bin/sh" >> $$(I_$(1))/CONTROL/postinst
	echo "[ -z \"\$$$$IPKG_INSTROOT\" ] || exit" >> $$(I_$(1))/CONTROL/postinst
	echo ". /etc/functions.sh" >> $$(I_$(1))/CONTROL/postinst
	echo "load_modules /etc/modules.d/$(6)-$(2)" >> $$(I_$(1))/CONTROL/postinst
	chmod 0755 $$(I_$(1))/CONTROL/postinst
endif
	$(8)
	$(IPKG_BUILD) $$(I_$(1)) $(PACKAGE_DIR) $(MAKE_TRACE)

endef

KERNEL_IPKG:=$(LINUX_BUILD_DIR)/kernel_$(LINUX_VERSION)-$(BOARD)-$(LINUX_RELEASE)_$(ARCH).ipk
INSTALL_TARGETS := $(KERNEL_IPKG)
TARGETS := 

