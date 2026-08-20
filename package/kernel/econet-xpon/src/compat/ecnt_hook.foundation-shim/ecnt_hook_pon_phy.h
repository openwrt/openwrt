/*
 * compat shim for <ecnt_hook/ecnt_hook_pon_phy.h>
 * Optical-PHY event enums + the event payload shared between phy/* and the
 * MAC driver.
 */
#ifndef _COMPAT_ECNT_HOOK_PON_PHY_H_
#define _COMPAT_ECNT_HOOK_PON_PHY_H_
#include "ecnt_hook/ecnt_common.h"

/* PHY event types (drive phy_event_dispatch / handle_hw_irq_event) */
typedef enum {
	PHY_EVENT_PHYRDY_INT = 0,
	PHY_EVENT_PHY_ILLG_INT,
	PHY_EVENT_PHY_LOF_INT,
	PHY_EVENT_I2CM_INT,
	PHY_EVENT_TF_INT,
	PHY_EVENT_TRANS_INT,
	PHY_EVENT_TRANS_LOS_INT,
	PHY_EVENT_TRANS_LOS_ILLG_INT,
	PHY_EVENT_TRANS_SD_FAIL_INT,
	PHY_EVENT_TX_POWER_ON,
	PHY_EVENT_TX_POWER_OFF,
	PHY_EVENT_CALIBRATION_START,
	PHY_EVENT_CALIBRATION_STOP,
	PHY_EVENT_START_ROGUE_MODE,
	PHY_EVENT_STOP_ROGUE_MODE,
} PHY_Event_Type_t;

/* event source */
typedef enum {
	PON_PHY_EVENT_SOURCE_HW_IRQ = 0,
	PON_PHY_EVENT_SOURCE_SW_POLL,
} PHY_Event_Source_t;

typedef struct {
	PHY_Event_Type_t   id;
	PHY_Event_Source_t src;
} PON_PHY_Event_data_t;

#endif /* _COMPAT_ECNT_HOOK_PON_PHY_H_ */
