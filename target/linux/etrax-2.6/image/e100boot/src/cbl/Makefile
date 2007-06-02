#
# Makefile to generate .ima files for e100boot
#
# Hacked by ronny, rehacked by ronny
#
# $Id: Makefile,v 1.26 2003/03/13 14:18:31 cii Exp $
#

DIRS	:= net net_noleds ser ser_noleds

all:	$(DIRS)
	@for d in $(DIRS); do \
	  echo -e "\n### Making $$d"; \
	  make -C $$d || exit; \
	done
	@echo "***************************************************************"
	@./free_size
	@echo "***************************************************************"

clean:
	rm -f *.ima
	@for d in $(DIRS); do \
	  echo -e "\n### Cleaning $$d"; \
	  make -C $$d clean || exit; \
	done;
