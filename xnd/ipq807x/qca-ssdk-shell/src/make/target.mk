
include $(PRJ_PATH)/make/$(OS)_opt.mk

include $(PRJ_PATH)/make/tools.mk

obj: $(OBJ_LIST)
	$(OBJ_LOOP)

dep: build_dir $(DEP_LIST)
	$(DEP_LOOP)

$(OBJ_LIST): %.o : %.c %.d
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) -c $< -o $(DST_DIR)/$@

$(DEP_LIST) : %.d : %.c
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) -MM $< > $(DST_DIR)/$@.tmp
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $(DST_DIR)/$@.tmp > $(DST_DIR)/$@
	$(RM) -f $(DST_DIR)/$@.tmp;

build_dir: $(DST_DIR)

$(DST_DIR):
	$(MKDIR) -p $(DST_DIR)

.PHONY: clean
clean: clean_o clean_d
	$(CLEAN_LOOP)

.PHONY: clean_o
clean_o: clean_obj
	$(CLEAN_OBJ_LOOP)

.PHONY: clean_d
clean_d: clean_dep
	$(CLEAN_DEP_LOOP)

clean_obj:
ifneq (,$(word 1, $(OBJ_FILE)))
	$(RM) -f $(OBJ_FILE)
endif

clean_dep:
ifneq (,$(word 1, $(DEP_FILE)))
	$(RM) -f $(DEP_FILE)
endif

ifneq (,$(word 1, $(DEP_FILE)))
  sinclude $(DEP_FILE)
endif	
