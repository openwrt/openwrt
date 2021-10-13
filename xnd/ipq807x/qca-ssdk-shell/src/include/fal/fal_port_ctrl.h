/*
 * Copyright (c) 2014, 2016-2019, The Linux Foundation. All rights reserved.
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all copies.
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*qca808x_start*/
/**
 * @defgroup fal_port_ctrl FAL_PORT_CONTROL
 * @{
 */
#ifndef _FAL_PORTCTRL_H_
#define _FAL_PORTCTRL_H_

#ifdef __cplusplus
extern "c" {
#endif

#include "sw.h"
#include "fal_type.h"

    typedef enum {
        FAL_HALF_DUPLEX = 0,
        FAL_FULL_DUPLEX,
        FAL_DUPLEX_BUTT = 0xffff
    }
                      fal_port_duplex_t;

    typedef enum
    {
        FAL_SPEED_10    = 10,
        FAL_SPEED_100   = 100,
        FAL_SPEED_1000  = 1000,
        FAL_SPEED_2500  = 2500,
        FAL_SPEED_5000  = 5000,
        FAL_SPEED_10000 = 10000,
        FAL_SPEED_BUTT  = 0xffff,
    } fal_port_speed_t;

    typedef enum
    {
        FAL_CABLE_STATUS_NORMAL  = 0,
        FAL_CABLE_STATUS_SHORT   = 1,
        FAL_CABLE_STATUS_OPENED  = 2,
        FAL_CABLE_STATUS_INVALID = 3,
        FAL_CABLE_STATUS_CROSSOVERA = 4,
        FAL_CABLE_STATUS_CROSSOVERB = 5,
        FAL_CABLE_STATUS_CROSSOVERC = 6,
        FAL_CABLE_STATUS_CROSSOVERD = 7,
        FAL_CABLE_STATUS_LOW_MISMATCH = 8,
        FAL_CABLE_STATUS_HIGH_MISMATCH = 9,
        FAL_CABLE_STATUS_BUTT = 0xffff,
    } fal_cable_status_t;

#define FAL_ENABLE      1
#define FAL_DISABLE     0
#define FAL_MAX_PORT_NUMBER     8

//phy autoneg adv
#define FAL_PHY_ADV_10T_HD      0x01
#define FAL_PHY_ADV_10T_FD      0x02
#define FAL_PHY_ADV_100TX_HD    0x04
#define FAL_PHY_ADV_100TX_FD    0x08
//#define FAL_PHY_ADV_1000T_HD    0x100
#define FAL_PHY_ADV_1000T_FD    0x200
#define FAL_PHY_ADV_1000BX_HD    0x400
#define FAL_PHY_ADV_1000BX_FD    0x800
#define FAL_PHY_ADV_2500T_FD    0x1000
#define FAL_PHY_ADV_5000T_FD    0x2000
#define FAL_PHY_ADV_10000T_FD   0x4000

#define FAL_PHY_ADV_10G_R_FD    0x8000

#define FAL_PHY_ADV_FE_SPEED_ALL   \
    (FAL_PHY_ADV_10T_HD | FAL_PHY_ADV_10T_FD | FAL_PHY_ADV_100TX_HD |\
     FAL_PHY_ADV_100TX_FD)

#define FAL_PHY_ADV_GE_SPEED_ALL   \
    (FAL_PHY_ADV_10T_HD | FAL_PHY_ADV_10T_FD | FAL_PHY_ADV_100TX_HD |\
     FAL_PHY_ADV_100TX_FD | FAL_PHY_ADV_1000T_FD)

#define FAL_PHY_ADV_BX_SPEED_ALL   \
    (FAL_PHY_ADV_1000BX_HD | FAL_PHY_ADV_1000BX_FD |FAL_PHY_ADV_10G_R_FD)

#define FAL_PHY_ADV_XGE_SPEED_ALL   \
		(FAL_PHY_ADV_2500T_FD | FAL_PHY_ADV_5000T_FD | FAL_PHY_ADV_10000T_FD)

#define FAL_PHY_ADV_PAUSE       0x10
#define FAL_PHY_ADV_ASY_PAUSE   0x20
#define FAL_PHY_FE_ADV_ALL         \
    (FAL_PHY_ADV_FE_SPEED_ALL | FAL_PHY_ADV_PAUSE | FAL_PHY_ADV_ASY_PAUSE)
#define FAL_PHY_GE_ADV_ALL         \
    (FAL_PHY_ADV_GE_SPEED_ALL | FAL_PHY_ADV_PAUSE | FAL_PHY_ADV_ASY_PAUSE)

#define FAL_PHY_COMBO_ADV_ALL         \
    (FAL_PHY_ADV_BX_SPEED_ALL | FAL_PHY_ADV_GE_SPEED_ALL | FAL_PHY_ADV_XGE_SPEED_ALL |\
    FAL_PHY_ADV_PAUSE | FAL_PHY_ADV_ASY_PAUSE)

//phy capablity
#define FAL_PHY_AUTONEG_CAPS   0x01
#define FAL_PHY_100T2_HD_CAPS  0x02
#define FAL_PHY_100T2_FD_CAPS  0x04
#define FAL_PHY_10T_HD_CAPS    0x08
#define FAL_PHY_10T_FD_CAPS    0x10
#define FAL_PHY_100X_HD_CAPS   0x20
#define FAL_PHY_100X_FD_CAPS   0x40
#define FAL_PHY_100T4_CAPS     0x80
//#define FAL_PHY_1000T_HD_CAPS  0x100
#define FAL_PHY_1000T_FD_CAPS  0x200
//#define FAL_PHY_1000X_HD_CAPS  0x400
#define FAL_PHY_1000X_FD_CAPS  0x800

//phy partner capablity
#define FAL_PHY_PART_10T_HD   0x1
#define FAL_PHY_PART_10T_FD   0x2
#define FAL_PHY_PART_100TX_HD 0x4
#define FAL_PHY_PART_100TX_FD 0x8
//#define FAL_PHY_PART_1000T_HD 0x10
#define FAL_PHY_PART_1000T_FD 0x20

//phy interrupt flag
#define FAL_PHY_INTR_SPEED_CHANGE         0x1
#define FAL_PHY_INTR_DUPLEX_CHANGE        0x2
#define FAL_PHY_INTR_STATUS_UP_CHANGE     0x4
#define FAL_PHY_INTR_STATUS_DOWN_CHANGE   0x8
#define FAL_PHY_INTR_BX_FX_STATUS_UP_CHANGE   0x10
#define FAL_PHY_INTR_BX_FX_STATUS_DOWN_CHANGE   0x20
#define FAL_PHY_INTR_MEDIA_STATUS_CHANGE   0x40
#define FAL_PHY_INTR_WOL_STATUS   0x80
#define FAL_PHY_INTR_POE_STATUS   0x100

    typedef enum
    {
        FAL_NO_HEADER_EN = 0,
        FAL_ONLY_MANAGE_FRAME_EN,
        FAL_ALL_TYPE_FRAME_EN
    } fal_port_header_mode_t;

    typedef struct
    {
        a_uint16_t pair_a_status;
        a_uint16_t pair_b_status;
        a_uint16_t pair_c_status;
        a_uint16_t pair_d_status;
        a_uint32_t pair_a_len;
        a_uint32_t pair_b_len;
        a_uint32_t pair_c_len;
        a_uint32_t pair_d_len;
    } fal_port_cdt_t;

/*below is new add for malibu phy*/

/** Phy mdix mode */
typedef enum {
    PHY_MDIX_AUTO        = 0, /**< Auto MDI/MDIX */
    PHY_MDIX_MDI         = 1, /**< Fixed MDI */
    PHY_MDIX_MDIX        = 2  /**< Fixed MDIX */
} fal_port_mdix_mode_t;

/** Phy mdix status */
typedef enum {
    PHY_MDIX_STATUS_MDI        = 0, /**< Fixed MDI */
    PHY_MDIX_STATUS_MDIX       = 1  /**< Fixed MDIX */

} fal_port_mdix_status_t;

/** Phy master mode */
typedef enum {
    PHY_MASTER_MASTER    = 0,   /**< Phy manual MASTER configuration */
    PHY_MASTER_SLAVE     = 1,   /**< Phy manual SLAVE configuration */
    PHY_MASTER_AUTO      = 2    /**< Phy automatic MASTER/SLAVE configuration */
} fal_port_master_t;
/*qca808x_end*/
/** Phy preferred medium type */
typedef enum {
    PHY_MEDIUM_COPPER    = 0,   /**< Copper */
    PHY_MEDIUM_FIBER     = 1,   /**< Fiber */

} fal_port_medium_t;

/** Phy pages */
typedef enum {
    PHY_SGBX_PAGES      = 0,    /**< sgbx pages */
    PHY_COPPER_PAGES    = 1     /**< copper pages */

} fal_port_reg_pages_t;


/** Phy preferred Fiber mode */
typedef enum {
    PHY_FIBER_100FX     = 0,    /**< 100FX fiber mode */
    PHY_FIBER_1000BX    = 1,    /**< 1000BX fiber mode */
    PHY_FIBER_10G_R = 2,	/**< 10G-R fiber mode */

} fal_port_fiber_mode_t;

/** Phy reset status */
typedef enum {
    PHY_RESET_DONE        = 0,  /**< Phy reset done */
    PHY_RESET_BUSY        = 1   /**< Phy still in reset process */
} fal_port_reset_status_t;

/** Phy auto-negotiation status */
typedef enum {
    PHY_AUTO_NEG_STATUS_BUSY   = 0, /**< Phy still in auto-negotiation process */
    PHY_AUTO_NEG_STATUS_DONE   = 1  /**< Phy auto-negotiation done */
} fal_port_auto_neg_status_t;
/*qca808x_start*/

/** Phy interface mode */
	typedef enum {
		PHY_PSGMII_BASET = 0,
				/**< PSGMII mode */
		PHY_PSGMII_BX1000 = 1,
				/**< PSGMII BX1000 mode */
		PHY_PSGMII_FX100 = 2,
				/**< PSGMII FX100 mode */
		PHY_PSGMII_AMDET = 3,
				/**< PSGMII Auto mode */
		PHY_SGMII_BASET = 4,
				/**< SGMII mode */
		PORT_QSGMII,
				/**<QSGMII mode*/
		PORT_SGMII_PLUS,
				/**<PORT_SGMII_PLUS mode*/
		PORT_USXGMII,
				/**<PORT_USXGMII mode*/
		PORT_10GBASE_R,
				/**<PORT_10GBASE_R mode*/
		PORT_RGMII_BASET,
				/**< RGMII mode */
		PORT_RGMII_BX1000,
				/**< RGMII BX1000 mode */
		PORT_RGMII_FX100,
				/**< RGMII FX100 mode */
		PORT_RGMII_AMDET,
				/**< RGMII Auto mode */
		PHY_PSGMII_FIBER,
				/** <PHY_PSGMII_FIBER mode */
		PORT_SGMII_FIBER,
				/** <PORT SGMII_FIBER mode*/
		PORT_INTERFACE_MODE_MAX = 0xFF
	} fal_port_interface_mode_t;

/** Phy counter information */
typedef struct {
	a_uint32_t RxGoodFrame;
	a_uint32_t RxBadCRC;
	a_uint32_t TxGoodFrame;
	a_uint32_t TxBadCRC;
	a_uint32_t SysRxGoodFrame;
	a_uint32_t SysRxBadCRC;
	a_uint32_t SysTxGoodFrame;
	a_uint32_t SysTxBadCRC;
} fal_port_counter_info_t;

/*above is new add for malibu phy*/
/*qca808x_end*/
/* phy eee */
#define FAL_PHY_EEE_10BASE_T  0x1
#define FAL_PHY_EEE_100BASE_T  0x2
#define FAL_PHY_EEE_1000BASE_T  0x4
#define FAL_PHY_EEE_2500BASE_T  0x8
#define FAL_PHY_EEE_5000BASE_T  0x10
#define FAL_PHY_EEE_10000BASE_T  0x20
typedef struct {
	a_uint32_t enable;
	a_uint32_t capability;
	a_uint32_t lpi_sleep_timer;
	a_uint32_t advertisement;
	a_uint32_t lpi_tx_enable;
	a_uint32_t eee_status;
	a_uint32_t lpi_wakeup_timer;
	a_uint32_t link_partner_advertisement;
} fal_port_eee_cfg_t;
enum
{
	/*port contorl*/
	FUNC_ADPT_PORT_LOCAL_LOOPBACK_GET = 0,
	FUNC_ADPT_PORT_AUTONEG_RESTART,
	FUNC_ADPT_PORT_DUPLEX_SET,
	FUNC_ADPT_PORT_RXMAC_STATUS_GET,
	FUNC_ADPT_PORT_CDT,
	FUNC_ADPT_PORT_TXMAC_STATUS_SET,
	FUNC_ADPT_PORT_COMBO_FIBER_MODE_SET,
	FUNC_ADPT_PORT_COMBO_MEDIUM_STATUS_GET,
	FUNC_ADPT_PORT_MAGIC_FRAME_MAC_SET,
	FUNC_ADPT_PORT_POWERSAVE_SET,
	FUNC_ADPT_PORT_HIBERNATE_SET,
	FUNC_ADPT_PORT_8023AZ_GET,
	FUNC_ADPT_PORT_RXFC_STATUS_GET,
	FUNC_ADPT_PORT_TXFC_STATUS_GET,
	FUNC_ADPT_PORT_REMOTE_LOOPBACK_SET,
	FUNC_ADPT_PORT_FLOWCTRL_SET,
	FUNC_ADPT_PORT_MRU_SET,
	FUNC_ADPT_PORT_AUTONEG_STATUS_GET,
	FUNC_ADPT_PORT_TXMAC_STATUS_GET,
	FUNC_ADPT_PORT_MDIX_GET,
	FUNC_ADPT_PORTS_LINK_STATUS_GET,
	FUNC_ADPT_PORT_MAC_LOOPBACK_SET,
	FUNC_ADPT_PORT_PHY_ID_GET,
	FUNC_ADPT_PORT_MRU_GET,
	FUNC_ADPT_PORT_POWER_ON,
	FUNC_ADPT_PORT_SPEED_SET,
	FUNC_ADPT_PORT_INTERFACE_MODE_GET,
	FUNC_ADPT_PORT_DUPLEX_GET,
	FUNC_ADPT_PORT_AUTONEG_ADV_GET,
	FUNC_ADPT_PORT_MDIX_STATUS_GET,
	FUNC_ADPT_PORT_MTU_SET,
	FUNC_ADPT_PORT_LINK_STATUS_GET,

	FUNC_ADPT_PORT_8023AZ_SET,
	FUNC_ADPT_PORT_POWERSAVE_GET,
	FUNC_ADPT_PORT_COMBO_PREFER_MEDIUM_GET,
	FUNC_ADPT_PORT_COMBO_PREFER_MEDIUM_SET,
	FUNC_ADPT_PORT_POWER_OFF,
	FUNC_ADPT_PORT_TXFC_STATUS_SET,
	FUNC_ADPT_PORT_COUNTER_SET,
	FUNC_ADPT_PORT_COMBO_FIBER_MODE_GET,
	FUNC_ADPT_PORT_LOCAL_LOOPBACK_SET,
	FUNC_ADPT_PORT_WOL_STATUS_SET,
	FUNC_ADPT_PORT_MAGIC_FRAME_MAC_GET,
	FUNC_ADPT_PORT_FLOWCTRL_GET,
	FUNC_ADPT_PORT_RXMAC_STATUS_SET,
	FUNC_ADPT_PORT_COUNTER_GET,
	FUNC_ADPT_PORT_INTERFACE_MODE_SET,
	FUNC_ADPT_PORT_MAC_LOOPBACK_GET,
	FUNC_ADPT_PORT_HIBERNATE_GET,
	FUNC_ADPT_PORT_AUTONEG_ADV_SET,
	FUNC_ADPT_PORT_REMOTE_LOOPBACK_GET,
	FUNC_ADPT_PORT_COUNTER_SHOW,
	FUNC_ADPT_PORT_AUTONEG_ENABLE,
	FUNC_ADPT_PORT_MTU_GET,
	FUNC_ADPT_PORT_INTERFACE_MODE_STATUS_GET,
	FUNC_ADPT_PORT_RESET,
	FUNC_ADPT_PORT_RXFC_STATUS_SET,
	FUNC_ADPT_PORT_SPEED_GET,
	FUNC_ADPT_PORT_MDIX_SET,
	FUNC_ADPT_PORT_WOL_STATUS_GET,
	FUNC_ADPT_PORT_MAX_FRAME_SIZE_SET,
	FUNC_ADPT_PORT_MAX_FRAME_SIZE_GET,
	FUNC_ADPT_PORT_SOURCE_FILTER_GET,
	FUNC_ADPT_PORT_SOURCE_FILTER_SET,

	FUNC_ADPT_PORT_INTERFACE_MODE_APPLY,
	FUNC_ADPT_PORT_INTERFACE_3AZ_STATUS_SET,
	FUNC_ADPT_PORT_INTERFACE_3AZ_STATUS_GET,
	FUNC_ADPT_PORT_PROMISC_MODE_SET,
	FUNC_ADPT_PORT_PROMISC_MODE_GET,
	FUNC_ADPT_PORT_FLOWCTRL_FORCEMODE_SET,
	FUNC_ADPT_PORT_FLOWCTRL_FORCEMODE_GET,
};

	typedef enum {
		MRU_MTU_FORWARD = 0,
		MRU_MTU_DROP = 1,
		MRU_MTU_CPYCPU = 2,
		MRU_MTU_RDTCPU = 3,
	} fal_mtu_action_t;

	typedef enum {
		MRU_MRU_FORWARD = 0,
		MRU_MRU_DROP = 1,
		MRU_MRU_CPYCPU = 2,
		MRU_MRU_RDTCPU = 3,
	} fal_mru_action_t;

	typedef struct {
		a_uint32_t		mtu_size;
		fal_mtu_action_t	action;
	} fal_mtu_ctrl_t;

	typedef struct {
		a_uint32_t		mru_size;
		fal_mru_action_t	action;
	} fal_mru_ctrl_t;

	typedef enum {
		FAL_SRC_FILTER_MODE_VP = 0,
		FAL_SRC_FILTER_MODE_PHYSICAL = 1,
	} fal_src_filter_mode_t;

	typedef struct {
		a_bool_t	src_filter_enable;
		fal_src_filter_mode_t	src_filter_mode;
	} fal_src_filter_config_t;

	typedef struct {
		a_bool_t enable;
		a_bool_t crc_stripped;
		a_uint32_t loopback_rate; /*Mpps*/
	}fal_loopback_config_t;

	sw_error_t
	fal_port_max_frame_size_set(a_uint32_t dev_id, fal_port_t port_id,
			a_uint32_t max_frame);

	sw_error_t
	fal_port_max_frame_size_get(a_uint32_t dev_id, fal_port_t port_id,
			a_uint32_t *max_frame);

	sw_error_t
	fal_port_mtu_set(a_uint32_t dev_id, fal_port_t port_id,
			fal_mtu_ctrl_t *ctrl);

	sw_error_t
	fal_port_mtu_get(a_uint32_t dev_id, fal_port_t port_id,
			fal_mtu_ctrl_t *ctrl);

	sw_error_t
	fal_port_mru_set(a_uint32_t dev_id, fal_port_t port_id,
			fal_mru_ctrl_t *ctrl);

	sw_error_t
	fal_port_mru_get(a_uint32_t dev_id, fal_port_t port_id,
			fal_mru_ctrl_t *ctrl);
/*qca808x_start*/
    sw_error_t
    fal_port_duplex_set(a_uint32_t dev_id, fal_port_t port_id,
                        fal_port_duplex_t duplex);



    sw_error_t
    fal_port_duplex_get(a_uint32_t dev_id, fal_port_t port_id,
                        fal_port_duplex_t * pduplex);



    sw_error_t
    fal_port_speed_set(a_uint32_t dev_id, fal_port_t port_id,
                       fal_port_speed_t speed);



    sw_error_t
    fal_port_speed_get(a_uint32_t dev_id, fal_port_t port_id,
                       fal_port_speed_t * pspeed);



    sw_error_t
    fal_port_autoneg_status_get(a_uint32_t dev_id, fal_port_t port_id,
                                a_bool_t * status);



    sw_error_t
    fal_port_autoneg_enable(a_uint32_t dev_id, fal_port_t port_id);



    sw_error_t
    fal_port_autoneg_restart(a_uint32_t dev_id, fal_port_t port_id);



    sw_error_t
    fal_port_autoneg_adv_set(a_uint32_t dev_id, fal_port_t port_id,
                             a_uint32_t autoadv);



    sw_error_t
    fal_port_autoneg_adv_get(a_uint32_t dev_id, fal_port_t port_id,
                             a_uint32_t * autoadv);


/*qca808x_end*/
    sw_error_t
    fal_port_hdr_status_set(a_uint32_t dev_id, fal_port_t port_id,
                            a_bool_t enable);



    sw_error_t
    fal_port_hdr_status_get(a_uint32_t dev_id, fal_port_t port_id,
                            a_bool_t * enable);



    sw_error_t
    fal_port_flowctrl_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable);



    sw_error_t
    fal_port_flowctrl_get(a_uint32_t dev_id, fal_port_t port_id, a_bool_t *enable);


    sw_error_t
    fal_port_flowctrl_forcemode_set(a_uint32_t dev_id, fal_port_t port_id,
                                    a_bool_t enable);


    sw_error_t
    fal_port_flowctrl_forcemode_get(a_uint32_t dev_id, fal_port_t port_id,
                                    a_bool_t * enable);


    sw_error_t
    fal_port_powersave_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable);


    sw_error_t
    fal_port_powersave_get(a_uint32_t dev_id, fal_port_t port_id, a_bool_t *enable);

