
ifeq ($(CONFIG_SIFLOWER_DPNS_TMU_DEBUGFS),y)
    dpnsTmu-y += sf_tmu_dbg.o
endif

ifeq ($(CONFIG_SIFLOWER_DPNS_TMU_GENL),y)
    dpnsTmu-y += sf_tmu_genl.o
endif

dpnsTmu-y += sf_tmu_core.o

obj-$(CONFIG_SIFLOWER_DPNS_TMU) += dpnsTmu.o
