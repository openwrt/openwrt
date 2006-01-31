LOADADDR = 0x80400000		# RAM start + 4M
KERNEL_ENTRY = 0x80001000
RAMSIZE = 0x00100000		# 1MB

CROSS_COMPILE = mips-linux-

.S.s:
	$(CPP) $(CFLAGS) $< -o $*.s
.S.o:
	$(CC) $(CFLAGS) -c $< -o $*.o
.c.o:
	$(CC) $(CFLAGS) -c $< -o $*.o

CC =       $(CROSS_COMPILE)gcc
LD =       $(CROSS_COMPILE)ld
OBJCOPY =  $(CROSS_COMPILE)objcopy
OBJDUMP =  $(CROSS_COMPILE)objdump

CFLAGS = -fno-builtin -Os -G 0 -mno-abicalls -fno-pic -Wall -DRAMSIZE=${RAMSIZE} -DKERNEL_ENTRY=${KERNEL_ENTRY} -D_LZMA_IN_CB
# CFLAGS = -fno-builtin -Os -G 0 -mno-abicalls -fno-pic -Wall -DRAMSIZE=${RAMSIZE}

O_FORMAT = $(shell $(OBJDUMP) -i | head -2 | grep elf32)

# Drop some uninteresting sections in the kernel.
# This is only relevant for ELF kernels but doesn't hurt a.out
drop-sections   = .reginfo .mdebug .comment
strip-flags     = $(addprefix --remove-section=,$(drop-sections))


all : lzma.elf

lzma.lds: lzma.lds.in
	sed -e 's,@LOADADDR@,$(LOADADDR),g' $< >$@

kernel.o: vmlinux.lzma lzma.lds
	$(LD) -r -b binary --oformat $(O_FORMAT) -o $@ $<

lzma.elf: start.o decompress.o LzmaDecode.o kernel.o
	$(LD) -s -Tlzma.lds -o $@ $^

clean:
	rm -f *.o lzma.elf
