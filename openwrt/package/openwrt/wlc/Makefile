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

PKG_NAME:=wlc
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

EXTRA_CFLAGS := -c -I. -I../include

all: compile

WLC_OBJS:=
define OBJ_template
$(PKG_BUILD_DIR)/$(1): $(PKG_BUILD_DIR)
	$(TARGET_CC) $(TARGET_CFLAGS) $(EXTRA_CFLAGS) -o $$@ $$(patsubst %.o,%.c,$(1))
WLC_OBJS += $(PKG_BUILD_DIR)/$(1)
endef

OBJS := wlc.o
$(foreach obj,$(OBJS),$(eval $(call OBJ_template,$(obj))))

$(PKG_BUILD_DIR):
	mkdir -p $(PKG_BUILD_DIR)

$(PKG_BUILD_DIR)/wlc: $(WLC_OBJS)
	$(TARGET_CC) -o $@ $^ -lshared 

$(TARGET_DIR)/usr/sbin/wlc: $(PKG_BUILD_DIR)/wlc
	install -m 755 $^ $@
	$(STRIP) $@

compile: $(PKG_BUILD_DIR)/wlc
install: $(TARGET_DIR)/usr/sbin/wlc

clean:
	rm -rf $(PKG_BUILD_DIR)
