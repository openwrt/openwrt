CC = gcc
CFLAGS += -Wall
LDFLAGS += -lubox

obj = mtd.o jffs2.o crc32.o md5.o
obj.seama = seama.o md5.o
obj.ar71xx = trx.o $(obj.seama)
obj.brcm = trx.o
obj.brcm47xx = $(obj.brcm)
obj.bcm53xx = $(obj.brcm)
obj.brcm63xx = imagetag.o
obj.ramips = $(obj.seama)

ifdef FIS_SUPPORT
  obj += fis.o
endif

mtd: $(obj) $(obj.$(TARGET))
clean:
	rm -f *.o jffs2
