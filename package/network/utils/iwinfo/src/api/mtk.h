#ifndef __MTK_H__
#define __MTK_H__

#define MAC_ADDR_LENGTH		6
#define MAX_NUMBER_OF_MAC	116
typedef unsigned char 	UCHAR;
typedef char		CHAR;
typedef unsigned int	UINT32;
typedef unsigned short	USHORT;
typedef short		SHORT;
typedef unsigned long	ULONG;

#if WIRELESS_EXT <= 11
#ifndef SIOCDEVPRIVATE
#define SIOCDEVPRIVATE				0x8BE0
#endif
#define SIOCIWFIRSTPRIV				SIOCDEVPRIVATE
#endif

#define RTPRIV_IOCTL_SET			(SIOCIWFIRSTPRIV + 0x02)
#define RTPRIV_IOCTL_GSITESURVEY		(SIOCIWFIRSTPRIV + 0x0D)
#define RTPRIV_IOCTL_GET_MAC_TABLE		(SIOCIWFIRSTPRIV + 0x0F)
#define RTPRIV_IOCTL_GET_MAC_TABLE_STRUCT	(SIOCIWFIRSTPRIV + 0x1F)

#define MODE_CCK	0
#define MODE_OFDM	1
#define MODE_HTMIX	2

/* MIMO Tx parameter, ShortGI, MCS, STBC, etc.  these are fields in TXWI. Don't change this definition!!! */
typedef union _MACHTTRANSMIT_SETTING {
        struct {
                USHORT MCS:7;   /* MCS */
                USHORT BW:1;    /*channel bandwidth 20MHz or 40 MHz */
                USHORT ShortGI:1;
                USHORT STBC:2;  /*SPACE */
                USHORT rsv:3;
                USHORT MODE:2;  /* Use definition MODE_xxx. */
        } field;
        USHORT word;
} MACHTTRANSMIT_SETTING, *PMACHTTRANSMIT_SETTING;

typedef struct _RT_802_11_MAC_ENTRY {
	UCHAR ApIdx;
	UCHAR Addr[MAC_ADDR_LENGTH];
	UCHAR Aid;
	UCHAR Psm;		/* 0:PWR_ACTIVE, 1:PWR_SAVE */
	UCHAR MimoPs;		/* 0:MMPS_STATIC, 1:MMPS_DYNAMIC, 3:MMPS_Enabled */
	CHAR AvgRssi0;
	CHAR AvgRssi1;
	CHAR AvgRssi2;
	CHAR Signal;
	CHAR Noise;
	UINT32 ConnectedTime;
	MACHTTRANSMIT_SETTING TxRate;
	MACHTTRANSMIT_SETTING RxRate;
	ULONG TxBitRate;
	ULONG RxBitRate;
	UINT32          LastRxRate;
	SHORT           StreamSnr[3];                           /* BF SNR from RXWI. Units=0.25 dB. 22 dB offset removed */
	SHORT           SoundingRespSnr[3];                     /* SNR from Sounding Response. Units=0.25 dB. 22 dB offset removed */
	UINT32		TxPackets;
	UINT32		RxPackets;
	UINT32		InactiveTime;				/* time since last activity (u32, msecs) */
} RT_802_11_MAC_ENTRY, *PRT_802_11_MAC_ENTRY;

typedef struct _RT_802_11_MAC_TABLE {
        ULONG Num;
        RT_802_11_MAC_ENTRY Entry[MAX_NUMBER_OF_MAC];
} RT_802_11_MAC_TABLE, *PRT_802_11_MAC_TABLE;

#endif // __MTK_H__
