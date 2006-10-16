openwrt.pdf: Makefile openwrt.tex config.tex network.tex network-scripts.tex network-scripts.tex wireless.tex build.tex
	$(MAKE) cleanup
	pdflatex openwrt.tex
	pdflatex openwrt.tex
	$(MAKE) cleanup
	
clean: cleanup
	rm -f openwrt.pdf

cleanup: FORCE
	rm -f *.log *.aux *.toc *.out


FORCE:
.PHONY: FORCE
