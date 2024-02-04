local ds = require "luci.dispatcher"
local nxo = require "nixio"
local nfs = require "nixio.fs"
local ipc = require "luci.ip"
local sys = require "luci.sys"
local utl = require "luci.util"
local dsp = require "luci.dispatcher"
local uci = require "luci.model.uci"
local lng = require "luci.i18n"
local jsc = require "luci.jsonc"
local http = luci.http
local SYS = require "luci.sys"
local m, s

m = Map("appfilter", translate(""),
    translate("The feature library is used to describe app features, app filtering effect and number-dependent feature library"))

local rule_count = 0
local version = ""
local format = ""
if nixio.fs.access("/tmp/feature.cfg") then
    rule_count = tonumber(SYS.exec("cat /tmp/feature.cfg | wc -l"))
    version = SYS.exec("cat /tmp/feature.cfg |grep \"#version\" | awk '{print $2}'")
end
format=SYS.exec("uci get appfilter.feature.format")
if format == "" then
    format="v2.0"
end

local display_str = "<strong>"..translate("Current version")..":  </strong>" .. version .. 
                    "<br><strong>"..translate("Feature format")..":</strong>  " ..format ..
                    "<br><strong>"..translate("App number")..":</strong>  " ..rule_count ..
                    "<br><strong>"..translate("Feature download")..":</strong><a href=\"http://www.openappfilter.com\" target=\"_blank\">www.openappfilter.com</a>"
s = m:section(TypedSection, "feature", translate("App Feature"), display_str)

fu = s:option(FileUpload, "")
fu.template = "cbi/oaf_upload"
s.anonymous = true

um = s:option(DummyValue, "rule_data")
um.template = "cbi/oaf_dvalue"

local dir, fd
dir = "/tmp/upload/"
nixio.fs.mkdir(dir)
http.setfilehandler(function(meta, chunk, eof)
    local feature_file = "/etc/appfilter/feature.cfg"
    local f_format="v1.0"
    if not fd then
        if not meta then
            return
        end
        if meta and chunk then
            fd = nixio.open(dir .. meta.file, "w")
        end
        if not fd then
            return
        end
    end
    if chunk and fd then
        fd:write(chunk)
    end
    if eof and fd then
        fd:close()
        local fd2 = io.open("/tmp/upload/" .. meta.file)
        local version_line = fd2:read("*l");
        local format_line = fd2:read("*l");
        fd2:close()
        local ret = string.match(version_line, "#version")
        if ret ~= nil then
            if string.match(format_line, "#format") then
                f_format = SYS.exec("echo '"..format_line.."'|awk '{print $2}'")
            end
            if not string.match(f_format, format)  then
                um.value = translate("Failed to update feature file, format error"..",feature format:"..f_format)
                os.execute("rm /tmp/upload/* -fr");
                return
            end
            local cmd = "cp /tmp/upload/" .. meta.file .. " " .. feature_file;
            os.execute(cmd);
            os.execute("chmod 666 " .. feature_file);
            os.execute("rm /tmp/appfilter -fr");
            os.execute("uci set appfilter.feature.update=1");
            luci.sys.exec("/etc/init.d/appfilter restart");
            um.value = translate("Update the feature file successfully, please refresh the page")
        else
            um.value = translate("Failed to update feature file, format error")
        end
        os.execute("rm /tmp/upload/* -fr");
    end

end)

if luci.http.formvalue("upload") then
    local f = luci.http.formvalue("ulfile")
    if #f <= 0 then
        -- um.value = translate("No specify upload file.")
    end
elseif luci.http.formvalue("download") then
    Download()
end
return m
