#ifndef __ATE_H__
#define	__ATE_H__


typedef unsigned char   boolean;
typedef unsigned char	u8;
typedef int		s32;
typedef unsigned long	u32;
typedef unsigned short	u16;

typedef	unsigned short		UINT16;
typedef	unsigned long		UINT32;
typedef unsigned long long	UINT64;
extern int ate_debug_level; /* default : ate_debug_level == 2 */

#ifndef TRUE
#define TRUE              (1)
#define FALSE             (0)
#endif

/*
 *	IEEE 802.3 Ethernet magic constants.  The frame sizes omit the preamble
 *	and FCS/CRC (frame check sequence). 
 */

#define ETH_ALEN	6		/* Octets in one ethernet addr	 */
#define ETH_HLEN	14		/* Total octets in header.	 */

/* 
 *	Ethernet Protocol ID used by RaCfg Protocol
 */

#define ETH_P_RACFG	0x2880

/*
 * This is an RaCfg frame header 
 */
 struct racfghdr {
	u32		magic_no;
	u16		comand_type;
	u16		comand_id;
	u16		length;
	u16		sequence;
	u8		data[2048];	
}  __attribute__((packed));

struct reg_str  {
	u32		address;
	u32		value;
} __attribute__((packed));


struct cmd_id_tbl  {
	u16		command_id;
	u16		length;
} __attribute__((packed));


#define RACFG_MAGIC_NO		0x18142880 /* RALINK:0x2880 */

#define NEW_MAGIC_NO	0x19142880 /* RALINK:0x2880 */
/* 
 * RaCfg frame Comand Type
 */

/* use in bootstrapping state */

#define RACFG_CMD_TYPE_PASSIVE_MASK 0x7FFF

/* 
 * Bootstrapping command group 
 */

/* command type */
/* for rt28xx and rt2880 host */
/* iNIC does not need this daemon */
#define RACFG_CMD_TYPE_ETHREQ		0x0008

/* command id */
#define RACFG_CMD_RF_WRITE_ALL		0x0000
#define RACFG_CMD_E2PROM_READ16		0x0001
#define RACFG_CMD_E2PROM_WRITE16	0x0002
#define RACFG_CMD_E2PROM_READ_ALL	0x0003
#define RACFG_CMD_E2PROM_WRITE_ALL	0x0004
#define RACFG_CMD_IO_READ			0x0005
#define RACFG_CMD_IO_WRITE			0x0006
#define RACFG_CMD_IO_READ_BULK		0x0007
#define RACFG_CMD_BBP_READ8			0x0008
#define RACFG_CMD_BBP_WRITE8		0x0009
#define RACFG_CMD_BBP_READ_ALL		0x000a
#define RACFG_CMD_GET_COUNTER		0x000b
#define RACFG_CMD_CLEAR_COUNTER		0x000c
#define RACFG_CMD_RSV1				0x000d
#define RACFG_CMD_RSV2				0x000e
#define RACFG_CMD_RSV3				0x000f
#define RACFG_CMD_TX_START			0x0010
#define RACFG_CMD_GET_TX_STATUS		0x0011
#define RACFG_CMD_TX_STOP			0x0012
#define RACFG_CMD_RX_START			0x0013
#define RACFG_CMD_RX_STOP			0x0014
#define RACFG_CMD_GET_NOISE_LEVEL	0x0015

#define RACFG_CMD_ATE_START			0x0080
#define RACFG_CMD_ATE_STOP			0x0081
#define HQA_SetBandMode				0x152C 
extern unsigned short cmd_id_len_tbl[];

#define SIZE_OF_CMD_ID_TABLE    (sizeof(cmd_id_len_tbl) / sizeof(unsigned short) )

