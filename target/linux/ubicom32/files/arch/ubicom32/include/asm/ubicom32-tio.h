/*
 * arch/ubicom32/include/asm/ubicom32-tio.h
 *   Threaded I/O interface definitions.
 *
 * (C) Copyright 2009, Ubicom, Inc.
 *
 * This file is part of the Ubicom32 Linux Kernel Port.
 *
 * The Ubicom32 Linux Kernel Port is free software: you can redistribute
 * it and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 2 of the
 * License, or (at your option) any later version.
 *
 * The Ubicom32 Linux Kernel Port is distributed in the hope that it
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
 * the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the Ubicom32 Linux Kernel Port.  If not,
 * see <http://www.gnu.org/licenses/>.
 *
 * Ubicom32 implementation derived from (with many thanks):
 *   arch/m68knommu
 *   arch/blackfin
 *   arch/parisc
 */
#ifndef _ASM_UBICOM32_UBICOM32_TIO_H
#define _ASM_UBICOM32_UBICOM32_TIO_H

extern u8_t usb_tio_read_u16(u32_t address, u16_t *data);
extern u8_t usb_tio_read_u8(u32_t address, u8_t *data);

extern u8_t usb_tio_write_u16(u32_t address, u16_t data);
extern u8_t usb_tio_write_u8(u32_t address, u8_t data);

extern u8_t usb_tio_read_fifo(u32_t address, u32_t buffer, u32_t bytes);
extern u8_t usb_tio_write_fifo(u32_t address, u32_t buffer, u32_t bytes);
extern u8_t usb_tio_write_fifo_sync(u32_t address, u32_t buffer, u32_t bytes);
extern void usb_tio_read_int_status(u8_t *int_usb, u16_t *int_tx, u16_t *int_rx);

#endif /* _ASM_UBICOM32_UBICOM32_TIO_H */
