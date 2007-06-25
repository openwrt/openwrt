/*
 * Cellvision/SparkLAN boards
 */

#if defined(CONFIG_BOARD_CAS630)
#  define CONFIG_BOARD_NAME	"CAS-630"
#endif

#if defined(CONFIG_BOARD_CAS670)
#  define CONFIG_BOARD_NAME	"CAS-670"
#endif

#if defined(CONFIG_BOARD_CAS700)
#  define CONFIG_BOARD_NAME	"CAS-700"
#endif

#if defined(CONFIG_BOARD_CAS790)
#  define CONFIG_BOARD_NAME	"CAS-790"
#endif

#if defined(CONFIG_BOARD_CAS771)
#  define CONFIG_BOARD_NAME	"CAS-771"
#endif

#if defined(CONFIG_BOARD_CAS861)
#  define CONFIG_BOARD_NAME	"CAS-861"
#endif

#if defined(CONFIG_BOARD_NFS101U)
#  define CONFIG_BOARD_NAME	"NFS-101U"
#endif

#if defined(CONFIG_BOARD_NFS202U)
#  define CONFIG_BOARD_NAME	"NFS-202U"
#endif

/*
 * Edimax boards
 */
#if defined(CONFIG_BOARD_BR6104K)
#  define CONFIG_BOARD_NAME	"BR-6104K"
#endif

#if defined(CONFIG_BOARD_BR6104KP)
#  define CONFIG_BOARD_NAME	"BR-6104KP"
#endif

/*
 * Infineon boards
 */
#if defined(CONFIG_BOARD_EASY5120)
#  define CONFIG_BOARD_NAME	"EASY 5120"
#endif

#if defined(CONFIG_BOARD_EASY5120RT)
#  define CONFIG_BOARD_NAME	"EASY 5120-RT"
#endif

#if defined(CONFIG_BOARD_EASY5120PATA)
#  define CONFIG_BOARD_NAME	"EASY 5120P-ATA"
#endif

#if defined(CONFIG_BOARD_EASY83000)
#  define CONFIG_BOARD_NAME	"EASY 83000"
#endif

/*
 * ZyXEL boards
 */
#if defined(CONFIG_BOARD_P334WT)
#  define CONFIG_BOARD_NAME	"P-334WT"
#endif

#if defined(CONFIG_BOARD_P335)
#  define CONFIG_BOARD_NAME	"P-335"
#endif

/*
 * Default values
 */
#ifndef CONFIG_BOARD_NAME
#  define CONFIG_BOARD_NAME	"ADM5120"
#endif
