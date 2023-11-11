/******************************************************************************
 *
 * Copyright(c) 2007 - 2019 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 *****************************************************************************/
#define _HCI_INTF_C_

#include <drv_types.h>

#include <platform_ops.h>
#include <rtw_trx_usb.h>

#ifndef CONFIG_USB_HCI
#error "CONFIG_USB_HCI shall be on!\n"
#endif

#ifdef CONFIG_80211N_HT
extern int rtw_ht_enable;
extern int rtw_bw_mode;
extern int rtw_ampdu_enable;/* for enable tx_ampdu */
#endif

#ifdef CONFIG_GLOBAL_UI_PID
int ui_pid[3] = {0, 0, 0};
#endif


static int rtw_dev_suspend(struct usb_interface *intf, pm_message_t message);
static int rtw_dev_resume(struct usb_interface *intf);


static int rtw_dev_probe(struct usb_interface *pusb_intf, const struct usb_device_id *pdid);
static void rtw_dev_remove(struct usb_interface *pusb_intf);

static void rtw_dev_shutdown(struct device *dev)
{
	struct usb_interface *usb_intf = container_of(dev, struct usb_interface, dev);
	struct dvobj_priv *dvobj = NULL;
	_adapter *adapter = NULL;

	RTW_INFO("%s\n", __func__);

	if (usb_intf) {
		dvobj = usb_get_intfdata(usb_intf);
		if (dvobj) {
			adapter = dvobj_get_primary_adapter(dvobj);
			if (adapter) {
				if (!dev_is_surprise_removed(dvobj)) {
					#ifdef CONFIG_WOWLAN
					struct pwrctrl_priv *pwrctl = adapter_to_pwrctl(adapter);

					#ifdef CONFIG_GPIO_WAKEUP
					/*default wake up pin change to BT*/
					RTW_INFO("%s:default wake up pin change to BT\n", __FUNCTION__);
					/* ToDo: clear pin mux code is not ready
					rtw_hal_switch_gpio_wl_ctrl(adapter, WAKEUP_GPIO_IDX, _FALSE); */
					#endif /* CONFIG_GPIO_WAKEUP */

					if (pwrctl->wowlan_mode == _TRUE)
						RTW_PRINT("%s wowlan_mode ==_TRUE do not run rtw_hw_stop()\n", __FUNCTION__);
					else
					#endif
					{
						if (rtw_hw_is_init_completed(dvobj))
						rtw_hw_stop(dvobj);
						dev_set_surprise_removed(dvobj);
					}
				}
			}
			ATOMIC_SET(&dvobj->continual_io_error, MAX_CONTINUAL_IO_ERR + 1);
		}
	}
}

#if (LINUX_VERSION_CODE <= KERNEL_VERSION(2, 6, 23))
/* Some useful macros to use to create struct usb_device_id */
#define USB_DEVICE_ID_MATCH_VENDOR			 0x0001
#define USB_DEVICE_ID_MATCH_PRODUCT			 0x0002
#define USB_DEVICE_ID_MATCH_DEV_LO			 0x0004
#define USB_DEVICE_ID_MATCH_DEV_HI			 0x0008
#define USB_DEVICE_ID_MATCH_DEV_CLASS			 0x0010
#define USB_DEVICE_ID_MATCH_DEV_SUBCLASS		 0x0020
#define USB_DEVICE_ID_MATCH_DEV_PROTOCOL		 0x0040
#define USB_DEVICE_ID_MATCH_INT_CLASS			 0x0080
#define USB_DEVICE_ID_MATCH_INT_SUBCLASS		 0x0100
#define USB_DEVICE_ID_MATCH_INT_PROTOCOL		 0x0200
#define USB_DEVICE_ID_MATCH_INT_NUMBER		 0x0400


#define USB_DEVICE_ID_MATCH_INT_INFO \
	(USB_DEVICE_ID_MATCH_INT_CLASS | \
	 USB_DEVICE_ID_MATCH_INT_SUBCLASS | \
	 USB_DEVICE_ID_MATCH_INT_PROTOCOL)


#define USB_DEVICE_AND_INTERFACE_INFO(vend, prod, cl, sc, pr) \
	.match_flags = USB_DEVICE_ID_MATCH_INT_INFO \
		       | USB_DEVICE_ID_MATCH_DEVICE, \
		       .idVendor = (vend), \
				   .idProduct = (prod), \
						.bInterfaceClass = (cl), \
						.bInterfaceSubClass = (sc), \
						.bInterfaceProtocol = (pr)

/**
 * USB_VENDOR_AND_INTERFACE_INFO - describe a specific usb vendor with a class of usb interfaces
 * @vend: the 16 bit USB Vendor ID
 * @cl: bInterfaceClass value
 * @sc: bInterfaceSubClass value
 * @pr: bInterfaceProtocol value
 *
 * This macro is used to create a struct usb_device_id that matches a
 * specific vendor with a specific class of interfaces.
 *
 * This is especially useful when explicitly matching devices that have
 * vendor specific bDeviceClass values, but standards-compliant interfaces.
 */
#define USB_VENDOR_AND_INTERFACE_INFO(vend, cl, sc, pr) \
	.match_flags = USB_DEVICE_ID_MATCH_INT_INFO \
		       | USB_DEVICE_ID_MATCH_VENDOR, \
		       .idVendor = (vend), \
				   .bInterfaceClass = (cl), \
						   .bInterfaceSubClass = (sc), \
						   .bInterfaceProtocol = (pr)

/* ----------------------------------------------------------------------- */
#endif


#define USB_VENDER_ID_REALTEK		0x0BDA


