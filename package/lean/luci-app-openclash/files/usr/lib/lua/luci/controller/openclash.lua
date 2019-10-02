module("luci.controller.openclash", package.seeall)

function index()
	if not nixio.fs.access("/etc/config/openclash") then
		return
	end


	entry({"admin", "services", "openclash"},alias("admin", "services", "openclash", "client"), _("OpenClash"), 50).dependent = true
	entry({"admin", "services", "openclash", "client"},form("openclash/client"),_("Overviews"), 20).leaf = true
	entry({"admin", "services", "openclash", "status"},call("action_status")).leaf=true
	entry({"admin", "services", "openclash", "state"},call("action_state")).leaf=true
	entry({"admin", "services", "openclash", "startlog"},call("action_start")).leaf=true
	entry({"admin", "services", "openclash", "currentversion"},call("action_currentversion"))
	entry({"admin", "services", "openclash", "lastversion"},call("action_lastversion"))
	entry({"admin", "services", "openclash", "update"},call("action_update"))
	entry({"admin", "services", "openclash", "update_ma"},call("action_update_ma"))
	entry({"admin", "services", "openclash", "opupdate"},call("action_opupdate"))
	entry({"admin", "services", "openclash", "coreupdate"},call("action_coreupdate"))
	entry({"admin", "services", "openclash", "settings"},cbi("openclash/settings"),_("Global Settings"), 30).leaf = true
	entry({"admin", "services", "openclash", "servers"},cbi("openclash/servers"),_("Severs&Groups"), 40).leaf = true
  entry({"admin", "services", "openclash", "servers-config"},cbi("openclash/servers-config"), nil).leaf = true
  entry({"admin", "services", "openclash", "groups-config"},cbi("openclash/groups-config"), nil).leaf = true
	entry({"admin", "services", "openclash", "config"},form("openclash/config"),_("Server Config"), 50).leaf = true
	entry({"admin", "services", "openclash", "log"},form("openclash/log"),_("Logs"), 60).leaf = true

	
end


local function is_running()
	return luci.sys.call("pidof clash >/dev/null") == 0
end

local function is_web()
	return luci.sys.call("pidof clash >/dev/null") == 0
end

local function is_watchdog()
	return luci.sys.exec("ps |grep openclash_watchdog.sh |grep -v grep 2>/dev/null |sed -n 1p")
end

local function config_check()
  local yaml = luci.sys.call("ls -l /etc/openclash/config.yaml >/dev/null 2>&1")
  local yml = luci.sys.call("ls -l /etc/openclash/config.yml >/dev/null 2>&1")
  local proxy,group,rule
  if (yaml == 0) then
     proxy = luci.sys.call("egrep '^ {0,}Proxy:' /etc/openclash/config.yaml >/dev/null 2>&1")
     group = luci.sys.call("egrep '^ {0,}Proxy Group:' /etc/openclash/config.yaml >/dev/null 2>&1")
     rule = luci.sys.call("egrep '^ {0,}Rule:' /etc/openclash/config.yaml >/dev/null 2>&1")
  else
     if (yml == 0) then
        proxy = luci.sys.call("egrep '^ {0,}Proxy:' /etc/openclash/config.yml >/dev/null 2>&1")
        group = luci.sys.call("egrep '^ {0,}Proxy Group:' /etc/openclash/config.yml >/dev/null 2>&1")
        rule = luci.sys.call("egrep '^ {0,}Rule:' /etc/openclash/config.yml >/dev/null 2>&1")
     end
  end
  if (yaml == 0) or (yml == 0) then
     if (proxy == 0) then
        proxy = ""
     else
        proxy = " - 代理服务器"
     end
     if (group == 0) then
        group = ""
     else
        group = " - 策略组"
     end
     if (rule == 0) then
        rule = ""
     else
        rule = " - 规则"
     end
	   return proxy..group..rule
	elseif (yaml ~= 0) and (yml ~= 0) then
	   return "1"
	end
end

local function cn_port()
	return luci.sys.exec("uci get openclash.config.cn_port 2>/dev/null")
end

local function mode()
	return luci.sys.exec("uci get openclash.config.en_mode 2>/dev/null")
end

local function config()
   local config_update = luci.sys.exec("ls -l --full-time /etc/openclash/config.bak 2>/dev/null |awk '{print $6,$7;}'")
   if (config_update ~= "") then
      return config_update
   else
      local yaml = luci.sys.call("ls -l /etc/openclash/config.yaml >/dev/null 2>&1")
      if (yaml == 0) then
         return "0"
      else
         local yml = luci.sys.call("ls -l /etc/openclash/config.yml >/dev/null 2>&1")
         if (yml == 0) then
            return "0"
         else
            return "1"
         end
      end
   end
end

local function ipdb()
	return luci.sys.exec("ls -l --full-time /etc/openclash/Country.mmdb 2>/dev/null |awk '{print $6,$7;}'")
end

local function lhie1()
	return luci.sys.exec("ls -l --full-time /etc/openclash/lhie1.yaml 2>/dev/null |awk '{print $6,$7;}'")
end

local function ConnersHua()
	return luci.sys.exec("ls -l --full-time /etc/openclash/ConnersHua.yaml 2>/dev/null |awk '{print $6,$7;}'")
