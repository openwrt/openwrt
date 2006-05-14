# Main makefile for the packages
include $(TOPDIR)/rules.mk
include ../rules.mk

package-y += base-files
package-$(BR2_PACKAGE_KMOD_ALSA) += alsa
package-$(BR2_PACKAGE_KMOD_FUSE) += fuse
package-$(BR2_PACKAGE_KMOD_HOSTAP) += hostap
package-$(BR2_PACKAGE_KMOD_MADWIFI) += madwifi
package-$(BR2_PACKAGE_KMOD_MADWIFI_OLD) += madwifi-old
package-$(BR2_PACKAGE_KMOD_MINI_FO) += mini_fo
package-$(BR2_PACKAGE_KMOD_OPENSWAN) += openswan
package-$(BR2_PACKAGE_KMOD_SHFS) += shfs
package-$(BR2_PACKAGE_KMOD_SPCA5XX) += spca5xx
package-$(BR2_PACKAGE_KMOD_SWITCH) += switch
package-$(BR2_PACKAGE_KMOD_UEAGLE_ATM) += ueagle-atm
package-$(BR2_PACKAGE_KMOD_DIAG) += diag
package-$(BR2_PACKAGE_KMOD_WLCOMPAT) += wlcompat
package-$(BR2_PACKAGE_KMOD_ZD1211) += zd1211

wlcompat-compile: base-files-compile

all: compile install
clean: $(patsubst %,%-clean,$(package-) $(package-y) $(package-m))
prepare: $(patsubst %,%-prepare,$(package-y) $(package-m))
compile: $(patsubst %,%-compile,$(package-y) $(package-m))
install: $(patsubst %,%-install,$(package-y))

KPKG_MAKEOPTS += BUILD_DIR="$(LINUX_BUILD_DIR)"

%-prepare:
	@$(START_TRACE) "target/linux/package/$(patsubst %-prepare,%,$@)/prepare: "
	$(MAKE) -C $(patsubst %-prepare,%,$@) \
		$(KPKG_MAKEOPTS) \
		prepare
	@$(CMD_TRACE) " done"
	@$(END_TRACE)

%-compile:
	@$(START_TRACE) "target/linux/package/$(patsubst %-compile,%,$@)-compile: "
	$(MAKE) -C $(patsubst %-compile,%,$@) \
		$(KPKG_MAKEOPTS) \
		compile
	@$(CMD_TRACE) " done"
	@$(END_TRACE)

%-install:
	@$(START_TRACE) "target/linux/package/$(patsubst %-install,%,$@)-install: "
	$(MAKE) -C $(patsubst %-install,%,$@) \
		$(KPKG_MAKEOPTS) \
		install
	@$(CMD_TRACE) " done"
	@$(END_TRACE)

%-clean:
	@$(START_TRACE) "target/linux/package/$(patsubst %-clean,%,$@)-clean: "
	@$(MAKE) -C $(patsubst %-clean,%,$@) \
		$(KPKG_MAKEOPTS) \
		clean
	@$(CMD_TRACE) " done"
	@$(END_TRACE)

