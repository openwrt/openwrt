#ifndef LINUX_UCMB_H_
#define LINUX_UCMB_H_

struct ucmb_platform_data {
	const char *name;
	unsigned long gpio_cs;
	unsigned int gpio_sck;
	unsigned int gpio_miso;
	unsigned int gpio_mosi;

	struct platform_device *pdev; /* internal */
};

#endif /* LINUX_UCMB_H_ */
