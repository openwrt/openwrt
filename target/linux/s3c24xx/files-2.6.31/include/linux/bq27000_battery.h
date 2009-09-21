#ifndef __BQ27000_BATTERY_H__
#define __BQ27000_BATTERY_H__

void bq27000_charging_state_change(struct platform_device *pdev);

struct bq27000_platform_data {
	const char 	*name;
	int		rsense_mohms;
	int (*hdq_read)(int);
	int (*hdq_write)(int, u8);
	int (*hdq_initialized)(void);
	int (*get_charger_online_status)(void);
	int (*get_charger_active_status)(void);
};

#endif
