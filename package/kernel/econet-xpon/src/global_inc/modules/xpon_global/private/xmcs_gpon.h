#ifndef _XPON_GLOBAL_PRIVATE_XMCS_GPON_H_
#define _XPON_GLOBAL_PRIVATE_XMCS_GPON_H_

#include "../public/xmcs_gpon.h"
#include "xmcs_const.h"

typedef enum {
    XMCS_GPON_TRTCM_SCALE_1B = 0,
    XMCS_GPON_TRTCM_SCALE_2B,
    XMCS_GPON_TRTCM_SCALE_4B, 
    XMCS_GPON_TRTCM_SCALE_8B,
    XMCS_GPON_TRTCM_SCALE_16B,
    XMCS_GPON_TRTCM_SCALE_32B,
    XMCS_GPON_TRTCM_SCALE_64B,
    XMCS_GPON_TRTCM_SCALE_128B,
    XMCS_GPON_TRTCM_SCALE_256B,
    XMCS_GPON_TRTCM_SCALE_512B,
    XMCS_GPON_TRTCM_SCALE_1K,
    XMCS_GPON_TRTCM_SCALE_2K,
    XMCS_GPON_TRTCM_SCALE_4K,
    XMCS_GPON_TRTCM_SCALE_8K,
    XMCS_GPON_TRTCM_SCALE_16K,
    XMCS_GPON_TRTCM_SCALE_32K,
    XMCS_GPON_TRTCM_SCALE_ITEMS
} XMCSGPON_TrtcmScale_t;

typedef enum {
    GPON_COUNTER_TYPE_GEM = 0,
    GPON_COUNTER_TYPE_ETHERNET,
} GPON_COUNTER_TYPE_t ;

typedef enum {
    GPON_BURST_MODE_OVERHEAD_LEN_DEFAULT = 0,
    GPON_BURST_MODE_OVERHEAD_LEN_MORE_THAN_128,
} GPON_BURST_MODE_OVERHEAD_LEN_T ;

typedef enum {
    Doze = 0,
    Sleep,
    WSleep,
} GPON_PLOAMu_SLEEP_MODE_t ;

/* struct definition for GPON common command */
struct XMCS_GponSnPasswd_S {
    __u8  sn[GPON_SN_LENS] ;
    __u8  passwd[GPON_PASSWD_LENS] ;
    __u8  EmergencyState  ;
    __u8  PasswdLength;
    __u8  hexFlag;
} ;

struct XMCS_GponActTimer_S {
    __u32    to1Timer ;
    __u32    to2Timer ;
} ;

struct XMCS_GponTrtcmConfig_S {
    XPON_Mode_t             trtcmMode ;
    XMCSGPON_TrtcmScale_t   trtcmScale ;
} ;

struct XMCS_GponTrtcmParams_S {
    __u8   channel ;
    __u16  cirValue ;
    __u16  cbsUnit ;
    __u16  pirValue ;
    __u16  pbsUnit ;
} ;

struct XMCS_GponTodCfg_S {
    __u32    superframe ;
    __u32    sec ;
    __u32    nanosec ;
} ;

struct XMCS_EqdOffset_S {
    unsigned char O4 ;
    unsigned char O5 ;
    unsigned char eqdOffsetFlag;
} ;
struct XMCS_GponGetCounter_S {
    __u32 gponGetCounter_table[16];
};

typedef struct {
    __u32 aesSpf;
    __u32 activeKey[4];
    __u32 shadowKey[4];
} GPON_DEV_ENCRYPT_KEY_INFO_T;


typedef struct {
    XPON_Mode_t     omci;
    XPON_Mode_t     data;
} GPON_DEV_UP_TRAFFIC_T;

#ifdef TCSUPPORT_CPU_EN7521
typedef enum {
    SNIFFER_MODE_LAN0 = 0x8001,
    SNIFFER_MODE_LAN1 = 0x8002,
    SNIFFER_MODE_LAN2 = 0x8004,
    SNIFFER_MODE_LAN3 = 0x8008,
} GPON_DEV_SNIFFER_MODE_LAN_PORT_T ;

typedef struct {
    GPON_DEV_SNIFFER_MODE_LAN_PORT_T lan_port;
    __u16          tx_da;
    __u16          tx_sa;
    __u16          tx_ethertype;
    __u16          tx_vid;
    __u16          tx_tpid;
    __u16          rx_da;
    __u16          rx_sa;
    __u16          rx_ethertype;
    __u16          rx_vid;
    __u16          rx_tpid;
    XPON_Mode_t     packet_padding;
    XPON_Mode_t     enable;
} GPON_DEV_SNIFFER_MODE_T;

