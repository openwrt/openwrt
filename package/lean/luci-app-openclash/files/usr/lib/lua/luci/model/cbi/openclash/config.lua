
local NXFS = require "nixio.fs"
local SYS  = require "luci.sys"
local HTTP = require "luci.http"
local DISP = require "luci.dispatcher"
local UTIL = require "luci.util"
local fs = require "luci.openclash"
local uci = require("luci.model.uci").cursor()
local CHIF = "0"

ful = SimpleForm("upload", translate("Server Configuration"), nil)
ful.reset = false
ful.submit = false

sul =ful:section(SimpleSection, "")
o = sul:option(FileUpload, "")
o.template = "openclash/upload"
um = sul:option(DummyValue, "", nil)
um.template = "openclash/dvalue"

local dir, fd, clash
clash = "/etc/openclash/clash"
dir = "/etc/openclash/"
HTTP.setfilehandler(
	function(meta, chunk, eof)
		if not fd then
			if not meta then return end

			if	meta and chunk then fd = nixio.open(dir .. meta.file, "w") end

			if not fd then
				um.value = translate("upload file error.")
				return
			end
		end
		if chunk and fd then
			fd:write(chunk)
		end
		if eof and fd then
			fd:close()
			fd = nil
			if (meta.file == "config.yml") then
			   SYS.call("cp /etc/openclash/config.yml /etc/openclash/config.bak")
			   SYS.call("mv /etc/openclash/config.yml /etc/openclash/config.yaml")
			elseif (meta.file == "config.yaml") then
			   SYS.call("cp /etc/openclash/config.yaml /etc/openclash/config.bak")
			end
			if (meta.file == "clash") then
			   NXFS.chmod(clash, 755)
			end
			um.value = translate("File saved to") .. ' "/etc/openclash"'
			CHIF = "1"
		end
	end
)

if HTTP.formvalue("upload") then
	local f = HTTP.formvalue("ulfile")
	if #f <= 0 then
		um.value = translate("No specify upload file.")
	end
end

m = SimpleForm("openclash")
m.reset = false
m.submit = false

local tab = {
 {user, default}
}

s = m:section(Table, tab)

local conf = "/etc/openclash/config.yaml"
local yconf = "/etc/openclash/config.yml"
local dconf = "/etc/openclash/default.yaml"
local bconf = "/etc/openclash/config.bak"

sev = s:option(Value, "user")
sev.template = "cbi/tvalue"
sev.description = translate("You Can Modify config file Here, Except The Settings That Were Taken Over")
sev.rows = 20
sev.wrap = "off"
sev.cfgvalue = function(self, section)
	return NXFS.readfile(conf) or NXFS.readfile(yconf) or NXFS.readfile(bconf) or NXFS.readfile(dconf) or ""
end
sev.write = function(self, section, value)
if (CHIF == "0") then
    value = value:gsub("\r\n?", "\n")
		NXFS.writefile("/etc/openclash/config.yaml", value)
end
end

def = s:option(Value, "default")
def.template = "cbi/tvalue"
def.description = translate("Default Config File With Correct General-Settings")
def.rows = 20
def.wrap = "off"
def.readonly = true
def.cfgvalue = function(self, section)
	return NXFS.readfile(dconf) or ""
end
def.write = function(self, section, value)
end


local t = {
    {Commit, Apply, Download}
}

a = m:section(Table, t)

o = a:option(Button, "Commit") 
o.inputtitle = translate("Commit Configurations")
o.inputstyle = "apply"
o.write = function()
  uci:commit("openclash")
end

o = a:option(Button, "Apply")
o.inputtitle = translate("Apply Configurations")
o.inputstyle = "apply"
o.write = function()
  uci:set("openclash", "config", "enable", 1)
  uci:commit("openclash")
  SYS.call("/etc/init.d/openclash restart >/dev/null 2>&1 &")
  HTTP.redirect(DISP.build_url("admin", "services", "openclash"))
end

o = a:option(Button, "Download") 
o.inputtitle = translate("Download Configurations")
o.inputstyle = "apply"
o.write = function ()
	local sPath, sFile, fd, block
	sPath = "/etc/openclash/config.yaml"
	sFile = NXFS.basename(sPath)
	if fs.isdirectory(sPath) then
		fd = io.popen('tar -C "%s" -cz .' % {sPath}, "r")
		sFile = sFile .. ".tar.gz"
	else
		fd = nixio.open(sPath, "r")
	end
	if not fd then
		return
	end
	HTTP.header('Content-Disposition', 'attachment; filename="%s"' % {sFile})
	HTTP.prepare_content("application/octet-stream")
	while true do
		block = fd:read(nixio.const.buffersize)
		if (not block) or (#block ==0) then
			break
		else
			HTTP.write(block)
		end
	end
	fd:close()
	HTTP.close()
end

return ful , m
