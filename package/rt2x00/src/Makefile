rt2x00lib-objs := rt2x00dev.o rt2x00mac.o

EXTRA_CFLAGS += -DCONFIG_RT2X00_LIB_FIRMWARE

obj-m				+= rt2x00lib.o rt2x00pci.o rt2x00usb.o

obj-$(CONFIG_RT2400PCI)		+= rt2400pci.o
obj-$(CONFIG_RT2500PCI)		+= rt2500pci.o
obj-$(CONFIG_RT61PCI)		+= rt61pci.o
obj-$(CONFIG_RT2500USB)		+= rt2500usb.o
obj-$(CONFIG_RT73USB)		+= rt73usb.o
