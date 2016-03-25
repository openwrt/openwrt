#
# Copyright (C) 2006-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

OPENWRT_GIT = http://git.openwrt.org

DOWNLOAD_RDEP=$(STAMP_PREPARED) $(HOST_STAMP_PREPARED)

# Try to guess the download method from the URL
define dl_method
$(strip \
  $(if $(2),$(2), \
    $(if $(filter @GNOME/% @GNU/% @KERNEL/% @SF/% @SAVANNAH/% ftp://% http://% https://% file://%,$(1)),default, \
      $(if $(filter git://%,$(1)),git, \
        $(if $(filter svn://%,$(1)),svn, \
          $(if $(filter cvs://%,$(1)),cvs, \
            $(if $(filter hg://%,$(1)),hg, \
              $(if $(filter sftp://%,$(1)),bzr, \
                unknown \
              ) \
            ) \
          ) \
        ) \
      ) \
    ) \
  ) \
)
endef

# code for creating tarballs from cvs/svn/git/bzr/hg/darcs checkouts - useful for mirror support
dl_pack/bz2=$(TAR) cjf $(1) $(2)
dl_pack/gz=$(TAR) czf $(1) $(2)
dl_pack/xz=$(TAR) c $(2) | xz -zc > $(1)
dl_pack/unknown=echo "ERROR: Unknown pack format for file $(1)"; false
define dl_pack
	$(if $(dl_pack/$(call ext,$(1))),$(dl_pack/$(call ext,$(1))),$(dl_pack/unknown))
endef

define DownloadMethod/unknown
	@echo "ERROR: No download method available"; false
endef

define DownloadMethod/default
	$(SCRIPT_DIR)/download.pl "$(DL_DIR)" "$(FILE)" "$(MD5SUM)" $(foreach url,$(URL),"$(url)")
endef

define wrap_mirror
	$(if $(if $(MIRROR),$(filter-out x,$(MIRROR_MD5SUM))),@$(SCRIPT_DIR)/download.pl "$(DL_DIR)" "$(FILE)" "$(MIRROR_MD5SUM)" || ( $(1) ),$(1))
endef

define DownloadMethod/cvs
	$(call wrap_mirror, \
		echo "Checking out files from the cvs repository..."; \
		mkdir -p $(TMP_DIR)/dl && \
		cd $(TMP_DIR)/dl && \
		rm -rf $(SUBDIR) && \
		[ \! -d $(SUBDIR) ] && \
		cvs -d $(URL) export $(VERSION) $(SUBDIR) && \
		echo "Packing checkout..." && \
		$(call dl_pack,$(TMP_DIR)/dl/$(FILE),$(SUBDIR)) && \
		mv $(TMP_DIR)/dl/$(FILE) $(DL_DIR)/ && \
		rm -rf $(SUBDIR); \
	)
endef

define DownloadMethod/svn
	$(call wrap_mirror, \
		echo "Checking out files from the svn repository..."; \
		mkdir -p $(TMP_DIR)/dl && \
		cd $(TMP_DIR)/dl && \
		rm -rf $(SUBDIR) && \
		[ \! -d $(SUBDIR) ] && \
		( svn help export | grep -q trust-server-cert && \
		svn export --non-interactive --trust-server-cert -r$(VERSION) $(URL) $(SUBDIR) || \
		svn export --non-interactive -r$(VERSION) $(URL) $(SUBDIR) ) && \
		echo "Packing checkout..." && \
		$(call dl_pack,$(TMP_DIR)/dl/$(FILE),$(SUBDIR)) && \
		mv $(TMP_DIR)/dl/$(FILE) $(DL_DIR)/ && \
		rm -rf $(SUBDIR); \
	)
endef

define DownloadMethod/git
	$(call wrap_mirror, \
		echo "Checking out files from the git repository..."; \
		mkdir -p $(TMP_DIR)/dl && \
		cd $(TMP_DIR)/dl && \
		rm -rf $(SUBDIR) && \
		[ \! -d $(SUBDIR) ] && \
		git clone $(URL) $(SUBDIR) --recursive && \
		(cd $(SUBDIR) && git checkout $(VERSION) && git submodule update) && \
		echo "Packing checkout..." && \
		rm -rf $(SUBDIR)/.git && \
		$(call dl_pack,$(TMP_DIR)/dl/$(FILE),$(SUBDIR)) && \
		mv $(TMP_DIR)/dl/$(FILE) $(DL_DIR)/ && \
		rm -rf $(SUBDIR); \
	)
endef

define DownloadMethod/bzr
	$(call wrap_mirror, \
		echo "Checking out files from the bzr repository..."; \
		mkdir -p $(TMP_DIR)/dl && \
		cd $(TMP_DIR)/dl && \
		rm -rf $(SUBDIR) && \
		[ \! -d $(SUBDIR) ] && \
		bzr export -r$(VERSION) $(SUBDIR) $(URL) && \
		echo "Packing checkout..." && \
		$(call dl_pack,$(TMP_DIR)/dl/$(FILE),$(SUBDIR)) && \
		mv $(TMP_DIR)/dl/$(FILE) $(DL_DIR)/ && \
		rm -rf $(SUBDIR); \
	)
endef

define DownloadMethod/hg
	$(call wrap_mirror, \
		echo "Checking out files from the hg repository..."; \
		mkdir -p $(TMP_DIR)/dl && \
		cd $(TMP_DIR)/dl && \
		rm -rf $(SUBDIR) && \
		[ \! -d $(SUBDIR) ] && \
		hg clone -r $(VERSION) $(URL) $(SUBDIR) && \
		find $(SUBDIR) -name .hg | xargs rm -rf && \
		echo "Packing checkout..." && \
		$(call dl_pack,$(TMP_DIR)/dl/$(FILE),$(SUBDIR)) && \
		mv $(TMP_DIR)/dl/$(FILE) $(DL_DIR)/ && \
		rm -rf $(SUBDIR); \
	)
endef

define DownloadMethod/darcs
	$(call wrap_mirror, \
		echo "Checking out files from the darcs repository..."; \
		mkdir -p $(TMP_DIR)/dl && \
		cd $(TMP_DIR)/dl && \
		rm -rf $(SUBDIR) && \
		[ \! -d $(SUBDIR) ] && \
		darcs get -t $(VERSION) $(URL) $(SUBDIR) && \
		find $(SUBDIR) -name _darcs | xargs rm -rf && \
		echo "Packing checkout..." && \
		$(call dl_pack,$(TMP_DIR)/dl/$(FILE),$(SUBDIR)) && \
		mv $(TMP_DIR)/dl/$(FILE) $(DL_DIR)/ && \
		rm -rf $(SUBDIR); \
	)
endef

Validate/cvs=VERSION SUBDIR
Validate/svn=VERSION SUBDIR
Validate/git=VERSION SUBDIR
Validate/bzr=VERSION SUBDIR
Validate/hg=VERSION SUBDIR
Validate/darcs=VERSION SUBDIR

define Download/Defaults
  URL:=
  FILE:=
  PROTO:=
  MD5SUM:=
  SUBDIR:=
  MIRROR:=1
  MIRROR_MD5SUM:=x
  VERSION:=
endef

define Download
  $(eval $(Download/Defaults))
  $(eval $(Download/$(1)))
  $(foreach FIELD,URL FILE $(Validate/$(call dl_method,$(URL),$(PROTO))),
    ifeq ($($(FIELD)),)
      $$(error Download/$(1) is missing the $(FIELD) field.)
    endif
  )

  $(foreach dep,$(DOWNLOAD_RDEP),
    $(dep): $(DL_DIR)/$(FILE)
  )
  download: $(DL_DIR)/$(FILE)

  $(DL_DIR)/$(FILE):
	mkdir -p $(DL_DIR)
	$(if $(DownloadMethod/$(call dl_method,$(URL),$(PROTO))),$(DownloadMethod/$(call dl_method,$(URL),$(PROTO))),$(DownloadMethod/unknown))

endef
