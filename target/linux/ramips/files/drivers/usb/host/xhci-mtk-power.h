#ifndef _XHCI_MTK_POWER_H
#define _XHCI_MTK_POWER_H

#include <linux/usb.h>
#include "xhci.h"
#include "xhci-mtk.h"

void enableXhciAllPortPower(struct xhci_hcd *xhci);
void enableAllClockPower(void);
void disablePortClockPower(void);
void enablePortClockPower(int port_index, int port_rev);

#endif
