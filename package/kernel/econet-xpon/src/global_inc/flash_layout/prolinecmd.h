/******************************************************************************/
/*
 * Copyright (C) 1994-2008 TrendChip Technologies, Corp.
 * All Rights Reserved.
 *
 * TrendChip Confidential; Need to Know only.
 * Protected as an unpublished work.
 *
 * The computer program listings, specifications and documentation
 * herein are the property of TrendChip Technologies, Corp. and
 * shall not be reproduced, copied, disclosed, or used in whole or
 * in part for any reason without the prior express written permission of
 * TrendChip Technologies, Corp.
 */
/******************************************************************************/
#ifndef _PROLINECMD_H
#define _PROLINECMD_H

#ifdef TCSUPPORT_PRODUCTIONLINE
#include    "flash_layout/tc_partition.h"

#define SUCCESS 0
#define FAIL -1

/*common struct*/
typedef	struct _operPara{
	int 	flashsize;		//cover size of flash size
	int 	flashoffset;		//offset of base address of current zone
	char zonename[16];	//name of zone
	char readfile[64];		//mtd cmd used
	char writefile[64];		//mtd cmd used
}operPara;

#define PL_CP 0	
typedef struct _proLineFlag{
	int type;

	int flag;	//according to types
	struct _operPara para;
} proLineFlag;

/*
PL:product line
CP:cwmp parameter
*/

#define WEBPWDLEN 16
#define WEBACCOUNTLEN 16
#if !defined(TCSUPPORT_CHS) 
#define HWSNLEN 0
#endif

#if defined(TCSUPPORT_RESERVEAREA_EXTEND) || defined(TCSUPPORT_C7) || (defined(TCSUPPORT_CT_E8B_ADSL) && defined(TCSUPPORT_CPU_MT7505))
enum PL_CP_Flags {
    PL_CP_PRODUCTCLASS=0,
    PL_CP_MANUFACUREROUI,
    PL_CP_SERIALNUM,
    PL_CP_SSID,
    PL_CP_WPAKEY,
    PL_CP_WEBPWD,
    PL_CP_PPPUSRNAME,
    PL_CP_PPPPWD,
    PL_CP_CFEUSRNAME,
    PL_CP_CFEPWD,
    PL_CP_XPONSN,
    PL_CP_XPONPWD,
    PL_CP_MACADDR,
    PL_CP_TELNET,
    PL_FON_KEYWORD,
    PL_FON_MAC,
    PL_CP_BARCODE,
    PL_CP_XPONMODE,
    PL_CP_SSID2nd,
    PL_CP_WPAKEY2nd,
    PL_CP_WEB_ACCOUNT,
    PL_CP_HW_VER,
    PL_CP_ROMFILE_SELECT,
    PL_CP_MT7570BOB,
    PL_CP_END
};

#define FLAG_TELNET_DISABLE "DIS"
#define FLAG_TELNET_ENABLE "ENB"

#define DEFAULT_CFE_USERNAME "admin"
#define DEFAULT_CFE_PWD "REDACTED-PW"