/*qca808x_start*/
    sw_error_t
    fal_port_hibernate_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable);


    sw_error_t
    fal_port_hibernate_get(a_uint32_t dev_id, fal_port_t port_id, a_bool_t *enable);
    sw_error_t
    fal_port_cdt(a_uint32_t dev_id, fal_port_t port_id, a_uint32_t mdi_pair,
                 fal_cable_status_t *cable_status, a_uint32_t *cable_len);

/*qca808x_end*/
    sw_error_t
    fal_port_rxhdr_mode_set(a_uint32_t dev_id, fal_port_t port_id,
                            fal_port_header_mode_t mode);


    sw_error_t
    fal_port_rxhdr_mode_get(a_uint32_t dev_id, fal_port_t port_id,
                            fal_port_header_mode_t * mode);


    sw_error_t
    fal_port_txhdr_mode_set(a_uint32_t dev_id, fal_port_t port_id,
                            fal_port_header_mode_t mode);


    sw_error_t
    fal_port_txhdr_mode_get(a_uint32_t dev_id, fal_port_t port_id,
                            fal_port_header_mode_t * mode);


    sw_error_t
    fal_header_type_set(a_uint32_t dev_id, a_bool_t enable, a_uint32_t type);


    sw_error_t
    fal_header_type_get(a_uint32_t dev_id, a_bool_t * enable, a_uint32_t * type);


    sw_error_t
    fal_port_txmac_status_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable);


    sw_error_t
    fal_port_txmac_status_get(a_uint32_t dev_id, fal_port_t port_id, a_bool_t * enable);


    sw_error_t
    fal_port_rxmac_status_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable);


    sw_error_t
    fal_port_rxmac_status_get(a_uint32_t dev_id, fal_port_t port_id, a_bool_t * enable);


    sw_error_t
    fal_port_txfc_status_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable);


    sw_error_t
    fal_port_txfc_status_get(a_uint32_t dev_id, fal_port_t port_id, a_bool_t * enable);


    sw_error_t
    fal_port_rxfc_status_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable);


    sw_error_t
    fal_port_rxfc_status_get(a_uint32_t dev_id, fal_port_t port_id, a_bool_t * enable);


    sw_error_t
    fal_port_bp_status_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable);


    sw_error_t
    fal_port_bp_status_get(a_uint32_t dev_id, fal_port_t port_id, a_bool_t * enable);


    sw_error_t
    fal_port_link_forcemode_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable);


    sw_error_t
    fal_port_link_forcemode_get(a_uint32_t dev_id, fal_port_t port_id, a_bool_t * enable);