/* DID_USB_v916_20130116 */
static struct usb_device_id rtw_usb_id_tbl[] = {
#ifdef CONFIG_RTL8852A
	/*=== Realtek demoboard ===*/
	{USB_DEVICE_AND_INTERFACE_INFO(USB_VENDER_ID_REALTEK, 0x8832, 0xff, 0xff, 0xff), .driver_info = RTL8852A},
	{USB_DEVICE_AND_INTERFACE_INFO(USB_VENDER_ID_REALTEK, 0x885A, 0xff, 0xff, 0xff), .driver_info = RTL8852A},
	{USB_DEVICE_AND_INTERFACE_INFO(USB_VENDER_ID_REALTEK, 0x885C, 0xff, 0xff, 0xff), .driver_info = RTL8852A},
#endif /* CONFIG_RTL8852A */

#ifdef CONFIG_RTL8852B
	/*=== Realtek demoboard ===*/
	{USB_DEVICE_AND_INTERFACE_INFO(USB_VENDER_ID_REALTEK, 0xB832, 0xff, 0xff, 0xff), .driver_info = RTL8852B},
	{USB_DEVICE_AND_INTERFACE_INFO(USB_VENDER_ID_REALTEK, 0xB83A, 0xff, 0xff, 0xff), .driver_info = RTL8852B},
	{USB_DEVICE_AND_INTERFACE_INFO(USB_VENDER_ID_REALTEK, 0xB852, 0xff, 0xff, 0xff), .driver_info = RTL8852B},
	{USB_DEVICE_AND_INTERFACE_INFO(USB_VENDER_ID_REALTEK, 0xB85A, 0xff, 0xff, 0xff), .driver_info = RTL8852B},
	{USB_DEVICE_AND_INTERFACE_INFO(USB_VENDER_ID_REALTEK, 0xA85B, 0xff, 0xff, 0xff), .driver_info = RTL8852B},
#endif /* CONFIG_RTL8852B */
#ifdef CONFIG_RTL8852BP
	/*=== Realtek demoboard ===*/
	{USB_DEVICE_AND_INTERFACE_INFO(USB_VENDER_ID_REALTEK, 0xA85C, 0xff, 0xff, 0xff), .driver_info = RTL8852BP},
#endif /* CONFIG_RTL8852BP */
#ifdef CONFIG_RTL8851B
	/*=== Realtek demoboard ===*/
	{USB_DEVICE_AND_INTERFACE_INFO(USB_VENDER_ID_REALTEK, 0xB851, 0xff, 0xff, 0xff), .driver_info = RTL8851B},
#endif /* CONFIG_RTL8851B */

#ifdef CONFIG_RTL8852C
	/*=== Realtek demoboard ===*/
	{USB_DEVICE_AND_INTERFACE_INFO(USB_VENDER_ID_REALTEK, 0xC85A, 0xff, 0xff, 0xff), .driver_info = RTL8852C},
	{USB_DEVICE_AND_INTERFACE_INFO(USB_VENDER_ID_REALTEK, 0xC832, 0xff, 0xff, 0xff), .driver_info = RTL8852C},
	{USB_DEVICE_AND_INTERFACE_INFO(USB_VENDER_ID_REALTEK, 0xC85D, 0xff, 0xff, 0xff), .driver_info = RTL8852C},
#endif /* CONFIG_RTL8852C */

	{}	/* Terminating entry */
};

MODULE_DEVICE_TABLE(usb, rtw_usb_id_tbl);

int const rtw_usb_id_len = sizeof(rtw_usb_id_tbl) / sizeof(struct usb_device_id);

static struct specific_device_id specific_device_id_tbl[] = {
	{.idVendor = USB_VENDER_ID_REALTEK, .idProduct = 0x8177, .flags = SPEC_DEV_ID_DISABLE_HT}, /* 8188cu 1*1 dongole, (b/g mode only) */
	{.idVendor = USB_VENDER_ID_REALTEK, .idProduct = 0x817E, .flags = SPEC_DEV_ID_DISABLE_HT}, /* 8188CE-VAU USB minCard (b/g mode only) */
	{.idVendor = 0x0b05, .idProduct = 0x1791, .flags = SPEC_DEV_ID_DISABLE_HT},
	{.idVendor = 0x13D3, .idProduct = 0x3311, .flags = SPEC_DEV_ID_DISABLE_HT},
	{.idVendor = 0x13D3, .idProduct = 0x3359, .flags = SPEC_DEV_ID_DISABLE_HT}, /* Russian customer -Azwave (8188CE-VAU  g mode) */
#ifdef RTK_DMP_PLATFORM
	{.idVendor = USB_VENDER_ID_REALTEK, .idProduct = 0x8111, .flags = SPEC_DEV_ID_ASSIGN_IFNAME}, /* Realtek 5G dongle for WiFi Display */
	{.idVendor = 0x2019, .idProduct = 0xAB2D, .flags = SPEC_DEV_ID_ASSIGN_IFNAME}, /* PCI-Abocom 5G dongle for WiFi Display */
#endif /* RTK_DMP_PLATFORM */
	{}
};

struct rtw_usb_drv {
	struct usb_driver usbdrv;
	int drv_registered;
};

struct rtw_usb_drv usb_drv = {
	.usbdrv.name = (char *)DRV_NAME,
	.usbdrv.probe = rtw_dev_probe,
	.usbdrv.disconnect = rtw_dev_remove,
	.usbdrv.id_table = rtw_usb_id_tbl,
	.usbdrv.suspend =  rtw_dev_suspend,
	.usbdrv.resume = rtw_dev_resume,
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 22))
	.usbdrv.reset_resume   = rtw_dev_resume,
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 19))
	.usbdrv.drvwrap.driver.shutdown = rtw_dev_shutdown,
#else
	.usbdrv.driver.shutdown = rtw_dev_shutdown,
#endif
};

static inline int RT_usb_endpoint_dir_in(const struct usb_endpoint_descriptor *epd)
{
	return (epd->bEndpointAddress & USB_ENDPOINT_DIR_MASK) == USB_DIR_IN;
}

static inline int RT_usb_endpoint_dir_out(const struct usb_endpoint_descriptor *epd)
{
	return (epd->bEndpointAddress & USB_ENDPOINT_DIR_MASK) == USB_DIR_OUT;
}

static inline int RT_usb_endpoint_xfer_int(const struct usb_endpoint_descriptor *epd)
{
	return (epd->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK) == USB_ENDPOINT_XFER_INT;
}

