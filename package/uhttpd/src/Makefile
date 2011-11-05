CGI_SUPPORT ?= 1
LUA_SUPPORT ?= 1
TLS_SUPPORT ?= 1
UHTTPD_TLS ?= cyassl

CFLAGS ?= -I./lua-5.1.4/src $(TLS_CFLAGS) -O0 -ggdb3
LDFLAGS ?= -L./lua-5.1.4/src

CFLAGS += -Wall --std=gnu99

ifeq ($(UHTTPD_TLS),openssl)
  TLS_LDFLAGS ?= -L./openssl-0.9.8m -lssl
  TLS_CFLAGS ?= -I./openssl-0.9.8m/include -DTLS_IS_OPENSSL
else
  TLS_LDFLAGS ?= -L./cyassl-1.4.0/src/.libs -lcyassl
  TLS_CFLAGS ?= -I./cyassl-1.4.0/include -DTLS_IS_CYASSL
endif

OBJ := uhttpd.o uhttpd-file.o uhttpd-utils.o
LIB := -Wl,--export-dynamic -lcrypt -ldl

TLSLIB :=
LUALIB :=

HAVE_SHADOW=$(shell echo 'int main(void){ return !getspnam("root"); }' | \
	$(CC) -include shadow.h -xc -o/dev/null - 2>/dev/null && echo yes)

ifeq ($(HAVE_SHADOW),yes)
  CFLAGS += -DHAVE_SHADOW
endif

ifeq ($(TLS_SUPPORT),1)
  CFLAGS += -DHAVE_TLS
endif

ifeq ($(CGI_SUPPORT),1)
  CFLAGS += -DHAVE_CGI
endif

ifeq ($(LUA_SUPPORT),1)
  CFLAGS += -DHAVE_LUA
endif


world: compile

ifeq ($(CGI_SUPPORT),1)
  OBJ += uhttpd-cgi.o
endif

ifeq ($(LUA_SUPPORT),1)
  LUALIB := uhttpd_lua.so

  $(LUALIB): uhttpd-lua.c
		$(CC) $(CFLAGS) $(LDFLAGS) $(FPIC) \
			-shared -lm -llua -ldl \
			-o $(LUALIB) uhttpd-lua.c
endif

ifeq ($(TLS_SUPPORT),1)
  TLSLIB := uhttpd_tls.so

  $(TLSLIB): uhttpd-tls.c
		$(CC) $(CFLAGS) $(LDFLAGS) $(FPIC) \
			-shared $(TLS_LDFLAGS) \
			-o $(TLSLIB) uhttpd-tls.c
endif

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

compile: $(OBJ) $(TLSLIB) $(LUALIB)
	$(CC) -o uhttpd $(LDFLAGS) $(LIB) $(OBJ)

clean:
	rm -f *.o *.so uhttpd
