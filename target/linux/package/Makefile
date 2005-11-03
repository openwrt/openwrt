# Main makefile for the packages
include $(TOPDIR)/rules.mk

ifneq ($(BOARD),ar7)
ifneq ($(BOARD)-$(KERNEL),brcm-2.6)
package-$(BR2_PACKAGE_KMOD_ALSA) += alsa
endif
endif
package-$(BR2_PACKAGE_KMOD_FUSE) += fuse
package-$(BR2_PACKAGE_KMOD_HOSTAP) += hostap
package-$(BR2_PACKAGE_KMOD_MADWIFI) += madwifi
package-$(BR2_PACKAGE_KMOD_MINI_FO) += mini_fo
ifneq ($(BOARD)-$(KERNEL),brcm-2.6)
package-$(BR2_PACKAGE_KMOD_OPENSWAN) += openswan
endif
package-$(BR2_PACKAGE_KMOD_SHFS) += shfs
package-y += base-files

ifeq ($(BOARD)-$(KERNEL),brcm-2.4)
package-$(BR2_PACKAGE_KMOD_DIAG) += diag
package-$(BR2_PACKAGE_KMOD_WLCOMPAT) += wlcompat
wlcompat-compile: base-files-compile
endif

all: compile install
clean: $(patsubst %,%-clean,$(package-) $(package-y) $(package-m))
prepare: $(patsubst %,%-prepare,$(package-y) $(package-m))
compile: $(patsubst %,%-compile,$(package-y) $(package-m))
install: $(patsubst %,%-install,$(package-y))

MAKEOPTS:=	BOARD="$(BOARD)" \
		KERNEL="$(KERNEL)" \
		IPKG="$(IPKG)" \
		TARGET_DIR="$(TARGET_DIR)" \
		BUILD_DIR="$(BUILD_DIR)" \
		KERNEL_DIR="$(KERNEL_DIR)" \
		LINUX_VERSION="$(LINUX_VERSION)" \
		KERNEL_RELEASE="$(KERNEL_RELEASE)"


%-prepare:
	@$(START_TRACE) "target/linux/package/$(patsubst %-prepare,%,$@)/prepare: "
	$(MAKE) -C $(patsubst %-prepare,%,$@) \
		$(MAKEOPTS) \
		prepare
	@$(CMD_TRACE) " done"
	@$(END_TRACE)

%-compile:
	@$(START_TRACE) "target/linux/package/$(patsubst %-compile,%,$@)-compile: "
	$(MAKE) -C $(patsubst %-compile,%,$@) \
		$(MAKEOPTS) \
		compile
	@$(CMD_TRACE) " done"
	@$(END_TRACE)

%-install:
	@$(START_TRACE) "target/linux/package/$(patsubst %-install,%,$@)-install: "
	$(MAKE) -C $(patsubst %-install,%,$@) \
		$(MAKEOPTS) \
		install
	@$(CMD_TRACE) " done"
	@$(END_TRACE)

%-clean:
	@$(START_TRACE) "target/linux/package/$(patsubst %-clean,%,$@)-clean: "
	@$(MAKE) -C $(patsubst %-clean,%,$@) clean
	@$(CMD_TRACE) " done"
	@$(END_TRACE)

