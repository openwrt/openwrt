CGI_SUPPORT ?= 1
LUA_SUPPORT ?= 1
TLS_SUPPORT ?= 1

CFLAGS ?= -I./lua-5.1.4/src -I./cyassl-1.4.0/include -O0 -ggdb3
LDFLAGS ?= -L./lua-5.1.4/src -L./cyassl-1.4.0/src/.libs

CFLAGS += -Wall --std=gnu99

OBJ = uhttpd.o uhttpd-file.o uhttpd-utils.o
LIB = -Wl,--export-dynamic -lcrypt -ldl

TLSLIB =
LUALIB =


world: compile

ifeq ($(CGI_SUPPORT),1)
  OBJ += uhttpd-cgi.o
  CFLAGS += -DHAVE_CGI
endif

ifeq ($(LUA_SUPPORT),1)
  CFLAGS += -DHAVE_LUA
  LUALIB = uhttpd_lua.so

  $(LUALIB): uhttpd-lua.c
		$(CC) $(CFLAGS) $(LDFLAGS) $(FPIC) \
			-shared -lm -llua -ldl \
			-o $(LUALIB) uhttpd-lua.c
endif

ifeq ($(TLS_SUPPORT),1)
  CFLAGS += -DHAVE_TLS
  TLSLIB = uhttpd_tls.so

  $(TLSLIB): uhttpd-tls.c
		$(CC) $(CFLAGS) $(LDFLAGS) $(FPIC) \
			-shared -lcyassl \
			-o $(TLSLIB) uhttpd-tls.c
endif

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

compile: $(OBJ) $(TLSLIB) $(LUALIB)
	$(CC) -o uhttpd $(LDFLAGS) $(LIB) $(OBJ)

clean:
	rm -f *.o *.so uhttpd

