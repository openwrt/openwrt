all: libipt_FULLCONENAT.so libip6t_FULLCONENAT.so

libipt_FULLCONENAT.so: libipt_FULLCONENAT.o
	$(CC) -shared -lxtables -o $@ $^;
libipt_FULLCONENAT.o: libipt_FULLCONENAT.c
	$(CC) ${CFLAGS} -fPIC -c -o $@ $<;
libip6t_FULLCONENAT.so: libip6t_FULLCONENAT.o
	$(CC) -shared -lxtables -o $@ $^;
libip6t_FULLCONENAT.o: libip6t_FULLCONENAT.c
	$(CC) ${CFLAGS} -fPIC -c -o $@ $<;

obj-m += xt_FULLCONENAT.o