#define PL_CP_PRODUCTCLASS_FLAG (1<<PL_CP_PRODUCTCLASS)
#define PL_CP_MANUFACUREROUI_FLAG (1<<PL_CP_MANUFACUREROUI)
#define PL_CP_SERIALNUM_FLAG (1<<PL_CP_SERIALNUM)
#define PL_CP_SSID_FLAG (1<<PL_CP_SSID)
#define PL_CP_WPAKEY_FLAG (1<<PL_CP_WPAKEY)
#define PL_CP_WEBPWD_FLAG (1<<PL_CP_WEBPWD)
#define PL_CP_PPPUSRNAME_FLAG (1<<PL_CP_PPPUSRNAME)
#define PL_CP_PPPPWD_FLAG (1<<PL_CP_PPPPWD)
#define PL_CP_CFEUSRNAME_FLAG (1<<PL_CP_CFEUSRNAME)
#define PL_CP_CFEPWD_FLAG (1<<PL_CP_CFEPWD)
#define PL_CP_XPONSN_FLAG (1<<PL_CP_XPONSN)
#define PL_CP_XPONPWD_FLAG (1<<PL_CP_XPONPWD)
#define PL_CP_MACADDR_FLAG (1<<PL_CP_MACADDR)
#define PL_FON_KEYWORD_FLAG (1<<PL_FON_KEYWORD)
#define PL_FON_MAC_FLAG (1<<PL_FON_MAC)
#define PL_CP_BARCODE_FLAG (1<<PL_CP_BARCODE)
#define PL_CP_XPONMODE_FLAG (1<<PL_CP_XPONMODE)
#define PL_CP_TELNET_FLAG (1<<PL_CP_TELNET)
#define PL_CP_SSID2nd_FLAG (1<<PL_CP_SSID2nd)
#define PL_CP_WPAKEY2nd_FLAG (1<<PL_CP_WPAKEY2nd)
#define PL_CP_WEB_ACCOUNT_FLAG (1<<PL_CP_WEB_ACCOUNT)
#define PL_CP_HW_VER_FLAG	(1<<PL_CP_HW_VER)
#define PL_CP_ROMFILE_SELECT_FLAG	(1<<PL_CP_ROMFILE_SELECT)
#define PL_CP_MT7570BOB_FLAG	(1<<PL_CP_MT7570BOB)

#define PRDDUCTCLASSLEN 64
#define MANUFACUREROUILEN 64
#define SERIALNUMLEN 128
#define SSIDLEN 32
#define WPAKEYLEN 64
#define PPPUSRNAMELEN 64
#define PPPPWDLEN 64
#define CFEUSRNAMELEN 64
#define CFEPWDLEN 64
#define XPONSNLEN 32
#define XPONPWDLEN 32
#define MACADDRLEN 32
#define XPONMODELEN	8
#define FONKEYWORDLEN 65
#define FONMACLEN 18
#define BARCODELEN	32
#define TELNETLEN 4
#define HWVERLEN 64
#define ROMFILESELECTLEN 64
#define MT7570BOBLEN 225



typedef struct _proline_Para{
	int flag;
	int magic;
	char telnet[TELNETLEN];
	char barcode[BARCODELEN];
	char productclass[PRDDUCTCLASSLEN];
	char manufacturerOUI[MANUFACUREROUILEN];
	char serialnum[SERIALNUMLEN];
	char ssid[SSIDLEN];
	char wpakey[WPAKEYLEN];
	char webpwd[WEBPWDLEN];
	char pppusrname[PPPUSRNAMELEN];
	char ppppwd[PPPPWDLEN];
	char cfeusrname[CFEUSRNAMELEN];
	char cfepwd[CFEPWDLEN];
	char xponsn[XPONSNLEN];
	char xponpwd[XPONPWDLEN];
	char macaddr[MACADDRLEN];
	char xponmode[XPONMODELEN];
	char fonKeyword[FONKEYWORDLEN];
	char fonMac[FONMACLEN];
	char ssid2nd[SSIDLEN];
	char wpakey2nd[WPAKEYLEN];
	char webAcct[WEBACCOUNTLEN];
	char hwver[HWVERLEN];
	char romfileselect[ROMFILESELECTLEN];
	char mt7570bob[MT7570BOBLEN];
	char reserve[799]; 
} proline_Para;
#elif defined(TCSUPPORT_CY)
#define PL_CP_PRODUCTCLASS 0
#define PL_CP_MANUFACUREROUI 1
#define PL_CP_SERIALNUM 2
#if defined(TCSUPPORT_FON)
#define PL_FON_KEYWORD 6
#define PL_FON_MAC 7
#endif
#define	PL_CP_BARCODE	8

#define PL_CP_PRODUCTCLASS_FLAG (1<<0)
#define PL_CP_MANUFACUREROUI_FLAG (1<<1)
#define PL_CP_SERIALNUM_FLAG (1<<2)
#if defined(TCSUPPORT_FON)
#define PL_FON_KEYWORD_FLAG (1<<6)
#define PL_FON_MAC_FLAG (1<<7)
#endif
#define PL_CP_BARCODE_FLAG	(1<<8)

