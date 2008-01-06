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
# $Id: Makefile,v 1.2 2005/03/29 03:32:18 mbm Exp $

EXTRA_CFLAGS += -I$(TOPDIR)/arch/mips/bcm947xx/include -DBCMDRIVER=1 -DBCMDMA64=1

O_TARGET	:= wl$(MOD_NAME).o

obj-y		:= wl_mod$(MOD_NAME).o
obj-y		+= bcmutils.o hnddma.o linux_osl.o

obj-m		:= $(O_TARGET)

wl_mod$(MOD_NAME).o: wl_apsta$(MOD_NAME).o
	perl -ne 's,eth%d,wl%d\x00,g,print' < $< > $@

modules: wl$(MOD_NAME).o

include $(TOPDIR)/Rules.make
