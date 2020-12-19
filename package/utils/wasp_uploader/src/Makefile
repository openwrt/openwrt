.PHONY: all clean install dist

# Top directory for building complete system, fall back to this directory
ROOTDIR    ?= $(shell pwd)

VERSION = 2
NAME    = wasp_uploader
PKG     = $(NAME)-$(VERSION)
ARCHIVE = $(PKG).tar.xz

PREFIX ?= /usr/local/
CFLAGS ?= -Wall -Wextra -Werror
LDLIBS  = 

objs_stage1 = wasp_uploader_stage1.o
objs_stage2 = wasp_uploader_stage2.o
hdrs = $(wildcard *.h)

%.o: %.c $(hdrs) Makefile
	@printf "  CC      $(subst $(ROOTDIR)/,,$(shell pwd)/$@)\n"
	@$(CC) $(CFLAGS) -c $< -o $@

all: wasp_uploader_stage1 wasp_uploader_stage2

wasp_uploader_stage1: $(objs_stage1)
	@printf "  CC      $(subst $(ROOTDIR)/,,$(shell pwd)/$@)\n"
	@$(CC) $(LDFLAGS) $(LDLIBS) -o $@ $^

wasp_uploader_stage2: $(objs_stage2)
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
	@cp wasp_uploader_stage1 $(DESTDIR)/$(PREFIX)/bin/
	@cp wasp_uploader_stage2 $(DESTDIR)/$(PREFIX)/bin/
