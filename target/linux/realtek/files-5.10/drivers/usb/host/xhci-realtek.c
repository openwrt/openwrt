// SPDX-License-Identifier: GPL-2.0
/*
 * xHCI host controller driver for Realtek SoCs
 *
 * Copyright (C) 2020      Markus Stockhausen <markus.stockhausen@gmx.de>
 * Copyright (C) 2006-2012 Tony Wu <tonywu@realtek.com>
 */

#include <linux/delay.h>
#include <linux/device.h>
#include <linux/dma-mapping.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/types.h>
#include <linux/timer.h>
#include <linux/usb.h>

#include "xhci.h"

#define USB2_PHY_DELAY	{ mdelay(10); }
#define USB3_PHY_DELAY	{ mdelay(5); }

#define SYS_USB_PHY 	((void *)0xb8000090) 	
#define R_B804C280	((void *)0xb804c280)
#define HW_STRAP	((void *)0xb8000008)
#define BSP_USB3_EXT	((void *)0xb8140000)
#define U3_IPCFG	((void *)0xb8140008)
#define CLK_MGR		((void *)0xb8000010)	
#define I2C_BASE	((void *)0xb8140000)
#define I2C_REG4	((void *)0xb8140010)
#define I2C_REG5	((void *)0xb8140014)

struct rtxh_dev {
	struct device	*dev;
	struct usb_hcd	*hcd;
	void __iomem	*base;
};

void rtxh_u2_set_phy(unsigned char reg, unsigned char val)
{	
 	int oneportsel=1;
	unsigned int tmp;
	unsigned char reg_l = (reg & 0x0f);
	unsigned char reg_h = (reg & 0xf0) >> 4;

	tmp = ioread32(SYS_USB_PHY); 
	tmp = tmp & ~(0xff<<11);
	
	if(oneportsel==0)
		iowrite32((val << 0) | tmp,SYS_USB_PHY);
	else
		iowrite32((val << 11) | tmp, SYS_USB_PHY);
	
	USB2_PHY_DELAY;
	
	iowrite32((reg_l << 8) | 0x02000000,R_B804C280); 
	USB2_PHY_DELAY;
	iowrite32((reg_h << 8) | 0x02000000,R_B804C280); 
	USB2_PHY_DELAY;
}

void rtxh_u2_set_phy_parameters(void)
{
	int phy40M;

	phy40M=(ioread32(HW_STRAP)&(1<<24))>>24;

	rtxh_u2_set_phy(0xe0, 0x44);
	rtxh_u2_set_phy(0xe1, 0xe8);
	rtxh_u2_set_phy(0xe2, 0x9a);
	rtxh_u2_set_phy(0xe3, 0xa1);
	if(phy40M==0)
		rtxh_u2_set_phy( 0xe4, 0x33);
	rtxh_u2_set_phy(0xe5, 0x95);
	rtxh_u2_set_phy(0xe6, 0x98);
	if(phy40M==0)
		rtxh_u2_set_phy(0xe7, 0x66);	
	rtxh_u2_set_phy(0xf5, 0x49);	
	if(phy40M==0)
		rtxh_u2_set_phy(0xf7, 0x11);	
}

void rtxh_u3_set_phy(unsigned int addr,unsigned int value)
{
	unsigned int readback;

	iowrite32(addr<<8, BSP_USB3_EXT);
	USB2_PHY_DELAY;
	readback = ioread32(BSP_USB3_EXT);
	USB2_PHY_DELAY;
	iowrite32((value << 16) | (addr << 8) | 1, BSP_USB3_EXT);
	USB2_PHY_DELAY;
	readback = ioread32(BSP_USB3_EXT);
	USB2_PHY_DELAY;
	iowrite32(addr << 8, BSP_USB3_EXT);
	USB2_PHY_DELAY;
	readback = ioread32(BSP_USB3_EXT);
	USB2_PHY_DELAY;
	
	if(readback != ((value << 16) | (addr << 8) | 0x10))
		pr_err("usb phy set error (addr=%x, value=%x read=%x)\n",addr,value,readback);
}