static inline int RT_usb_endpoint_xfer_bulk(const struct usb_endpoint_descriptor *epd)
{
	return (epd->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK) == USB_ENDPOINT_XFER_BULK;
}

static inline int RT_usb_endpoint_is_bulk_in(const struct usb_endpoint_descriptor *epd)
{
	return RT_usb_endpoint_xfer_bulk(epd) && RT_usb_endpoint_dir_in(epd);
}

static inline int RT_usb_endpoint_is_bulk_out(const struct usb_endpoint_descriptor *epd)
{
	return RT_usb_endpoint_xfer_bulk(epd) && RT_usb_endpoint_dir_out(epd);
}

static inline int RT_usb_endpoint_is_int_in(const struct usb_endpoint_descriptor *epd)
{
	return RT_usb_endpoint_xfer_int(epd) && RT_usb_endpoint_dir_in(epd);
}

static inline int RT_usb_endpoint_num(const struct usb_endpoint_descriptor *epd)
{
	return epd->bEndpointAddress & USB_ENDPOINT_NUMBER_MASK;
}

static u8 rtw_init_intf_priv(struct dvobj_priv *dvobj)
{
	u8 rst = _SUCCESS;
	PUSB_DATA pusb_data = dvobj_to_usb(dvobj);

#ifdef CONFIG_USB_VENDOR_REQ_MUTEX
	_rtw_mutex_init(&pusb_data->usb_vendor_req_mutex);
#endif


#ifdef CONFIG_USB_VENDOR_REQ_BUFFER_PREALLOC
	pusb_data->usb_alloc_vendor_req_buf = rtw_zmalloc(MAX_USB_IO_CTL_SIZE);
	if (pusb_data->usb_alloc_vendor_req_buf == NULL) {
		RTW_INFO("alloc usb_vendor_req_buf failed... /n");
		rst = _FAIL;
		goto exit;
	}
	pusb_data->usb_vendor_req_buf  =
		(u8 *)N_BYTE_ALIGMENT((SIZE_PTR)(pusb_data->usb_alloc_vendor_req_buf), ALIGNMENT_UNIT);
exit:
#endif

	return rst;

}

static u8 rtw_deinit_intf_priv(struct dvobj_priv *dvobj)
{
	u8 rst = _SUCCESS;
	PUSB_DATA pusb_data = dvobj_to_usb(dvobj);

#ifdef CONFIG_USB_VENDOR_REQ_BUFFER_PREALLOC
	if (pusb_data->usb_vendor_req_buf)
		rtw_mfree(pusb_data->usb_alloc_vendor_req_buf, MAX_USB_IO_CTL_SIZE);
#endif

#ifdef CONFIG_USB_VENDOR_REQ_MUTEX
	_rtw_mutex_free(&pusb_data->usb_vendor_req_mutex);
#endif

	return rst;
}

static unsigned int rtw_endpoint_max_bpi(struct usb_device *dev,
					 struct usb_host_endpoint *ep)
{
	u16 psize;
	u16 mult = 1;
	int max_size_1 = 0, max_size_2 = 0;

	switch (dev->speed) {
	case USB_SPEED_SUPER:
#if ((LINUX_VERSION_CODE >= KERNEL_VERSION(4, 4, 20) && LINUX_VERSION_CODE < KERNEL_VERSION(4, 5, 0)) ||\
		LINUX_VERSION_CODE >= KERNEL_VERSION(4, 6, 0))
	case USB_SPEED_SUPER_PLUS:
		max_size_1 = le16_to_cpu(ep->ss_ep_comp.wBytesPerInterval);
		max_size_2 = usb_endpoint_maxp(&ep->desc);
#endif
		break;
	case USB_SPEED_HIGH:
		psize = usb_endpoint_maxp(&ep->desc);
		#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 9, 71))
		mult = usb_endpoint_maxp_mult(&ep->desc);
		max_size_1 = psize * mult;
		#endif
		max_size_2 = usb_endpoint_maxp(&ep->desc);
		break;
	case USB_SPEED_WIRELESS:
		max_size_1 = max_size_2 = usb_endpoint_maxp(&ep->desc);
		break;
	default:
		max_size_1 = max_size_2 = usb_endpoint_maxp(&ep->desc);
		break;

	}
	RTW_INFO("USB EP MAX_PKT_SZ:%d-%d\n",max_size_1, max_size_2);
	return max_size_1;
}

