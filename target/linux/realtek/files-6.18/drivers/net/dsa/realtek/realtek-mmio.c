// SPDX-License-Identifier: GPL-2.0-only

#include "realtek.h"
#include "realtek-mmio.h"
#include "rtl83xx.h"

static const struct realtek_interface_info realtek_mmio_info = {
	.syscon = true,
};

/**
 * realtek_mmio_probe() - Probe a platform device for an MMIO-connected switch
 * @pdev: platform_device to probe on.
 *
 * This function should be used as the .probe in a platform_driver. After
 * calling the common probe function for both interfaces, it calls a common
 * function to register the DSA switch.
 *
 * Context: Can sleep. Takes and releases priv->map_lock.
 * Return: Returns 0 on success, a negative error on failure.
 */
int realtek_mmio_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct realtek_priv *priv;
	int ret;

	priv = rtl83xx_probe(dev, &realtek_mmio_info);
	if (IS_ERR(priv))
		return PTR_ERR(priv);

	ret = rtl83xx_register_switch(priv);
	if (ret) {
		rtl83xx_remove(priv);
		return ret;
	}

	return 0;
}
EXPORT_SYMBOL_NS_GPL(realtek_mmio_probe, "REALTEK_DSA");

/**
 * realtek_smi_remove() - Remove the driver of a SMI-connected switch
 * @pdev: platform_device to be removed.
 *
 * This function should be used as the .remove in a platform_driver. First
 * it unregisters the DSA switch and then it calls the common remove function.
 *
 * Context: Can sleep.
 * Return: Nothing.
 */
void realtek_mmio_remove(struct platform_device *pdev)
{
	struct realtek_priv *priv = platform_get_drvdata(pdev);

	if (!priv)
		return;

	rtl83xx_unregister_switch(priv);

	rtl83xx_remove(priv);
}
EXPORT_SYMBOL_NS_GPL(realtek_mmio_remove, "REALTEK_DSA");

/**
 * realtek_mmio_shutdown() - Shutdown the driver of a MMIO-connected switch
 * @pdev: platform_device shutting down.
 *
 * This function should be used as the .shutdown in a platform_driver. It calls
 * the common shutdown function.
 *
 * Context: Can sleep.
 * Return: Nothing.
 */
void realtek_mmio_shutdown(struct platform_device *pdev)
{
	struct realtek_priv *priv = platform_get_drvdata(pdev);

	if (!priv)
		return;

	rtl83xx_shutdown(priv);
}
EXPORT_SYMBOL_NS_GPL(realtek_mmio_shutdown, "REALTEK_DSA");
