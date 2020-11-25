-- Copyright 2018 Eric Luehrsen <ericluehrsen@gmail.com>
-- Licensed to the public under the Apache License 2.0.

local m5, s5
local ztype, zones, servers, fallback, enabled

local fs = require "nixio.fs"
local ut = require "luci.util"
local sy = require "luci.sys"
local ds = require "luci.dispatcher"
local resolvfile = "/tmp/resolv.conf.auto"
local logerr = ut.exec("logread -e 'unbound.*error.*ssl library'")

m5 = Map("unbound")
s5 = m5:section(TypedSection, "zone", "Zones",
    translatef("Organize directed forward, stub, and authoritative zones"
    .. " <a href=\"%s\" target=\"_blank\">(help)</a>.",
    "https://www.unbound.net/",
    "https://github.com/openwrt/packages/blob/master/net/unbound/files/README.md"))

s5.addremove = true
s5.anonymous = true
s5.sortable = true
s5.template = "cbi/tblsection"
s5.extedit = ds.build_url("admin/services/unbound/zones/%s")

ztype = s5:option(DummyValue, "DummyType", translate("Type"))
ztype.rawhtml = true

zones = s5:option(DummyValue, "DummyZones", translate("Zones"))
zones.rawhtml = true

servers = s5:option(DummyValue, "DummyServers", translate("Servers"))
servers.rawhtml = true

fallback = s5:option(Flag, "fallback", translate("Fallback"))
fallback.rmempty = false

enabled = s5:option(Flag, "enabled", translate("Enable"))
enabled.rmempty = false


if logerr and (#logerr > 0) then
    logerr = logerr:sub((1 + #logerr - math.min(#logerr, 250)), #logerr)
    m5.message = translatef( "Note: SSL/TLS library is missing an API. "
        .. "Please review syslog. >> logread ... " .. logerr )
end


function s5.create(self, section)
    created = TypedSection.create(self, section)
end


function s5.parse(self, ...)
    TypedSection.parse(self, ...)
end


function ztype.cfgvalue(self, s)
    -- Format a meaningful tile for the Zone Type column
    local itxt = self.map:get(s, "zone_type")
    local itls = self.map:get(s, "tls_upstream")


    if itxt and itxt:match("forward") then
        if itls and (itls == "1") then
            return translate("Forward TLS")

        else
            return translate("Forward")
        end

    elseif itxt and itxt:match("stub") then
        return translate("Recurse")

    elseif itxt and itxt:match("auth") then
        return translate("AXFR")

    else
        return translate("Undefined")
    end
end


function zones.cfgvalue(self, s)
    -- Format a meaningful sentence for the Zones viewed column
    local xtxt, otxt
    local itxt = self.map:get(s, "zone_name")
    local itype = self.map:get(s, "zone_type")


    for xtxt in ut.imatch(itxt) do
        if (xtxt == ".") then
            -- zone_name lists
            xtxt = translate("(root)")
        end


        if otxt and (#otxt > 0) then
            otxt = otxt .. ", <var>%s</var>" % xtxt

        else
            otxt = "<var>%s</var>" % xtxt
        end
    end


    if otxt and (#otxt > 0) then
        if itype and itype:match("forward") then
            -- from zone_type create a readable hint for the action
            otxt = translate("accept upstream results for ") .. otxt

        elseif itype and itype:match("stub") then
            otxt = translate("select recursion for ") .. otxt

        elseif itype and itype:match("auth") then
            otxt = translate("prefetch zone files for ") .. otxt

        else
            otxt = translate("unknown action for ") .. otxt
        end


        return otxt

    else
        return "(empty)"
    end
end


function servers.cfgvalue(self, s)
    -- Format a meaningful sentence for the Servers (and URL) column
    local xtxt, otxt, rtxt, found
    local itxt = self.map:get(s, "server")
    local iurl = self.map:get(s, "url_dir")
    local itype = self.map:get(s, "zone_type")
    local itls = self.map:get(s, "tls_upstream")
    local iidx = self.map:get(s, "tls_index")
    local irslv = self.map:get(s, "resolv_conf")


    for xtxt in ut.imatch(itxt) do
        if otxt and (#otxt > 0) then
            -- bundle and make pretty the server list
            otxt = otxt .. ", <var>%s</var>" % xtxt

        else
            otxt = "<var>%s</var>" % xtxt
        end
    end


    if otxt and (#otxt > 0) then
        otxt = translate("use nameservers ") .. otxt
    end


    if otxt and (#otxt > 0)
    and itls and (itls == "1")
    and iidx and (#iidx > 0) then
        -- show TLS certificate name index if provided
        otxt = otxt .. translatef(
                    " with default certificate for <var>%s</var>", iidx)
    end


    if iurl and (#iurl > 0) and itype and itype:match("auth") then
        if otxt and (#otxt > 0) then
            -- include optional URL filed for auth-zone: type
            otxt = otxt .. translatef(", and try <var>%s</var>", iurl)

        else
            otxt = translatef("download from <var>%s</var>", iurl)
        end
    end


    if irslv and (irslv == "1") and itype and itype:match("forward") then
        for xtxt in ut.imatch(fs.readfile(resolvfile)) do
            if xtxt:match("nameserver") then
                found = true

            elseif (found == true) then
                if rtxt and (#rtxt > 0) then
                    -- fetch name servers from resolv.conf
                    rtxt = rtxt .. ", <var>%s</var>" % xtxt

                else
                    rtxt = "<var>%s</var>" % xtxt
                end


                found = false
            end
        end


        if otxt and (#otxt > 0) and rtxt and (#rtxt > 0) then
            otxt = otxt .. translatef(
                    ", and <var>%s</var> entries ", resolvfile) .. rtxt

        elseif rtxt and (#rtxt > 0) then
            otxt = translatef(
                    "use <var>%s</var> nameservers ", resolvfile) .. rtxt
        end
    end


    if otxt and (#otxt > 0) then
        return otxt

    else
        return "(empty)"
    end
end


function m5.on_commit(self)
    if sy.init.enabled("unbound") then
        -- Restart Unbound with configuration
        sy.call("/etc/init.d/unbound restart >/dev/null 2>&1")

    else
        sy.call("/etc/init.d/unbound stop >/dev/null 2>&1")
    end
end


return m5

