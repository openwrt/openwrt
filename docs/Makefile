MAIN = openwrt.tex
DEPS = $(MAIN) Makefile config.tex network.tex network-scripts.tex network-scripts.tex wireless.tex build.tex adding.tex bugs.tex

all:
	$(MAKE) cleanup
	latex $(MAIN)
	$(MAKE) openwrt.pdf openwrt.html
	$(MAKE) cleanup

openwrt.html: $(DEPS)
	htlatex $(MAIN)

openwrt.pdf: $(DEPS)
	pdflatex $(MAIN)
	
clean: cleanup
	rm -f openwrt.pdf openwrt.html openwrt.css

cleanup: FORCE
	rm -f *.log *.aux *.toc *.out *.lg *.dvi *.idv *.4ct *.4tc *.xref *.tmp *.dvi


FORCE:
.PHONY: FORCE
