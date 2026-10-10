SUBTARGET:=ipq807x
BOARDNAME:=Qualcomm Atheros IPQ807x
DEFAULT_PACKAGES += ath11k-firmware-ipq8074 \
	tc-tiny kmod-sched-flower kmod-sched-prio \
	kmod-sched-act-police kmod-sched-act-pedit \
	kmod-sched-mqprio kmod-sched-ets kmod-sched-act-vlan dcb \
	kmod-sched-red kmod-sched-gred \
	ethtool-full devlink ppe-qos

define Target/Description
	Build firmware images for Qualcomm Atheros IPQ807x based boards.
endef
