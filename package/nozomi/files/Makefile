
O_TARGET	:= noz.o

export-objs	:= nozomi.o kfifo.o

list-multi := noz.o
noz-objs := nozomi.o kfifo.o

obj-m := noz.o

include $(TOPDIR)/Rules.make

noz.o:	$(noz-objs)
	$(LD) -r -o $@ $(noz-objs) 
