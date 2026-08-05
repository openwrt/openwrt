/*
 * compat shim for <asm/tc3162/ledcetrl.h>  (TrendChip LED control)
 * hwNAT/SDK LED framework is not present; map to no-ops for the compile
 * milestone.  TODO EN7528: wire to gpio-leds / led trigger
 */
#ifndef _COMPAT_ASM_TC3162_LEDCETRL_H_
#define _COMPAT_ASM_TC3162_LEDCETRL_H_

/* LED identifiers referenced by the GPON/PHY code */
enum led_ctrl_id {
	LED_XPON_STATUS = 0,
	LED_XPON_TRYING_STATUS,
	LED_XPON_UNREG_STATUS,
	LED_INTERNET_STATUS,
	LED_INTERNET_TRYING_STATUS,
	LED_INTERNET_ACT_STATUS,
	LED_PHY_TX_POWER_DISABLE,
	LED_PHY_VCC_DISABLE,
};

static inline void ledTurnOn(int led)  { (void)led; }
static inline void ledTurnOff(int led) { (void)led; }
static inline void ledcetrl(int led, int on) { (void)led; (void)on; }

#endif /* _COMPAT_ASM_TC3162_LEDCETRL_H_ */
