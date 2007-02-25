ifeq ($(TOPDIR),)
  TOPDIR:=${CURDIR}/..
endif

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/prereq.mk

MAIN = openwrt.tex
DEPS = $(MAIN) Makefile config.tex network.tex network-scripts.tex network-scripts.tex wireless.tex build.tex adding.tex bugs.tex $(TMP_DIR)/.prereq-docs

all:
	$(NO_TRACE_MAKE) cleanup
	latex $(MAIN)
	$(NO_TRACE_MAKE) openwrt.pdf openwrt.html
	$(NO_TRACE_MAKE) cleanup

$(TMP_DIR)/.prereq-docs:
	$(NO_TRACE_MAKE) prereq
	touch $@

openwrt.html: $(DEPS)
	htlatex $(MAIN)

openwrt.pdf: $(DEPS)
	pdflatex $(MAIN)
	
clean: cleanup
	rm -f openwrt.pdf openwrt.html openwrt.css

cleanup: FORCE
	rm -f *.log *.aux *.toc *.out *.lg *.dvi *.idv *.4ct *.4tc *.xref *.tmp *.dvi

$(eval $(call RequireCommand,pdflatex, \
	You need to install LaTeX to build the OpenWrt documentation \
))
$(eval $(call RequireCommand,htlatex, \
	You need to install tex4ht to build the OpenWrt documentation \
))

FORCE:
.PHONY: FORCE