typedef struct {
    __u32            dba_backdoor_total_buf;
    __u32            dba_backdoor_green_buf;
    __u32            dba_backdoor_yellow_buf;
    XPON_Mode_t     enable;
} GPON_DEV_DBA_BACKDOOR_T;

typedef struct {
    __u16          dba_slight_modify_total_buf;
    __u16          dba_slight_modify_green_buf;
    __u16          dba_slight_modify_yellow_buf;
    XPON_Mode_t     enable;
} GPON_DEV_SLIGHT_MODIFY_T;

#endif

#ifdef TCSUPPORT_CPU_EN7521
typedef enum {
	SEND_PLOAMU_BEFORE = 0,
	SEND_PLOAMU_AFTER,
} GPON_DEV_SEND_PLOAMU_WAIT_MODE_T ;
#endif

/**********************************************************************************
***********************************************************************************/
#define GPON_MAGIC                          (0xD9)

/* GPON IO Command */
#define GPON_IOS_SN_PASSWD                  _IOW(GPON_MAGIC, 0x01, struct XMCS_GponSnPasswd_S *)
#define GPON_IOS_SN                         _IOW(GPON_MAGIC, 0x02, struct XMCS_GponSnPasswd_S *)
#define GPON_IOS_PASSWD                     _IOW(GPON_MAGIC, 0x03, struct XMCS_GponSnPasswd_S *)
#define GPON_IOS_EMERGENCY_STATE            _IOW(GPON_MAGIC, 0x04, XPON_Mode_t)
#define GPON_IOS_ACT_TIMER                  _IOW(GPON_MAGIC, 0x05, struct XMCS_GponActTimer_S *)
#define GPON_IOS_ACT_TIMER_TO1              _IOW(GPON_MAGIC, 0x06, __u32)
#define GPON_IOS_ACT_TIMER_TO2              _IOW(GPON_MAGIC, 0x07, __u32)
#define GPON_IOG_CSR                        _IOR(GPON_MAGIC, 0x08, ulong)
#define GPON_IOG_INT_MASK                   _IOR(GPON_MAGIC, 0x09, __u32 *)
#define GPON_IOG_GEM_TABLE_INFO             _IOR(GPON_MAGIC, 0x0A, ulong)
#define GPON_IOG_TCONT_TABLE_INFO           _IOR(GPON_MAGIC, 0x0B, ulong)
#define GPON_IOS_IDLE_GEM					_IOW(GPON_MAGIC, 0x0C, ushort)
#define GPON_IOG_IDLE_GEM					_IOR(GPON_MAGIC, 0x0D, ushort *)
#define GPON_IOG_ONU_INFO                   _IOR(GPON_MAGIC, 0x10, struct XMCS_GponOnuInfo_S *)
#define GPON_IOS_DBA_BLOCK_SIZE             _IOW(GPON_MAGIC, 0x11, __u16)
#define GPON_IOG_DBA_BLOCK_SIZE             _IOR(GPON_MAGIC, 0x11, __u16 *)
#define GPON_IOS_GPON_TRTCM_MODE            _IOW(GPON_MAGIC, 0x12, struct XMCS_GponTrtcmConfig_S *)
#define GPON_IOS_GPON_TRTCM_SCALE           _IOW(GPON_MAGIC, 0x13, struct XMCS_GponTrtcmConfig_S *)
#define GPON_IOG_GPON_TRTCM_CONFIG          _IOR(GPON_MAGIC, 0x14, struct XMCS_GponTrtcmConfig_S *)
#define GPON_IOS_GPON_TRTCM_PARAMS          _IOW(GPON_MAGIC, 0x15, struct XMCS_GponTrtcmParams_S *)
#define GPON_IOG_GPON_TRTCM_PARAMS          _IOR(GPON_MAGIC, 0x15, struct XMCS_GponTrtcmParams_S *)
#define GPON_IOS_TOD_CFG                    _IOW(GPON_MAGIC, 0x20, struct XMCS_GponTodCfg_S *)
#define GPON_IOG_CURRENT_TOD                _IOR(GPON_MAGIC, 0x20, struct XMCS_GponTodCfg_S *)
#define GPON_IOG_NEW_TOD                    _IOR(GPON_MAGIC, 0x21, struct XMCS_GponTodCfg_S *)
#define GPON_IOG_TOD_SWITCH_TIME            _IOR(GPON_MAGIC, 0x22, __u32 *)
#define GPON_IOG_EQD_OFFSET                 _IOR(GPON_MAGIC, 0x23, struct XMCS_EqdOffset_S *)
#define GPON_IOS_EQD_OFFSET_FLAG            _IOW(GPON_MAGIC, 0x23, char)
#define GPON_IOS_EQD_OFFSET_O4              _IOW(GPON_MAGIC, 0x24, __u8)
#define GPON_IOS_EQD_OFFSET_O5              _IOW(GPON_MAGIC, 0x25, __u8)
#define GPON_IOG_GEM_COUNTER                _IOR(GPON_MAGIC, 0x30, struct XMCS_GponGemCounter_S *)
#define GPON_IOS_CLEAR_GEM_COUNTER          _IOW(GPON_MAGIC, 0x30, __u16)
#define GPON_IOG_SUPER_FRAME_COUNTER        _IOR(GPON_MAGIC, 0x32, __u32 *)
#define GPON_IOS_COUNTER_TYPE               _IOW(GPON_MAGIC, 0x33, GPON_COUNTER_TYPE_t)
#define GPON_IOG_COUNTER_TYPE               _IOR(GPON_MAGIC, 0x33, GPON_COUNTER_TYPE_t *)
#define GPON_IOS_RESPONSE_TIME              _IOW(GPON_MAGIC, 0x34, __u16)
#define GPON_IOG_RESPONSE_TIME              _IOR(GPON_MAGIC, 0x34, __u16 *)
#define GPON_IOS_INTERNAL_DELAY_FINE_TUNE   _IOW(GPON_MAGIC, 0x35, __u8)
#define GPON_IOG_INTERNAL_DELAY_FINE_TUNE   _IOR(GPON_MAGIC, 0x35, __u8 *)
#define GPON_IOS_BURST_MODE_OVERHEAD_LEN    _IOW(GPON_MAGIC, 0x36, int)
#define GPON_IOG_BURST_MODE_OVERHEAD_LEN    _IOR(GPON_MAGIC, 0x36, int *)
#define GPON_IOS_CLEAR_COUNTER              _IOW(GPON_MAGIC, 0x37, int)
#define GPON_IOS_KEY_SWITCH_TIME            _IOW(GPON_MAGIC, 0x38, __u32)
#define GPON_IOS_ENCRYPT_KEY                _IOW(GPON_MAGIC, 0x39, __u8 *)
#define GPON_IOG_ENCRYPT_KEY_INFO           _IOR(GPON_MAGIC, 0x39, GPON_DEV_ENCRYPT_KEY_INFO_T *)
#define GPON_IOS_DYING_GASP_MODE            _IOW(GPON_MAGIC, 0x3A, GPON_SW_HW_SELECT_T)
#define GPON_IOG_DYING_GASP_MODE            _IOR(GPON_MAGIC, 0x3A, GPON_SW_HW_SELECT_T *)
#define GPON_IOS_DYING_GASP_NUM             _IOW(GPON_MAGIC, 0x3B, __u32)
#define GPON_IOG_DYING_GASP_NUM             _IOR(GPON_MAGIC, 0x3B, __u32 *)
#define GPON_IOS_UP_TRAFFIC                 _IOW(GPON_MAGIC, 0x3C, GPON_DEV_UP_TRAFFIC_T *)
#define GPON_IOG_UP_TRAFFIC                 _IOR(GPON_MAGIC, 0x3C, GPON_DEV_UP_TRAFFIC_T *)

