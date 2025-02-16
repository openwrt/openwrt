
local NXFS = require "nixio.fs"
local SYS  = require "luci.sys"
local HTTP = require "luci.http"
local DISP = require "luci.dispatcher"
local UTIL = require "luci.util"
local fs = require "luci.openclash"
local uci = require("luci.model.uci").cursor()

m = SimpleForm("openclash",translate("OpenClash"))
m.description = translate("A Clash Client For OpenWrt")
m.reset = false
m.submit = false

m:section(SimpleSection).template  = "openclash/status"

function IsYamlFile(e)
	e=e or""
	local e=string.lower(string.sub(e,-5,-1))
	return e == ".yaml"
end
function IsYmlFile(e)
	e=e or""
	local e=string.lower(string.sub(e,-4,-1))
	return e == ".yml"
end

function config_check(CONFIG_FILE)
	local yaml = fs.isfile(CONFIG_FILE)
	if yaml then
		yaml = SYS.exec(string.format('ruby -ryaml -rYAML -I "/usr/share/openclash" -E UTF-8 -e "puts YAML.load_file(\'%s\')" 2>/dev/null',CONFIG_FILE))
		if yaml ~= "false\n" and yaml ~= "" then
			return "Config Normal"
		else
			return "Config Abnormal"
		end
	elseif (yaml ~= 0) then
		return "File Not Exist"
	end
end

local e,a={}
for t,o in ipairs(fs.glob("/etc/openclash/config/*"))do
	a=fs.stat(o)
	if a then
		e[t]={}
		e[t].num=string.format(t)
		e[t].name=fs.basename(o)
		BACKUP_FILE="/etc/openclash/backup/".. e[t].name
		if fs.mtime(BACKUP_FILE) then
			e[t].mtime=os.date("%Y-%m-%d %H:%M:%S",fs.mtime(BACKUP_FILE))
		else
			e[t].mtime=os.date("%Y-%m-%d %H:%M:%S",a.mtime)
		end
		if uci:get("openclash", "config", "config_path") and string.sub(uci:get("openclash", "config", "config_path"), 23, -1) == e[t].name then
			e[t].state=translate("Enabled")
		else
			e[t].state=translate("Disabled")
		end
		e[t].check=translate(config_check(o))
	end
end

form = SimpleForm("openclash")
form.reset = false
form.submit = false
if a then
	tb=form:section(Table,e)
	st=tb:option(DummyValue,"state",translate("State"))
	nm=tb:option(DummyValue,"name",translate("Config Alias"))
	sb=tb:option(DummyValue,"name",translate("Subscription Info"))
	mt=tb:option(DummyValue,"mtime",translate("Update Time"))
	ck=tb:option(DummyValue,"check",translate("Grammar Check"))
	st.template="openclash/cfg_check"
	ck.template="openclash/cfg_check"
	sb.template="openclash/sub_info_show"

	btnis=tb:option(Button,"switch",translate("SwiTch"))
	btnis.template="openclash/other_button"
	btnis.render=function(o,t,a)
		if not e[t] then return false end
		if IsYamlFile(e[t].name) or IsYmlFile(e[t].name) then
			a.display=""
		else
			a.display="none"
		end
		o.inputstyle="apply"
		Button.render(o,t,a)
	end
	btnis.write=function(a,t)
		fs.unlink("/tmp/Proxy_Group")
		uci:set("openclash", "config", "config_path", "/etc/openclash/config/"..e[t].name)
		uci:set("openclash", "config", "enable", 1)
		uci:commit("openclash")
		SYS.call("/etc/init.d/openclash restart >/dev/null 2>&1 &")
		HTTP.redirect(luci.dispatcher.build_url("admin", "services", "openclash", "client"))
	end
	
	up=tb:option(DummyValue, "name", translate("Update"))
	up.template = "openclash/update_config"
	up.render = function(o,t,a)
		local display = "none"
		uci:foreach("openclash", "config_subscribe",
		function(s)
			if s.name == fs.filename(e[t].name) then
				display = ""
			end
		end)
		o.display = display
		DummyValue.render(o,t,a)
	end
end

if not a then
	local tc = {
		{create, upload}
	}
	tb=form:section(Table,tc)
	btncr=tb:option(Button,"create",translate("Config Subscribe"))
	btncr.template="openclash/other_button"
	btncr.inputstyle="apply"
	btncr.write=function()
		local sid = uci:add("openclash", "config_subscribe")
		if sid then
			uci:commit("openclash")
			HTTP.redirect(luci.dispatcher.build_url("admin", "services", "openclash", "config-subscribe-edit", sid))
			return
		end
	end

	btnup=tb:option(Button,"upload",translate("Upload Config File"))
	btnup.template="openclash/other_button"
	btnup.inputstyle="apply"
	btnup.write=function()
		HTTP.redirect(luci.dispatcher.build_url("admin", "services", "openclash", "config"))
	end
end

s = SimpleForm("openclash")
s.reset = false
s.submit = false
s:section(SimpleSection).template  = "openclash/myip"

local t = {
    {enable, disable}
}

ap = SimpleForm("openclash")
ap.reset = false
ap.submit = false

ss = ap:section(Table, t)

o = ss:option(Button, "enable", " ")
o.inputtitle = translate("Enable OpenClash")
o.inputstyle = "apply"
o.write = function()
	uci:set("openclash", "config", "enable", 1)
	uci:commit("openclash")
	SYS.call("/etc/init.d/openclash restart >/dev/null 2>&1 &")
end

o = ss:option(Button, "disable", " ")
o.inputtitle = translate("Disable OpenClash")
o.inputstyle = "reset"
o.write = function()
	uci:set("openclash", "config", "enable", 0)
	uci:commit("openclash")
	SYS.call("ps | grep openclash | grep -v grep | awk '{print $1}' | xargs -r kill -9 >/dev/null 2>&1")
	SYS.call("/etc/init.d/openclash stop >/dev/null 2>&1 &")
end

d = SimpleForm("openclash")
d.title = translate("Credits")
d.reset = false
d.submit = false
d:section(SimpleSection).template  = "openclash/developer"

dler = SimpleForm("openclash")
dler.reset = false
dler.submit = false
dler:section(SimpleSection).template  = "openclash/dlercloud"

m:append(Template("openclash/select_git_cdn"))

if uci:get("openclash", "config", "dler_token") then
	return m, dler, form, s, ap, d
else
	return m, form, s, ap, d
end
