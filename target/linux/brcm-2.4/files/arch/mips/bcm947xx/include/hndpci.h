/*
 * HND SiliconBackplane PCI core software interface.
 *
 * Copyright 2007, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 */

#ifndef _hndpci_h_
#define _hndpci_h_

extern int sbpci_read_config(sb_t *sbh, uint bus, uint dev, uint func, uint off, void *buf,
                             int len);
extern int extpci_read_config(sb_t *sbh, uint bus, uint dev, uint func, uint off, void *buf,
                              int len);
extern int sbpci_write_config(sb_t *sbh, uint bus, uint dev, uint func, uint off, void *buf,
                              int len);
extern int extpci_write_config(sb_t *sbh, uint bus, uint dev, uint func, uint off, void *buf,
                               int len);
extern void sbpci_ban(uint16 core);
extern int sbpci_init(sb_t *sbh);
extern int sbpci_init_pci(sb_t *sbh);
extern void sbpci_init_cores(sb_t *sbh);
extern void sbpci_arb_park(sb_t *sbh, uint parkid);

#define PCI_PARK_NVRAM    0xff

#endif /* _hndpci_h_ */