static void rtxh_u3_get_phy(unsigned int addr,unsigned int *value)
{
	unsigned int readback;
	unsigned int readback2;

	iowrite32(addr << 8,BSP_USB3_EXT);
	USB2_PHY_DELAY;
	readback = ioread32(BSP_USB3_EXT);
	USB2_PHY_DELAY;
	iowrite32(addr << 8,BSP_USB3_EXT);
	USB2_PHY_DELAY;
	readback2 = ioread32(BSP_USB3_EXT);
	USB2_PHY_DELAY;

	*value = ((readback2 >> 16) & 0xffff);
}

void rtxh_u3_reset_phy(void)
{
	iowrite32(ioread32(U3_IPCFG) | (1<<9), U3_IPCFG);

	iowrite32(ioread32(U3_IPCFG) & ~(1<<10), U3_IPCFG);
	mdelay(100);

	iowrite32(ioread32(U3_IPCFG) | (1<<10), U3_IPCFG);
	mdelay(100);
}

void rtxh_u2_reset_phy(void)  
{
	iowrite32(ioread32(U3_IPCFG) | (1<<7), U3_IPCFG);

	iowrite32(ioread32(U3_IPCFG) & ~(1<<8), U3_IPCFG);
	mdelay(100);

	iowrite32(ioread32(U3_IPCFG) | (1<<8), U3_IPCFG);
	mdelay(100);
}

void rtxh_u3_reset_mac_bus(void)
{
	iowrite32(ioread32(U3_IPCFG) | (1<<20), U3_IPCFG);

	iowrite32(ioread32(U3_IPCFG) & ~(1<<21), U3_IPCFG);
	mdelay(100);

	iowrite32(ioread32(U3_IPCFG) | (1<<21), U3_IPCFG);
	mdelay(100);
}

void rtxh_u3_set_phy_parameters(void) //just for rle0371
{
	int phy40M;

	iowrite32(ioread32(CLK_MGR) | (1<<21) | (1<<19) | (1<<20), CLK_MGR);  //enable usb 3 ip
	iowrite32(ioread32(U3_IPCFG) & ~(1<<6), U3_IPCFG);   //u3 reg big endia
	
	rtxh_u3_reset_mac_bus();

	//setting u2 of u3 phy
	iowrite32(ioread32(SYS_USB_PHY) & ~(1<<20), SYS_USB_PHY);
	iowrite32(ioread32(SYS_USB_PHY) | (1<<19)|(1<<21), SYS_USB_PHY);        //0x00280500;

 
	phy40M=(ioread32(HW_STRAP)&(1<<24))>>24;
	pr_info("UPHY: 8198c ASIC u3 of u3 %s phy patch\n", (phy40M==1) ? "40M" : "25M");

	rtxh_u3_set_phy(0x00, 0x4A78);
	rtxh_u3_set_phy(0x01, 0xC0CE);
	rtxh_u3_set_phy(0x02, 0xE048);
	rtxh_u3_set_phy(0x03, 0x2770);
	rtxh_u3_set_phy(0x04, (phy40M==1) ? 0x5800 : 0x5000);
	rtxh_u3_set_phy(0x05, (phy40M==1) ? 0x60EA : 0x6182);
	rtxh_u3_set_phy(0x06, (phy40M==1) ?0x4168 : 0x6178 );  //0108
	rtxh_u3_set_phy(0x07, 0x2E40);
	rtxh_u3_set_phy(0x08, (phy40M==1) ? 0x4F61 : 0x31B1);
	rtxh_u3_set_phy(0x09, 0x923C);
	rtxh_u3_set_phy(0x0A, 0x9240);
	rtxh_u3_set_phy(0x0B, (phy40M==1) ? 0x8B15 : 0x8B1D);  //0108
	rtxh_u3_set_phy(0x0C, 0xDC6A);
	rtxh_u3_set_phy(0x0D, (phy40M==1) ? 0x148A : 0x158a);  //0108
	rtxh_u3_set_phy(0x0E, (phy40M==1) ? 0x98E1 : 0xA8c9);  //0108
	rtxh_u3_set_phy(0x0F, 0x8000);
	
	rtxh_u3_set_phy(0x10, 0x000C);
	rtxh_u3_set_phy(0x11, 0x4C00);
	rtxh_u3_set_phy(0x12, 0xFC00);
	rtxh_u3_set_phy(0x13, 0x0C81);
	rtxh_u3_set_phy(0x14, 0xDE01);
	rtxh_u3_set_phy(0x19, 0xE102);
	rtxh_u3_set_phy(0x1A, 0x1263);
	rtxh_u3_set_phy(0x1B, 0xC7FD);
	rtxh_u3_set_phy(0x1C, 0xCB00);
	rtxh_u3_set_phy(0x1D, 0xA03F);
	rtxh_u3_set_phy(0x1E, 0xC2E0);
	
	rtxh_u3_set_phy(0x20, 0xB7F0);
	rtxh_u3_set_phy(0x21, 0x0407);
	rtxh_u3_set_phy(0x22, 0x0016);
	rtxh_u3_set_phy(0x23, 0x0CA1);
	rtxh_u3_set_phy(0x24, 0x93F1);
	rtxh_u3_set_phy(0x25, 0x2BDD);
	rtxh_u3_set_phy(0x26, (phy40M==1) ? 0xA06D : 0x646F);  //0108
	rtxh_u3_set_phy(0x27, (phy40M==1) ? 0x8068 : 0x8107);
	rtxh_u3_set_phy(0x28, (phy40M==1) ? 0xE060 : 0xE020);  //0108
	rtxh_u3_set_phy(0x29, 0x3080);
	rtxh_u3_set_phy(0x2A, 0x3082);
	rtxh_u3_set_phy(0x2B, 0x2038);
	rtxh_u3_set_phy(0x2C, 0x7E30);
	rtxh_u3_set_phy(0x2D, 0x15DC);
	rtxh_u3_set_phy(0x2E, 0x792F);
	
	rtxh_u3_set_phy(0x04, (phy40M==1) ? 0x7800 : 0x7000);
	rtxh_u3_set_phy(0x09, 0x923C);
	rtxh_u3_set_phy(0x09, 0x903C);
	rtxh_u3_set_phy(0x09, 0x923C);
}

