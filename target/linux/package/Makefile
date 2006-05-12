# Main makefile for the packages
include $(TOPDIR)/rules.mk
include ../rules.mk

package-y += base-files
package-$(CONFIG_PACKAGE_KMOD_ALSA) += alsa
package-$(CONFIG_PACKAGE_KMOD_FUSE) += fuse
package-$(CONFIG_PACKAGE_KMOD_HOSTAP) += hostap
package-$(CONFIG_PACKAGE_KMOD_BCM43XX_STANDALONE) += bcm43xx-standalone
package-$(CONFIG_PACKAGE_KMOD_IEEE80211_DSCAPE) += ieee80211-dscape
package-$(CONFIG_PACKAGE_KMOD_MADWIFI) += madwifi
package-$(CONFIG_PACKAGE_KMOD_MADWIFI_OLD) += madwifi-old
package-$(CONFIG_PACKAGE_KMOD_MINI_FO) += mini_fo
package-$(CONFIG_PACKAGE_KMOD_OPENSWAN) += openswan
package-$(CONFIG_PACKAGE_KMOD_SHFS) += shfs
package-$(CONFIG_PACKAGE_KMOD_SPCA5XX) += spca5xx
package-$(CONFIG_PACKAGE_KMOD_SWITCH) += switch
package-$(CONFIG_PACKAGE_KMOD_UEAGLE_ATM) += ueagle-atm
package-$(CONFIG_PACKAGE_KMOD_DIAG) += diag
package-$(CONFIG_PACKAGE_KMOD_WLCOMPAT) += wlcompat
package-$(CONFIG_PACKAGE_KMOD_ZD1211) += zd1211

bcm43xx-standalone-compile: ieee80211-dscape-compile
wlcompat-compile: base-files-compile

all: compile install
clean: $(patsubst %,%-clean,$(package-) $(package-y) $(package-m))
prepare: $(patsubst %,%-prepare,$(package-y) $(package-m))
compile: $(patsubst %,%-compile,$(package-y) $(package-m))
install: $(patsubst %,%-install,$(package-y))

KPKG_MAKEOPTS += BUILD_DIR="$(LINUX_BUILD_DIR)"

%-prepare:
	$(MAKE) -C $(patsubst %-prepare,%,$@) \
		$(KPKG_MAKEOPTS) \
		prepare

%-compile:
	$(MAKE) -C $(patsubst %-compile,%,$@) \
		$(KPKG_MAKEOPTS) \
		compile

%-install:
	$(MAKE) -C $(patsubst %-install,%,$@) \
		$(KPKG_MAKEOPTS) \
		install

%-clean:
	@$(MAKE) -C $(patsubst %-clean,%,$@) \
		$(KPKG_MAKEOPTS) \
		clean