static struct dvobj_priv *usb_dvobj_init(struct usb_interface *usb_intf,
					const struct usb_device_id *pdid)
{
	int	i;
	int	status = _FAIL;
	struct dvobj_priv *dvobj;
	struct usb_device_descriptor	*pdev_desc;
	struct usb_host_config		*phost_conf;
	struct usb_config_descriptor	*pconf_desc;
	struct usb_host_interface	*phost_iface;
	struct usb_interface_descriptor	*piface_desc;
	struct usb_host_endpoint	*phost_endp;
	struct usb_endpoint_descriptor	*pendp_desc;
	struct usb_device		*pusbd;
	PUSB_DATA pusb_data;

	dvobj = devobj_init();
	if (dvobj == NULL)
		goto exit;

	pusb_data = dvobj_to_usb(dvobj);

	pusb_data->pusbintf = usb_intf;
	pusbd = pusb_data->pusbdev = interface_to_usbdev(usb_intf);
	usb_set_intfdata(usb_intf, dvobj);

	pusb_data->RtNumInPipes = 0;
	pusb_data->RtNumOutPipes = 0;

	pdev_desc = &pusbd->descriptor;

#if 0
	RTW_INFO("\n8712_usb_device_descriptor:\n");
	RTW_INFO("bLength=%x\n", pdev_desc->bLength);
	RTW_INFO("bDescriptorType=%x\n", pdev_desc->bDescriptorType);
	RTW_INFO("bcdUSB=%x\n", pdev_desc->bcdUSB);
	RTW_INFO("bDeviceClass=%x\n", pdev_desc->bDeviceClass);
	RTW_INFO("bDeviceSubClass=%x\n", pdev_desc->bDeviceSubClass);
	RTW_INFO("bDeviceProtocol=%x\n", pdev_desc->bDeviceProtocol);
	RTW_INFO("bMaxPacketSize0=%x\n", pdev_desc->bMaxPacketSize0);
	RTW_INFO("idVendor=%x\n", pdev_desc->idVendor);
	RTW_INFO("idProduct=%x\n", pdev_desc->idProduct);
	RTW_INFO("bcdDevice=%x\n", pdev_desc->bcdDevice);
	RTW_INFO("iManufacturer=%x\n", pdev_desc->iManufacturer);
	RTW_INFO("iProduct=%x\n", pdev_desc->iProduct);
	RTW_INFO("iSerialNumber=%x\n", pdev_desc->iSerialNumber);
	RTW_INFO("bNumConfigurations=%x\n", pdev_desc->bNumConfigurations);
#endif

	phost_conf = pusbd->actconfig;
	pconf_desc = &phost_conf->desc;

#if 0
	RTW_INFO("\n[USB] configuration_descriptor:\n");
	RTW_INFO("bNumInterfaces=%x\n", pconf_desc->bNumInterfaces);
	RTW_INFO("bLength=%x\n", pconf_desc->bLength);
	RTW_INFO("bDescriptorType=%x\n", pconf_desc->bDescriptorType);
	RTW_INFO("wTotalLength=%x\n", pconf_desc->wTotalLength);
	RTW_INFO("bConfigurationValue=%x\n", pconf_desc->bConfigurationValue);
	RTW_INFO("iConfiguration=%x\n", pconf_desc->iConfiguration);
	RTW_INFO("bmAttributes=%x\n", pconf_desc->bmAttributes);
	RTW_INFO("bMaxPower=%x\n", pconf_desc->bMaxPower);
#endif

	phost_iface = &usb_intf->altsetting[0];
	piface_desc = &phost_iface->desc;
#if 0
	RTW_INFO("\n[USB] usb_interface_descriptor:\n");
	RTW_INFO("bInterfaceNumber=%x\n", piface_desc->bInterfaceNumber);
	RTW_INFO("bAlternateSetting=%x\n", piface_desc->bAlternateSetting);
	RTW_INFO("bLength=%x\n", piface_desc->bLength);
	RTW_INFO("bDescriptorType=%x\n", piface_desc->bDescriptorType);
	RTW_INFO("bNumEndpoints=%x\n", piface_desc->bNumEndpoints);
	RTW_INFO("bInterfaceClass=%x\n", piface_desc->bInterfaceClass);
	RTW_INFO("bInterfaceSubClass=%x\n", piface_desc->bInterfaceSubClass);
	RTW_INFO("bInterfaceProtocol=%x\n", piface_desc->bInterfaceProtocol);
	RTW_INFO("iInterface=%x\n", piface_desc->iInterface);
#endif

	pusb_data->nr_endpoint = piface_desc->bNumEndpoints;
	if (pusb_data->nr_endpoint > MAX_ENDPOINT_NUM) {
		RTW_ERR("USB EP_Number : %d > RT DEF-MAX_EP_NUM :%d\n",
				pusb_data->nr_endpoint, MAX_ENDPOINT_NUM);
		rtw_warn_on(1);
	}

	/* RTW_INFO("\ndump usb_endpoint_descriptor:\n"); */

	for (i = 0; i < pusb_data->nr_endpoint; i++) {
		phost_endp = phost_iface->endpoint + i;
		if (phost_endp) {
			pendp_desc = &phost_endp->desc;

			RTW_INFO("\nusb_endpoint_descriptor(%d):\n", i);
			RTW_INFO("bLength=%x\n", pendp_desc->bLength);
			RTW_INFO("bDescriptorType=%x\n", pendp_desc->bDescriptorType);
			RTW_INFO("bEndpointAddress=%x\n", pendp_desc->bEndpointAddress);
			/* RTW_INFO("bmAttributes=%x\n",pendp_desc->bmAttributes); */
			RTW_INFO("wMaxPacketSize=%d\n", le16_to_cpu(pendp_desc->wMaxPacketSize));
			RTW_INFO("bInterval=%x\n", pendp_desc->bInterval);
			/* RTW_INFO("bRefresh=%x\n",pendp_desc->bRefresh); */
			/* RTW_INFO("bSynchAddress=%x\n",pendp_desc->bSynchAddress); */

			if (RT_usb_endpoint_is_bulk_in(pendp_desc)) {
				RTW_INFO("RT_usb_endpoint_is_bulk_in = %x\n", RT_usb_endpoint_num(pendp_desc));
				pusb_data->RtInPipe[pusb_data->RtNumInPipes] = RT_usb_endpoint_num(pendp_desc);
				pusb_data->inpipe_type[pusb_data->RtNumInPipes] = REALTEK_USB_BULK_IN_EP_IDX;
				pusb_data->RtNumInPipes++;
				RTW_INFO("USB#%d bulkin size:%d", pusb_data->RtNumOutPipes,
					rtw_endpoint_max_bpi(pusbd, phost_endp));
			} else if (RT_usb_endpoint_is_int_in(pendp_desc)) {
				RTW_INFO("RT_usb_endpoint_is_int_in = %x, Interval = %x\n", RT_usb_endpoint_num(pendp_desc), pendp_desc->bInterval);
				pusb_data->RtInPipe[pusb_data->RtNumInPipes] = RT_usb_endpoint_num(pendp_desc);
				pusb_data->inpipe_type[pusb_data->RtNumInPipes] = REALTEK_USB_IN_INT_EP_IDX;
				pusb_data->RtNumInPipes++;
			} else if (RT_usb_endpoint_is_bulk_out(pendp_desc)) {
				RTW_INFO("RT_usb_endpoint_is_bulk_out = %x\n", RT_usb_endpoint_num(pendp_desc));
				pusb_data->RtOutPipe[pusb_data->RtNumOutPipes] = RT_usb_endpoint_num(pendp_desc);
				RTW_INFO("USB#%d bulkout size:%d", pusb_data->RtNumOutPipes,
					rtw_endpoint_max_bpi(pusbd, phost_endp));
				pusb_data->RtNumOutPipes++;
			}
			/*pusb_data->ep_num[i] = RT_usb_endpoint_num(pendp_desc);*/
		}
	}

	RTW_INFO("nr_endpoint=%d, in_num=%d, out_num=%d\n\n",
		pusb_data->nr_endpoint, pusb_data->RtNumInPipes, pusb_data->RtNumOutPipes);

	switch (pusbd->speed) {
	case USB_SPEED_LOW:
	case USB_SPEED_FULL:
		RTW_INFO("USB_SPEED_FULL\n");
		pusb_data->usb_speed = RTW_USB_SPEED_FULL;/*U2- 1.1 - 1.5MBs*/
		pusb_data->usb_bulkout_size = USB_FULL_SPEED_BULK_SIZE;
		break;
	case USB_SPEED_HIGH:
		RTW_INFO("USB_SPEED_HIGH\n");
		pusb_data->usb_speed = RTW_USB_SPEED_HIGH;/*U2- 2.1 - 60MBs*/
		pusb_data->usb_bulkout_size = USB_HIGH_SPEED_BULK_SIZE;
		break;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 31))
	case USB_SPEED_SUPER:
		RTW_INFO("USB_SPEED_SUPER\n");
		pusb_data->usb_speed = RTW_USB_SPEED_SUPER;/*U3- 3.0 - 640MBs*/
		pusb_data->usb_bulkout_size = USB_SUPER_SPEED_BULK_SIZE;
		break;
