--[[
LuCI - Lua Configuration Interface - vsftpd support

Script by Admin @ NVACG.org (af_xj@hotmail.com , xujun@smm.cn)
Some codes is based on luci-app-upnp, TKS.
The Author of luci-app-upnp is Steven Barth <steven@midlink.org> and Jo-Philipp Wich <xm@subsignal.org>

Licensed under the GPL License, Version 3.0 (the "license");
you may not use this file except in compliance with the License.
you may obtain a copy of the License at

	http://www.gnu.org/licenses/gpl.txt

$Id$
]]--

module("luci.controller.vsftpd",package.seeall)

function index()
	require("luci.i18n")
	luci.i18n.loadc("vsftpd")
	if not nixio.fs.access("/etc/config/vsftpd") then
		return
	end
	
	local page = entry({"admin","services","vsftpd"},cbi("vsftpd"),_("FTP Service"))
	page.i18n="vsftpd"
	page.dependent=true
	
	entry({"admin","services","vsftpd","status"}, call("connection_status")).leaf = true
end

function connection_status()
  local cmd = io.popen("ps | grep vsftpd | grep -v grep | grep -v IDLE")
  if cmd then
    local conn = { }
    while true do
      local ln = cmd:read("*l")
      if not ln then
        break
      elseif ln:match("^.%d+.-%d+.%d+.%d+.%d+:.[a-z]+") then
        local num,ip,act = ln:match("^.(%d+).-(%d+.%d+.%d+.%d+):.([a-z]+)")
        if num and ip and act then
            num   = tonumber(num)
            conn[#conn+1]= {
              num   = num,
              ip    = ip,
              user  = "",
              act   = act:upper(),
              file  = ""
            }
        end
        
      elseif ln:match("^.%d+.-%d+.%d+.%d+.%d+/%w+:.%a+.[%w%p]+") then
        local num,ip,user,act,file = ln:match("^.(%d+).-(%d+.%d+.%d+.%d+)/(%w+):.(%u+).([%w%p]+)")
        if num and ip and act then
            num   = tonumber(num)
            conn[#conn+1]= {
              num   = num,
              ip    = ip,
              user  = user,
              act   = act,
              file  = file
            }
        end
      end
    end
  
  cmd:close()
  luci.http.prepare_content("application/json")
  luci.http.write_json(conn)
  end
end
