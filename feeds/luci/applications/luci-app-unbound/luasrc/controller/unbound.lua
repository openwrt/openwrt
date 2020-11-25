-- Copyright 2008 Steven Barth <steven@midlink.org>
-- Copyright 2008 Jo-Philipp Wich <jow@openwrt.org>
-- Copyright 2017 Eric Luehrsen <ericluehrsen@gmail.com>
-- Licensed to the public under the Apache License 2.0.

module("luci.controller.unbound", package.seeall)


function index()
    local fs = require "nixio.fs"
    local ucl = luci.model.uci.cursor()
    local valman = ucl:get_first("unbound", "unbound", "manual_conf")


    if not fs.access("/etc/config/unbound") then
        return
    end


    -- Expanded View
    entry({"admin", "services", "unbound"},
        firstchild(), _("Recursive DNS")).dependent = false

    -- UCI Tab(s)
    entry({"admin", "services", "unbound", "configure"},
        cbi("unbound/configure"), _("Unbound"), 10)


    if (valman == "0") then
        entry({"admin", "services", "unbound", "zones"},
            arcombine(cbi("unbound/zones"), cbi("unbound/zone-details")),
            _("Zones"), 15).leaf = true
    end


    -- Status Tab(s)
    entry({"admin", "services", "unbound", "status"},
        firstchild(), _("Status"), 20)

    entry({"admin", "services", "unbound", "status", "syslog"},
        call("QuerySysLog"), _("Log"), 50).leaf = true


    if fs.access("/usr/sbin/unbound-control") then
        -- Require unbound-control to execute
        entry({"admin", "services", "unbound", "status", "statistics"},
            call("QueryStatistics"), _("Statistics"), 10).leaf = true

        entry({"admin", "services", "unbound", "status", "localdata"},
            call("QueryLocalData"), _("Local Data"), 20).leaf = true

        entry({"admin", "services", "unbound", "status", "localzone"},
            call("QueryLocalZone"), _("Local Zones"), 30).leaf = true

        entry({"admin", "services", "unbound", "status", "dumpcache"},
            call("QueryDumpCache"), _("DNS Cache"), 40).leaf = true
    else
        entry({"admin", "services", "unbound", "status", "statistics"},
            call("ShowEmpty"), _("Statistics"), 10).leaf = true
    end


    -- Raw File Tab(s)
    entry({"admin", "services", "unbound", "files"},
        firstchild(), _("Files"), 30)


    if (valman == "0") then
        entry({"admin", "services", "unbound", "files", "uci"},
            form("unbound/uciedit"), _("Edit: UCI"), 5).leaf = true

        entry({"admin", "services", "unbound", "files", "base"},
            call("ShowUnboundConf"), _("Show: Unbound"), 10).leaf = true

    else
        entry({"admin", "services", "unbound", "files", "base"},
            form("unbound/manual"), _("Edit: Unbound"), 10).leaf = true
    end


    entry({"admin", "services", "unbound", "files", "server"},
        form("unbound/server"), _("Edit: Server"), 20).leaf = true

    entry({"admin", "services", "unbound", "files", "extended"},
        form("unbound/extended"), _("Edit: Extended"), 30).leaf = true


    if fs.access("/var/lib/unbound/dhcp.conf") then
        entry({"admin", "services", "unbound", "files", "dhcp"},
            call("ShowDHCPConf"), _("Show: DHCP"), 40).leaf = true
    end


    if fs.access("/var/lib/unbound/adb_list.overall") then
        entry({"admin", "services", "unbound", "files", "adblock"},
            call("ShowAdblock"), _("Show: Adblock"), 50).leaf = true
    end
end


function ShowEmpty()
    local lclhead = "Unbound Control"
    local lcldesc = luci.i18n.translate(
        "This could display more statistics with the unbound-control package.")

    luci.template.render("unbound/show-empty",
        {heading = lclhead, description = lcldesc})
end


function QuerySysLog()
    local lcldata = luci.util.exec("logread -e 'unbound'")
    local lcldesc = luci.i18n.translate(
        "This shows syslog filtered for events involving Unbound.")

    luci.template.render("unbound/show-textbox",
        {heading = "", description = lcldesc, content = lcldata})
end


function QueryStatistics()
    local lcldata = luci.util.exec(
        "unbound-control -c /var/lib/unbound/unbound.conf stats_noreset")

    local lcldesc = luci.i18n.translate(
        "This shows Unbound self reported performance statistics.")

    luci.template.render("unbound/show-textbox",
        {heading = "", description = lcldesc, content = lcldata})
end


function QueryLocalData()
    local lcldata = luci.util.exec(
        "unbound-control -c /var/lib/unbound/unbound.conf list_local_data")

    local lcldesc = luci.i18n.translate(
        "This shows Unbound 'local-data:' entries from default, .conf, or control.")

    luci.template.render("unbound/show-textbox",
        {heading = "", description = lcldesc, content = lcldata})
end


function QueryLocalZone()
    local lcldata = luci.util.exec(
        "unbound-control -c /var/lib/unbound/unbound.conf list_local_zones")

    local lcldesc = luci.i18n.translate(
        "This shows Unbound 'local-zone:' entries from default, .conf, or control.")

    luci.template.render("unbound/show-textbox",
        {heading = "", description = lcldesc, content = lcldata})
end


function QueryDumpCache()
    local tp = require "luci.template"
    local tr = require "luci.i18n"
    local lcldesc 
    local lcldata = luci.util.exec(
        "unbound-control -c /var/lib/unbound/unbound.conf dump_cache")


    if #lcldata > 262144 then
        lcldesc = tr.translate(
            "Unbound cache is too large to display in LuCI.")

        tp.render("unbound/show-empty",
            {heading = "", description = lcldesc})

    else
        lcldesc = tr.translate(
            "This shows 'ubound-control dump_cache' for auditing records including DNSSEC.")

        tp.render("unbound/show-textbox",
            {heading = "", description = lcldesc, content = lcldata})
    end

end


function ShowUnboundConf()
    local unboundfile = "/var/lib/unbound/unbound.conf"
    local lcldata = nixio.fs.readfile(unboundfile)
    local lcldesc = luci.i18n.translate(
        "This shows '" .. unboundfile .. "' generated from UCI configuration.")

    luci.template.render("unbound/show-textbox",
        {heading = "", description = lcldesc, content = lcldata})
end


function ShowDHCPConf()
    local dhcpfile = "/var/lib/unbound/dhcp.conf"
    local lcldata = nixio.fs.readfile(dhcpfile)
    local lcldesc = luci.i18n.translate(
        "This shows '" .. dhcpfile .. "' list of hosts from DHCP hook scripts.")

    luci.template.render("unbound/show-textbox",
        {heading = "", description = lcldesc, content = lcldata})
end


function ShowAdblock()
    local fs = require "nixio.fs"
    local tp = require "luci.template"
    local tr = require "luci.i18n"
    local adblockfile = "/var/lib/unbound/adb_list.overall"
    local lcldata, lcldesc


    if fs.stat(adblockfile).size > 262144 then
        lcldesc = tr.translate(
            "Adblock domain list is too large to display in LuCI.")

        tp.render("unbound/show-empty",
            {heading = "", description = lcldesc})

    else
        lcldata = fs.readfile(adblockfile)
        lcldesc = tr.translate(
            "This shows '" .. adblockfile .. "' list of adblock domains." )

        tp.render("unbound/show-textbox",
            {heading = "", description = lcldesc, content = lcldata})
    end
end

