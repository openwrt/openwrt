#ifndef _ATHEROS_GPIO_H_
#define _ATHEROS_GPIO_H_

#include "ar531x.h"

/* Common AR531X global variables */
/* extern u32 ar531x_gpio_intr_Mask; */

/* AR5312 exported routines */
#ifdef CONFIG_ATHEROS_AR5312
asmlinkage void ar5312_gpio_irq_dispatch(void);
#endif

/* AR5315 exported routines */
#ifdef CONFIG_ATHEROS_AR5315
asmlinkage void ar5315_gpio_irq_dispatch(void);
#endif

/*
 * Wrappers for the generic GPIO layer
 */

/* Sets a gpio to input, or returns ENXIO for non-existent gpio */
static inline int gpio_direction_input(unsigned gpio) {
	DO_AR5312(	if (gpio > AR531X_NUM_GPIO) {			\
				return -ENXIO;				\
			} else {					\
				sysRegWrite(AR531X_GPIO_CR,		\
					sysRegRead(AR531X_GPIO_CR) |	\
					AR531X_GPIO_CR_I(gpio) );	\
				return 0;				\
			}						\
	)
	DO_AR5315(	if (gpio > AR5315_NUM_GPIO) {			\
				return -ENXIO;				\
			} else {					\
				sysRegWrite(AR5315_GPIO_CR,		\
					( sysRegRead(AR5315_GPIO_CR) &	\
					  ~(AR5315_GPIO_CR_M(gpio)) ) |	\
					  AR5315_GPIO_CR_I(gpio) );	\
				return 0;				\
			}						\
	)
	return -ENXIO;
}

/* Sets a gpio to output with value, or returns ENXIO for non-existent gpio */
static inline int gpio_direction_output(unsigned gpio, int value) {
	DO_AR5312(	if (gpio > AR531X_NUM_GPIO) {			\
				return -ENXIO;				\
			} else {					\
				sysRegWrite(AR531X_GPIO_DO,		\
					( (sysRegRead(AR531X_GPIO_DO) &	\
					  ~(1 << gpio) ) |		\
					  ((value!=0) << gpio)) );	\
				sysRegWrite(AR531X_GPIO_CR,		\
					( sysRegRead(AR531X_GPIO_CR) &	\
					  ~(AR531X_GPIO_CR_M(gpio)) )); \
				return 0;				\
			}						\
	)
	DO_AR5315(	if (gpio > AR5315_NUM_GPIO) {			\
				return -ENXIO;				\
			} else {					\
				sysRegWrite(AR5315_GPIO_DO,		\
					( (sysRegRead(AR5315_GPIO_DO) &	\
					  ~(1 << gpio)) |		\
					  ((value!=0) << gpio)) );	\
				sysRegWrite(AR5315_GPIO_CR,		\
					sysRegRead(AR5315_GPIO_CR) |	\
					AR5315_GPIO_CR_O(gpio) );	\
				return 0;				\
			}						\
	)
	return -ENXIO;
}

/* Reads the gpio pin.  Unchecked function */
static inline int gpio_get_value(unsigned gpio) {
	DO_AR5312(return (sysRegRead(AR531X_GPIO_DI) & (1 << gpio));)
	DO_AR5315(return (sysRegRead(AR5315_GPIO_DI) & (1 << gpio));)
	return 0;
}

/* Writes to the gpio pin.  Unchecked function */
static inline void gpio_set_value(unsigned gpio, int value) {
	DO_AR5312(	sysRegWrite(AR531X_GPIO_DO,	\
			( (sysRegRead(AR531X_GPIO_DO) &	\
			  ~(1 << gpio)) |		\
			  ((value!=0) << gpio)) );	\
	)
	DO_AR5315(	sysRegWrite(AR5315_GPIO_DO,	\
			( (sysRegRead(AR5315_GPIO_DO) &	\
			  ~(1 << gpio)) |		\
			  ((value!=0) << gpio)) );	\
	)
}

static inline int gpio_request(unsigned gpio, const char *label) {
	return 0;
}

static inline void gpio_free(unsigned gpio) {
}

/* Returns IRQ to attach for gpio.  Unchecked function */
static inline int gpio_to_irq(unsigned gpio) {
	return AR531X_GPIO_IRQ(gpio);
}

/* Returns gpio for IRQ attached.  Unchecked function */
static inline int irq_to_gpio(unsigned irq) {
	return (irq - (AR531X_GPIO_IRQ(0)));
}

#include <asm-generic/gpio.h> /* cansleep wrappers */

#endif
