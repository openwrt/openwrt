-- Copyright (C) 2018 XiaoShan mivm.cn

local m, s ,o

m = Map("k3usb", translate("k3usb"), translate("k3 usb mode"))

s = m:section(TypedSection, "config", translate("usb mode") )
s.anonymous = true

o = s:option(Flag, "usb3_enable", translate("usb3 enable :"), translate("usb3 mode enable"))
o.default = 1

return m
