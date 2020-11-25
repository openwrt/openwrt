module("luci.controller.snmpd", package.seeall)

function index()
    entry({"admin", "services", "snmpd"}, cbi("snmpd"), "SNMPD")
end