#define RACFG_CMD_ATE_START_TX_CARRIER		0x0100
#define RACFG_CMD_ATE_START_TX_CONT			0x0101
#define RACFG_CMD_ATE_START_TX_FRAME		0x0102
#define RACFG_CMD_ATE_SET_BW	            0x0103
#define RACFG_CMD_ATE_SET_TX_POWER0	        0x0104
#define RACFG_CMD_ATE_SET_TX_POWER1			0x0105
#define RACFG_CMD_ATE_SET_FREQ_OFFSET		0x0106
#define RACFG_CMD_ATE_GET_STATISTICS		0x0107
#define RACFG_CMD_ATE_RESET_COUNTER			0x0108
#define RACFG_CMD_ATE_SEL_TX_ANTENNA		0x0109
#define RACFG_CMD_ATE_SEL_RX_ANTENNA		0x010a
#define RACFG_CMD_ATE_SET_PREAMBLE			0x010b
#define RACFG_CMD_ATE_SET_CHANNEL			0x010c
#define RACFG_CMD_ATE_SET_ADDR1				0x010d
#define RACFG_CMD_ATE_SET_ADDR2				0x010e
#define RACFG_CMD_ATE_SET_ADDR3				0x010f
#define RACFG_CMD_ATE_SET_RATE				0x0110
#define RACFG_CMD_ATE_SET_TX_FRAME_LEN		0x0111
#define RACFG_CMD_ATE_SET_TX_FRAME_COUNT	0x0112
#define RACFG_CMD_ATE_START_RX_FRAME		0x0113

extern unsigned short ate_cmd_id_len_tbl[];

#define SIZE_OF_ATE_CMD_ID_TABLE    (sizeof(ate_cmd_id_len_tbl) / sizeof(unsigned short) )

#define RTPRIV_IOCTL_ATE			(SIOCIWFIRSTPRIV + 0x08)

#define RALINK_REG(x)  x

// Endian byte swapping codes
#define SWAP16(x) \
	((UINT16)( \
	(((UINT16)(x) & (UINT16) 0x00ffU) << 8) | \
	(((UINT16)(x) & (UINT16) 0xff00U) >> 8) ))

#define SWAP32(x) \
	((UINT32)( \
	(((UINT32)(x) & (UINT32) 0x000000ffUL) << 24) | \
	(((UINT32)(x) & (UINT32) 0x0000ff00UL) <<  8) | \
	(((UINT32)(x) & (UINT32) 0x00ff0000UL) >>  8) | \
	(((UINT32)(x) & (UINT32) 0xff000000UL) >> 24) ))

#define SWAP64(x) \
	((UINT64)( \
	(UINT64)(((UINT64)(x) & (UINT64) 0x00000000000000ffULL) << 56) | \
	(UINT64)(((UINT64)(x) & (UINT64) 0x000000000000ff00ULL) << 40) | \
	(UINT64)(((UINT64)(x) & (UINT64) 0x0000000000ff0000ULL) << 24) | \
	(UINT64)(((UINT64)(x) & (UINT64) 0x00000000ff000000ULL) <<  8) | \
	(UINT64)(((UINT64)(x) & (UINT64) 0x000000ff00000000ULL) >>  8) | \
	(UINT64)(((UINT64)(x) & (UINT64) 0x0000ff0000000000ULL) >> 24) | \
	(UINT64)(((UINT64)(x) & (UINT64) 0x00ff000000000000ULL) >> 40) | \
	(UINT64)(((UINT64)(x) & (UINT64) 0xff00000000000000ULL) >> 56) ))

// Roger finish
#define RT2880_REG(x)  x

#ifdef big_endian
#define cpu2le64(x) SWAP64((x))
#define le2cpu64(x) SWAP64((x))
#define cpu2le32(x) SWAP32((x))
#define le2cpu32(x) SWAP32((x))
#define cpu2le16(x) SWAP16((x))
#define le2cpu16(x) SWAP16((x))
#define cpu2be64(x) ((UINT64)(x))
#define be2cpu64(x) ((UINT64)(x))
#define cpu2be32(x) ((UINT32)(x))
#define be2cpu32(x) ((UINT32)(x))
#define cpu2be16(x) ((UINT16)(x))
#define be2cpu16(x) ((UINT16)(x))

#else /* Little Endian */
#define cpu2le64(x) ((UINT64)(x))
#define le2cpu64(x) ((UINT64)(x))
#define cpu2le32(x) ((UINT32)(x))
#define le2cpu32(x) ((UINT32)(x))
#define cpu2le16(x) ((UINT16)(x))
#define le2cpu16(x) ((UINT16)(x))
#define cpu2be64(x) SWAP64((x))
#define be2cpu64(x) SWAP64((x))
#define cpu2be32(x) SWAP32((x))
#define be2cpu32(x) SWAP32((x))
#define cpu2be16(x) SWAP16((x))
#define be2cpu16(x) SWAP16((x))
#endif

#ifndef os_memcpy
#define os_memcpy(d, s, n) memcpy((d), (s), (n))
#endif

