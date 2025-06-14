# xz-loader make file
# (c) 2025 Markus Stockhausen
#
# This is the make file for the xz-loader. It tries to avoid copying files around where
# possible. Therefore it is controlled by the following input parameters
#
# XZ_IN_IMAGE:	The filename of an XZ compressed kernel image. This is required
# XZ_OUT_IMAGE: The filename of the kernel image with the xz-loader prepended. If not
#		given it will be created as image.bin into the BUILD_DIR.
# BUILD_DIR: 	The temporary build dir. If not given it will be set to "build".
#
# To add it into the OpenWrt toolchain just create two new build commands
#
# define Build/xz-loader
#   $(MAKE) all clean -C xz-loader CROSS_COMPILE="$(TARGET_CROSS)" \
#	    XZ_IN_IMAGE="$@" XZ_OUT_IMAGE="$@.new" BUILD_DIR="$@.build"
#   mv "$@.new" "$@"
# endef
#
# define Build/xz
#   $(STAGING_DIR_HOST)/bin/xz -9 --stdout "$@" > "$@.new"
#   mv "$@.new" "$@"
# endef
#
# Use them in a new kernel build recipe
#
# define Device/uimage-xz-loader
#   KERNEL/xz-loader := kernel-bin | append-dtb | xz | xz-loader
#   KERNEL := $$(KERNEL/xz-loader) | uImage none
#   KERNEL_INITRAMFS := $$(KERNEL/xz) | uImage none
# endef
#
# And finally add it to the target device. E.g.
#
# define Device/linksys_lgs310c
#   $(Device/uimage-xz-loader)
#   ...
# endef

CC		:= $(CROSS_COMPILE)gcc
LD		:= $(CROSS_COMPILE)ld
OBJCOPY		:= $(CROSS_COMPILE)objcopy
OBJDUMP		:= $(CROSS_COMPILE)objdump

CFLAGS		= -fpic -mabicalls -O2 -DXZ_USE_CRC64 \
		  -DPRINTF_SUPPORT_DECIMAL_SPECIFIERS=0 \
		  -DPRINTF_SUPPORT_EXPONENTIAL_SPECIFIERS=0 \
		  -DPRINTF_SUPPORT_LONG_LONG=0 \
		  -DPRINTF_ALIAS_STANDARD_FUNCTION_NAMES_SOFT=1 \
		  -Dputchar_=board_putchar \
		  -fno-builtin-printf \
		  -Ilib/printf -Ilib/xz-embedded -Iinclude -Ilib

ASFLAGS		= -fpic -msoft-float -Iinclude

LDFLAGS		= -static -nostdlib -T linker/linker.ld --no-warn-mismatch

O_FORMAT 	= $(shell $(OBJDUMP) -i | head -2 | grep elf32)

SOURCES		= src/startup.S src/main.c src/board.c src/memory.c \
		  lib/printf/printf.c \
		  lib/xz-embedded/xz_crc32.c lib/xz-embedded/xz_crc64.c \
		  lib/xz-embedded/xz_dec_lzma2.c lib/xz-embedded/xz_dec_stream.c

BUILD_DIR	?= build

IMAGE_OBJ	:= $(BUILD_DIR)/image.o
IMAGE_ELF     	:= $(BUILD_DIR)/image.elf

XZ_OUT_IMAGE   	?= $(BUILD_DIR)/image.bin

OBJECTS_C	= $(filter %.c,$(SOURCES))
OBJECTS_S	= $(filter %.S,$(SOURCES))

OBJECTS		:= $(OBJECTS_S:.S=.o) $(OBJECTS_C:.c=.o)
OBJECTS		:= $(patsubst %.o, $(BUILD_DIR)/%.o, $(OBJECTS)) $(IMAGE_OBJ)

ifneq ($(MAKECMDGOALS),clean)
ifndef XZ_IN_IMAGE
$(error Compressed kernel image not given via XZ_IN_IMAGE)
endif
endif

all: $(XZ_OUT_IMAGE)

install:

$(BUILD_DIR)/%.o : %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR)/%.o : %.S
	@mkdir -p $(dir $@)
	$(CC) $(ASFLAGS) -c -o $@ $<

$(IMAGE_OBJ): $(XZ_IN_IMAGE)
	$(OBJCOPY) -I binary -O $(O_FORMAT) --rename-section .data=.kernel $< $@

$(IMAGE_ELF): $(OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $(OBJECTS)

$(XZ_OUT_IMAGE): $(IMAGE_ELF)
	$(OBJCOPY) -O binary $< $@

clean:
	rm -rf $(BUILD_DIR)/

