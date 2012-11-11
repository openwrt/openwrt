#
# Makefile for DWC_otg Highspeed USB controller driver
#

EXTRA_CFLAGS	+= -DDWC_HS_ELECT_TST
#EXTRA_CFLAGS	+= -Dlinux -DDWC_HS_ELECT_TST
#EXTRA_CFLAGS	+= -DDWC_EN_ISOC

ifneq ($(CONFIG_DWC_HOST_ONLY),)
EXTRA_CFLAGS    += -DDWC_HOST_ONLY
endif

ifneq ($(CONFIG_DWC_DEVICE_ONLY),)
EXTRA_CFLAGS    += -DDWC_DEVICE_ONLY
endif

ifneq ($(CONFIG_DWC_DEBUG),)
EXTRA_CFLAGS	+= -DDEBUG
endif

obj-$(CONFIG_USB_DWC_OTG)	:= dwc_otg.o

dwc_otg-objs	:= otg_driver.o otg_attr.o
dwc_otg-objs	+= otg_cil.o otg_cil_intr.o
dwc_otg-objs	+= otg_pcd.o otg_pcd_intr.o
dwc_otg-objs	+= otg_hcd.o otg_hcd_intr.o otg_hcd_queue.o
