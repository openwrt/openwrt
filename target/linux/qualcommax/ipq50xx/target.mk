SUBTARGET:=ipq50xx
BOARDNAME:=Qualcomm Atheros IPQ50xx
DEFAULT_PACKAGES += ath11k-firmware-ipq5018 ath11k-firmware-qcn6122

define Target/Description
	Build firmware images for Qualcomm Atheros IPQ50xx based boards.
endef