/*qca808x_start*/
    sw_error_t
    fal_port_link_status_get(a_uint32_t dev_id, fal_port_t port_id, a_bool_t * status);

    sw_error_t
    fal_ports_link_status_get(a_uint32_t dev_id, a_uint32_t * status);
/*qca808x_end*/
    sw_error_t
    fal_port_mac_loopback_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable);


    sw_error_t
    fal_port_mac_loopback_get(a_uint32_t dev_id, fal_port_t port_id, a_bool_t * enable);

	sw_error_t
	fal_port_congestion_drop_set(a_uint32_t dev_id, fal_port_t port_id, a_uint32_t queue_id, a_bool_t enable);


	sw_error_t
	fal_port_congestion_drop_get(a_uint32_t dev_id, fal_port_t port_id, a_uint32_t queue_id, a_bool_t * enable);

	sw_error_t
	fal_ring_flow_ctrl_thres_set(a_uint32_t dev_id, a_uint32_t ring_id, a_uint8_t on_thres, a_uint8_t off_thres);


	sw_error_t
	fal_ring_flow_ctrl_thres_get(a_uint32_t dev_id, a_uint32_t ring_id, a_uint8_t *on_thres, a_uint8_t *off_thres);
/*qca808x_start*/
    sw_error_t
    fal_port_8023az_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable);


    sw_error_t
    fal_port_8023az_get(a_uint32_t dev_id, fal_port_t port_id, a_bool_t *enable);

    sw_error_t
    fal_port_mdix_set(a_uint32_t dev_id, fal_port_t port_id,
                       fal_port_mdix_mode_t mode);
    sw_error_t
    fal_port_mdix_get(a_uint32_t dev_id, fal_port_t port_id,
                        fal_port_mdix_mode_t * mode);
   sw_error_t
   fal_port_mdix_status_get(a_uint32_t dev_id, fal_port_t port_id,
                        fal_port_mdix_status_t * mode);
