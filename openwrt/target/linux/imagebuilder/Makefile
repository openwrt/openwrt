default: clean all

TOPDIR:=${shell pwd}
export TOPDIR
IB:=1
export IB
include ./rules.mk

LINUX_BUILD_DIR:=$(BUILD_DIR)/linux-$(KERNEL)-$(BOARD)
IPKG_KERNEL:=IPKG_TMP=$(BUILD_DIR)/tmp IPKG_INSTROOT=$(LINUX_BUILD_DIR)/root IPKG_CONF_DIR=$(LINUX_BUILD_DIR) $(SCRIPT_DIR)/ipkg -force-defaults -force-depends

define image_template

all: all-$(1)-$(2)

all-$(1)-$(2):
	@for listfile in ./lists/*.$(1)-$(2); do \
		$(MAKE) -s `echo $$$$listfile | sed 's,.*lists/\(.*\).$(1)-$(2),BOARD=$(1) KERNEL=$(2) LIST=\1,'` build; \
	done
endef

include ./kernel.mk

clean:
	@rm -rf $(LINUX_BUILD_DIR)/root*
	@rm -rf $(BIN_DIR)
all:

build:
	echo
	echo '### BUILDING IMAGE FROM lists/$(LIST).$(BOARD)-$(KERNEL)'
	echo
	rm -rf $(LINUX_BUILD_DIR)/root*
	mkdir -p $(BIN_DIR)/$(LIST)
	mkdir -p $(LINUX_BUILD_DIR)/tmp
	mkdir -p $(LINUX_BUILD_DIR)/root
	echo -e 'dest root /\noption offline_root $(LINUX_BUILD_DIR)/root' > $(LINUX_BUILD_DIR)/ipkg.conf
	$(IPKG_KERNEL) install $(BUILD_DIR)/linux-$(KERNEL)-$(BOARD)/kernel[-_]*.ipk
	for package in `cat lists/$(LIST).$(BOARD)-$(KERNEL)`; do \
		$(IPKG_KERNEL) install `ls packages/$${package}_*`; \
	done
	$(MAKE) -S -C image KERNEL="$(KERNEL)" BOARD="$(BOARD)" BIN_DIR="$(BIN_DIR)/$(LIST)" install
