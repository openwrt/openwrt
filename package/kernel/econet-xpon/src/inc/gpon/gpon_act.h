#ifndef _GPON_ACT_H_
#define _GPON_ACT_H_

#include <xmcs/xmcs_const.h>
#include <linux/timer.h>	/* PORT 6.18: struct timer_list for timer_setup() callbacks */

void gpon_act_deactive_onu(void) ;
void gpon_act_to1_timer_expires(struct timer_list *t) ;	/* PORT 6.18: timer_setup() sig */
void gpon_act_to2_timer_expires(struct timer_list *t) ;	/* PORT 6.18: timer_setup() sig */
void gpon_act_change_gpon_state(const uint state) ;	/* PORT: match gpon_act.c definition */
uint gpon_act_get_gpon_state(void) ;
void gpon_act_led_config(void);
int gpon_act_init(void) ;
int gpon_act_deinit(void);


#endif /* _GPON_ACT_H_ */