#define PRDDUCTCLASSLEN 64
#define MANUFACUREROUILEN 64
#define SERIALNUMLEN 128

#if !defined(TCSUPPORT_PRODUCTIONLINE_CONTENT) 
#define PRODUCTCONTENTLEN 0
#endif
#if !defined(TCSUPPORT_PRODUCTIONLINE_DATE) 
#define PROGDATELEN 0
#define MFTDATELEN 0
#endif
#if defined(TCSUPPORT_FON)
#define FONKEYWORDLEN 65
#define FONMACLEN 18
#else
#define FONKEYWORDLEN 0
#define FONMACLEN 0
#endif
#define BARCODELEN	32
#define RESERVELEN (PROLINE_CWMPPARA_RA_SIZE-PRDDUCTCLASSLEN-MANUFACUREROUILEN-SERIALNUMLEN-PROGDATELEN-MFTDATELEN-PRODUCTCONTENTLEN-FONKEYWORDLEN-FONMACLEN-BARCODELEN-2*sizeof(int))

typedef struct _proline_cwmpPara{
	int flag;
	char productclass[PRDDUCTCLASSLEN];
	char manufacturerOUI[MANUFACUREROUILEN];
	char serialnum[SERIALNUMLEN];
#if defined(TCSUPPORT_FON)
	char fonKeyword[FONKEYWORDLEN];
	char fonMac[FONMACLEN];
#endif
	char barcode[BARCODELEN];
	char reserve[RESERVELEN];
	int magic;
} proline_cwmpPara;
typedef struct _proline_cwmpPara proline_Para;

#else // #ifdef TCSUPPORT_RESERVEAREA_EXTEND
#define PL_CP_PRODUCTCLASS 0
#define PL_CP_MANUFACUREROUI 1
#define PL_CP_SERIALNUM 2
#if defined(TCSUPPORT_FON) || defined(TCSUPPORT_FON_V2)
#define PL_FON_KEYWORD 6
#define PL_FON_MAC 7
#endif
#if defined(TCSUPPORT_WAN_GPON) || defined(TCSUPPORT_WAN_EPON)
#define PL_PON_MACNUM  8
#define PL_PON_USERADMIN_PASSWD 9
#if defined(TCSUPPORT_WAN_GPON)
#define PL_GPON_SN  10
#define PL_GPON_PASSWD  11
#endif
#endif
#define PL_CP_WEBPWD 12
#define PL_CP_WEB_ACCOUNT 13
#define PL_CP_TELNET 14


#define PL_CP_PRODUCTCLASS_FLAG (1<<0)
#define PL_CP_MANUFACUREROUI_FLAG (1<<1)
#define PL_CP_SERIALNUM_FLAG (1<<2)
#if defined(TCSUPPORT_FON) || defined(TCSUPPORT_FON_V2)
#define PL_FON_KEYWORD_FLAG (1<<6)
#define PL_FON_MAC_FLAG (1<<7)
#endif
#if defined(TCSUPPORT_WAN_GPON) || defined(TCSUPPORT_WAN_EPON)
#define PL_PON_MACNUM_FLAG  (1<<8)
#define PL_PON_USERADMIN_PASSWD_FLAG (1<<9)
#if defined(TCSUPPORT_WAN_GPON)
#define PL_GPON_SN_FLAG     (1<<10)
#define PL_GPON_PASSWD_FLAG (1<<11)
#endif
#endif

#define PL_CP_WEBPWD_FLAG      (1<<12)
#define PL_CP_WEB_ACCOUNT_FLAG (1<<13)
#define PL_CP_TELNET_FLAG (1<<14)
#if defined(TCSUPPORT_WAN_GPON)
#define PL_GPON_EXTPWD_FLAG (1<<16)
#endif


/****************************************
 * NEED TO PAY ATTENTION:
 * For upgrade on using network device, the prolinecmd
 * area need be reserved for feature. 
 * So the General Feature must not use compile.
 * When area is not enough, extend it.
 ***************************************/
#define PRDDUCTCLASSLEN 64
#define MANUFACUREROUILEN 64
#define SERIALNUMLEN 128

