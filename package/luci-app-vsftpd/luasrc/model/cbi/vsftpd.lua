--[[
LuCI - Lua Configuration Interface - vsftpd support

Script by Admin @ NVACG.org (af_xj@hotmail.com , xujun@smm.cn)
Some codes is based on luci-app-upnp, TKS.
The Author of luci-app-upnp is Steven Barth <steven@midlink.org> and Jo-Philipp Wich <xm@subsignal.org>

Licensed under the GPL License, Version 3.0 (the "license");
you may not use this file except in compliance with the License.
you may obtain a copy of the License at

	http://www.gnu.org/licenses/gpl.txt

$Id$
]]--

require("luci.sys")
require("luci.util")

local running=(luci.sys.call("pidof vsftpd > /dev/null") == 0)

m=Map("vsftpd",translate("FTP Service"),translate("Use this page, you can share your file under web via ftp."))

s=m:section(TypedSection,"vsftpd",translate("vsFTPd Settings"))
s.addremove=false
s.anonymous=true

v=m:section(TypedSection,"guests",translate("Virtual User List"))
v.addremove=true
v.anonymous=true
v.template="cbi/tblsection"
v.sortable=false

gst_enable=v:option(Flag,"enable",translate("Enable"))
gst_enable.default=1
gst_user=v:option(Value,"user",translate("User Name"))
gst_user.rmempty=false
gst_pass=v:option(Value,"pass",translate("Password"))
gst_pass.rmempty=false
gst_root=v:option(Value,"root",translate("Root dir"))
gst_root.default="/tmp/none"
gst_root.rmempty=false
gst_umask=v:option(Value,"umask",translate("uMask"))
gst_umask.default="022"
gst_umask.datatype="range(0,777)"
gst_umask.rmempty=false
gst_umask:value("000","000")
gst_umask:value("022","022")
gst_umask:value("027","027")
gst_speed=v:option(Value,"speed",translate("Speed Limit"))
gst_speed.default=0
gst_speed.rmempty=0
gst_speed.placeholder=translate("In KB/s. 0 means unlimited.")
gst_speed.datatype="range(0,128000)"
gst_speed:value("0",translate("Unlimit"))
gst_speed:value("64","64")
gst_speed:value("128","128")
gst_speed:value("256","256")
gst_speed:value("512","512")
gst_speed:value("1024","1024")
gst_speed:value("2048","2048")
gst_down=v:option(Flag,"download",translate("Download"))
gst_down.default=1
gst_up=v:option(Flag,"upload",translate("Upload"))
gst_up.default=0
gst_chown=v:option(Flag,"chown",translate("Chown"))
gst_chown.default=0



m:section(SimpleSection).template="vsftpd_status"

s:tab("general",translate("Global"))
s:tab("localuser",translate("Local User"))
s:tab("virtualuser",translate("Virtual User"))
s:tab("anonymous",translate("Anonymous"))
s:tab("userlist",translate("User List"))
s:tab("template",translate("Template"))



enable=s:taboption("general",Flag,"enabled",translate("Enabled"))
enable.rmempty=false
function enable.cfgvalue(self,section)
	return luci.sys.init.enabled("vsftpd") and self.enabled or self.disabled
end
function enable.write(self,section,value)
	if value == "1" then
		if running then
			luci.sys.call("/etc/init.d/vsftpd stop >/dev/null")
	  	end	
		luci.sys.call("/etc/init.d/vsftpd enable >/dev/null")
		luci.sys.call("/etc/init.d/vsftpd start >/dev/null")
	else
		luci.sys.call("/etc/init.d/vsftpd stop >/dev/null")
		luci.sys.call("/etc/init.d/vsftpd disable >/dev/null")
	end