#if ((LINUX_VERSION_CODE >= KERNEL_VERSION(4, 4, 20) && LINUX_VERSION_CODE < KERNEL_VERSION(4, 5, 0)) ||\
		LINUX_VERSION_CODE >= KERNEL_VERSION(4, 6, 0))
	case USB_SPEED_SUPER_PLUS:
		RTW_INFO("USB_SPEED_SUPER_PLUS\n");
		pusb_data->usb_speed = RTW_USB_SPEED_SUPER_10G;/*U3- 3.1 - 1280MBs*/
		pusb_data->usb_bulkout_size = USB_SUPER_SPEED_BULK_SIZE;
		break;
#endif
#endif
	default:
		RTW_INFO("USB_SPEED_UNKNOWN(%d)\n", pusbd->speed);
		pusb_data->usb_speed = RTW_USB_SPEED_UNKNOWN;
		break;
	}

	if (pusb_data->usb_speed == RTW_USB_SPEED_UNKNOWN) {
		RTW_INFO("UNKNOWN USB SPEED MODE, ERROR !!!\n");
		goto free_dvobj;
	}

	if (rtw_init_intf_priv(dvobj) == _FAIL) {
		goto free_dvobj;
	}

	/*step 1-1., get chip_id via driver_info*/
	dvobj->interface_type = RTW_HCI_USB;
	dvobj->ic_id = pdid->driver_info;
	dvobj->intf_ops = &usb_ops;

	/* .3 misc */
	rtw_reset_continual_io_error(dvobj);

	usb_get_dev(pusbd);

	status = _SUCCESS;

free_dvobj:
	if (status != _SUCCESS && dvobj) {
		usb_set_intfdata(usb_intf, NULL);
		devobj_deinit(dvobj);
		dvobj = NULL;
	}
exit:
	return dvobj;
}

static void usb_dvobj_deinit(struct usb_interface *usb_intf)
{
	struct dvobj_priv *dvobj = usb_get_intfdata(usb_intf);

	usb_set_intfdata(usb_intf, NULL);
	if (dvobj) {
		rtw_deinit_intf_priv(dvobj);
		devobj_deinit(dvobj);
	}

	/* RTW_INFO("%s %d\n", __func__, ATOMIC_READ(&usb_intf->dev.kobj.kref.refcount)); */
	usb_put_dev(interface_to_usbdev(usb_intf));

}

static int usb_reprobe_switch_usb_mode(_adapter *adapter)
{
	struct registry_priv *registry_par = &adapter->registrypriv;
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);
	PUSB_DATA pusb_data = dvobj_to_usb(dvobj);
	u8 ret = _FALSE;

	/* registry not allow driver to switch usb mode */
	if (registry_par->switch_usb_mode == 0)
		goto exit;
	else if (registry_par->switch_usb_mode == 1 && pusb_data->usb_speed < RTW_USB_SPEED_SUPER)
		 ret = rtw_phl_cmd_force_usb_switch(dvobj->phl, RTW_USB_SPEED_SUPER, HW_BAND_0, PHL_CMD_DIRECTLY, 0);
	else if (registry_par->switch_usb_mode == 2 && pusb_data->usb_speed >= RTW_USB_SPEED_SUPER)
		 ret = rtw_phl_cmd_force_usb_switch(dvobj->phl, RTW_USB_SPEED_HIGH, HW_BAND_0, PHL_CMD_DIRECTLY, 0);
exit:
	return ret;
}


static void process_spec_devid(const struct usb_device_id *pdid)
{
	u16 vid, pid;
	u32 flags;
	int i;
	int num = sizeof(specific_device_id_tbl) / sizeof(struct specific_device_id);

	for (i = 0; i < num; i++) {
		vid = specific_device_id_tbl[i].idVendor;
		pid = specific_device_id_tbl[i].idProduct;
		flags = specific_device_id_tbl[i].flags;

#ifdef CONFIG_80211N_HT
		if ((pdid->idVendor == vid) && (pdid->idProduct == pid) && (flags & SPEC_DEV_ID_DISABLE_HT)) {
			/*GEORGIA_TODO_FIXIT_MULTI_IC*/
			rtw_ht_enable = 0;
			rtw_bw_mode = 0;
			rtw_ampdu_enable = 0;
		}
#endif

#ifdef RTK_DMP_PLATFORM
		/* Change the ifname to wlan10 when PC side WFD dongle plugin on DMP platform. */
		/* It is used to distinguish between normal and PC-side wifi dongle/module. */
		if ((pdid->idVendor == vid) && (pdid->idProduct == pid) && (flags & SPEC_DEV_ID_ASSIGN_IFNAME)) {
			extern char *ifname;
			strncpy(ifname, "wlan10", 6);
			/* RTW_INFO("%s()-%d: ifname=%s, vid=%04X, pid=%04X\n", __FUNCTION__, __LINE__, ifname, vid, pid); */
		}
#endif /* RTK_DMP_PLATFORM */

	}
}

