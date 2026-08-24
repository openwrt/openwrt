SUBTARGET:=ipq60xx
BOARDNAME:=Qualcomm Atheros IPQ60xx
DEFAULT_PACKAGES += ath11k-firmware-ipq6018 \
	tc-tiny kmod-sched-flower kmod-sched-prio \
	kmod-sched-act-police kmod-sched-act-pedit \
	kmod-sched-mqprio kmod-sched-ets kmod-sched-act-vlan dcb \
	ethtool-full devlink

define Target/Description
	Build firmware images for Qualcomm Atheros IPQ60xx based boards.
endef
