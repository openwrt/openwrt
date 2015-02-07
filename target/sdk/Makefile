# 
# Copyright (C) 2006-2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk
include $(INCLUDE_DIR)/host.mk
include $(INCLUDE_DIR)/version.mk

override MAKEFLAGS=

SDK_NAME:=OpenWrt-SDK-$(if $(CONFIG_VERSION_FILENAMES),$(VERSION_NUMBER)-)$(BOARD)$(if $(SUBTARGET),-$(SUBTARGET))_gcc-$(GCCV)$(DIR_SUFFIX).$(HOST_OS)-$(HOST_ARCH)
SDK_BUILD_DIR:=$(BUILD_DIR)/$(SDK_NAME)

STAGING_SUBDIR_HOST := staging_dir/host
STAGING_SUBDIR_TARGET := staging_dir/target-$(ARCH)$(ARCH_SUFFIX)_$(LIBC)-$(LIBCV)$(if $(CONFIG_arm),_eabi)
STAGING_SUBDIR_TOOLCHAIN := staging_dir/toolchain-$(ARCH)$(ARCH_SUFFIX)_gcc-$(GCCV)_$(LIBC)-$(LIBCV)$(if $(CONFIG_arm),_eabi)

EXCLUDE_DIRS:=*/ccache \
	*/stamp \
	*/stampfiles \
	*/man \
	*/info \
	*/root-* \
	initial \
	*.install.clean \
	*.install.flags \
	*.install \
	*/doc

SDK_DIRS = \
		$(STAGING_SUBDIR_HOST) \
		$(STAGING_SUBDIR_TARGET) \
		$(STAGING_SUBDIR_TOOLCHAIN)

BASE_FEED:=$(shell git config --get remote.origin.url 2>/dev/null | sed -ne 's/^/src-git base /p')
BASE_FEED:=$(if $(BASE_FEED),$(BASE_FEED),$(shell cd $(TOPDIR); LC_ALL=C git svn info 2>/dev/null | sed -ne 's/^URL: /src-gitsvn base /p'))
BASE_FEED:=$(if $(BASE_FEED),$(BASE_FEED),$(shell cd $(TOPDIR); LC_ALL=C svn info 2>/dev/null | sed -ne 's/^URL: /src-svn base /p'))

KDIR_BASE = $(patsubst $(TOPDIR)/%,%,$(LINUX_DIR))

KERNEL_FILES_ARCH = \
	Makefile* \
	module.lds \
	Kbuild.platforms \
	*/Platform \
	include \
	*/include \
	scripts \
	kernel/asm-offsets.s

KERNEL_FILES_BASE := \
	.config \
	Makefile \
	scripts \
	include \
	Module.symvers \
	$(addprefix arch/$(LINUX_KARCH)/,$(KERNEL_FILES_ARCH))

KERNEL_FILES := $(patsubst $(TOPDIR)/%,%,$(wildcard $(addprefix $(LINUX_DIR)/,$(KERNEL_FILES_BASE))))

all: compile

$(BIN_DIR)/$(SDK_NAME).tar.bz2: clean
	mkdir -p $(SDK_BUILD_DIR)/dl $(SDK_BUILD_DIR)/package
	$(CP) $(INCLUDE_DIR) $(SCRIPT_DIR) $(TOPDIR)/docs $(SDK_BUILD_DIR)/
	$(TAR) -cf - -C $(TOPDIR) \
		$(foreach exclude,$(EXCLUDE_DIRS),--exclude="$(exclude)") \
		$(SDK_DIRS) $(KERNEL_FILES) | \
		$(TAR) -xf - -C $(SDK_BUILD_DIR)

	@-( \
		find \
			$(SDK_BUILD_DIR)/$(STAGING_SUBDIR_HOST)/bin \
			$(SDK_BUILD_DIR)/$(STAGING_SUBDIR_HOST)/usr/bin \
			$(SDK_BUILD_DIR)/$(STAGING_SUBDIR_TOOLCHAIN)/bin \
			$(SDK_BUILD_DIR)/$(STAGING_SUBDIR_TOOLCHAIN)/*/bin \
			$(SDK_BUILD_DIR)/$(STAGING_SUBDIR_TOOLCHAIN)/libexec \
			-type f; \
		find \
			$(SDK_BUILD_DIR)/$(STAGING_SUBDIR_HOST)/lib \
			$(SDK_BUILD_DIR)/$(STAGING_SUBDIR_HOST)/usr/lib \
			-type f -name \*.so\* -or -name \*.a; \
	) | xargs strip 2>/dev/null >/dev/null

	mkdir -p $(SDK_BUILD_DIR)/target/linux
	$(CP) $(GENERIC_PLATFORM_DIR) $(PLATFORM_DIR) $(SDK_BUILD_DIR)/target/linux/
	rm -rf \
		$(SDK_BUILD_DIR)/target/linux/*/files* \
		$(SDK_BUILD_DIR)/target/linux/*/patches*
	./convert-config.pl $(TOPDIR)/.config > $(SDK_BUILD_DIR)/Config-build.in
	$(CP) -L \
		$(TOPDIR)/LICENSE \
		$(TOPDIR)/rules.mk \
		$(TOPDIR)/.config \
		./files/Config.in \
		./files/Makefile \
		./files/README.SDK \
		$(SDK_BUILD_DIR)/
	$(CP) \
		$(TOPDIR)/package/Makefile \
		$(SDK_BUILD_DIR)/package/

	-rm -f $(SDK_BUILD_DIR)/feeds.conf.default
	$(if $(BASE_FEED),echo "$(BASE_FEED)" > $(SDK_BUILD_DIR)/feeds.conf.default)
	if [ -f $(TOPDIR)/feeds.conf ]; then \
		cat $(TOPDIR)/feeds.conf >> $(SDK_BUILD_DIR)/feeds.conf.default; \
	else \
		cat $(TOPDIR)/feeds.conf.default >> $(SDK_BUILD_DIR)/feeds.conf.default; \
	fi
	$(SED) 's,^# REVISION:=.*,REVISION:=$(REVISION),g' $(SDK_BUILD_DIR)/include/version.mk
	$(SED) '/LINUX_VERMAGIC:=/ { s,unknown,$(LINUX_VERMAGIC),g }' $(SDK_BUILD_DIR)/include/kernel.mk
	find $(SDK_BUILD_DIR) -name .git | $(XARGS) rm -rf
	find $(SDK_BUILD_DIR) -name .svn | $(XARGS) rm -rf
	find $(SDK_BUILD_DIR) -name CVS | $(XARGS) rm -rf
	(cd $(BUILD_DIR); \
		tar cfj $@ $(SDK_NAME); \
	)

download:
prepare:
compile: $(BIN_DIR)/$(SDK_NAME).tar.bz2
install: compile

clean:
	rm -rf $(SDK_BUILD_DIR) $(BIN_DIR)/$(SDK_NAME).tar.bz2
