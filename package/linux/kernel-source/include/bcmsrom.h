/*
 * Misc useful routines to access NIC srom
 *
 * Copyright 2004, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 * $Id$
 */

#ifndef	_bcmsrom_h_
#define	_bcmsrom_h_

extern int srom_var_init(void *sbh, uint bus, void *curmap, void *osh, char **vars, int *count);

extern int srom_read(uint bus, void *curmap, void *osh, uint byteoff, uint nbytes, uint16 *buf);
extern int srom_write(uint bus, void *curmap, void *osh, uint byteoff, uint nbytes, uint16 *buf);
extern int srom_parsecis(uint8 *cis, char **vars, int *count);
	   
#endif	/* _bcmsrom_h_ */