#ifdef TCSUPPORT_CPU_EN7521
#define GPON_IOS_SNIFFER_GTC                _IOW(GPON_MAGIC, 0x3D, GPON_DEV_SNIFFER_MODE_T *)
#define GPON_IOG_SNIFFER_GTC                _IOR(GPON_MAGIC, 0x3D, GPON_DEV_SNIFFER_MODE_T *)
#define GPON_IOS_EXT_BST_LEN_PLOAMD_FILTER_IN_O5    _IOW(GPON_MAGIC, 0x3E, XPON_Mode_t)
#define GPON_IOG_EXT_BST_LEN_PLOAMD_FILTER_IN_O5    _IOR(GPON_MAGIC, 0x3E, XPON_Mode_t *)
#define GPON_IOS_UP_OVERHEAD_PLOAMD_FILTER_IN_O5    _IOW(GPON_MAGIC, 0x3F, XPON_Mode_t)
#define GPON_IOG_UP_OVERHEAD_PLOAMD_FILTER_IN_O5    _IOR(GPON_MAGIC, 0x3F, XPON_Mode_t *)
#define GPON_IOS_TX_4BYTES_ALIGN            _IOW(GPON_MAGIC, 0x40, XPON_Mode_t)
#define GPON_IOG_TX_4BYTES_ALIGN            _IOR(GPON_MAGIC, 0x40, XPON_Mode_t *)
#define GPON_IOS_DBA_BACKDOOR               _IOW(GPON_MAGIC, 0x41, XPON_Mode_t)
#define GPON_IOG_DBA_BACKDOOR               _IOR(GPON_MAGIC, 0x41, GPON_DEV_DBA_BACKDOOR_T *)
#define GPON_IOS_DBA_BACKDOOR_TOTAL         _IOW(GPON_MAGIC, 0x42, __u32)
#define GPON_IOS_DBA_BACKDOOR_GREEN         _IOW(GPON_MAGIC, 0x43, __u32)
#define GPON_IOS_DBA_BACKDOOR_YELLOW        _IOW(GPON_MAGIC, 0x44, __u32)
#define GPON_IOS_DBA_SLIGHT_MODIFY          _IOW(GPON_MAGIC, 0x45, XPON_Mode_t)
#define GPON_IOG_DBA_SLIGHT_MODIFY          _IOR(GPON_MAGIC, 0x45, GPON_DEV_SLIGHT_MODIFY_T *)
#define GPON_IOS_DBA_SLIGHT_MODIFY_TOTAL    _IOW(GPON_MAGIC, 0x46, __u16)
#define GPON_IOS_DBA_SLIGHT_MODIFY_GREEN    _IOW(GPON_MAGIC, 0x47, __u16)
#define GPON_IOS_DBA_SLIGHT_MODIFY_YELLOW   _IOW(GPON_MAGIC, 0x48, __u16)
#define GPON_IOS_O3_O4_PLOAM_CTRL           _IOW(GPON_MAGIC, 0x49, GPON_SW_HW_SELECT_T)
#define GPON_IOG_O3_O4_PLOAM_CTRL           _IOR(GPON_MAGIC, 0x49, GPON_SW_HW_SELECT_T *)
#define GPON_IOG_TX_SYNC                    _IOR(GPON_MAGIC, 0x4A, __u8 *)
#define GPON_IOS_AES_KEY_SWITCH_BY_SW       _IOW(GPON_MAGIC, 0x4B, ulong)
#define GPON_IOS_SW_RESET                   _IOW(GPON_MAGIC, 0x4C, ulong)
#define GPON_IOS_SW_RESYNC                  _IOW(GPON_MAGIC, 0x4D, ulong)
#define GPON_IOS_RESET_SERVICE              _IOW(GPON_MAGIC, 0x4E, __u32)
#endif

