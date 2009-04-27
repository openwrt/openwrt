#
# Makefile for the BCM947xx specific kernel interface routines
# under Linux.
#

EXTRA_CFLAGS+=-I$(TOPDIR)/arch/mips/bcm947xx/include -DBCMDRIVER

O_TARGET        := bcm947xx.o

export-objs     := export.o
obj-y		:= prom.o setup.o time.o sbmips.o gpio.o
obj-y		+= nvram.o cfe_env.o hndpmu.o
obj-y		+= sbutils.o utils.o bcmsrom.o hndchipc.o
obj-$(CONFIG_PCI) += sbpci.o pcibios.o
obj-y 		+= export.o

include $(TOPDIR)/Rules.make
