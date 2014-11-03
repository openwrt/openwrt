LTQ_TAPI_BUILD_DEPENDS:=TARGET_lantiq_xway:kmod-ltq-tapi \
	TARGET_lantiq_xway:kmod-ltq-vmmc \
	TARGET_lantiq_xrx200:kmod-ltq-tapi \
	TARGET_lantiq_xrx200:kmod-ltq-vmmc \
	TARGET_lantiq_falcon:kmod-ltq-tapi \
	TARGET_lantiq_falcon:kmod-ltq-vmmc

LTQ_TAPI_TARGET:=@(TARGET_lantiq_falcon||TARGET_lantiq_xway||TARGET_lantiq_xrx200)

LTQ_TAPI_DEPENDS:=$(LTQ_TAPI_TARGET) \
	+TARGET_lantiq_xway:kmod-ltq-tapi \
	+TARGET_lantiq_xway:kmod-ltq-vmmc \
	+TARGET_lantiq_xrx200:kmod-ltq-tapi \
	+TARGET_lantiq_xrx200:kmod-ltq-vmmc \
	+TARGET_lantiq_falcon:kmod-ltq-tapi \
	+TARGET_lantiq_falcon:kmod-ltq-vmmc
