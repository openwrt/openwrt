#
# Makefile for Broadcom Home Networking Division (HND) shared driver code
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

O_TARGET	:= hnd.o

HND_OBJS	:= bcmutils.o hnddma.o linux_osl.o sbutils.o bcmsrom.o

ifneq ($(CONFIG_BCM947XX),y)
HND_OBJS	+= nvramstubs.o
endif

export-objs	:= shared_ksyms.o
obj-y		:= shared_ksyms.o $(HND_OBJS)
obj-m		:= $(O_TARGET)

include $(TOPDIR)/Rules.make

shared_ksyms.c: shared_ksyms.sh $(HND_OBJS)
	sh -e $< $(HND_OBJS) > $@
