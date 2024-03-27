#
# Makefile for MikroTik RouterBoard platform specific drivers
#
obj-$(CONFIG_MIKROTIK_RB_SYSFS)     += routerboot.o rb_hardconfig.o rb_softconfig.o
ifneq ($(filter y,$(CONFIG_CPU_LITTLE_ENDIAN) $(CONFIG_ARCH_IPQ40XX)),)
obj-y				    += rb_hardconfig_lz77.o
endif