static int rtw_dev_suspend(struct usb_interface *pusb_intf, pm_message_t message)
{
	struct dvobj_priv *dvobj;
	struct pwrctrl_priv *pwrpriv;
	struct debug_priv *pdbgpriv;
	_adapter *padapter;
	int ret = 0;


	dvobj = usb_get_intfdata(pusb_intf);
	pwrpriv = dvobj_to_pwrctl(dvobj);
	pdbgpriv = &dvobj->drv_dbg;
	padapter = dvobj_get_primary_adapter(dvobj);

	if (pwrpriv->bInSuspend == _TRUE) {
		RTW_INFO("%s bInSuspend = %d\n", __FUNCTION__, pwrpriv->bInSuspend);
		pdbgpriv->dbg_suspend_error_cnt++;
		goto exit;
	}

	ret =  rtw_suspend_common(padapter);

exit:
	return ret;
}

static int rtw_resume_process(_adapter *padapter)
{
	int ret;
	struct pwrctrl_priv *pwrpriv = adapter_to_pwrctl(padapter);
	struct dvobj_priv *pdvobj = padapter->dvobj;
	struct debug_priv *pdbgpriv = &pdvobj->drv_dbg;


	if (pwrpriv->bInSuspend == _FALSE) {
		pdbgpriv->dbg_resume_error_cnt++;
		RTW_INFO("%s bInSuspend = %d\n", __FUNCTION__, pwrpriv->bInSuspend);
		return -1;
	}

	/*
	 * Due to usb wow suspend flow will cancel read/write port via intf_stop and
	 * bReadPortCancel and bWritePortCancel are set _TRUE in intf_stop.
	 * But they will not be clear in intf_start during wow resume flow.
	 * It should move to os_intf in the feature.
	 */
	RTW_ENABLE_FUNC(pdvobj, DF_RX_BIT);
	RTW_ENABLE_FUNC(pdvobj, DF_TX_BIT);

	ret =  rtw_resume_common(padapter);

	return ret;
}

static int rtw_dev_resume(struct usb_interface *pusb_intf)
{
	struct dvobj_priv *dvobj;
	struct pwrctrl_priv *pwrpriv;
	struct debug_priv *pdbgpriv;
	_adapter *padapter;
	struct mlme_ext_priv *pmlmeext;
	int ret = 0;


	dvobj = usb_get_intfdata(pusb_intf);
	pwrpriv = dvobj_to_pwrctl(dvobj);
	pdbgpriv = &dvobj->drv_dbg;
	padapter = dvobj_get_primary_adapter(dvobj);
	pmlmeext = &padapter->mlmeextpriv;

	RTW_INFO("==> %s (%s:%d)\n", __FUNCTION__, current->comm, current->pid);
	pdbgpriv->dbg_resume_cnt++;


	if (pwrpriv->wowlan_mode || pwrpriv->wowlan_ap_mode) {
		rtw_resume_lock_suspend();
		ret = rtw_resume_process(padapter);
		rtw_resume_unlock_suspend();
	} else {
#ifdef CONFIG_RESUME_IN_WORKQUEUE
		rtw_resume_in_workqueue(pwrpriv);
#else
		if (rtw_is_earlysuspend_registered(pwrpriv)) {
			/* jeff: bypass resume here, do in late_resume */
			rtw_set_do_late_resume(pwrpriv, _TRUE);
		} else {
			rtw_resume_lock_suspend();
			ret = rtw_resume_process(padapter);
			rtw_resume_unlock_suspend();
		}
#endif
	}

	pmlmeext->last_scan_time = rtw_get_current_time();
	RTW_INFO("<========  %s return %d\n", __FUNCTION__, ret);

	return ret;
}


/*
 * drv_init() - a device potentially for us
 *
 * notes: drv_init() is called when the bus driver has located a card for us to support.
 *        We accept the new device by returning 0.
*/
_adapter *rtw_usb_primary_adapter_init(struct dvobj_priv *dvobj,
	struct usb_interface *pusb_intf)
{
	_adapter *padapter = NULL;
	int status = _FAIL;
	u8 hw_mac_addr[ETH_ALEN] = {0};

	padapter = (_adapter *)rtw_zvmalloc(sizeof(*padapter));
	if (padapter == NULL)
		goto exit;

	padapter->dvobj = dvobj;

	/*registry_priv*/
	if (rtw_load_registry(padapter) != _SUCCESS)
		goto free_adapter;

	dvobj->padapters[dvobj->iface_nums++] = padapter;
	padapter->iface_id = IFACE_ID0;

	/* set adapter_type/iface type for primary padapter */
	padapter->isprimary = _TRUE;
	padapter->adapter_type = PRIMARY_ADAPTER;

	/* step 5. */
	if (rtw_init_drv_sw(padapter) == _FAIL) {
		goto free_adapter;
	}

#ifdef CONFIG_PM
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 18))
	if (dvobj_to_pwrctl(dvobj)->bSupportRemoteWakeup) {
		dvobj_to_usb(dvobj)->pusbdev->do_remote_wakeup = 1;
		pusb_intf->needs_remote_wakeup = 1;
		device_init_wakeup(&pusb_intf->dev, 1);
		RTW_INFO("pwrctrlpriv.bSupportRemoteWakeup~~~~~~\n");
		RTW_INFO("pwrctrlpriv.bSupportRemoteWakeup~~~[%d]~~~\n",
			device_may_wakeup(&pusb_intf->dev));
	}