end

local function ConnersHua_return()
	return luci.sys.exec("ls -l --full-time /etc/openclash/ConnersHua_return.yaml 2>/dev/null |awk '{print $6,$7;}'")
end

local function daip()
	return luci.sys.exec("uci get network.lan.ipaddr")
end

local function dase()
	return luci.sys.exec("uci get openclash.config.dashboard_password 2>/dev/null")
end

local function check_lastversion()
	return luci.sys.exec("sh /usr/share/openclash/openclash_version.sh && sed -n '/^https:/,$p' /tmp/openclash_last_version 2>/dev/null")
end

local function check_currentversion()
	return luci.sys.exec("sed -n '/^data:image/,$p' /etc/openclash/openclash_version 2>/dev/null")
end

local function startlog()
	return luci.sys.exec("sed -n '$p' /tmp/openclash_start.log 2>/dev/null")
end

local function coremodel()
  local coremodel = luci.sys.exec("cat /proc/cpuinfo |grep 'cpu model' 2>/dev/null |awk -F ': ' '{print $2}' 2>/dev/null")
  if not coremodel or coremodel == "" then
     return luci.sys.exec("opkg status libc 2>/dev/null |grep 'Architecture' |awk -F ': ' '{print $2}' 2>/dev/null")
  else
     return coremodel
  end
end

local function coremodel2()
	return luci.sys.exec("opkg status libc 2>/dev/null |grep 'Architecture' |awk -F ': ' '{print $2}' 2>/dev/null")
end

local function corecv()
if not nixio.fs.access("/etc/openclash/clash") then
  return "0"
else
	return luci.sys.exec("/etc/openclash/clash -v 2>/dev/null 2>/dev/null |awk -F ' ' '{print $2}'")
end
end

local function corelv()
	return luci.sys.exec("sh /usr/share/openclash/clash_version.sh && sed -n 1p /tmp/clash_last_version 2>/dev/null")
end

local function opcv()
	return luci.sys.exec("sed -n 1p /etc/openclash/openclash_version 2>/dev/null")
end

local function oplv()
   return luci.sys.exec("sh /usr/share/openclash/openclash_version.sh && sed -n 1p /tmp/openclash_last_version 2>/dev/null |sed 's/^v//g' 2>/dev/null")
end

local function opup()
   return luci.sys.exec("rm -rf /tmp/*_last_version 2>/dev/null && sh /usr/share/openclash/openclash_version.sh && sh /usr/share/openclash/openclash_update.sh >/dev/null 2>&1 &")
end

local function coreup()
   return luci.sys.exec("uci set openclash.config.enable=1 && uci commit openclash && rm -rf /tmp/*_last_version 2>/dev/null && sh /usr/share/openclash/clash_version.sh && sh /usr/share/openclash/openclash_core.sh >/dev/null 2>&1 &")
end

local function corever()
   return luci.sys.exec("uci get openclash.config.core_version 2>/dev/null")
end

local function upchecktime()
   local corecheck = luci.sys.exec("ls -l --full-time /tmp/clash_last_version 2>/dev/null |awk '{print $6,$7;}'")
   local opcheck
   if not corecheck or corecheck == "" then
      opcheck = luci.sys.exec("ls -l --full-time /tmp/openclash_last_version 2>/dev/null |awk '{print $6,$7;}'")
      if not opcheck or opcheck == "" then
         return "1"
      else
         return opcheck
      end
   else
      return corecheck
   end
end


function action_status()
	luci.http.prepare_content("application/json")
	luci.http.write_json({
	  clash = is_running(),
		watchdog = is_watchdog(),
		daip = daip(),
		dase = dase(),
		web = is_web(),
		cn_port = cn_port(),
		mode = mode();
	})
end
function action_state()
	luci.http.prepare_content("application/json")
	luci.http.write_json({
		config_check = config_check(),
		config = config(),
		lhie1 = lhie1(),
		ConnersHua = ConnersHua(),
		ConnersHua_return = ConnersHua_return(),
		ipdb = ipdb();
	})
end

function action_lastversion()
	luci.http.prepare_content("application/json")
	luci.http.write_json({
			lastversion = check_lastversion();
	})
end

function action_currentversion()
	luci.http.prepare_content("application/json")
	luci.http.write_json({
			currentversion = check_currentversion();
	})
end

function action_start()
	luci.http.prepare_content("application/json")
	luci.http.write_json({
			startlog = startlog();
	})
end

function action_update()
	luci.http.prepare_content("application/json")
	luci.http.write_json({
			coremodel = coremodel(),
			coremodel2 = coremodel2(),
			corecv = corecv(),
			opcv = opcv(),
			corever = corever(),
			upchecktime = upchecktime(),
			corelv = corelv(),
			oplv = oplv();
	})
end

function action_update_ma()
	luci.http.prepare_content("application/json")
	luci.http.write_json({
			oplv = oplv(),
			corever = corever();
	})
end

function action_opupdate()
	luci.http.prepare_content("application/json")
	luci.http.write_json({
			opup = opup();
	})
end

function action_coreupdate()
	luci.http.prepare_content("application/json")
	luci.http.write_json({
			coreup = coreup();
	})
end