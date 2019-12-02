module("luci.controller.AdGuardHome",package.seeall)
nixio=require"nixio"
local uci=require"luci.model.uci".cursor()
function index()
entry({"admin", "services", "AdGuardHome"},
		alias("admin", "services", "AdGuardHome", "base"),
		_("AdGuard Home"), 10).dependent = true
	entry({"admin","services","AdGuardHome","base"},cbi("AdGuardHome/base"),_("Base Setting"),1).leaf = true
    entry({"admin","services","AdGuardHome","log"},form("AdGuardHome/log"),_("Log"),2).leaf = true
	entry({"admin","services","AdGuardHome","manual"},cbi("AdGuardHome/manual"),_("Manual Config"),3).leaf = true
    entry({"admin","services","AdGuardHome","status"},call("act_status")).leaf=true
	entry({"admin", "services", "AdGuardHome", "check"}, call("check_update"))
	entry({"admin", "services", "AdGuardHome", "doupdate"}, call("do_update"))
end 

function act_status()
  local e={}
  binpath=uci:get("AdGuardHome","AdGuardHome","binpath")
  e.running=luci.sys.call("pgrep "..binpath.." >/dev/null")==0
  luci.http.prepare_content("application/json")
  luci.http.write_json(e)
end
function do_update()
nixio.fs.writefile("/var/run/lucilogpos","0")
luci.sys.exec("(touch /var/run/update_core ; sh /usr/share/AdGuardHome/update_core.sh >/tmp/AdGuardHome_update.log;rm /var/run/update_core) &")
luci.http.prepare_content("application/json")
luci.http.write('')
end

function check_update()
	luci.http.prepare_content("text/plain; charset=utf-8")
	logpos=nixio.fs.readfile("/var/run/lucilogpos")
	if (logpos ~= nil) then
	fdp=tonumber(logpos)
	else
	fdp=0
	end
	f=io.open("/tmp/AdGuardHome_update.log", "r+")
	f:seek("set",fdp)
	a=f:read(8192)
	if (a==nil) then
	a=""
	end
	fdp=f:seek()
	nixio.fs.writefile("/var/run/lucilogpos",tostring(fdp))
	f:close()
if nixio.fs.access("/var/run/update_core") then
	luci.http.write(a)
else
	luci.http.write(a.."\0")
end
end
