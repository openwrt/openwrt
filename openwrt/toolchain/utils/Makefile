include $(TOPDIR)/rules.mk

TARGETS:=sstrip
UTILS_BUILD_DIR:=$(TOOL_BUILD_DIR)/utils

OS:=$(shell uname)
ifeq ($(OS),Darwin)
CFLAGS += -I./include
endif

prepare: $(UTILS_BUILD_DIR)
compile: prepare $(patsubst %,$(UTILS_BUILD_DIR)/%,$(TARGETS))
install: compile
	mkdir -p $(STAGING_DIR)/bin
	cp -a $(UTILS_BUILD_DIR)/* $(STAGING_DIR)/bin/
package:
clean:
	rm -rf $(UTILS_BUILD_DIR)

$(UTILS_BUILD_DIR):
	mkdir -p $(UTILS_BUILD_DIR)
	
$(UTILS_BUILD_DIR)/%: src/%.c
	$(CC) $(CFLAGS) -o $@ $(patsubst $(UTILS_BUILD_DIR)/%,src/%.c,$@)
	chmod 755 $@

