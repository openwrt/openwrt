include $(TOPDIR)/rules.mk

TARGETS := addpattern trx
ifeq ($(BR2_TARGET_BELKIN),y)
TARGETS += belkin-bin
endif
ifeq ($(BR2_TARGET_MOTOROLA),y)
TARGETS += motorola-bin
endif

UTILS_BUILD_DIR:=$(BUILD_DIR)/target-utils

prepare: $(UTILS_BUILD_DIR)
compile: prepare $(patsubst %,$(UTILS_BUILD_DIR)/%,$(TARGETS))
	mkdir -p $(STAGING_DIR)/bin
	cp -a $(UTILS_BUILD_DIR)/* $(STAGING_DIR)/bin/
install: compile
package:
clean:
	rm -rf $(UTILS_BUILD_DIR)

$(UTILS_BUILD_DIR):
	mkdir -p $(UTILS_BUILD_DIR)
	
$(UTILS_BUILD_DIR)/%: src/%.c
	$(CC) -o $@ $(patsubst $(UTILS_BUILD_DIR)/%,src/%.c,$@)
	chmod 755 $@

