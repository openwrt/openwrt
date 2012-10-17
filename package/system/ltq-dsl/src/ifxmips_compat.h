#ifndef _IFXMIPS_COMPAT_H__
#define _IFXMIPS_COMPAT_H__

#define	IFX_SUCCESS	0
#define IFX_ERROR	(-1)

#define ATM_VBR_NRT     ATM_VBR
#define ATM_VBR_RT      6
#define ATM_UBR_PLUS    7
#define ATM_GFR         8

#define NUM_ENTITY(x)	(sizeof(x) / sizeof(*(x)))

#define SET_BITS(x, msb, lsb, value)	\
    (((x) & ~(((1 << ((msb) + 1)) - 1) ^ ((1 << (lsb)) - 1))) | (((value) & ((1 << (1 + (msb) - (lsb))) - 1)) << (lsb)))


#define IFX_PP32_ETOP_CFG		0x16020
#define IFX_PP32_ETOP_MDIO_CFG		0x11804
#define IFX_PP32_ETOP_IG_PLEN_CTRL	0x16080
#define IFX_PP32_ENET_MAC_CFG		0x1840

#define IFX_RCU_DOMAIN_PPE		(1 << 8)
#define IFX_RCU_MODULE_ATM

#define IFX_PMU_ENABLE    1
#define IFX_PMU_DISABLE   0

#define IFX_PMU_MODULE_DSL_DFE    (1 << 9)
#define IFX_PMU_MODULE_AHBS		  (1 << 13)
#define IFX_PMU_MODULE_PPE_QSB    (1 << 18)
#define IFX_PMU_MODULE_PPE_SLL01  (1 << 19)
#define IFX_PMU_MODULE_PPE_TC     (1 << 21)
#define IFX_PMU_MODULE_PPE_EMA    (1 << 22)
#define IFX_PMU_MODULE_PPE_TOP    (1 << 29)

extern void ltq_pmu_enable(unsigned int module);
extern void ltq_pmu_disable(unsigned int module);

#define ifx_pmu_set(a,b)	{if(a == IFX_PMU_ENABLE) ltq_pmu_enable(b); else ltq_pmu_disable(b);}

#define PPE_TOP_PMU_SETUP(__x)    ifx_pmu_set(IFX_PMU_MODULE_PPE_TOP, (__x))
#define PPE_SLL01_PMU_SETUP(__x)  ifx_pmu_set(IFX_PMU_MODULE_PPE_SLL01, (__x))
#define PPE_TC_PMU_SETUP(__x)     ifx_pmu_set(IFX_PMU_MODULE_PPE_TC, (__x))
#define PPE_EMA_PMU_SETUP(__x)    ifx_pmu_set(IFX_PMU_MODULE_PPE_EMA, (__x))
#define PPE_QSB_PMU_SETUP(__x)    ifx_pmu_set(IFX_PMU_MODULE_PPE_QSB, (__x))
#define PPE_TPE_PMU_SETUP(__x)    ifx_pmu_set(IFX_PMU_MODULE_AHBS, (__x))
#define DSL_DFE_PMU_SETUP(__x)    ifx_pmu_set(IFX_PMU_MODULE_DSL_DFE, (__x))

#define IFX_REG_W32(_v, _r)               __raw_writel((_v), (_r))

#define CONFIG_IFXMIPS_DSL_CPE_MEI	y

#define INT_NUM_IM2_IRL24	(INT_NUM_IM2_IRL0 + 24)

#endif