void rtxh_u3_train_phy(void)
{
	volatile u32 tmp;
	static int test_count_u3 = 0;
	u8 *base=(u8 *)0xb8040000; // TODO take from DT

	while(1) {
		test_count_u3++;
		if(test_count_u3>100)
			break;

 		if((ioread32(base+0x430)&0x1000)==0x1000) {
			pr_info("%s %d SS device found!\n",__FUNCTION__,__LINE__);

			//clear U3 WRC & CSC 
			tmp=ioread32(base+0x430);
			tmp|=(1<<17)|(1<<19);  // WRC CSC.
			tmp&=~(1<<1); // don't disable port
			iowrite32(tmp, base+0x430);

			pr_info("%s %d 0x420=%x\n",__FUNCTION__,__LINE__,ioread32(base+0x420));				
			pr_info("%s %d 0x430=%x\n",__FUNCTION__,__LINE__,ioread32(base+0x430));	
				
			break;  // SS device found!
		}

		//warm port reset for USB 3.0
 		//pr_info("Do Warm Reset for U3\n");
		tmp=ioread32(base+0x430);
		tmp|=(1<<31)|(1<<0);  // warm port reset and set CCS.
		tmp&=~(1<<1); // don't disable port
		iowrite32(tmp, base+0x430);
			
 		mdelay(500);			

 		if((ioread32(base+0x430)&0xfff)==0x2a0) {
 			//try USB 2.0 Port Reset
 			//pr_info("try Port Reset for U2\n");
 			iowrite32(ioread32(base+0x420)|0x10,base+0x420);
 			mdelay(200);

 			// stop USB 2.0 Port Enable
 			iowrite32(ioread32(base+0x420)|0x2,base+0x420);
 			mdelay(200);				
			//clear U3 WRC & CSC 
	 		tmp=ioread32(base+0x430);
	 		tmp|=(1<<17)|(1<<19);  // WRC CSC.
	 		tmp&=~(1<<1); // don't disable port
	 		iowrite32(tmp,base+0x430);
 			break;
 		}
	} 
}

void rtxh_init_phy(void)
{
	rtxh_u3_set_phy_parameters();
	rtxh_u2_set_phy_parameters();
	
	rtxh_u3_reset_phy();
	rtxh_u2_reset_phy();

	rtxh_u3_train_phy();
}

static void rtxh_quirks(struct device *dev, struct xhci_hcd *xhci)
{
	/*
	 * As of now platform drivers don't provide MSI support so we ensure
	 * here that the generic code does not try to make a pci_dev from our
	 * dev struct in order to setup MSI
	 */
	pr_info("ADD QUIRK\n");
	xhci->quirks |= XHCI_PLAT;
}

