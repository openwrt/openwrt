include $(TOPDIR)/rules.mk

TARGETS := addpattern trx motorola-bin dgfirmware

UTILS_BUILD_DIR:=$(BUILD_DIR)/target-utils

prepare: $(UTILS_BUILD_DIR)
compile: prepare $(patsubst %,$(UTILS_BUILD_DIR)/%,$(TARGETS))
	mkdir -p $(STAGING_DIR)/bin
	$(CP) $(UTILS_BUILD_DIR)/* $(STAGING_DIR)/bin/
install: compile
package:
clean:
	rm -rf $(UTILS_BUILD_DIR)
	for f in $(TARGETS); do \
		rm -f $(STAGING_DIR)/bin/$$f ; \
	done

$(UTILS_BUILD_DIR):
	mkdir -p $(UTILS_BUILD_DIR)
	
$(UTILS_BUILD_DIR)/%: src/%.c
	$(CC) -o $@ $(patsubst $(UTILS_BUILD_DIR)/%,src/%.c,$@)
	chmod 755 $@

