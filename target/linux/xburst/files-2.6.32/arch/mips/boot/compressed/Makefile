#
# linux/arch/mips/boot/compressed/Makefile
#
# create a compressed zImage from the original vmlinux
#

targets		:= zImage vmlinuz vmlinux.bin.gz head.o misc.o piggy.o dummy.o

OBJS 		:= $(obj)/head.o $(obj)/misc.o

LD_ARGS 	:= -T $(obj)/ld.script -Ttext 0x80600000 -Bstatic
OBJCOPY_ARGS 	:= -O elf32-tradlittlemips

ENTRY 		:= $(obj)/../tools/entry
FILESIZE 	:= $(obj)/../tools/filesize

drop-sections	= .reginfo .mdebug .comment .note .pdr .options .MIPS.options
strip-flags	= $(addprefix --remove-section=,$(drop-sections))


$(obj)/vmlinux.bin.gz: vmlinux
	rm -f $(obj)/vmlinux.bin.gz
	$(OBJCOPY) -O binary $(strip-flags) vmlinux $(obj)/vmlinux.bin
	gzip -v9f $(obj)/vmlinux.bin

$(obj)/head.o: $(obj)/head.S $(obj)/vmlinux.bin.gz vmlinux
	$(CC) $(KBUILD_AFLAGS) \
	-DIMAGESIZE=$(shell sh $(FILESIZE) $(obj)/vmlinux.bin.gz) \
	-DKERNEL_ENTRY=$(shell sh $(ENTRY) $(NM) vmlinux ) \
	-DLOADADDR=$(loadaddr) \
	-c -o $(obj)/head.o $<

$(obj)/vmlinuz: $(OBJS) $(obj)/ld.script $(obj)/vmlinux.bin.gz $(obj)/dummy.o
	$(OBJCOPY) \
		--add-section=.image=$(obj)/vmlinux.bin.gz \
		--set-section-flags=.image=contents,alloc,load,readonly,data \
		$(obj)/dummy.o $(obj)/piggy.o
	$(LD) $(LD_ARGS) -o $@ $(OBJS) $(obj)/piggy.o
	$(OBJCOPY) $(OBJCOPY_ARGS) $@ $@ -R .comment -R .stab -R .stabstr -R .initrd -R .sysmap

zImage: $(obj)/vmlinuz
	$(OBJCOPY) -O binary $(obj)/vmlinuz $(obj)/zImage	
