local uci = require("luci.model.uci").cursor()

nextdns = Map("nextdns", translate("NextDNS"),
	translate("NextDNS Configuration.")
	.. "<br>"
	.. translatef("For further information, go to "
	..            "<a href=\"https://nextdns.io\" target=\"_blank\">nextdns.io</a>"))


function nextdns.on_after_commit(self)
	luci.sys.call("env -i /etc/init.d/nextdns restart >/dev/null 2>&1")
end

s = nextdns:section(TypedSection, "nextdns", translate("General"))
s.anonymous = true

enabled = s:option(Flag, "enabled", translate("Enabled"),
	translate("Enable NextDNS."))
enabled.rmempty = false

conf = s:option(Value, "config", translate("Configuration ID"),
	translate("The ID of your NextDNS configuration.")
	.. "<br>"
	.. translate("Go to nextdns.io to create a configuration."))
conf.rmempty = false

report_client_info = s:option(Flag, "report_client_info", translate("Report Client Info"),
	translate("Expose LAN clients information in NextDNS analytics."))
report_client_info.rmempty = false

hardened_privacy = s:option(Flag, "hardened_privacy", translate("Hardened Privacy"),
	translate("When enabled, use DNS servers located in jurisdictions with strong privacy laws.")
	.. "<br>"
	.. translate("Available locations are: Switzerland, Iceland, Finland, Panama and Hong Kong."))
hardened_privacy.rmempty = false

log_query = s:option(Flag, "log_query", translate("Log Queries"),
	translate("Log individual queries to system log."))
log_query.rmempty = false

return nextdns
