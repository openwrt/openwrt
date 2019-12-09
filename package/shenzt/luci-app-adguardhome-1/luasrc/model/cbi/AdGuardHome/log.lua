local e=require"nixio.fs"
local uci=require"luci.model.uci".cursor()
logfile=uci:get("AdGuardHome","AdGuardHome","logfile")

f=SimpleForm("logview")
f.reset = false
f.submit = false
t=f:field(TextValue,"conf")
t.rmempty=true
t.rows=20
t.template="AdGuardHome/log"
t.readonly="readonly"
nixio.fs.writefile("/var/run/lucilogpos","0")
return f