#endif
#endif
	/* 2012-07-11 Move here to prevent the 8723AS-VAU BT auto suspend influence */
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 33))
	if (usb_autopm_get_interface(pusb_intf) < 0)
		RTW_INFO("can't get autopm:\n");
#endif
#ifdef CONFIG_BTC
	dvobj_to_pwrctl(dvobj)->autopm_cnt = 1;
#endif

	/* get mac addr */
	rtw_hw_get_mac_addr(dvobj, hw_mac_addr);
	/* set mac addr */
	rtw_macaddr_cfg(adapter_mac_addr(padapter), hw_mac_addr);

	RTW_INFO("bDriverStopped:%s, bSurpriseRemoved:%s, netif_up:%d, hw_init_completed:%d\n"
		 , dev_is_drv_stopped(dvobj) ? "True" : "False"
		 , dev_is_surprise_removed(dvobj) ? "True" : "False"
		 , padapter->netif_up
		 , rtw_hw_get_init_completed(dvobj)
		);

	status = _SUCCESS;

free_adapter:
	if (status != _SUCCESS && padapter) {
		rtw_vmfree((u8 *)padapter, sizeof(*padapter));
		padapter = NULL;
	}
exit:
	return padapter;
}

static void rtw_usb_primary_adapter_deinit(_adapter *padapter)
{
	RTW_INFO(FUNC_ADPT_FMT"\n", FUNC_ADPT_ARG(padapter));

#ifdef CONFIG_BTC
	if (1 == adapter_to_pwrctl(padapter)->autopm_cnt) {
		struct mlme_priv *pmlmepriv = &padapter->mlmepriv;
		struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);
		PUSB_DATA usb_data = dvobj_to_usb(dvobj);

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 33))
		usb_autopm_put_interface(usb_data->pusbintf);
#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 20))
		usb_autopm_enable(usb_data->pusbintf);
#else
		usb_autosuspend_device(usb_data->pusbdev, 1);
#endif
		adapter_to_pwrctl(padapter)->autopm_cnt--;
	}
#endif

	rtw_free_drv_sw(padapter);

	/* TODO: use rtw_os_ndevs_deinit instead at the first stage of driver's dev deinit function */
	rtw_os_ndev_free(padapter);

	rtw_vmfree((u8 *)padapter, sizeof(_adapter));

}

static void rtw_usb_drop_all_phl_rx_pkt(struct dvobj_priv *dvobj)
{
	u16 rx_pkt_num = 0;
	struct rtw_recv_pkt *rx_req = NULL;

	rx_pkt_num = rtw_phl_query_new_rx_num(GET_PHL_INFO(dvobj));

	if (rx_pkt_num) {
		RTW_INFO("%s, rx_pkt not empty !!(%d)\n", __func__, rx_pkt_num);

		while (rx_pkt_num--) {
			rx_req = rtw_phl_query_rx_pkt(GET_PHL_INFO(dvobj));
			if (rx_req) {
				struct sk_buff *skb = rx_req->os_priv;

				rtw_phl_return_rxbuf(GET_PHL_INFO(dvobj), (u8 *)rx_req);
				rtw_skb_free(skb);
			}
		}
	}
}

static int rtw_dev_probe(struct usb_interface *pusb_intf, const struct usb_device_id *pdid)
{
	_adapter *padapter = NULL;
	struct dvobj_priv *dvobj;

	RTW_INFO("+%s\n", __func__);

	/* step 0. */
	process_spec_devid(pdid);

	/* Initialize dvobj_priv */
	dvobj = usb_dvobj_init(pusb_intf, pdid);
	if (dvobj == NULL) {
		RTW_ERR("usb_dvobj_init Failed!\n");
		goto exit;
	}

	if (devobj_trx_resource_init(dvobj) == _FAIL)
		goto free_dvobj;

	/*init hw - register and get chip-info */
	if (rtw_hw_init(dvobj) == _FAIL) {
		RTW_ERR("rtw_hw_init Failed!\n");
		goto free_trx_reso;
	}

	padapter = rtw_usb_primary_adapter_init(dvobj, pusb_intf);
	if (padapter == NULL) {
		RTW_ERR("rtw_usb_primary_adapter_init Failed!\n");
		goto free_hw;
	}

	if (usb_reprobe_switch_usb_mode(padapter) == _TRUE) {
		RTW_ERR("usb_reprobe_switch_usb_mode Failed!\n");
		goto free_if_prim;
	}

#ifdef CONFIG_CONCURRENT_MODE
	if (rtw_drv_add_vir_ifaces(dvobj) == _FAIL)
		goto free_if_vir;
#endif

	if (rtw_adapter_link_init(dvobj) != _SUCCESS)
		goto free_adapter_link;

	/*init data of dvobj from registary and ic spec*/
	if (devobj_data_init(dvobj) == _FAIL) {
		RTW_ERR("devobj_data_init Failed!\n");
		goto free_devobj_data;
	}

#ifdef CONFIG_GLOBAL_UI_PID
	if (ui_pid[1] != 0) {
		RTW_INFO("ui_pid[1]:%d\n", ui_pid[1]);
		rtw_signal_process(ui_pid[1], SIGUSR2);
	}
#endif

	/* dev_alloc_name && register_netdev */
	if (rtw_os_ndevs_init(dvobj) != _SUCCESS) {
		RTW_ERR("rtw_os_ndevs_init Failed!\n");
		goto free_devobj_data;
	}

	/* Update link_mlme_priv's ht/vht/he priv from padapter->mlmepriv */
	rtw_init_link_capab(dvobj);

#ifdef CONFIG_HOSTAPD_MLME
	hostapd_mode_init(padapter);
#endif
	rtw_hw_dump_hal_spec(RTW_DBGDUMP, dvobj);

	RTW_INFO("-%s success\n", __func__);
	return 0; /*_SUCCESS*/

free_devobj_data:
	devobj_data_deinit(dvobj);

free_adapter_link:
	rtw_adapter_link_deinit(dvobj);

#ifdef CONFIG_CONCURRENT_MODE
free_if_vir:
	rtw_drv_stop_vir_ifaces(dvobj);
	rtw_drv_free_vir_ifaces(dvobj);
#endif

free_if_prim:
	if (padapter)
		rtw_usb_primary_adapter_deinit(padapter);
free_hw:
	rtw_hw_deinit(dvobj);

free_trx_reso:
	devobj_trx_resource_deinit(dvobj);

free_dvobj:
	usb_dvobj_deinit(pusb_intf);
exit:
	return -ENODEV;
}

