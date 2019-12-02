local e=require"nixio.fs"
local uci=require"luci.model.uci".cursor()
logfile=uci:get("AdGuardHome","AdGuardHome","logfile")

f=SimpleForm("logview")
t=f:field(TextValue,"conf")
t.rmempty=true
t.rows=20
function t.cfgvalue()
if (logfile==nil) then
	return "no log available"
end
return e.readfile(logfile)
end
t.readonly="readonly"
return f