#ifndef os_memset
#define os_memset(s, c, n) memset(s, c, n)
#endif

#ifndef os_strlen
#define os_strlen(s) strlen(s)
#endif

#ifndef os_strncpy
#define os_strncpy(d, s, n) strncpy((d), (s), (n))
#endif

#ifndef os_strchr
#define os_strchr(s, c) strchr((s), (c))
#endif

#ifndef os_strstr
#define os_strstr(s, c) strstr((s), (c))
#endif

#ifndef os_strcmp
#define os_strcmp(s1, s2) strcmp((s1), (s2))
#endif

/* Total octets before in-band payload.	 */
#define PRE_PAYLOADLEN	(ETH_HLEN + (sizeof(struct racfghdr) - 2048/* u8 data[2048] */))

/* For Interface Abstraction */
#define PKT_BUF_SIZE 1600 /*Size of Packet Buffer for packets from host*/
#define RA_CFG_HLEN	12	/* Length of Racfg header*/
#define NEW_MAGIC_ADDITIONAL_LEN (sizeof(struct new_cfghdr)-sizeof(struct racfghdr))
#ifndef IFNAMSIZ
#define IFNAMSIZ 16
#endif
enum { MSG_MSGDUMP, MSG_DEBUG, MSG_INFO, MSG_WARNING, MSG_ERROR };
#define CMD_QUEUE_SIZE 16
struct cmd_queue {
	int un_served;
	int served;
	int cmd_len[CMD_QUEUE_SIZE];
	unsigned char cmd_arr[CMD_QUEUE_SIZE][PKT_BUF_SIZE];
};

struct COMMON_PRIV {
	struct cmd_queue q;
	unsigned char reserved[1]; 
};

struct MULTI_PROC_OPS {
	int (*multi_proc_init)(void **dri_if, int idx, int dri_if_num, void *pkt_proc_logic);
	int (*multi_insert_q)(void *dri_if, unsigned char *pkt, int len);
	int (*multi_lock_q)(void *dri_if);
	int (*multi_unlock_q)(void *dri_if);
	int (*multi_wait_data)(void *dri_if);
	int (*multi_sig_data)(void *dri_if);
	int (*multi_proc_close)(void *dri_if);
};

/*
 * This is an New frame header 
 */
 struct new_cfghdr {
	u32	new_magic_no;
	u16 	ver_id;
	char 	if_name[IFNAMSIZ];
 	u32	magic_no;
	u16	comand_type;
	u16	comand_id;
	u16	length;
	u16	sequence;
	u8	data[2048];	
}  __attribute__((packed));

/* Abstraction of InterFace from Host*/
struct HOST_IF {
	boolean need_set_mac;
	boolean unicast;
	unsigned char da[ETH_ALEN];
	unsigned char ownmac[ETH_ALEN];
	int (*init_if)(struct HOST_IF *fd,char *bridge_ifname);
	int (*rev_cmd)(unsigned char *buf, int size);
	int (*rsp2host)(unsigned char *data, int size);
	int (*close)();
};

struct DRI_IF {
	int sock_ioctl;
	int (*init_if)(struct DRI_IF *fd,char *driver_ifname);
	int (*send)(struct DRI_IF *fd,unsigned char *buf, int size);
	int (*recv)(struct DRI_IF *fd,unsigned char *data, int size);
	int (*close)(struct DRI_IF *fd);
	// Place here for alignment
	char ifname[IFNAMSIZ];
	unsigned char mac[6];
	//struct cmd_queue q;
	struct MULTI_PROC_OPS *ops;
	void *priv_data;	/* Private data for multi-proc*/
	int status; /* 0:Idle, 1:Serving, 2:Queue Full */	
};
#ifdef DBG
extern void ate_printf(int level, char *fmt, ...);
#else // DBG //
#define ate_printf(args...) do { } while (0)
#define ate_hexdump(l,t,b,le) do { } while (0)
#endif // DBG //

#ifndef OFFSET_OF
    /* To suppress lint Warning 413: Likely use of null pointer, and assign it a magic number 0x100 */
    #ifdef _lint
    #define OFFSET_OF(_type, _field)    ((u32)&(((_type *)0x100)->_field) - 0x100)
    #else
    #define OFFSET_OF(_type, _field)    ((u32)&(((_type *)0)->_field))
    #endif /* _lint */
#endif /* OFFSET_OF */

#endif /* __ATE_H__*/
