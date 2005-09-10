# Main makefile for the packages
include $(TOPDIR)/rules.mk

package-$(BR2_PACKAGE_KMOD_FUSE) += fuse
package-$(BR2_PACKAGE_KMOD_MINI_FO) += mini_fo
package-$(BR2_PACKAGE_KMOD_SHFS) += shfs
package-$(BR2_PACKAGE_KMOD_OPENSWAN) += openswan
package-$(BR2_PACKAGE_KMOD_MADWIFI) += madwifi
package-y += openwrt

ifeq ($(BOARD)-$(KERNEL),brcm-2.4)
package-$(BR2_PACKAGE_KMOD_WLCOMPAT) += wlcompat
wlcompat-compile: openwrt-compile
endif

all: compile install
clean: $(patsubst %,%-clean,$(package-) $(package-y) $(package-m))
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
	$(MAKE) -C $(patsubst %-prepare,%,$@) \
		$(MAKEOPTS) \
		prepare

%-compile: %-prepare 
	$(MAKE) -C $(patsubst %-compile,%,$@) \
		$(MAKEOPTS) \
		compile

%-install: %-compile
	$(MAKE) -C $(patsubst %-install,%,$@) \
		$(MAKEOPTS) \
		install

%-clean:
	@$(MAKE) -C $(patsubst %-clean,%,$@) clean

