#
# Makefile for the Broadcom wl driver
#
# Copyright 2004, Broadcom Corporation
# All Rights Reserved.
# 
# THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
# KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
# SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
# FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
#
# $Id$
#

O_TARGET	:= wl.o

WL_OBJS		:= wl_linux.o wlc.o d11ucode.o wlc_phy.o wlc_rate.o wlc_led.o wlc_security.o rc4.o tkhash.o bcmwpa.o

INSUP_OBJS	:= aes.o aeskeywrap.o hmac.o md5.o passhash.o prf.o rijndael-alg-fst.o sha1.o 

# Alternate ioctl interfaces
ifeq ($(CONFIG_NET_WIRELESS),y)
WL_OBJS		+= wlc_cmn_ioctl.o
endif
ifeq ($(CONFIG_WL_OID),y)
WL_OBJS		+= wl_oid.o
endif

ifeq ($(CONFIG_WL_STA),y)
WL_OBJS	+= $(INSUP_OBJS)
endif

# Prefix driver variants
WL_APOBJS	:= $(foreach obj,$(WL_OBJS),ap_$(obj))
WL_STAOBJS	:= $(foreach obj,$(WL_OBJS) wlc_sup.o,sta_$(obj))
WL_APSTAOBJS	:= $(foreach obj,$(WL_OBJS) wlc_sup.o,apsta_$(obj))
ifneq ($(CONFIG_WL_STA),y)
WL_APSTAOBJS	+= $(foreach obj,$(INSUP_OBJS), apsta_$(obj))
endif

# Either or both
ifeq ($(CONFIG_WL_AP),y)
AP		:= AP
endif
ifeq ($(CONFIG_WL_STA),y)
STA		:= STA
endif

# Build all variants as modules but link only one of them
export-objs	:=
obj-y		:= $(WL_$(AP)$(STA)OBJS)
obj-m		:= $(O_TARGET)
variant-objs	:= $(WL_APOBJS) $(WL_STAOBJS) $(WL_APSTAOBJS)

EXTRA_CFLAGS	+= -DDMA

include $(TOPDIR)/Rules.make


