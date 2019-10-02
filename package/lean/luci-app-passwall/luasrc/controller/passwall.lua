-- Copyright 2018-2019 Lienol <lawlienol@gmail.com>
module("luci.controller.passwall", package.seeall)
local appname = "passwall"
local http = require "luci.http"
local kcptun = require "luci.model.cbi.passwall.api.kcptun"
local brook = require "luci.model.cbi.passwall.api.brook"
local v2ray = require "luci.model.cbi.passwall.api.v2ray"

function index()
    if not nixio.fs.access("/etc/config/passwall") then return end
    entry({"admin", "vpn"}, firstchild(), "VPN", 45).dependent = false
    entry({"admin", "vpn", "passwall", "show"}, call("show_menu")).leaf = true
    entry({"admin", "vpn", "passwall", "hide"}, call("hide_menu")).leaf = true
    if nixio.fs.access("/etc/config/passwall") and                                                                      nixio.fs.access("/etc/config/passwall_show") then
        entry({"admin", "vpn", "passwall"},
              alias("admin", "vpn", "passwall", "settings"), _("Pass Wall"), 1).dependent =
            true
    end
    entry({"admin", "vpn", "passwall", "settings"}, cbi("passwall/global"),
          _("Basic Settings"), 1).dependent = true
    entry({"admin", "vpn", "passwall", "server_list"},
          cbi("passwall/server_list"), _("Server List"), 2).dependent = true

     entry({"admin", "vpn", "passwall", "subscription"},cbi("passwall/subscription"), _("Subscription Managenent"),3).leaf = true

    entry({"admin", "vpn", "passwall", "auto_switch"},
          cbi("passwall/auto_switch"), _("Auto Switch"), 4).leaf = true
    entry({"admin", "vpn", "passwall", "other"}, cbi("passwall/other"),
          _("Other Settings"), 94).leaf = true
    if nixio.fs.access("/usr/sbin/haproxy") then
        entry({"admin", "vpn", "passwall", "balancing"},
              cbi("passwall/balancing"), _("Load Balancing"), 95).leaf = true
    end
     
    entry({"admin", "vpn", "passwall", "rule"}, cbi("passwall/rule"),
          _("Rule Update"), 96).leaf = true
    entry({"admin", "vpn", "passwall", "acl"}, cbi("passwall/acl"),
          _("Access control"), 97).leaf = true
    entry({"admin", "vpn", "passwall", "rulelist"}, cbi("passwall/rulelist"),
          _("Set Blacklist And Whitelist"), 98).leaf = true

   entry({"admin", "vpn", "passwall", "status"},form("passwall/status"),_("Status"), 99).leaf = true
    entry({"admin", "vpn", "passwall", "log"}, cbi("passwall/log"),
          _("Watch Logs"), 100).leaf = true
    entry({"admin", "vpn", "passwall", "serverconfig"},
          cbi("passwall/serverconfig")).leaf = true

    entry({"admin", "vpn", "passwall", "link_add_server"},
          call("link_add_server")).leaf = true
    entry({"admin", "vpn", "passwall", "get_log"}, call("get_log")).leaf = true
    entry({"admin", "vpn", "passwall", "clear_log"}, call("clear_log")).leaf =
        true
    entry({"admin", "vpn", "passwall", "server_status"}, call("server_status")).leaf =
        true
    entry({"admin", "vpn", "passwall", "connect_status"}, call("connect_status")).leaf =
        true
    entry({"admin", "vpn", "passwall", "check_port"}, call("check_port")).leaf =
        true
    entry({"admin", "vpn", "passwall", "auto_ping_server"},
          call("auto_ping_server")).leaf = true
    entry({"admin", "vpn", "passwall", "ping_server"}, call("ping_server")).leaf =
        true
    entry({"admin", "vpn", "passwall", "set_server"}, call("set_server")).leaf =
        true
    entry({"admin", "vpn", "passwall", "copy_server"}, call("copy_server")).leaf =
        true
    entry({"admin", "vpn", "passwall", "update_rules"}, call("update_rules")).leaf =
        true
    entry({"admin", "vpn", "passwall", "kcptun_check"}, call("kcptun_check")).leaf =
        true
    entry({"admin", "vpn", "passwall", "kcptun_update"}, call("kcptun_update")).leaf =
        true
    entry({"admin", "vpn", "passwall", "brook_check"}, call("brook_check")).leaf =
        true
    entry({"admin", "vpn", "passwall", "brook_update"}, call("brook_update")).leaf =
        true
    entry({"admin", "vpn", "passwall", "v2ray_check"}, call("v2ray_check")).leaf =
        true
    entry({"admin", "vpn", "passwall", "v2ray_update"}, call("v2ray_update")).leaf =
        true
