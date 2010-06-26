# for SGlinux builds
-include $(ROOTDIR)/modules/.config

obj-$(CONFIG_OCF_KIRKWOOD) += mv_cesa.o

mv_cesa-y := cesa/mvCesa.o cesa/mvLru.o cesa/mvMD5.o cesa/mvSHA1.o cesa/AES/mvAesAlg.o cesa/AES/mvAesApi.o cesa/mvCesaDebug.o cesa_ocf_drv.o

# Extra objects required by the CESA driver
mv_cesa-y += mvHal/kw_family/ctrlEnv/mvCtrlEnvLib.o mvHal/kw_family/boardEnv/mvBoardEnvLib.o mvHal/mv_hal/twsi/mvTwsi.o mvHal/kw_family/ctrlEnv/sys/mvCpuIf.o mvHal/kw_family/ctrlEnv/sys/mvAhbToMbus.o mvHal/kw_family/ctrlEnv/sys/mvSysDram.o mvHal/linux_oss/mvOs.o mvHal/kw_family/ctrlEnv/mvCtrlEnvAddrDec.o mvHal/mv_hal/gpp/mvGpp.o mvHal/kw_family/ctrlEnv/sys/mvSysPex.o mvHal/mv_hal/pex/mvPex.o mvHal/kw_family/boardEnv/mvBoardEnvSpec.o mvHal/common/mvCommon.o mvHal/common/mvDebug.o mvHal/kw_family/ctrlEnv/sys/mvSysCesa.o

ifdef src
EXTRA_CFLAGS += -I$(src)/.. -I$(src)/cesa -I$(src)/mvHal -I$(src)/mvHal/common -I$(src)/mvHal/kw_family -I$(src)/mvHal/mv_hal -I$(src)/mvHal/linux_oss -I$(src)
endif

EXTRA_CFLAGS += -DMV_LINUX -DMV_CPU_LE -DMV_ARM -DMV_INCLUDE_CESA -DMV_INCLUDE_PEX -DMV_CACHE_COHERENCY=3
ifdef TOPDIR
-include $(TOPDIR)/Rules.make
endif

