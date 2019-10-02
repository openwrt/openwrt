-- Copyright 2015
-- Matthew
-- Licensed to the public under the Apache License 2.0.

local fs = require "nixio.fs"

local state_msg = ""
local ss_redir_on = (luci.sys.call("pidof cifsd > /dev/null") == 0)
if ss_redir_on then	
	state_msg = "<b><font color=\"green\">" .. translate("Running") .. "</font></b>"
else
	state_msg = "<b><font color=\"red\">" .. translate("Not running") .. "</font></b>"
end

m = Map("cifs", translate("Mounting NAT drives"),
	translate("Allows you mounting Nat drives") .. " - " .. state_msg)

s = m:section(TypedSection, "cifs", "Cifs")
s.anonymous = true



s:tab("general",  translate("General Settings"))

switch = s:taboption("general", Flag, "enabled", translate("Enable"))
switch.rmempty = false

workgroup = s:taboption("general", Value, "workgroup", translate("Workgroup"))
workgroup.default = "WORKGROUP"

mountarea = s:taboption("general", Value, "mountarea", translate("Mount Area")
	, translate("All the Mounted NAT Drives will be centralized into this folder."))
mountarea.default = "/tmp/mnt"
mountarea.rmempty = false

delay = s:taboption("general", Value, "delay", translate("Delay")
	,translate("Delay command runing for wait till your drivers online.\n Only work in start mode(/etc/init.d/cifs start) "))
delay:value("0")
delay:value("3")
delay:value("5")
delay:value("7")
delay:value("10")
delay.default = "5"


iocharset = s:taboption("general", Value, "iocharset", translate("Iocharset")
        , translate("Character Encoding"))
iocharset.default = "utf8"



s = m:section(TypedSection, "natshare", translate("NAT Drivers"))
s.anonymous = true
s.addremove = true
s.template = "cbi/tblsection"

server = s:option(Value, "server", translate("Server")
        , translate("Server Name/IP"))
server.size = 6
server.rmempty = false

name = s:option(Value, "name", translate("Name")
        , translate("Mouting Folder Name"))
name.size = 6
name.rmempty = false

sec = s:option(Value, "sec", translate("Choose Safety Mode"))
sec:value("none")
sec:value("krb5")
sec:value("krb5i")
sec:value("ntlm")
sec:value("ntlmi")
sec:value("ntlmv2")
sec:value("ntlmv2i")
sec.rmempty = true
sec.size = 7

agm = s:option(Value, "agm", translate("Arguments"))
agm:value("rm", translate"rm: Read Only")
agm:value("rw", translate"rw: Read and Write")
agm:value("noperm")
agm:value("noacl")
agm:value("sfu")
agm:value("dirctio")
agm:value("file_mode=0755,dir_mode=0n755")
agm:value("nounix", translate"nounix: Disable Unix Extensions")
agm.rmempty = true
agm.size = 8

guest = s:option(Flag, "guest", translate("Using Guest"))
guest.rmempty = false
guest.enabled = "1"
guest.disabled = "0"

users = s:option(Value, "users", translate("Users"))
users.size = 3
users.rmempty = true

pwd = s:option(Value, "pwd", translate("password"))
pwd.rmempty = true
pwd.size = 3

local apply = luci.http.formvalue("cbi.apply")
if apply then
	io.popen("/etc/init.d/cifs restart")
end


return m
