# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/Default
	NAME:=Default Profile
	PRIORITY:=1
endef

define Profile/Default/Description
	Default package set compatible with most boards.
endef
<<<<<<< HEAD
=======

>>>>>>> d20007ce91e25385d792faedc3146cbfdc34a8c5
$(eval $(call Profile,Default))
