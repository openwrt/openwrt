/*
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 *   Copyright (C) 2007 John Crispin <blogic@openwrt.org>
 */
#ifndef _IFXMIPS_GPIO_H__
#define _IFXMIPS_GPIO_H__

extern int ifxmips_port_reserve_pin(unsigned int port, unsigned int pin);
extern int ifxmips_port_free_pin(unsigned int port, unsigned int pin);
extern int ifxmips_port_set_open_drain(unsigned int port, unsigned int pin);
extern int ifxmips_port_clear_open_drain(unsigned int port, unsigned int pin);
extern int ifxmips_port_set_pudsel(unsigned int port, unsigned int pin);
extern int ifxmips_port_clear_pudsel(unsigned int port, unsigned int pin);
extern int ifxmips_port_set_puden(unsigned int port, unsigned int pin);
extern int ifxmips_port_clear_puden(unsigned int port, unsigned int pin);
extern int ifxmips_port_set_stoff(unsigned int port, unsigned int pin);
extern int ifxmips_port_clear_stoff(unsigned int port, unsigned int pin);
extern int ifxmips_port_set_dir_out(unsigned int port, unsigned int pin);
extern int ifxmips_port_set_dir_in(unsigned int port, unsigned int pin);
extern int ifxmips_port_set_output(unsigned int port, unsigned int pin);
extern int ifxmips_port_clear_output(unsigned int port, unsigned int pin);
extern int ifxmips_port_get_input(unsigned int port, unsigned int pin);
extern int ifxmips_port_set_altsel0(unsigned int port, unsigned int pin);
extern int ifxmips_port_clear_altsel0(unsigned int port, unsigned int pin);
extern int ifxmips_port_set_altsel1(unsigned int port, unsigned int pin);
extern int ifxmips_port_clear_altsel1(unsigned int port, unsigned int pin);

#endif
