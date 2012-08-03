#
# Makefile for DWC_otg Highspeed USB controller driver
#

ifeq ($(CONFIG_DWC_OTG_DEBUG),y)
EXTRA_CFLAGS   += -DDEBUG
endif

# Use one of the following flags to compile the software in host-only or
# device-only mode based on the configuration selected by the user
ifeq ($(CONFIG_DWC_OTG_HOST_ONLY),y)
	EXTRA_CFLAGS   += -DDWC_OTG_HOST_ONLY -DDWC_HOST_ONLY
	EXTRA_CFLAGS   += -DDWC_OTG_EN_ISOC -DDWC_EN_ISOC
else ifeq ($(CONFIG_DWC_OTG_DEVICE_ONLY),y)
	EXTRA_CFLAGS   += -DDWC_OTG_DEVICE_ONLY
else
	EXTRA_CFLAGS   += -DDWC_OTG_MODE
endif

#	EXTRA_CFLAGS += -DDWC_HS_ELECT_TST
#	EXTRA_CFLAGS    += -DDWC_OTG_EXT_CHG_PUMP

ifeq ($(CONFIG_DWC_OTG_LANTIQ),y)
     EXTRA_CFLAGS += -Dlinux -D__LINUX__ -DDWC_OTG_IFX -DDWC_OTG_HOST_ONLY -DDWC_HOST_ONLY  -D__KERNEL__ 
endif
ifeq ($(CONFIG_DWC_OTG_LANTIQ),m)
     EXTRA_CFLAGS += -Dlinux -D__LINUX__ -DDWC_OTG_IFX -DDWC_HOST_ONLY -DMODULE -D__KERNEL__  -DDEBUG
endif

obj-$(CONFIG_DWC_OTG)	:= dwc_otg.o
dwc_otg-objs    := dwc_otg_hcd.o dwc_otg_hcd_intr.o dwc_otg_hcd_queue.o
#dwc_otg-objs	+= dwc_otg_pcd.o dwc_otg_pcd_intr.o 
dwc_otg-objs    += dwc_otg_attr.o 
dwc_otg-objs    += dwc_otg_cil.o dwc_otg_cil_intr.o
dwc_otg-objs	+= dwc_otg_ifx.o
dwc_otg-objs    += dwc_otg_driver.o

#obj-$(CONFIG_DWC_OTG_IFX)	:= dwc_otg_ifx.o
#dwc_otg_ifx-objs		:= dwc_otg_ifx.o