end

local function http_write_json(content)
    http.prepare_content("application/json")
    http.write_json(content or {code = 1})
end

function show_menu()
    luci.sys.call("touch /etc/config/passwall_show")
    luci.http.redirect(luci.dispatcher.build_url("admin", "vpn", "passwall"))
end

function hide_menu()
    luci.sys.call("rm -rf /etc/config/passwall_show")
    luci.http.redirect(luci.dispatcher.build_url("admin", "status", "overview"))
end

function link_add_server()
    local type = luci.http.formvalue("type")
    local link = luci.http.formvalue("link")
    if type == "SSR" then
        luci.sys.call('rm -f /tmp/ssr_links.conf && echo "' .. link ..
                          '" >> /tmp/ssr_links.conf')
        luci.sys.call("/usr/share/passwall/subscription_ssr.sh add >/dev/null")
    elseif type == "V2ray" then
        luci.sys.call('rm -f /tmp/v2ray_links.conf && echo "' .. link ..
                          '" >> /tmp/v2ray_links.conf')
        luci.sys
            .call("/usr/share/passwall/subscription_v2ray.sh add >/dev/null")
    end
end

function get_log()
    -- luci.sys.exec("[ -f /var/log/passwall.log ] && sed '1!G;h;$!d' /var/log/passwall.log > /var/log/passwall_show.log")
    luci.http.write(luci.sys.exec("cat /var/log/passwall.log"))
end

function clear_log() luci.sys.call("rm -rf > /var/log/passwall.log") end

function server_status()
    local tcp_redir_port = luci.sys.exec(
                               "echo -n `uci get " .. appname ..
                                   ".@global_proxy[0].tcp_redir_port`")
    local udp_redir_port = luci.sys.exec(
                               "echo -n `uci get " .. appname ..
                                   ".@global_proxy[0].udp_redir_port`")
    -- local dns_mode = luci.sys.exec("echo -n `uci get " .. appname .. ".@global[0].dns_mode`")
    local e = {}
    e.tcp_redir_status = luci.sys.call("ps -w | grep -v grep | grep -i -E '" ..
                                           appname ..
                                           "/TCP|brook tproxy -l 0.0.0.0:" ..
                                           tcp_redir_port .. "' >/dev/null") ==
                             0
    e.udp_redir_status = luci.sys.call("ps -w | grep -v grep | grep -i -E '" ..
                                           appname ..
                                           "/UDP|brook tproxy -l 0.0.0.0:" ..
                                           udp_redir_port .. "' >/dev/null") ==
                             0
    e.socks5_proxy_status = luci.sys.call(
                                "ps -w | grep -v grep | grep -i -E '" .. appname ..
                                    "/SOCKS5|brook client' >/dev/null") == 0
    e.dns_mode_status = luci.sys.call("netstat -apn | grep 7913 >/dev/null") ==
                            0
    e.haproxy_status = luci.sys.call(
                           "ps -w | grep -v grep | grep -i 'haproxy -f /var/etc/" ..
                               appname .. "/haproxy.cfg' >/dev/null") == 0
    e.kcptun_status = luci.sys.call(
                          "ps -w | grep -v grep | grep -i 'log /var/etc/" ..
                              appname .. "/kcptun' >/dev/null") == 0
    luci.http.prepare_content("application/json")
    luci.http.write_json(e)
end

function connect_status()
    local e = {}
    if luci.http.formvalue("type") == "google" then
        e.status = luci.sys.call(
                       "echo `curl -I -o /dev/null -s -m 10 --connect-timeout 5 -w %{http_code} 'https://www.google.com'` | grep 200 >/dev/null") ==
                       0
    else
        e.status = luci.sys.call(
                       "echo `curl -I -o /dev/null -s -m 10 --connect-timeout 2 -w %{http_code} 'http://www.baidu.com'` | grep 200 >/dev/null") ==
                       0
    end
    luci.http.prepare_content("application/json")
    luci.http.write_json(e)
end

function auto_ping_server()
    local e = {}
    e.index = luci.http.formvalue("index")
    e.ping = luci.sys.exec(
                 "ping -c 1 -W 1 %q 2>&1 | grep -o 'time=[0-9]*' | awk -F '=' '{print$2}'" %
                     luci.http.formvalue("domain"))
    luci.http.prepare_content("application/json")
    luci.http.write_json(e)
end

