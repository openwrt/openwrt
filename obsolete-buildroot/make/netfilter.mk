#############################################################
#
# netfilter
#
#############################################################

ifeq ($(strip $(USE_NETFILTER_SNAPSHOT)),)
USE_NETFILTER_SNAPSHOT=20040508
endif
NETFILTER_SOURCE:=patch-o-matic-$(USE_NETFILTER_SNAPSHOT).tar.bz2
NETFILTER_DIR:=$(BUILD_DIR)/patch-o-matic-$(USE_NETFILTER_SNAPSHOT)
NETFILTER_SITE:=ftp://ftp.netfilter.org/pub/patch-o-matic/snapshot
NETFILTER_CAT:=bzcat

# ipv6_mld breaks net/ipv6/mcast.c
NETFILTER_EXCLUDE:=--exclude submitted/89_ipv6_mld_netfilter.patch

NETFILTER_PATCHES:= \
	base \
	extra/CLASSIFY.patch \
	extra/CONNMARK.patch \
	extra/IPMARK.patch \
	extra/condition.patch \
	extra/h323-conntrack-nat.patch \
	extra/mms-conntrack-nat.patch \
	extra/pptp-conntrack-nat.patch \
	extra/string.patch

LINUX_DIR:=$(BUILD_DIR)/WRT54GS/release/src/linux/linux

$(DL_DIR)/$(NETFILTER_SOURCE):
	 echo $(WGET) -P $(DL_DIR) $(NETFILTER_SITE)/$(NETFILTER_SOURCE)

netfilter-source: $(DL_DIR)/$(NETFILTER_SOURCE)

$(NETFILTER_DIR)/.unpacked: $(DL_DIR)/$(NETFILTER_SOURCE)
	$(NETFILTER_CAT) $(DL_DIR)/$(NETFILTER_SOURCE) | tar -C $(BUILD_DIR) -xvf -
	#ignore whitespace in patches
	$(SED) "s,\-p1,\-l \-p1," $(NETFILTER_DIR)/runme
	touch $(NETFILTER_DIR)/.unpacked

$(LINUX_DIR)/.nf-patched: $(LINUX_DIR)/.patched $(NETFILTER_DIR)/.unpacked
	-(cd $(NETFILTER_DIR); KERNEL_DIR=$(LINUX_DIR) ./runme --batch $(NETFILTER_EXCLUDE) $(NETFILTER_PATCHES))
	touch $(LINUX_DIR)/.nf-patched

netfilter: $(LINUX_DIR)/.nf-patched

netfilter-clean:

netfilter-dirclean:
	rm -rf $(NETFILTER_DIR)
