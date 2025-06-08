# rt-loader make file
# (c) 2025 Markus Stockhausen
#
# This is the make file for the rt-loader (aka runtime or realtek loader). It tries to
# avoid copying files around where possible. Therefore it is controlled by the following
# input parameters
#
# KERNEL_IMG_IN:	The filename of an LZMA compressed kernel image. This is required
# KERNEL_IMG_OUT:	The filename of the kernel image with the rt-loader prepended.
#			If not given it will be created as image.bin into the BUILD_DIR.
# BUILD_DIR: 		The temporary build dir. If not given it will be set to "build".
#
# To add it into the OpenWrt toolchain just create two new build commands
#
# define Build/rt-loader
#   $(MAKE) all clean -C rt-loader CROSS_COMPILE="$(TARGET_CROSS)" \
#	    KERNEL_IMG_IN="$@" KERNEL_IMG_OUT="$@.new" BUILD_DIR="$@.build"
#   mv "$@.new" "$@"
# endef
#
# define Build/rt-compress
#   $(STAGING_DIR_HOST)/bin/xz --format=lzma -9 --stdout "$@" > "$@.new"
#   mv "$@.new" "$@"
# endef
#
# Use them in a new kernel build recipe
#
# define Device/uimage-rt-loader
#   KERNEL/rt-loader := kernel-bin | append-dtb | rt-compress | rt-loader
#   KERNEL := $$(KERNEL/rt-loader) | uImage none
#   KERNEL_INITRAMFS := $$(KERNEL/rt-loader) | uImage none
# endef
#
# And finally add it to the target device. E.g.
#
# define Device/linksys_lgs310c
#   $(Device/uimage-rt-loader)
#   ...
# endef

CC		:= $(CROSS_COMPILE)gcc
LD		:= $(CROSS_COMPILE)ld
OBJCOPY		:= $(CROSS_COMPILE)objcopy
OBJDUMP		:= $(CROSS_COMPILE)objdump

CFLAGS		= -fpic -mabicalls -O2 -fno-builtin-printf -Iinclude

ASFLAGS		= -fpic -msoft-float -Iinclude

LDFLAGS		= -static -nostdlib -T linker/linker.ld --no-warn-mismatch

O_FORMAT 	= $(shell $(OBJDUMP) -i | head -2 | grep elf32)

SOURCES		= src/startup.S src/main.c src/board.c src/memory.c src/unlzma.c

BUILD_DIR	?= build

IMAGE_OBJ	:= $(BUILD_DIR)/image.o
IMAGE_ELF     	:= $(BUILD_DIR)/image.elf

KERNEL_IMG_OUT	?= $(BUILD_DIR)/image.bin

OBJECTS_C	= $(filter %.c,$(SOURCES))
OBJECTS_S	= $(filter %.S,$(SOURCES))

OBJECTS		:= $(OBJECTS_S:.S=.o) $(OBJECTS_C:.c=.o)
OBJECTS		:= $(patsubst %.o, $(BUILD_DIR)/%.o, $(OBJECTS)) $(IMAGE_OBJ)

ifneq ($(MAKECMDGOALS),clean)
ifndef KERNEL_IMG_IN
$(error Compressed kernel image not given via KERNEL_IMG_IN)
endif
endif

all: $(KERNEL_IMG_OUT)

install:

$(BUILD_DIR)/%.o : %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR)/%.o : %.S
	@mkdir -p $(dir $@)
	$(CC) $(ASFLAGS) -c -o $@ $<

$(IMAGE_OBJ): $(KERNEL_IMG_IN)
	$(OBJCOPY) -I binary -O $(O_FORMAT) --rename-section .data=.kernel $< $@

$(IMAGE_ELF): $(OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $(OBJECTS)

$(KERNEL_IMG_OUT): $(IMAGE_ELF)
	$(OBJCOPY) -O binary $< $@

clean:
	rm -rf $(BUILD_DIR)/

