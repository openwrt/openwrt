#
# (C) Copyright 2003
# Wolfgang Denk, DENX Software Engineering, wd@denx.de.
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
# Danube board with MIPS 24Kc CPU core
#
sinclude $(OBJTREE)/board/$(BOARDDIR)/config.tmp

ifdef CONFIG_LZMA_BOOTSTRAP

ifdef BUILD_BOOTSTRAP

$(info BUILD_BOOTSTRAP )
#TEXT_BASE = 0xB0000000
TEXT_BASE = 0x80010000

else # BUILD_BOOTSTRAP

ifndef TEXT_BASE
$(info redefine TEXT_BASE = 0x80040000 )
TEXT_BASE = 0x80040000
endif

endif # BUILD_BOOTSTRAP

else

ifdef BUILD_BOOTSTRAP
$(error BUILD_BOOTSTRAP but not enabled in config)
endif

ifndef TEXT_BASE
## Standard: boot from ebu
$(info redefine TEXT_BASE = 0xB0000000 )
TEXT_BASE = 0xB0000000
## For testing: boot from RAM
# TEXT_BASE = 0x80100000
endif

endif # CONFIG_LZMA_BOOTSTRAP
