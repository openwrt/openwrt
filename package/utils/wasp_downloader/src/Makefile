.PHONY: all clean install dist

# Top directory for building complete system, fall back to this directory
ROOTDIR    ?= $(shell pwd)

VERSION = 2
NAME    = wasp_downloader
PKG     = $(NAME)-$(VERSION)
ARCHIVE = $(PKG).tar.xz

PREFIX ?= /usr/local/
CFLAGS ?= -Wall -Wextra -Werror
LDLIBS  = 

objs = wasp_downloader.o
hdrs = $(wildcard *.h)

%.o: %.c $(hdrs) Makefile
	@printf "  CC      $(subst $(ROOTDIR)/,,$(shell pwd)/$@)\n"
	@$(CC) $(CFLAGS) -c $< -o $@

all: wasp_downloader

wasp_downloader: $(objs)
	@printf "  CC      $(subst $(ROOTDIR)/,,$(shell pwd)/$@)\n"
	@$(CC) $(LDFLAGS) $(LDLIBS) -o $@ $^

clean:
	@rm -f *.o
	@rm -f $(TARGET)

dist:
	@echo "Creating $(ARCHIVE), with $(ARCHIVE).md5 in parent dir ..."
	@git archive --format=tar --prefix=$(PKG)/ v$(VERSION) | xz >../$(ARCHIVE)
	@(cd .. && sha256sum $(ARCHIVE) > $(ARCHIVE).sha256)

install: all
	@cp wasp_downloader $(DESTDIR)/$(PREFIX)/bin/
	@cp files/lib/preinit/90_wasp_downloader $(DESTDIR)/$(PREFIX)/lib/preinit/
