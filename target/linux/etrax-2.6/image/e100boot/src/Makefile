#
# Top Makefile for e100boot
# $Id: Makefile,v 1.20 2003/06/04 12:22:23 pkj Exp $
#

# Change these paths if necessary. Can also be specified on cmdline as
# 'make INSTALL_PATH=/foo/bar/'.

INSTALL_NAME     = e100boot
INSTALL_PATH     = /usr/local
INSTALL_PATH_BIN = $(INSTALL_PATH)/bin
INSTALL_PATH_DOC = $(INSTALL_PATH)/man/man1

DIRS = libpcap-0.4 sbl doc

-include $(AXIS_TOP_DIR)/tools/build/Rules.axis
ifdef prefix
INSTALL_PATH = $(prefix)
endif

INSTALL ?= install

all: conf $(DIRS)
	@for d in $(DIRS); do \
	  echo -e "\n### Making $$d"; \
	  $(MAKE) -C $$d || exit; \
	done

conf: $(DIRS)
	@for d in $(DIRS); do \
	  if [ -x $$d/configure ] && [ ! -e $$d/Makefile ]; then \
	    echo -e "\n### Configuring $$d"; \
	    cd $$d; ./configure || exit; cd ..; \
          fi; \
	done

tar: clean
	@echo -e "\n### Making tarball."
	tar -C ../ -zcf e100boot.tgz --exclude e100boot.tgz --exclude RCS --exclude CVS e100boot


install: all
	$(INSTALL) -d $(INSTALL_PATH_BIN) $(INSTALL_PATH_DOC)
	$(INSTALL) sbl/e100boot.stripped $(INSTALL_PATH_BIN)/$(INSTALL_NAME)
	$(INSTALL) -m 0644 doc/e100boot.1 $(INSTALL_PATH_DOC)/$(INSTALL_NAME).1

install.setuid:
	@echo -e "\n### Make install.setuid"; \
	if ! [ -e $(INSTALL_PATH_BIN)/$(INSTALL_NAME) ]; then \
	  echo -e "\n### Binary $(INSTALL_PATH_BIN)/$(INSTALL_NAME) does not exist! Make install first."; \
	elif [ `id -u` = 0 ]; then \
	  chown root.root $(INSTALL_PATH_BIN)/$(INSTALL_NAME); \
	  chmod +s $(INSTALL_PATH_BIN)/$(INSTALL_NAME); \
	else \
	  echo "### You must do this as root!"; \
	fi

clean:
	@for d in $(DIRS); do \
	if [ -e $$d/Makefile ]; then \
	  echo -e "\n### Cleaning $$d"; \
	  $(MAKE) -C $$d clean || exit; \
	fi; \
	done

configsubs:	conf configsubs-dirs

configsubs-dirs:
	$(MAKE) -C libpcap-0.4 configsubs
	$(MAKE) -C sbl configsubs
