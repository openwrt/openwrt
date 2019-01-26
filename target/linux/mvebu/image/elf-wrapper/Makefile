#
# Copyright (C) 2019 Tobias Schramm
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

ifeq ($(TARGET_DIR),)
TARGET_DIR	:= $(KDIR)
endif

PKG_NAME	:= elf-wrapper
PKG_BUILD_DIR	:= $(KDIR)/$(PKG_NAME)

CP		?= cp -rf
RM		?= rm -rf

.PHONY: clean wrap

$(PKG_BUILD_DIR)/.prepared:
	mkdir $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
	touch $@

wrap: $(PKG_BUILD_DIR)/.prepared
	$(CP) $(WRAPPER_FILE) $(PKG_BUILD_DIR)/zImage
	$(MAKE) -C $(PKG_BUILD_DIR) WRAPPER_LOADADDR="$(WRAPPER_LOADADDR)" \
		CROSS_COMPILE="$(TARGET_CROSS)" WRAPPER_FILE="$(WRAPPER_FILE)" \
		clean all

download:
prepare: $(PKG_BUILD_DIR)/.prepared
compile: wrap

install:

clean:
	$(RM) $(PKG_BUILD_DIR)
