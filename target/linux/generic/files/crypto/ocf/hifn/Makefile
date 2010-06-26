# for SGlinux builds
-include $(ROOTDIR)/modules/.config

obj-$(CONFIG_OCF_HIFN)     += hifn7751.o
obj-$(CONFIG_OCF_HIFNHIPP) += hifnHIPP.o

obj ?= .
EXTRA_CFLAGS += -I$(obj)/.. -I$(obj)/

ifdef TOPDIR
-include $(TOPDIR)/Rules.make
endif