/*
 * dev_remove() - our device is being removed
*/
/* rmmod module & unplug(SurpriseRemoved) will call r871xu_dev_remove() => how to recognize both */
static void rtw_dev_remove(struct usb_interface *pusb_intf)
{
	struct dvobj_priv *dvobj = usb_get_intfdata(pusb_intf);
#if defined(CONFIG_HAS_EARLYSUSPEND) || defined(CONFIG_ANDROID_POWER)
	struct pwrctrl_priv *pwrctl = dvobj_to_pwrctl(dvobj);
#endif
	_adapter *padapter = dvobj_get_primary_adapter(dvobj);

	RTW_INFO("+%s\n", __func__);

	dvobj->processing_dev_remove = _TRUE;

	/* TODO: use rtw_os_ndevs_deinit instead at the first stage of driver's dev deinit function */
	rtw_os_ndevs_unregister(dvobj);

	if (usb_drv.drv_registered == _TRUE) {
		/* RTW_INFO("r871xu_dev_remove():padapter->bSurpriseRemoved == _TRUE\n"); */
		dev_set_surprise_removed(dvobj);
	}


#if defined(CONFIG_HAS_EARLYSUSPEND) || defined(CONFIG_ANDROID_POWER)
	rtw_unregister_early_suspend(pwrctl);
#endif
	dev_set_drv_stopped(adapter_to_dvobj(padapter));	/*for stop thread*/
#if 0 /*#ifdef CONFIG_CORE_CMD_THREAD*/
	rtw_stop_cmd_thread(padapter);
#endif
#ifdef CONFIG_CONCURRENT_MODE
	rtw_drv_stop_vir_ifaces(dvobj);
#endif /* CONFIG_CONCURRENT_MODE */

	rtw_drv_stop_prim_iface(padapter);

	if (rtw_hw_is_init_completed(dvobj)) 
		rtw_hw_stop(dvobj);
	dev_set_surprise_removed(dvobj);

	rtw_usb_drop_all_phl_rx_pkt(dvobj);

	rtw_adapter_link_deinit(dvobj);

	rtw_usb_primary_adapter_deinit(padapter);

#ifdef CONFIG_CONCURRENT_MODE
	rtw_drv_free_vir_ifaces(dvobj);
#endif /* CONFIG_CONCURRENT_MODE */

	rtw_hw_deinit(dvobj);
	devobj_data_deinit(dvobj);
	devobj_trx_resource_deinit(dvobj);
	usb_dvobj_deinit(pusb_intf);

	RTW_INFO("-%s done\n", __func__);
	return;

}
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 24))
extern int console_suspend_enabled;
#endif

static int __init rtw_drv_entry(void)
{
	int ret = 0;

	RTW_PRINT("module init start\n");
	dump_drv_version(RTW_DBGDUMP);
#ifdef BTCOEXVERSION
	RTW_PRINT(DRV_NAME" BT-Coex version = %s\n", BTCOEXVERSION);
#endif /* BTCOEXVERSION */

#if (defined(CONFIG_RTKM) && defined(CONFIG_RTKM_BUILT_IN))
	ret = rtkm_prealloc_init();
	if (ret) {
		RTW_INFO("%s: pre-allocate memory failed!!(%d)\n", __FUNCTION__,
			 ret);
		goto exit;
	}
#endif /* CONFIG_RTKM */

	ret = platform_wifi_power_on();
	if (ret != 0) {
		RTW_INFO("%s: power on failed!!(%d)\n", __FUNCTION__, ret);
		ret = -1;
		goto exit;
	}
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 24))
	/* console_suspend_enabled=0; */
#endif

	usb_drv.drv_registered = _TRUE;
	rtw_suspend_lock_init();
	rtw_drv_proc_init();
	rtw_nlrtw_init();
	rtw_ndev_notifier_register();
	rtw_inetaddr_notifier_register();

	ret = usb_register(&usb_drv.usbdrv);

	if (ret != 0) {
		usb_drv.drv_registered = _FALSE;
		rtw_suspend_lock_uninit();
		rtw_drv_proc_deinit();
		rtw_nlrtw_deinit();
		rtw_ndev_notifier_unregister();
		rtw_inetaddr_notifier_unregister();
		goto exit;
	}

exit:
	RTW_PRINT("module init ret=%d\n", ret);
	return ret;
}

static void __exit rtw_drv_halt(void)
{
	RTW_PRINT("module exit start\n");

	usb_drv.drv_registered = _FALSE;

	usb_deregister(&usb_drv.usbdrv);

	platform_wifi_power_off();

	rtw_suspend_lock_uninit();
	rtw_drv_proc_deinit();
	rtw_nlrtw_deinit();
	rtw_ndev_notifier_unregister();
	rtw_inetaddr_notifier_unregister();

	RTW_PRINT("module exit success\n");

	rtw_mstat_dump(RTW_DBGDUMP);

#if (defined(CONFIG_RTKM) && defined(CONFIG_RTKM_BUILT_IN))
	rtkm_prealloc_destroy();
#elif (defined(CONFIG_RTKM) && defined(CONFIG_RTKM_STANDALONE))
	rtkm_dump_mstatus(RTW_DBGDUMP);
#endif /* CONFIG_RTKM */
}

module_init(rtw_drv_entry);
module_exit(rtw_drv_halt);
