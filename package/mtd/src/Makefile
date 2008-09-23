CC = gcc
CFLAGS += -Wall

obj = mtd.o jffs2.o crc32.o
obj.brcm = trx.o
obj.brcm47xx = $(obj.brcm)

mtd: $(obj) $(obj.$(TARGET))
clean:
	rm -f *.o jffs2 
