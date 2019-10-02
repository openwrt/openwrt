-- Copyright 2018-2019 Lienol <lawlienol@gmail.com>
module("luci.controller.ssr_libev_server", package.seeall)
local http = require "luci.http"

function index()
    if not nixio.fs.access("/etc/config/ssr_libev_server") then return end
    entry({"admin", "vpn"}, firstchild(), "VPN", 45).dependent = false
    entry({"admin", "vpn", "ssr_libev_server"}, cbi("ssr_libev_server/index"),
          _("SSR Libev Server"), 2).dependent = true
    entry({"admin", "vpn", "ssr_libev_server", "config"},
          cbi("ssr_libev_server/config")).leaf = true

    entry({"admin", "vpn", "ssr_libev_server", "users_status"},
          call("ssr_libev_users_status")).leaf = true
end

local function http_write_json(content)
    http.prepare_content("application/json")
    http.write_json(content or {code = 1})
end

function ssr_libev_users_status()
    local e = {}
    e.index = luci.http.formvalue("index")
    e.status = luci.sys.call(
                   "ps -w| grep -v grep | grep '/var/etc/ssr_libev_server/" ..
                       luci.http.formvalue("id") .. "' >/dev/null") == 0
    http_write_json(e)
end