/*qca808x_end*/
   sw_error_t
   fal_port_combo_prefer_medium_set(a_uint32_t dev_id, a_uint32_t port_id,
                          fal_port_medium_t medium);
   sw_error_t
   fal_port_combo_prefer_medium_get(a_uint32_t dev_id, a_uint32_t port_id,
                          fal_port_medium_t * medium);
  sw_error_t
  fal_port_combo_medium_status_get(a_uint32_t dev_id, a_uint32_t port_id,
                          fal_port_medium_t * medium);
  sw_error_t
  fal_port_combo_fiber_mode_set(a_uint32_t dev_id, a_uint32_t port_id,
                          fal_port_fiber_mode_t  mode);
  sw_error_t
  fal_port_combo_fiber_mode_get(a_uint32_t dev_id, a_uint32_t port_id,
                          fal_port_fiber_mode_t * mode);
/*qca808x_start*/
      sw_error_t
    fal_port_local_loopback_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable);

    sw_error_t
    fal_port_local_loopback_get(a_uint32_t dev_id, fal_port_t port_id, a_bool_t *enable);

         sw_error_t
    fal_port_remote_loopback_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable);

    sw_error_t
    fal_port_remote_loopback_get(a_uint32_t dev_id, fal_port_t port_id, a_bool_t *enable);

     sw_error_t
	    fal_port_reset(a_uint32_t dev_id, fal_port_t port_id);

	 sw_error_t
	    fal_port_power_off(a_uint32_t dev_id, fal_port_t port_id);

	 sw_error_t
	    fal_port_power_on(a_uint32_t dev_id, fal_port_t port_id);

    sw_error_t
    fal_port_magic_frame_mac_set (a_uint32_t dev_id, fal_port_t port_id,
				   fal_mac_addr_t * mac);

   sw_error_t
   fal_port_magic_frame_mac_get (a_uint32_t dev_id, fal_port_t port_id,
				   fal_mac_addr_t * mac);

 sw_error_t
    fal_port_phy_id_get (a_uint32_t dev_id, fal_port_t port_id,
		      a_uint16_t * org_id, a_uint16_t * rev_id);

 sw_error_t
    fal_port_wol_status_set (a_uint32_t dev_id, fal_port_t port_id,
			      a_bool_t enable);

 sw_error_t
    fal_port_wol_status_get (a_uint32_t dev_id, fal_port_t port_id,
			      a_bool_t * enable);
