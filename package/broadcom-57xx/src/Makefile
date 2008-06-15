#
# Broadcom BCM57XX Gigabit Ethernet driver make file.
#
# $Id: Makefile,v 1.1.1.4 2006/09/26 02:17:12 michael Exp $
#

O_TARGET = bcm57xx.o

EXTRA_CFLAGS += -DDBG=0 -DBCM_PROC_FS=1 -DT3_JUMBO_RCV_RCB_ENTRY_COUNT=256
EXTRA_CFLAGS += -DPCIX_TARGET_WORKAROUND=1 -DINCLUDE_TBI_SUPPORT=1 -DINCLUDE_5701_AX_FIX=1

export-objs :=

obj-y := b57um.o tigon3.o autoneg.o 5701rls.o tcp_seg.o hndgige.o bcmrobo.o
obj-m := $(O_TARGET)

include $(TOPDIR)/Rules.make
