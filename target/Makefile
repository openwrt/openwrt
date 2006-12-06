# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/target.mk

all: install

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

TARGETS-y := linux
TARGETS-$(CONFIG_SDK) += sdk

linux-install: $(BIN_DIR)
image_install: linux-install

IMAGE_DIR:=linux/$(BOARD)-$(KERNEL)/image


download: $(patsubst %,%-download,$(TARGETS-y))
prepare: linux-prepare
compile: linux-compile image_compile
install: image_clean $(patsubst %,%-install,$(TARGETS-y)) image_install
clean: $(patsubst %,%-clean,$(TARGETS-y)) image_clean

prereq: FORCE
	$(MAKE) -C linux/$(BOARD)-$(KERNEL) prereq
	$(MAKE) -C $(IMAGE_DIR) prereq

image_clean: FORCE
	$(MAKE) -C $(IMAGE_DIR) clean
	rm -f $(BIN_DIR)/openwrt-*
	
image_compile: FORCE
	$(MAKE) -C $(IMAGE_DIR) compile

image_install: image_compile
	$(MAKE) -C $(IMAGE_DIR) install

%-clean: FORCE
	$(MAKE) -C $(patsubst %-clean,%,$@) clean
%-download: FORCE
	$(MAKE) -C $(patsubst %-download,%,$@) download
%-prepare: FORCE
	$(MAKE) -C $(patsubst %-prepare,%,$@) prepare
%-compile: %-prepare
	$(MAKE) -C $(patsubst %-compile,%,$@) compile
%-install: %-compile
	$(MAKE) -C $(patsubst %-install,%,$@) install