end
listen_ipv6=s:taboption("general",Flag,"listen_ipv6",translate("Allow IPv6"))
listen_ipv6.rmempty=false
banner=s:taboption("general",Value,"ftpd_banner",translate("FTP banner"))
banner.rmempty=true
banner.placeholder="OpenWRT Router Embd FTP service."
max_clients=s:taboption("general",Value,"max_clients",translate("Max number of clients"))
max_clients:value("5","5")
max_clients:value("10","10")
max_clients:value("15","15")
max_clients:value("20","20")
max_clients.placeholder="10"
max_clients.datatype="range(1,100)"
max_clients.rmempty=true
max_per_ip=s:taboption("general",Value,"max_per_ip",translate("Max threads of per ip"))
max_per_ip:value("1","1")
max_per_ip:value("2","2")
max_per_ip:value("3","3")
max_per_ip:value("4","4")
max_per_ip:value("5","5")
max_per_ip.placeholder="5"
max_per_ip.datatype="range(1,10)"
max_per_ip.rmempty=true
ascii=s:taboption("general",ListValue,"ascii",translate("ASCII availabled"))
ascii:value("both",translate("Both Download and Upload"))
ascii:value("download",translate("Download only"))
ascii:value("upload",translate("Upload only"))
ascii:value("none",translate("None"))
port_20=s:taboption("general",Flag,"connect_from_port_20",translate("Data Port using 20"))
port_20.rmempty=false
pasv_enable=s:taboption("general",Flag,"pasv_enable",translate("Enable Pasv Mode"))
pasv_enable.rmempty=false
pasv_min_port=s:taboption("general",Value,"pasv_min_port",translate("Pasv Min Port"))
pasv_min_port.rmempty=true
pasv_min_port.placeholder="1024"
pasv_min_port.datatype="range(1024,65535)"
pasv_min_port:depends("pasv_enable",1)
pasv_max_port=s:taboption("general",Value,"pasv_max_port",translate("Pasv Max Port"))
pasv_max_port.rmempty=true
pasv_max_port.placeholder="65535"
pasv_max_port.datatype="range(1024,65535)"
pasv_max_port:depends("pasv_enable",1)
async_abor=s:taboption("general",Flag,"async_abor_enable",translate("Accept Special Cmd"))
async_abor.rmempty=false
ls_recurse=s:taboption("general",Flag,"ls_recurse_enable",translate("Allow exhaustive listing"))
ls_recurse.rmempty=false
dirmessage=s:taboption("general",Flag,"dirmessage_enable",translate("Enable DIR Message"))
dirmessage.rmempty=false
idle_timeout=s:taboption("general",Value,"idle_session_timeout",translate("Idle timeout"))
idle_timeout.rmempty=false
idle_timeout.placeholder="600"
transfer_timeout=s:taboption("general",Value,"data_connection_timeout",translate("Transfer timeout"))
transfer_timeout.rmempty=false
transfer_timeout.placeholder="200"
xferlog=s:taboption("general",Flag,"xferlog_enable",translate("Enable FTP Logging"))
xferlog.rmempty=false
log_path=s:taboption("general",Value,"log_path",translate("Log Path"))
log_path.rmempty=true
log_path.placeholder="/tmp/log/vsftpd.log"
log_path:depends("xferlog_enable",1)
proctitle=s:taboption("general",Flag,"proctitle_enable",translate("Monitor FTP session"))
proctitle.rmempty=false

local_enabled=s:taboption("localuser",Flag,"local_enable",translate("Allow local member"))
local_enabled.rmempty=false
local_write=s:taboption("localuser",Flag,"write_enable",translate("Member can write"))
local_write.rmempty=false
local_write:depends("local_enable",1)
local_chown=s:taboption("localuser",Flag,"chown_uploads",translate("Allow change permissions"))
local_chown.rmempty=false
local_chown:depends("local_enable",1)
local_chroot=s:taboption("localuser",Flag,"chroot_local_user",translate("Enable chroot"))
local_chroot.rmempty=false
local_chroot:depends("local_enable",1)
local_max_rate=s:taboption("localuser",Value,"local_max_rate",translate("Speed limit"),translate("In KB/s. 0 means unlimited."))
local_max_rate:value("0",translate("Unlimit"))
local_max_rate:value("64","64")
local_max_rate:value("128","128")
local_max_rate:value("256","256")
local_max_rate:value("512","512")
local_max_rate:value("1024","1024")
local_max_rate:value("2048","2048")
local_max_rate.placeholder="0"
local_max_rate.datatype="range(0,128000)"
local_max_rate.rmempty=true
local_max_rate:depends("local_enable",1)
local_umask=s:taboption("localuser",Value,"local_umask",translate("uMask for new uploads"),translate("The format for number likes ###, first bit for the file's Master. second bit for the Groups which Master have joined, last bit for other people. Every bit's value from 0 to 7: 4 means read, 2 means write, 1 means execute. The value of a bit is the sigma of above listed value. When a file created, the default value is 777\(that means everyone can read write and execute the file,\) and the vsftpd will deduct the value which you set from default value."))
local_umask.default="022"
local_umask:value("000","000")
local_umask:value("022","022")
local_umask:value("027","027")
local_umask.placeholder="000"
local_umask.datatype="range(0,777)"
local_umask.rmempty=true
local_umask:depends("local_enable",1)

