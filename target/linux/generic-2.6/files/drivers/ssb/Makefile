ssb-builtin-drivers-y					+= driver_chipcommon.o
ssb-builtin-drivers-$(CONFIG_SSB_DRIVER_MIPS)		+= driver_mipscore.o
ssb-builtin-drivers-$(CONFIG_SSB_DRIVER_PCICORE)	+= driver_pcicore.o

ssb-hostsupport-$(CONFIG_SSB_PCIHOST)			+= pci.o pcihost_wrapper.o
ssb-hostsupport-$(CONFIG_SSB_PCMCIAHOST)		+= pcmcia.o

obj-$(CONFIG_SSB) += ssb.o

ssb-objs	:= main.o scan.o \
		   $(ssb-hostsupport-y) $(ssb-builtin-drivers-y)
