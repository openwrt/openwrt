CC = gcc
CFLAGS += -Wall

obj = mtd.o jffs2.o crc32.o
obj.ar71xx = trx.o
obj.brcm = trx.o
obj.brcm47xx = $(obj.brcm)
obj.brcm63xx = imagetag.o

ifdef FIS_SUPPORT
  obj += fis.o
endif

mtd: $(obj) $(obj.$(TARGET))
clean:
	rm -f *.o jffs2 