function ping_server()
    local e = {}
    local server = luci.http.formvalue("server")
    e.ping = luci.sys.exec(
                 "echo -n `ping -c 1 -W 1 %q 2>&1 | grep -o 'time=[0-9]*' | awk -F '=' '{print$2}'`" %
                     server)
    luci.http.prepare_content("application/json")
    luci.http.write_json(e)
end

function set_server()
    local e = {}
    local protocol = luci.http.formvalue("protocol")
    local section = luci.http.formvalue("section")
    if protocol == "tcp" then
        luci.sys.call(
            "uci set passwall.@global[0].tcp_redir_server=" .. section ..
                " && uci commit passwall && /etc/init.d/passwall restart")
    elseif protocol == "udp" then
        luci.sys.call(
            "uci set passwall.@global[0].udp_redir_server=" .. section ..
                " && uci commit passwall && /etc/init.d/passwall restart")
    end
    luci.http.prepare_content("application/json")
    luci.http.write_json(e)
end

function copy_server()
    local e = {}
    local section = luci.http.formvalue("section")
    luci.http.prepare_content("application/json")
    luci.http.write_json(e)
end

function check_port()
    local retstring = "<br />"
    retstring = retstring ..
                    "<font color='red'>暂时不支持UDP检测</font><br />"
    local s
    local server_name = ""
    local uci = luci.model.uci.cursor()

    uci:foreach("passwall", "servers", function(s)
        local ret = ""
        local tcp_socket
        local udp_socket
        if (s.use_kcp and s.use_kcp == "1" and s.kcp_port) or
            (s.v2ray_transport and s.v2ray_transport == "mkcp" and s.server_port) then
            --[[local port = (s.use_kcp == "1" and s.kcp_port) and s.kcp_port or (s.v2ray_transport == "mkcp" and s.server_port) and s.server_port or nil
			if port then
				udp_socket = nixio.socket("inet", "dgram")
				udp_socket:setopt("socket", "rcvtimeo", 3)
				udp_socket:setopt("socket", "sndtimeo", 3)
				udp_socket:sendto("test", s.server, port)
				r,c,d=udp_socket:recvfrom(10)
				ret=""
			end--]]
        else
            if s.server_type and s.server and s.server_port and s.remarks then
                server_name = "%s：[%s] %s:%s" %
                                  {
                        s.server_type, s.remarks, s.server, s.server_port
                    }
            end
            tcp_socket = nixio.socket("inet", "stream")
            tcp_socket:setopt("socket", "rcvtimeo", 3)
            tcp_socket:setopt("socket", "sndtimeo", 3)
            ret = tcp_socket:connect(s.server, s.server_port)
            if tostring(ret) == "true" then
                retstring =
                    retstring .. "<font color='green'>" .. server_name ..
                        "   OK.</font><br />"
            else
                retstring = retstring .. "<font color='red'>" .. server_name ..
                                "   Error.</font><br />"
            end
            ret = ""
        end
        if tcp_socket then tcp_socket:close() end
        if udp_socket then udp_socket:close() end
    end)
    luci.http.prepare_content("application/json")
    luci.http.write_json({ret = retstring})
end

function update_rules()
    local update = luci.http.formvalue("update")
    luci.sys.call("nohup /usr/share/passwall/rule_update.sh '" .. update ..
                      "' 2>&1 &")
end

function kcptun_check()
    local json = kcptun.to_check("")
    http_write_json(json)
end

function kcptun_update()
    local json = nil
    local task = http.formvalue("task")
    if task == "extract" then
        json = kcptun.to_extract(http.formvalue("file"),
                                 http.formvalue("subfix"))
    elseif task == "move" then
        json = kcptun.to_move(http.formvalue("file"))
    else
        json = kcptun.to_download(http.formvalue("url"))
    end

    http_write_json(json)
end

function brook_check()
    local json = brook.to_check("")
    http_write_json(json)
end

function brook_update()
    local json = nil
    local task = http.formvalue("task")
    if task == "move" then
        json = brook.to_move(http.formvalue("file"))
    else
        json = brook.to_download(http.formvalue("url"))
    end

    http_write_json(json)
end

function v2ray_check()
    local json = v2ray.to_check("")
    http_write_json(json)
end

function v2ray_update()
    local json = nil
    local task = http.formvalue("task")
    if task == "extract" then
        json =
            v2ray.to_extract(http.formvalue("file"), http.formvalue("subfix"))
    elseif task == "move" then
        json = v2ray.to_move(http.formvalue("file"))
    else
        json = v2ray.to_download(http.formvalue("url"))
    end

    http_write_json(json)
end
