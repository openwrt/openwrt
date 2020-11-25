-- Copyright 2018 Chizhong Jin <pjincz@gmail.com>
-- Licensed to the public under the BSD 3-clause license

m = Map("luci_statistics",
		translate("cUrl Plugin Configuration"))

s = m:section(NamedSection, "collectd_curl")
s_enable = s:option(Flag, "enable", translate("Enable this plugin"))
s_enable.default = 0

page = m:section(TypedSection, "collectd_curl_page")
page.addremove = true
page.anonymous = true
page.template = "cbi/tblsection"
page.sortable = true

page_enable = page:option(Flag, "enable", translate("Enable"))
page_enable.default = 1

page_name = page:option(Value, "name", translate("Name"))

page_addr = page:option(Value, "url", translate("URL"))

return m
