# 
# Copyright (C) 2006-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk
include $(INCLUDE_DIR)/host.mk

override MAKEFLAGS=

PKG_OS:=$(shell uname -s)
PKG_CPU:=$(shell uname -m | sed "s/ //g")

IB_NAME:=OpenWrt-ImageBuilder-$(BOARD)-for-$(PKG_OS)-$(PKG_CPU)
PKG_BUILD_DIR:=$(BUILD_DIR)/$(IB_NAME)
IB_KDIR:=$(patsubst $(TOPDIR)/%,$(PKG_BUILD_DIR)/%,$(KERNEL_BUILD_DIR))
IB_LDIR:=$(patsubst $(TOPDIR)/%,$(PKG_BUILD_DIR)/%,$(LINUX_DIR))

all: compile

$(BIN_DIR)/$(IB_NAME).tar.bz2: clean
	rm -rf $(PKG_BUILD_DIR)
	mkdir -p $(IB_KDIR) $(IB_LDIR) $(PKG_BUILD_DIR)/staging_dir/host \
		$(PKG_BUILD_DIR)/target $(PKG_BUILD_DIR)/scripts
	-cp $(TOPDIR)/.config $(PKG_BUILD_DIR)/.config
	$(CP) \
		$(INCLUDE_DIR) $(SCRIPT_DIR) \
		$(TOPDIR)/rules.mk \
		./files/Makefile \
		./files/repositories.conf \
		$(TMP_DIR)/.targetinfo \
		$(TMP_DIR)/.packageinfo \
		$(PKG_BUILD_DIR)/
	$(SED) 's/$$A/$(BOARD)/' $(PKG_BUILD_DIR)/repositories.conf
	$(CP) $(PACKAGE_DIR) $(PKG_BUILD_DIR)/packages
	$(CP) $(STAGING_DIR_HOST)/bin $(PKG_BUILD_DIR)/staging_dir/host/
	$(CP) $(TOPDIR)/target/linux $(PKG_BUILD_DIR)/target/
	rm -rf \
		$(PKG_BUILD_DIR)/target/linux/*/files{,-*} \
		$(PKG_BUILD_DIR)/target/linux/*/patches{,-*}
	-cp $(KERNEL_BUILD_DIR)/* $(IB_KDIR)/ # don't copy subdirectories here
	-cp $(LINUX_DIR)/.config $(IB_LDIR)/
	echo REVISION:="$(REVISION)" > $(PKG_BUILD_DIR)/include/version.mk
	find $(PKG_BUILD_DIR) -name CVS -o -name .git -o -name .svn \
	  | $(XARGS) rm -rf
	$(TAR) c -C $(BUILD_DIR) $(IB_NAME) | bzip2 -c > $@

download:
prepare:
compile: $(BIN_DIR)/$(IB_NAME).tar.bz2
install: compile

clean: FORCE
	rm -rf $(PKG_BUILD_DIR) $(BIN_DIR)/$(IB_NAME).tar.bz2