guest_enabled=s:taboption("virtualuser",Flag,"guest_enable",translate("Enable virtual user"),translate("Enable virtual user will disable local user"))
guest_enabled.rmempty=false
guest_username=s:taboption("virtualuser",Value,"guest_username",translate("Map to local user"))
guest_username.rmempty=true
guest_username.default="ftp"
guest_username:depends("guest_enable",1)
for _, list_user in luci.util.vspairs(luci.util.split(luci.sys.exec("cat /etc/passwd | cut -f 1 -d:"))) do
    guest_username:value(list_user)
end

anon_enabled=s:taboption("anonymous",Flag,"anonymous_enable",translate("Allow anonymous"))
anon_enabled.rmempty=false
anon_upload=s:taboption("anonymous",Flag,"anon_upload_enable",translate("Anonymous can upload"))
anon_upload.rmempty=false
anon_upload:depends("anonymous_enable",1)
anon_mkdir=s:taboption("anonymous",Flag,"anon_mkdir_write_enable",translate("Anonymous can create folder"))
anon_mkdir.rmempty=false
anon_mkdir:depends("anonymous_enable",1)
anon_root=s:taboption("anonymous",Value,"anon_root",translate("Anonymous root"))
anon_root.rmempty=false
anon_max_rate=s:taboption("anonymous",Value,"anon_max_rate",translate("Speed limit"),translate("In KB/s. 0 means unlimited."))
anon_max_rate:value("0","0")
anon_max_rate:value("64","64")
anon_max_rate:value("128","128")
anon_max_rate:value("256","256")
anon_max_rate:value("512","512")
anon_max_rate:value("1024","1024")
anon_max_rate:value("2048","2048")
anon_max_rate.placeholder="0"
anon_max_rate.datatype="range(0,128000)"
anon_max_rate.rmempty=true
anon_max_rate:depends("anonymous_enable",1)
chown_username=s:taboption("anonymous",ListValue,"chown_username",translate("Chown User"))
chown_username:depends("anonymous_enable",1)
for _, list_user in luci.util.vspairs(luci.util.split(luci.sys.exec("cat /etc/passwd | cut -f 1 -d:"))) do
    chown_username:value(list_user)
end

local_userlist=s:taboption("userlist",Flag,"userlist_enable",translate("Enable userlist"))
local_userlist.rmempty=false
local_userlist:depends("local_enable",1)
local_userlist_type=s:taboption("userlist",ListValue,"userlist_type",translate("Userlist control type"))
local_userlist_type:value("allow","allow")
local_userlist_type:value("deny","deny")
list=s:taboption("userlist",DynamicList,"userlist",translate("User"))
for _, list_user in luci.util.vspairs(luci.util.split(luci.sys.exec("cat /etc/passwd | cut -f 1 -d:"))) do
    list:value(list_user)
end

tmpl=s:taboption("template",Value,"_tmpl","",translate("Here,you can edit the template of config file"))
tmpl.template = "cbi/tvalue"
tmpl.rows=20

function tmpl.cfgvalue(self, section)
	return nixio.fs.readfile("/etc/vsftpd.conf.template")
end

function tmpl.write(self, section, value)
	value = value:gsub("\r\n?", "\n")
	nixio.fs.writefile("/etc/vsftpd.conf.template", value)
end


return m
