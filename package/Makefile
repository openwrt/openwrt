# Main makefile for the packages
include $(TOPDIR)/rules.mk
COMPILE_PACKAGES:=$(patsubst %,%-compile,$(package-y) $(package-m))
INSTALL_PACKAGES:=$(patsubst %,%-install,$(package-y))

all: compile
clean: $(patsubst %,%-clean,$(package-) $(package-y) $(package-m))
compile: $(COMPILE_PACKAGES)
install: base-files-install $(INSTALL_PACKAGES)

$(COMPILE_PACKAGES): base-files-compile
$(INSTALL_PACKAGES): base-files-install

$(STAMP_DIR):
	mkdir -p $@

$(TARGET_DIR):
	mkdir -p $(TARGET_DIR)

%-prepare: $(STAMP_DIR) $(TARGET_DIR)
	@[ -f $(STAMP_DIR)/.$@ ] || { \
		$(START_TRACE) "package/$(patsubst %-prepare,%,$@)-prepare: "; \
		$(MAKE) -C $(patsubst %-prepare,%,$@) prepare && { \
			touch $(STAMP_DIR)/.$@; \
			$(CMD_TRACE) " done"; \
			$(END_TRACE); \
		} \
	}

%-compile:
	@[ -f $(STAMP_DIR)/.$@ ] || { \
		$(START_TRACE) "package/$(patsubst %-compile,%,$@)-compile: "; \
		$(MAKE) -C $(patsubst %-compile,%,$@) compile && { \
			touch $(STAMP_DIR)/.$(patsubst %-compile,%,$@)-prepare; \
			touch $(STAMP_DIR)/.$@; \
			$(CMD_TRACE) " done"; \
			$(END_TRACE); \
		} \
	}

%-install: %-compile
	@$(START_TRACE) "package/$(patsubst %-install,%,$@)-install: "
	@$(MAKE) -C $(patsubst %-install,%,$@) install
	@$(CMD_TRACE) " done"
	@$(END_TRACE)

%-rebuild: 
	@$(START_TRACE) "package/$(patsubst %-rebuild,%,$@)-rebuild: "
	@rm -f $(STAMP_DIR)/.$(patsubst %-rebuild,%,$@)-*
	$(MAKE) -C $(patsubst %-rebuild,%,$@) rebuild
	@$(CMD_TRACE) " done"
	@$(END_TRACE)

%-clean:
	@$(START_TRACE) "package/$(patsubst %-clean,%,$@)-clean: "
	@$(MAKE) -C $(patsubst %-clean,%,$@) clean
	@rm -f $(STAMP_DIR)/.$(patsubst %-clean,%,$@)-*
	@$(CMD_TRACE) " done"
	@$(END_TRACE)

