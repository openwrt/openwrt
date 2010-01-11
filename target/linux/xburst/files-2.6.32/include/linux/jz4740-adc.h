
#ifndef __LINUX_JZ4740_ADC
#define __LINUX_JZ4740_ADC

#include <linux/device.h>

enum jz_adc_battery_scale {
	JZ_ADC_BATTERY_SCALE_2V5, /* Mesures voltages up to 2.5V */
	JZ_ADC_BATTERY_SCALE_7V5, /* Mesures voltages up to 7.5V */
};

/*
 * jz4740_adc_read_battery_voltage - Read battery voltage from the ADC PBAT pin
 * @dev: Pointer to a jz4740-adc device
 * @scale: Whether to use 2.5V or 7.5V scale
 *
 * Returns: Battery voltage in mircovolts
 *
 * Context: Process
*/
long jz4740_adc_read_battery_voltage(struct device *dev,
					enum jz_adc_battery_scale scale);


#endif