/*qca808x_end*/
 sw_error_t
    fal_port_interface_mode_set(a_uint32_t dev_id, fal_port_t port_id, fal_port_interface_mode_t mode);

 sw_error_t
    fal_port_interface_mode_apply(a_uint32_t dev_id);

 sw_error_t
    fal_port_interface_mode_get (a_uint32_t dev_id, fal_port_t port_id,
			      fal_port_interface_mode_t * mode);
 /*qca808x_start*/
 sw_error_t
    fal_port_interface_mode_status_get (a_uint32_t dev_id, fal_port_t port_id,
			      fal_port_interface_mode_t * mode);
 sw_error_t
	fal_debug_phycounter_set(a_uint32_t dev_id, fal_port_t port_id,
			a_bool_t enable);
 
 sw_error_t
	fal_debug_phycounter_get(a_uint32_t dev_id, fal_port_t port_id,
			a_bool_t * enable);
 sw_error_t
	fal_debug_phycounter_show (a_uint32_t dev_id, fal_port_t port_id,
				  fal_port_counter_info_t * port_counter_info);
/*qca808x_end*/
 sw_error_t
       fal_port_source_filter_enable(a_uint32_t dev_id,
                       fal_port_t port_id, a_bool_t enable);
 sw_error_t
        fal_port_source_filter_status_get(a_uint32_t dev_id,
                       fal_port_t port_id, a_bool_t * enable);
 sw_error_t
 fal_port_interface_3az_status_set(a_uint32_t dev_id, fal_port_t port_id,
		 a_bool_t enable);
 
 sw_error_t
 fal_port_interface_3az_status_get(a_uint32_t dev_id, fal_port_t port_id,
		 a_bool_t * enable);
 sw_error_t
 fal_port_promisc_mode_set(a_uint32_t dev_id,fal_port_t port_id,a_bool_t enable);
 sw_error_t
 fal_port_promisc_mode_get(a_uint32_t dev_id,fal_port_t port_id,a_bool_t *enable);
 sw_error_t
fal_port_interface_eee_cfg_set(a_uint32_t dev_id, fal_port_t port_id,
	fal_port_eee_cfg_t *port_eee_cfg);
sw_error_t
fal_port_interface_eee_cfg_get(a_uint32_t dev_id, fal_port_t port_id,
	fal_port_eee_cfg_t *port_eee_cfg);

sw_error_t
fal_port_source_filter_config_get(a_uint32_t dev_id,
	fal_port_t port_id, fal_src_filter_config_t * src_filter_config);

sw_error_t
fal_port_source_filter_config_set(a_uint32_t dev_id,
	fal_port_t port_id, fal_src_filter_config_t *src_filter_config);

sw_error_t
fal_switch_port_loopback_get(a_uint32_t dev_id, fal_port_t port_id,
	fal_loopback_config_t *loopback_cfg);

sw_error_t
fal_switch_port_loopback_set(a_uint32_t dev_id, fal_port_t port_id,
	fal_loopback_config_t *loopback_cfg);

/*qca808x_start*/
#ifdef __cplusplus
}
#endif                          /* __cplusplus */
#endif                          /* _FAL_PORTCTRL_H_ */
/**
 * @}
 */
 /*qca808x_end*/
