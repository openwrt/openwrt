define Profile/EASY80920-NOR
  NAME:=EASY80920-NOR
endef

define Profile/EASY80920-NOR/Description
	Lantiq EASY80920 evalkit (NOR)
endef

$(eval $(call Profile,EASY80920-NOR))

define Profile/EASY80920-NAND
  NAME:=EASY80920-NAND
endef

define Profile/EASY80920-NAND/Description
	Lantiq EASY80920 evalkit (NAND)
endef

$(eval $(call Profile,EASY80920-NAND))

define Profile/ARV7525PW
  NAME:=ARV7525PW
endef

define Profile/ARV7525PW/Description
	Speedport W303V - ARV7525PW
endef

$(eval $(call Profile,ARV7525PW))
