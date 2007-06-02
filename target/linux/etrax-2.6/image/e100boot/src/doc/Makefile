OBJS=e100boot.1 e100boot.html

all: $(OBJS)

%.1: %.pod
	pod2man $< > $@

%.html: %.pod
	pod2html $< > $@
	@rm -f pod2htmd.x~~ pod2htmi.x~~

clean: 
	rm -f *cache *~

# The 'clean' doesn't remove the generated documentation as we want
# them in the distribution.

