
-- Auther Qier LU <lvqier@gmail.com>

module("luci.controller.dnsmasq-ipset", package.seeall)

I18N = require "luci.i18n"
translate = I18N.translate

function index()
    if not nixio.fs.access("/etc/config/dnsmasq-ipset") then
        return
    end

    entry({"admin", "network", "dnsmasq-ipset"}, cbi("dnsmasq-ipset"), _(translate("DNSmasq IP-Set")), 60).dependent = true
end
