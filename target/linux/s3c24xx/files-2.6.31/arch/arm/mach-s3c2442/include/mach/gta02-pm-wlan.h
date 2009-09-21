#ifndef __MACH_GTA02_PM_WLAN_H
#define __MACH_GTA02_PM_WLAN_H

void gta02_wlan_reset(int assert_reset);
int gta02_wlan_query_rfkill_lock(void);
void gta02_wlan_query_rfkill_unlock(void);
void gta02_wlan_set_rfkill_cb(int (*cb)(void *user, int on), void *user);
void gta02_wlan_clear_rfkill_cb(void);

#endif /* __MACH_GTA02_PM_WLAN_H */
