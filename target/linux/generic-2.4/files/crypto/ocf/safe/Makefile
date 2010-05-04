# for SGlinux builds
-include $(ROOTDIR)/modules/.config

obj-$(CONFIG_OCF_SAFE) += safe.o

obj ?= .
EXTRA_CFLAGS += -I$(obj)/.. -I$(obj)/

ifdef TOPDIR
-include $(TOPDIR)/Rules.make
endif

