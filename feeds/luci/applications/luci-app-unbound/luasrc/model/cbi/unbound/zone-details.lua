-- Copyright 2018 Eric Luehrsen <ericluehrsen@gmail.com>
-- Licensed to the public under the Apache License 2.0.

local sy = require "luci.sys"
local ds = require "luci.dispatcher"
local hp = require "luci.http"
local m7, s7
local ena, flb, zty, znm, srv, rlv, tlu
local prt, tlp, tli, url

arg[1] = arg[1] or ""
m7 = Map("unbound")
m7.redirect = ds.build_url("admin/services/unbound/zones")


if (arg[1] == "") then
    hp.redirect(m7.redirect)
    return

else
    s7 = m7:section(NamedSection, arg[1], "zone",
        translatef("Directed Zone"),
        translatef("Edit a forward, stub, or zone-file-cache zone "
        .. "for Unbound to use instead of recursion."))

    s7.anonymous = true
    s7.addremove = false

    ena = s7:option(Flag, "enabled", translate("Enabled"),
        translate("Enable this directed zone"))
    ena.rmempty = false

    flb = s7:option(Flag, "fallback", translate("Fall Back"),
        translate("Allow open recursion when record not in zone"))
    flb.rmempty = false

    zty = s7:option(ListValue, "zone_type", translate("Zone Type"))
    zty:value("auth_zone", translate("Authoritative (zone file)"))
    zty:value("stub_zone", translate("Stub (forced recursion)"))
    zty:value("forward_zone", translate("Forward (simple handoff)"))
    zty.rmempty = false

    znm = s7:option(DynamicList, "zone_name", translate("Zone Names"),
        translate("Zone (Domain) names included in this zone combination"))
    znm.placeholder="new.example.net."

    srv = s7:option(DynamicList, "server", translate("Servers"),
        translate("Servers for this zone; see README.md for optional form"))
    srv.placeholder="192.0.2.53"

    rlv = s7:option(Flag, "resolv_conf", translate("Use 'resolv.conf.auto'"),
        translate("Forward to upstream nameservers (ISP)"))
    rlv:depends("zone_type", "forward_zone")

    tlu = s7:option(Flag, "tls_upstream", translate("DNS over TLS"),
        translate("Connect to servers using TLS"))
    tlu:depends("zone_type", "forward_zone")

    prt = s7:option(Value, "port", translate("Server Port"),
        translate("Port servers will receive queries on"))
    prt:depends("tls_upstream", false)
    prt.datatype = "port"
    prt.placeholder="53"

    tlp = s7:option(Value, "tls_port", translate("Server TLS Port"),
        translate("Port servers will receive queries on"))
    tlp:depends("tls_upstream", true)
    tlp.datatype = "port"
    tlp.placeholder="853"

    tli = s7:option(Value, "tls_index", translate("TLS Name Index"),
        translate("Domain name to verify TLS certificate"))
    tli:depends("tls_upstream", true)
    tli.placeholder="dns.example.net"

    url = s7:option(Value, "url_dir", translate("Zone Download URL"),
        translate("Directory only part of URL"))
    url:depends("zone_type", "auth_zone")
    url.placeholder="https://www.example.net/dl/zones/"
end


function m7.on_commit(self)
    if sy.init.enabled("unbound") then
        -- Restart Unbound with configuration
        sy.call("/etc/init.d/unbound restart >/dev/null 2>&1")

    else
        sy.call("/etc/init.d/unbound stop >/dev/null 2>&1")
    end
end


return m7