/* called during probe() after chip reset completes */
static int rtxh_setup(struct usb_hcd *hcd)
{
	if (usb_hcd_is_primary_hcd(hcd))
		rtxh_init_phy();

	return xhci_gen_setup(hcd, rtxh_quirks);
}

static struct hc_driver __read_mostly rtxh_hc_driver;
static const struct xhci_driver_overrides rtxh_hc_overrides __initconst = {
	.reset = rtxh_setup,
};

static int rtxh_probe(struct platform_device *pdev)
{
	int ret, irq;
	struct usb_hcd *hcd;
	struct resource *res;
	struct xhci_hcd *xhci;
	struct rtxh_dev *xhdev;
	const struct hc_driver *driver;
	struct device *dev = &pdev->dev;

	if (usb_disabled())
		return -ENODEV;

	driver = &rtxh_hc_driver;
	xhdev = devm_kzalloc(dev, sizeof(*xhdev), GFP_KERNEL);
	if (!xhdev)
		return -ENOMEM;

	xhdev->dev = dev;

	irq = platform_get_irq(pdev, 0);
	if (irq < 0)
		return irq;

	xhdev->base = devm_platform_get_and_ioremap_resource(pdev, 0, &res);
	if (IS_ERR(xhdev->base))
		return PTR_ERR(xhdev->base);

	/* Initialize dma_mask and coherent_dma_mask to 32-bits */
	ret = dma_set_mask_and_coherent(dev, DMA_BIT_MASK(32));
	if (ret)
		goto err_out;

	hcd = usb_create_hcd(driver, dev, dev_name(dev));
	if (!hcd) {
		ret = -ENOMEM;
		goto err_out;
	}

	hcd->regs = xhdev->base;
	hcd->rsrc_start = res->start;
	hcd->rsrc_len = resource_size(res);

	xhdev->hcd = hcd;
	dev_set_drvdata(hcd->self.controller, xhdev);

//	ret = xhci_histb_host_enable(xhdev);
//	if (ret)
//		goto err_hcd;

	xhci = hcd_to_xhci(hcd);

//	device_wakeup_enable(hcd->self.controller);

	xhci->main_hcd = hcd;
	xhci->shared_hcd = usb_create_shared_hcd(driver, dev, dev_name(dev),
						 hcd);
	if (!xhci->shared_hcd) {
		ret = -ENOMEM;
		goto err_host;
	}

	ret = usb_add_hcd(hcd, irq, IRQF_SHARED);
	if (ret)
		goto err_usb3;

//	if (HCC_MAX_PSA(xhci->hcc_params) >= 4)
//		xhci->shared_hcd->can_do_streams = 1;

	ret = usb_add_hcd(xhci->shared_hcd, irq, IRQF_SHARED);
	if (ret)
		goto err_usb2;

//	device_enable_async_suspend(dev);
//	pm_runtime_put_noidle(dev);

	/*
	 * Prevent runtime pm from being on as default, users should enable
	 * runtime pm using power/control in sysfs.
	 */

	pm_runtime_forbid(dev);

	return 0;

err_usb2:
	usb_remove_hcd(hcd);
err_usb3:
	usb_put_hcd(xhci->shared_hcd);
err_host:
//	xhci_histb_host_disable(xhdev);
err_hcd:
	usb_put_hcd(hcd);
err_out:
	return ret;
}

static const struct of_device_id rtxh_of_match[] = {
	{ .compatible = "realtek,realtek-xhci"},
	{ },
};
MODULE_DEVICE_TABLE(of, rtxh_of_match);

static struct platform_driver rtxh_driver = {
	.probe	= rtxh_probe,
//	.remove	= rtxh_remove,
	.driver	= {
		.name = "xhci-realtek",
//		.pm = DEV_PM_OPS,
		.of_match_table = of_match_ptr(rtxh_of_match),
	},
};
MODULE_ALIAS("platform:xhci-realtek");

static int __init rtxh_init(void)
{
	xhci_init_driver(&rtxh_hc_driver, &rtxh_hc_overrides);
	return platform_driver_register(&rtxh_driver);
}
module_init(rtxh_init);

static void __exit rtxh_exit(void)
{
	platform_driver_unregister(&rtxh_driver);
}
module_exit(rtxh_exit);

MODULE_DESCRIPTION("Realtek xHCI Host Controller Driver");
MODULE_LICENSE("GPL v2");
