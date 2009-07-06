include ../Makefile.inc
CPPFLAGS += -I../kernel -I../user

wprobe-export: wprobe-export.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS)