#if !defined(TCSUPPORT_PRODUCTIONLINE_CONTENT) 
#define PRODUCTCONTENTLEN 0
#endif

#if !defined(TCSUPPORT_PRODUCTIONLINE_DATE) 
#define PROGDATELEN 0
#define MFTDATELEN 0
#endif

#if defined(TCSUPPORT_FON) || defined(TCSUPPORT_FON_V2)
#define FONKEYWORDLEN 65
#define FONMACLEN 18
#else
#define FONKEYWORDLEN 0
#define FONMACLEN 0
#endif
#if defined(TCSUPPORT_WAN_GPON) || defined(TCSUPPORT_WAN_EPON)
#define PONMACNUMLEN  3
#define PONUSERADMINPASSWDLEN 32
#if defined(TCSUPPORT_WAN_GPON)
#define GPONSNLEN  13
#if !defined(TCSUPPORT_C1_ZY) 
#define GPONPASSWDLEN 9
#define GPONEXTENDPWDLEN 15
#endif
#else
#define GPONSNLEN  0
#define GPONPASSWDLEN 0
#define GPONEXTENDPWDLEN 0
#endif
#else
#define PONMACNUMLEN  0
#define PONUSERADMINPASSWDLEN 0
#define GPONSNLEN  0
#define GPONPASSWDLEN 0
#define GPONEXTENDPWDLEN 0
#endif

#if 0// defined(TCSUPPORT_CY)
#define BARCODELEN	32
#else
#define BARCODELEN	0
#endif
#define RESERVELEN (PROLINE_CWMPPARA_RA_SIZE     \
                    - 2*sizeof(int)              \
                    - PRDDUCTCLASSLEN            \
                    - MANUFACUREROUILEN          \
                    - SERIALNUMLEN               \
                    - PROGDATELEN                \
                    - MFTDATELEN                 \
                    - PRODUCTCONTENTLEN          \
                    - FONKEYWORDLEN              \
                    - FONMACLEN                  \
                    - PONMACNUMLEN               \
                    - PONUSERADMINPASSWDLEN      \
                    - GPONSNLEN                  \
                    - GPONPASSWDLEN              \
                    - WEBPWDLEN                  \
                    - WEBACCOUNTLEN              \
                    - HWSNLEN                    \
                    - GPONEXTENDPWDLEN           \
                    )
                    
typedef struct _proline_cwmpPara{
	int flag;
	int magic;	
	char productclass[PRDDUCTCLASSLEN];
	char manufacturerOUI[MANUFACUREROUILEN];
	char serialnum[SERIALNUMLEN];

/**************************************************
 * Attention: 
 * remove compile for reserved area when upgrade on using device 
 *************************************************/
#if defined(TCSUPPORT_FON) || defined(TCSUPPORT_FON_V2)
	char fonKeyword[FONKEYWORDLEN];
	char fonMac[FONMACLEN];
#endif
#if defined(TCSUPPORT_WAN_GPON) || defined(TCSUPPORT_WAN_EPON)
    char ponMacNum[PONMACNUMLEN];
	char ponUseradminPasswd[PONUSERADMINPASSWDLEN];
#if defined(TCSUPPORT_WAN_GPON)
	char gponSn[GPONSNLEN];
	char gponPasswd[GPONPASSWDLEN];
#endif
#endif
    char webpwd[WEBPWDLEN];
    char webAcct[WEBACCOUNTLEN];
#if defined(TCSUPPORT_WAN_GPON)
	char gponExtPwd[GPONEXTENDPWDLEN];
#endif
	char reserve[RESERVELEN];
} proline_cwmpPara;

typedef struct _proline_cwmpPara proline_Para;

#endif


#define PROLINECMD_SOCK_PATH 	"/tmp/prolinecmd_sock"
#define PROLINECMD_CWMPPARA_FILE  "/tmp/prolinecwmppara"
#define PROLINECMD_CWMPPARAREAD_FILE  "/tmp/prolinecwmppararead"
#define PROLINECMD_CWMPPARAWRITE_FILE  "/tmp/prolinecwmpparwrite"

#define CWMPPARAMAGIC 0x12344321

#endif
#endif
