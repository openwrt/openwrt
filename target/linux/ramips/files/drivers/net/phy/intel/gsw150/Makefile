# This is a Reference file and can be modified by customer
# depends on customer build environment.
# Makefile for Switch API
#
# EXTRA_CFLAGS += -I$(KERNEL_SRC)$SourcePath

# Build as part of Kernel driver

# obj-$(GSW150)	+= gsw150.o

# modules:
# 	make -C $(KERNEL_DIR) M=$(PWD) modules

# modules_install:
# 	make -C $(KERNEL_DIR) M=$(PWD) modules_install

# clean:
# 	make -C $(KERNEL_DIR) M=$(PWD) clean

obj-$(CONFIG_GSW150_SUPPORT) := gsw150.o

gsw150-objs   := gsw_io_wrap.o
gsw150-objs   += gsw_sw_init.o
gsw150-objs   += gsw_api_func.o
gsw150-objs   += gsw_core.o
gsw150-objs   += gsw_pce.o
gsw150-objs   += gsw_vlan.o
gsw150-objs   += gsw_qos.o
gsw150-objs   += gsw_multi.o

#ccflags-y = -Wall -Wstrict-prototypes -Wno-trigraphs -Wframe-larger-than=4096 -Werror
