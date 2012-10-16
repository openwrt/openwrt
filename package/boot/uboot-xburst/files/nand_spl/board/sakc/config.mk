#
# (C) Copyright 2006
# Stefan Roese, DENX Software Engineering, sr@denx.de.
#
# See file CREDITS for list of people who contributed to this
# project.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 2 of
# the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston,
# MA 02111-1307 USA
#
#
# Ingenic JZ4740 Reference Platform
#

#
# TEXT_BASE for SPL:
#
# On JZ4730 platforms the SPL is located at 0x80000000...0x80001000,
# in the first 4kBytes of memory space in cache. So we set
# TEXT_BASE to starting address in internal cache here.
#
TEXT_BASE = 0x80000000
