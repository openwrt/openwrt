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

PKG_NAME:=libshared
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

EXTRA_CFLAGS := -c -I. -I../include

all: compile

LIBSHARED_OBJS:=
define OBJ_template
$(PKG_BUILD_DIR)/$(1): $(PKG_BUILD_DIR)
	$(TARGET_CC) $(TARGET_CFLAGS) $(EXTRA_CFLAGS) -o $$@ $$(patsubst %.o,%.c,$(1))
LIBSHARED_OBJS += $(PKG_BUILD_DIR)/$(1)
endef

OBJS := shutils.o wl.o wl_linux.o defaults.o linux_timer.o
$(foreach obj,$(OBJS),$(eval $(call OBJ_template,$(obj))))


$(PKG_BUILD_DIR):
	mkdir -p $(PKG_BUILD_DIR)

$(PKG_BUILD_DIR)/libshared.so: $(LIBSHARED_OBJS)
	$(TARGET_CC) -shared -o $@ $^

$(TARGET_DIR)/usr/lib/libshared.so: $(PKG_BUILD_DIR)/libshared.so
	install -m 644 $^ $@
	install -m 644 $^ $(STAGING_DIR)/lib/libshared.so
	$(STRIP) $@

compile: $(PKG_BUILD_DIR)/libshared.so
install: $(TARGET_DIR)/usr/lib/libshared.so

clean:
	rm -rf $(PKG_BUILD_DIR)
