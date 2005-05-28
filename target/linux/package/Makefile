# Main makefile for the packages
include $(TOPDIR)/rules.mk

package-$(BR2_PACKAGE_KMOD_FUSE) += fuse
package-$(BR2_PACKAGE_KMOD_SHFS) += shfs
package-$(BR2_PACKAGE_KMOD_OPENSWAN) += openswan

all: compile install
clean: $(patsubst %,%-clean,$(package-) $(package-y) $(package-m))
compile: $(patsubst %,%-compile,$(package-y) $(package-m))
install: $(patsubst %,%-install,$(package-y))

%-prepare:
	$(MAKE) -C $(patsubst %-prepare,%,$@) \
		BUILD_DIR="$(BUILD_DIR)" \
		KERNEL_DIR="$(KERNEL_DIR)" \
		LINUX_VERSION="$(LINUX_VERSION)" \
		prepare

%-compile: %-prepare 
	$(MAKE) -C $(patsubst %-compile,%,$@) \
		BUILD_DIR="$(BUILD_DIR)" \
		KERNEL_DIR="$(KERNEL_DIR)" \
		LINUX_VERSION="$(LINUX_VERSION)" \
		compile

%-install: %-compile
	$(MAKE) -C $(patsubst %-install,%,$@) \
		BUILD_DIR="$(BUILD_DIR)" \
		KERNEL_DIR="$(KERNEL_DIR)" \
		LINUX_VERSION="$(LINUX_VERSION)" \
		install

%-clean:
	@$(MAKE) -C $(patsubst %-clean,%,$@) clean

