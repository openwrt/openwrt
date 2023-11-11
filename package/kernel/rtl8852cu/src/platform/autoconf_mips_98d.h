#ifdef CONFIG_PLATFORM_RTL8198D

#define CONFIG_RTW_FC_FASTFWD

#ifndef CONFIG_PHL_USE_KMEM_ALLOC
#define CONFIG_PHL_USE_KMEM_ALLOC
#endif
#ifdef RTW_WKARD_RATE_DRV_CTRL
#undef RTW_WKARD_RATE_DRV_CTRL
#endif
#ifdef RTW_WKARD_RATE_INIT_6M
#undef RTW_WKARD_RATE_INIT_6M
#endif

/* CORE RX path shortcut */
#define CONFIG_RTW_CORE_RXSC

/* CORE TX path shortcut */
#define CONFIG_CORE_TXSC
#ifdef CONFIG_CORE_TXSC
/* TX shorcut amsdu */
#ifdef CONFIG_TX_AMSDU
//#define CONFIG_TXSC_AMSDU
#endif
/* PHL TX path shortcut */
#define CONFIG_PHL_TXSC
#endif/* CONFIG_CORE_TXSC */

/* Separate TX path into different CPUs */
//#define RTW_TX_CPU_BALANCE
#ifdef RTW_TX_CPU_BALANCE
	#define CPU_ID_TX_CORE		2
#endif

#endif
