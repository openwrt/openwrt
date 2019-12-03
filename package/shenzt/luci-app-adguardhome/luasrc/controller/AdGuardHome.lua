module("luci.controller.AdGuardHome",package.seeall)
function index()
if not nixio.fs.access("/etc/config/AdGuardHome")then
return
end
	entry({"admin","services","AdGuardHome"},cbi("AdGuardHome"),_("AdGuard Home"),30).dependent=true
    entry({"admin","services","AdGuardHome","status"},call("act_status")).leaf=true
end 

function act_status()
  local e={}
  e.running=luci.sys.call("pgrep -f AdGuardHome >/dev/null")==0
  luci.http.prepare_content("application/json")
  luci.http.write_json(e)
end
