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
 *
 *   Copyright (C) 2007 John Crispin <blogic@openwrt.org> 
 *
 */
#ifndef _IFXMIPS_IOCTL_H__
#define _IFXMIPS_IOCTL_H__

/*------------ LED */

struct danube_port_ioctl_parm
{
	int port;
	int pin;
	int value;
};

#define IFXMIPS_PORT_IOC_MAGIC		0xbf
#define IFXMIPS_PORT_IOCOD			_IOW(IFXMIPS_PORT_IOC_MAGIC,0,struct danube_port_ioctl_parm)
#define IFXMIPS_PORT_IOCPUDSEL		_IOW(IFXMIPS_PORT_IOC_MAGIC,1,struct danube_port_ioctl_parm)
#define IFXMIPS_PORT_IOCPUDEN		_IOW(IFXMIPS_PORT_IOC_MAGIC,2,struct danube_port_ioctl_parm)
#define IFXMIPS_PORT_IOCSTOFF		_IOW(IFXMIPS_PORT_IOC_MAGIC,3,struct danube_port_ioctl_parm)
#define IFXMIPS_PORT_IOCDIR			_IOW(IFXMIPS_PORT_IOC_MAGIC,4,struct danube_port_ioctl_parm)
#define IFXMIPS_PORT_IOCOUTPUT		_IOW(IFXMIPS_PORT_IOC_MAGIC,5,struct danube_port_ioctl_parm)
#define IFXMIPS_PORT_IOCINPUT		_IOWR(IFXMIPS_PORT_IOC_MAGIC,6,struct danube_port_ioctl_parm)
#define IFXMIPS_PORT_IOCALTSEL0		_IOW(IFXMIPS_PORT_IOC_MAGIC,7,struct danube_port_ioctl_parm)
#define IFXMIPS_PORT_IOCALTSEL1		_IOW(IFXMIPS_PORT_IOC_MAGIC,8,struct danube_port_ioctl_parm)

#endif