#define GPON_IOS_TEST_SEND_PLOAM_SLEEP_REQUEST  _IOW(GPON_MAGIC, 0x50, GPON_PLOAMu_SLEEP_MODE_t)
#define GPON_IOS_TEST_SEND_PLOAM_REI            _IOW(GPON_MAGIC, 0x51, __u32)
#define GPON_IOS_TEST_SEND_PLOAM_DYING_GASP     _IOW(GPON_MAGIC, 0x52, ulong)
#define GPON_IOS_TEST_TOD                       _IOW(GPON_MAGIC, 0x53, __u32)
#define GPON_IOS_GET_RX_ETHERNET_FRAME_COUNTER  _IOR(GPON_MAGIC, 0x54, struct XMCS_GponGetCounter_S *)
#define GPON_GET_TRAFFIC_STATE                  _IOR(GPON_MAGIC, 0x55, __u32)
#define XPON_GET_UPSTREAM_BLOCK_STATE           _IOR(GPON_MAGIC, 0x56, __u32)
#define GPON_IOS_TEST_HOTPLUG                   _IOW(GPON_MAGIC, 0x57, XPON_Mode_t)
#define GPON_GET_ONLINE_DURATION				_IOR(GPON_MAGIC, 0x58, uint)

#ifdef TCSUPPORT_CPU_EN7521
#define GPON_IOS_1PPS_HIGH_WIDTH					_IOW(GPON_MAGIC, 0x60, uint)
#define GPON_IOG_1PPS_HIGH_WIDTH					_IOR(GPON_MAGIC, 0x61, uint *)
#define GPON_IOS_SEND_PLOAMU_WAIT_MODE				_IOW(GPON_MAGIC, 0x62, GPON_DEV_SEND_PLOAMU_WAIT_MODE_T)
#define GPON_IOG_SEND_PLOAMU_WAIT_MODE				_IOR(GPON_MAGIC, 0x63, GPON_DEV_SEND_PLOAMU_WAIT_MODE_T *)
#endif /* TCSUPPORT_CPU_EN7521 */

#endif /* _XPON_GLOBAL_PRIVATE_XMCS_GPON_H_ */
