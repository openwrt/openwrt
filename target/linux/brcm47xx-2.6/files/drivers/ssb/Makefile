ssb-driver-chipcommon-y				:= driver_chipcommon/chipcommon.o
ssb-driver-mips-$(CONFIG_BCM947XX)	:= driver_mips/mips.o
ssb-driver-pci-$(CONFIG_SSB_DRIVER_PCICORE)	:= driver_pci/pcicore.o

ssb-$(CONFIG_SSB_PCIHOST)			+= pci.o
ssb-$(CONFIG_SSB_PCMCIAHOST)			+= pcmcia.o

obj-$(CONFIG_SSB) += ssb.o

ssb-objs	:= core.o scan.o \
		   $(ssb-y) $(ssb-m) \
		   $(ssb-driver-chipcommon-y) \
		   $(ssb-driver-mips-y) \
		   $(ssb-driver-pci-y)
