#
# Linux router shared code Makefile
#
# Copyright 2001-2003, Broadcom Corporation
# All Rights Reserved.
# 
# THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
# KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
# SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
# FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
#
# $Id$
include $(TOPDIR)/rules.mk

PKG_NAME:=libnvram
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

EXTRA_CFLAGS := -c -I. -I../include

all: compile

define OBJ_template
$(PKG_BUILD_DIR)/$(1): $(PKG_BUILD_DIR)
	$(TARGET_CC) $(TARGET_CFLAGS) $(EXTRA_CFLAGS) -o $$@ $$(patsubst %.o,%.c,$(1))
endef

OBJS := nvram_linux.o nvram_convert.o main.o
LIBNVRAM_OBJS:= $(PKG_BUILD_DIR)/nvram_linux.o $(PKG_BUILD_DIR)/nvram_convert.o
$(foreach obj,$(OBJS),$(eval $(call OBJ_template,$(obj))))

$(PKG_BUILD_DIR):
	mkdir -p $(PKG_BUILD_DIR)

$(PKG_BUILD_DIR)/nvram: $(PKG_BUILD_DIR)/main.o
	$(TARGET_CC) $(TARGET_CFLAGS) -o $@ $^ -L$(PKG_BUILD_DIR) -lnvram 

$(PKG_BUILD_DIR)/libnvram.so: $(LIBNVRAM_OBJS)
	$(TARGET_CC) -shared -o $@ $^

$(TARGET_DIR)/usr/lib/libnvram.so: $(PKG_BUILD_DIR)/libnvram.so
	install -m 644 $< $@
	install -m 644 $< $(STAGING_DIR)/lib/libnvram.so
	$(STRIP) $@

$(TARGET_DIR)/usr/sbin/nvram: $(PKG_BUILD_DIR)/nvram
	mkdir -p $(TARGET_DIR)/usr/sbin
	install -m 644 $< $@
	$(STRIP) $@

compile: $(PKG_BUILD_DIR)/libnvram.so $(PKG_BUILD_DIR)/nvram
install: $(TARGET_DIR)/usr/lib/libnvram.so $(TARGET_DIR)/usr/sbin/nvram

clean:
	rm -rf $(PKG_BUILD_DIR)
