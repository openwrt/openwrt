include $(LINUX_DIR)/.config

# translate linux kernel config to filenames
ext-$(CONFIG_IP_NF_NAT) += SNAT DNAT
ext-$(CONFIG_IP_NF_MATCH_LIMIT) += limit
ext-$(CONFIG_IP_NF_MATCH_MAC) += mac
ext-$(CONFIG_IP_NF_MATCH_PKTTYPE) += pkttype
ext-$(CONFIG_IP_NF_MATCH_MARK) += mark
ext-$(CONFIG_IP_NF_MATCH_MULTIPORT) += multiport
ext-$(CONFIG_IP_NF_MATCH_TOS) += tos
ext-$(CONFIG_IP_NF_MATCH_RECENT) += recent
ext-$(CONFIG_IP_NF_MATCH_ECN) += ecn
ext-$(CONFIG_IP_NF_MATCH_IPP2P) += ipp2p
ext-$(CONFIG_IP_NF_MATCH_DSCP) += dscp
ext-$(CONFIG_IP_NF_MATCH_AH_ESP) += ah esp
ext-$(CONFIG_IP_NF_MATCH_LENGTH) += length
ext-$(CONFIG_IP_NF_MATCH_TTL) += ttl
ext-$(CONFIG_IP_NF_MATCH_TCPMSS) += tcpmss
ext-$(CONFIG_IP_NF_MATCH_HELPER) += helper
#ext-$(CONFIG_IP_NF_MATCH_STATE) += state
#ext-$(CONFIG_IP_NF_MATCH_CONNTRACK) += conntrack
ext-$(CONFIG_IP_NF_MATCH_UNCLEAN) += unclean
ext-$(CONFIG_IP_NF_MATCH_OWNER) += owner
ext-$(CONFIG_IP_NF_MATCH_PHYSDEV) += physdev
ext-$(CONFIG_IP_NF_MATCH_LAYER7) += layer7
#ext-$(CONFIG_IP_NF_TARGET_MASQUERADE) += MASQUERADE
ext-$(CONFIG_IP_NF_TARGET_REDIRECT) += REDIRECT
ext-$(CONFIG_IP_NF_TARGET_REJECT) += REJECT
ext-$(CONFIG_IP_NF_TARGET_TOS) += TOS
ext-$(CONFIG_IP_NF_TARGET_ECN) += ECN
ext-$(CONFIG_IP_NF_TARGET_DSCP) += DSCP
ext-$(CONFIG_IP_NF_TARGET_MARK) += MARK
#ext-$(CONFIG_IP_NF_TARGET_LOG) += LOG
ext-$(CONFIG_IP_NF_TARGET_ULOG) += ULOG
#ext-$(CONFIG_IP_NF_TARGET_TCPMSS) += TCPMSS

# add extensions that don't depend on kernel config
ext-m += TTL
ext-y += icmp standard tcp udp state MASQUERADE conntrack TCPMSS LOG
