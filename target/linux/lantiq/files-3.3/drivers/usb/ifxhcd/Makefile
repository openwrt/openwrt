
#
# Makefile for USB Core files and filesystem
#
	ifxusb_host-objs    := ifxusb_driver.o
	ifxusb_host-objs    += ifxusb_ctl.o
	ifxusb_host-objs    += ifxusb_cif.o
	ifxusb_host-objs    += ifxusb_cif_h.o
	ifxusb_host-objs    += ifxhcd.o
	ifxusb_host-objs    += ifxhcd_es.o
	ifxusb_host-objs    += ifxhcd_intr.o
	ifxusb_host-objs    += ifxhcd_queue.o

ifeq ($(CONFIG_IFX_TWINPASS),y)
        EXTRA_CFLAGS        += -D__IS_TWINPASS__
endif
ifeq ($(CONFIG_IFX_DANUBE),y)
        EXTRA_CFLAGS        += -D__IS_DANUBE__
endif
ifeq ($(CONFIG_IFX_AMAZON_SE),y)
        EXTRA_CFLAGS        += -D__IS_AMAZON_SE__
endif
ifeq ($(CONFIG_IFX_AR9),y)
        EXTRA_CFLAGS        += -D__IS_AR9__
endif
ifeq ($(CONFIG_IFX_AMAZON_S),y)
        EXTRA_CFLAGS        += -D__IS_AR9__
endif
ifeq ($(CONFIG_IFX_VR9),y)
        EXTRA_CFLAGS        += -D__IS_VR9__
endif

ifeq ($(CONFIG_USB_HOST_IFX),y)
	EXTRA_CFLAGS  += -Dlinux -D__LINUX__
	EXTRA_CFLAGS  += -D__IS_HOST__
	EXTRA_CFLAGS  += -D__KERNEL__
endif

ifeq ($(CONFIG_USB_HOST_IFX),m)
	EXTRA_CFLAGS  += -Dlinux -D__LINUX__
	EXTRA_CFLAGS  += -D__IS_HOST__
	EXTRA_CFLAGS  += -D__KERNEL__
endif

ifeq ($(CONFIG_USB_DEBUG),y)
	EXTRA_CFLAGS  += -D__DEBUG__
	EXTRA_CFLAGS  += -D__ENABLE_DUMP__
endif

ifeq ($(CONFIG_USB_HOST_IFX_B),y)
        EXTRA_CFLAGS  += -D__IS_DUAL__
endif
ifeq ($(CONFIG_USB_HOST_IFX_1),y)
        EXTRA_CFLAGS  += -D__IS_FIRST__
endif
ifeq ($(CONFIG_USB_HOST_IFX_2),y)
        EXTRA_CFLAGS  += -D__IS_SECOND__
endif

ifeq ($(CONFIG_USB_HOST_IFX_FORCE_USB11),y)
	EXTRA_CFLAGS  += -D__FORCE_USB11__
endif
ifeq ($(CONFIG_USB_HOST_IFX_WITH_HS_ELECT_TST),y)
	EXTRA_CFLAGS  += -D__WITH_HS_ELECT_TST__
endif
ifeq ($(CONFIG_USB_HOST_IFX_WITH_ISO),y)
	EXTRA_CFLAGS  += -D__EN_ISOC__
endif
ifeq ($(CONFIG_USB_HOST_IFX_UNALIGNED_ADJ),y)
	EXTRA_CFLAGS  += -D__UNALIGNED_BUFFER_ADJ__
endif
ifeq ($(CONFIG_USB_HOST_IFX_UNALIGNED_CHK),y)
	EXTRA_CFLAGS  += -D__UNALIGNED_BUFFER_CHK__
endif

#	EXTRA_CFLAGS  += -D__DYN_SOF_INTR__
	EXTRA_CFLAGS  += -D__UEIP__
#	EXTRA_CFLAGS  += -D__EN_ISOC__
#	EXTRA_CFLAGS  += -D__EN_ISOC_SPLIT__

## 20110628 AVM/WK New flag for less SOF IRQs
	EXTRA_CFLAGS  += -D__USE_TIMER_4_SOF__
	
obj-$(CONFIG_USB_HOST_IFX)	+= ifxusb_host.o

