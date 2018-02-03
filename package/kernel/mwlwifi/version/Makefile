#
# package/kernel/mwlwifi/version/Makefile
#
# The main purpose of this Makefile is to update mwlwifi package's PKG_VERSION,
# according to the new PKG_SOURCE_VERSION, to a version that is consistent and
# able to reflect the actual upstream version. Moreover, PKG_MIRROR_HASH is
# also updated during the same process. Please see README.md for details.

LOCAL_DIR:=$(abspath .)
OPENWRT_DIR:=$(abspath ../../../..)
DL_DIR:=$(OPENWRT_DIR)/dl
PKG_DIR:=$(OPENWRT_DIR)/package/kernel/mwlwifi
PKG_MK:=$(PKG_DIR)/Makefile
PKG_VER_FORMAT:="%s-%s-g%.7s"
PKG_VER_PARAMS:="$${drv_ver}" "$${num_commits}" "$${src_rev}"
REPO_OUTDATE:=1
REPO_NAME:=upstream_repo
STAMP_NAMES:=edit fetch
ALL_STAMPS:=$(addsuffix -stamp,$(STAMP_NAMES))
PKG_SOURCE_VARS:=$(addprefix PKG_SOURCE_,URL PROTO VERSION)

define set-var
$(1):=$(2)
endef

$(foreach name,$(STAMP_NAMES),\
  $(eval $(call set-var,$(shell echo $(name) | tr 'a-z' 'A-Z')_STAMP,$(name)-stamp)))

$(foreach var,$(PKG_SOURCE_VARS),\
  $(eval $(call set-var,$(var),$(shell sed -rne 's/^ *$(var) *:= *//p' $(PKG_MK)))))

ifeq ($(strip $(REV)),)
  override REV:=$(PKG_SOURCE_VERSION)
endif
ifeq ($(strip $(REV)),)
  $(error Missing REV / PKG_SOURCE_VERSION)
endif
ifneq ($(strip $(PKG_SOURCE_PROTO)),git)
  $(error Cannot handle PKG_SOURCE_PROTO other than git)
endif
ifeq ($(strip $(PKG_SOURCE_URL)),)
  $(error Missing PKG_SOURCE_URL)
endif

EDIT_PREQ:=

ifneq ($(shell echo -n $(REV) | wc -c),40)
  REEDIT?=1
endif
ifneq ($(strip $(REV)),$(strip $(PKG_SOURCE_VERSION)))
  REEDIT?=1
endif
ifdef REEDIT
  EDIT_PREQ+=clean-edit-stamp
endif

ifdef FORCE_FETCH
  EDIT_PREQ+=clean-fetch-stamp
  override NO_FETCH:=
else
  ifeq ($(shell find -name $(FETCH_STAMP) -mtime -$(REPO_OUTDATE)),./$(FETCH_STAMP))
    NO_FETCH?=1
  else
    EDIT_PREQ+=clean-fetch-stamp
  endif
endif
ifeq ($(NO_FETCH),)
  EDIT_PREQ+=$(FETCH_STAMP)
endif

EDIT_PREQ+=$(PKG_MK)

all: $(EDIT_STAMP)

$(REPO_NAME):
ifneq ($(strip $(REPO_PATH)),)
	ln -s "$(REPO_PATH)" $@
else
	git clone $(PKG_SOURCE_URL) $@
endif

$(FETCH_STAMP): | $(REPO_NAME)
	cd $| && git fetch --all
	touch $@

sed-set='s|^( *$(1) *:= *).*|\1'"$(2)"'|'

$(EDIT_STAMP): $(EDIT_PREQ) | $(REPO_NAME)
	cd $| && ( \
	src_rev=`git log -1 --pretty=oneline $(REV)` && \
	src_rev=$${src_rev%% *} && \
	[ "$${src_rev}" ] && \
	drv_ver=`git show $${src_rev}:hif/pcie/dev.h | sed -rne 's/^#define PCIE_DRV_VERSION\s*"([^"]*)".*/\1/p'` && \
	[ "$${drv_ver}" ] && \
	since_rev=`git blame -L'/^#define PCIE_DRV_VERSION/',+1 $${src_rev} -- hif/pcie/dev.h` && \
	since_rev=$${since_rev%% *} && \
	[ "$${since_rev}" ] && \
	num_commits=`git log --oneline $${since_rev}..$${src_rev} | wc -l` && \
	pkg_ver=`printf $(PKG_VER_FORMAT) $(PKG_VER_PARAMS)` && \
	cd .. && \
	sed -i -r \
	  -e $(call sed-set,PKG_VERSION,$${pkg_ver}) \
	  -e $(call sed-set,PKG_SOURCE_VERSION,$${src_rev}) \
	  -e $(call sed-set,PKG_MIRROR_HASH,) \
	  $(PKG_MK) && \
	if [ -z "$(NO_REUSE_REPO)" ]; then \
	  sed -i -re $(call sed-set,PKG_SOURCE_URL,$(LOCAL_DIR)/$(REPO_NAME)) $(PKG_MK); \
	fi && \
	make -C $(OPENWRT_DIR) package/mwlwifi/download V=s && \
	sed -i -re $(call sed-set,PKG_SOURCE_URL,$(PKG_SOURCE_URL)) $(PKG_MK) && \
	[ -f $(DL_DIR)/mwlwifi-$${pkg_ver}.tar.xz ] && \
	sum=`sha256sum $(DL_DIR)/mwlwifi-$${pkg_ver}.tar.xz | cut -f1 -d' '` && \
	sed -i -re $(call sed-set,PKG_MIRROR_HASH,$${sum}) $(PKG_MK); \
	)
	touch $@

define default-clean-file
.PHONY: clean-$(1)
clean-$(1):
	rm -f $(1)
endef

$(foreach stamp,$(ALL_STAMPS),$(eval $(call default-clean-file,$(stamp))))

.PHONY: clean clean-all

clean: $(addprefix clean-,$(ALL_STAMPS))

clean-all: clean
	if [ -h $(REPO_NAME) ]; then \
		rm -f $(REPO_NAME); \
	elif [ -d $(REPO_NAME) ]; then \
		rm -rf $(REPO_NAME); \
	fi
