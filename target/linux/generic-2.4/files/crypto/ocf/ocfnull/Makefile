# for SGlinux builds
-include $(ROOTDIR)/modules/.config

obj-$(CONFIG_OCF_OCFNULL) += ocfnull.o

obj ?= .
EXTRA_CFLAGS += -I$(obj)/..

ifdef TOPDIR
-include $(TOPDIR)/Rules.make
endif

