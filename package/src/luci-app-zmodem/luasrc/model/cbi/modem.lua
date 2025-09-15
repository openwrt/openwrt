local m, section, m2, s2

m = Map("modem", translate("移动网络"))
section = m:section(TypedSection, "ndis", translate("蜂窝设置"))
section.anonymous = true
section.addremove = false
section:tab("general", translate("常规设置"))
section:tab("advanced", translate("高级设置"))

-- Check if /tmp/modconf.conf exists and contains "RM520" before loading the "原生IPV6设置" tab
-- Execute system command to read file content
local cmd = io.popen("cat /tmp/modconf.conf")
local content = cmd:read("*all") or ""
cmd:close()

--Check if the content is not empty
if content ~= "" then
     --Check if the content contains "RM520"
    if string.find(content, "RM520") then
        section:tab("nativeipv6", translate("原生IPV6设置"), translate("模组RM520N功能，用于启用模块的原生IPV6支持。如需启用直接在当前页面点击保存按钮后等待至少1分钟再刷新页面观察是否连接成功"))
    else
        section:tab("nativeipv6", translate("当前模组暂不支持IPV6设置"), translate("当前模组暂不支持IPV6设置，请尝试在最新的固件里面寻找支持。"))
        section:tab("nativeipv6"):addoption("note", translate("当前模组暂不支持IPV6设置"))
    end
else
    section:tab("nativeipv6", translate("未检测到模组信息或不支持"), translate("当前未检测到模组信息，请不要操作以下参数。"))
end

enable = section:taboption("general", Flag, "enable", translate("启用模块"))
enable.rmempty  = false


simsel= section:taboption("general", ListValue, "simsel", translate("SIM卡选择"))
simsel:value("0", translate("外置SIM卡"))
simsel:value("1", translate("内置SIM1"))
simsel:value("2", translate("内置SIM2"))
simsel.rmempty = true

pincode = section:taboption("general", Value, "pincode", translate("PIN密码"))
pincode.default=""
------
apnconfig = section:taboption("general", Value, "apnconfig", translate("APN接入点"))
apnconfig.rmempty = true

sim_card_stat = section:taboption("general", DummyValue, "sim_card_stat", translate("SIM卡状态"))
sim_card_stat.value = luci.sys.exec("cat /tmp/simcardstat")

current_mod = section:taboption("general", Value, "current_mod", translate("当前模组"))
current_mod.rmempty = true
current_mod.default = ""

function current_mod.cfgvalue(self, section)
    if nixio.fs.access("/tmp/modconf.conf") then
        return luci.sys.exec("cat /tmp/modconf.conf")
    else
        return "未知模块或未接入模式"
    end
end
------------
smode = section:taboption("advanced", ListValue, "smode", translate("网络制式"))
smode.default = "0"
smode:value("0", translate("自动"))
smode:value("1", translate("4G网络"))
smode:value("2", translate("5G网络"))

nrmode = section:taboption("advanced", ListValue, "nrmode", translate("5G模式"))
nrmode:value("0", translate("SA/NSA双模"))
nrmode:value("1", translate("SA模式"))
nrmode:value("2", translate("NSA模式"))
nrmode:depends("smode","2")

bandlist_lte = section:taboption("advanced", ListValue, "bandlist_lte", translate("LTE频段"))
bandlist_lte.default = "0"
bandlist_lte:value("0", translate("自动"))
bandlist_lte:value("1", translate("BAND 1"))
bandlist_lte:value("3", translate("BAND 3"))
bandlist_lte:value("5", translate("BAND 5"))
bandlist_lte:value("8", translate("BAND 8"))
bandlist_lte:value("34", translate("BAND 34"))
bandlist_lte:value("38", translate("BAND 38"))
bandlist_lte:value("39", translate("BAND 39"))
bandlist_lte:value("40", translate("BAND 40"))
bandlist_lte:value("41", translate("BAND 41"))
bandlist_lte:depends("smode","1")

bandlist_sa = section:taboption("advanced", ListValue, "bandlist_sa", translate("5G-SA频段"))
bandlist_sa.default = "0"
bandlist_sa:value("0", translate("自动"))
bandlist_sa:value("1", translate("BAND 1"))
bandlist_sa:value("3", translate("BAND 3"))
bandlist_sa:value("5", translate("BAND 5"))
bandlist_sa:value("8", translate("BAND 8"))
bandlist_sa:value("28", translate("BAND 28"))
bandlist_sa:value("41", translate("BAND 41"))
bandlist_sa:value("78", translate("BAND 78"))
bandlist_sa:value("79", translate("BAND 79"))
bandlist_sa:depends("nrmode","1")

bandlist_nsa = section:taboption("advanced", ListValue, "bandlist_nsa", translate("5G-NSA频段"))
bandlist_nsa.default = "0"
bandlist_nsa:value("0", translate("自动"))
bandlist_nsa:value("41", translate("BAND 41"))
bandlist_nsa:value("78", translate("BAND 78"))
bandlist_nsa:value("79", translate("BAND 79"))
bandlist_nsa:depends("nrmode","2")

earfcn = section:taboption("advanced", Value, "earfcn", translate("频点EARFCN"))
earfcn:depends("bandlist_lte","1")
earfcn:depends("bandlist_lte","3")
earfcn:depends("bandlist_lte","5")
earfcn:depends("bandlist_lte","8")
earfcn:depends("bandlist_lte","34")
earfcn:depends("bandlist_lte","38")
earfcn:depends("bandlist_lte","39")
earfcn:depends("bandlist_lte","40")
earfcn:depends("bandlist_lte","41")

earfcn:depends("bandlist_sa","1")
earfcn:depends("bandlist_sa","3")
earfcn:depends("bandlist_sa","5")
earfcn:depends("bandlist_sa","8")
earfcn:depends("bandlist_sa","28")
earfcn:depends("bandlist_sa","41")
earfcn:depends("bandlist_sa","78")
earfcn:depends("bandlist_sa","79")

earfcn:depends("bandlist_nsa","41")
earfcn:depends("bandlist_nsa","78")
earfcn:depends("bandlist_nsa","79")

earfcn.rmempty = true

cellid = section:taboption("advanced", Value, "cellid", translate("小区PCI"))
cellid:depends("bandlist_lte","1")
cellid:depends("bandlist_lte","3")
cellid:depends("bandlist_lte","5")
cellid:depends("bandlist_lte","8")
cellid:depends("bandlist_lte","34")
cellid:depends("bandlist_lte","38")
cellid:depends("bandlist_lte","39")
cellid:depends("bandlist_lte","40")
cellid:depends("bandlist_lte","41")

cellid:depends("bandlist_sa","1")
cellid:depends("bandlist_sa","3")
cellid:depends("bandlist_sa","5")
cellid:depends("bandlist_sa","8")
cellid:depends("bandlist_sa","28")
cellid:depends("bandlist_sa","41")
cellid:depends("bandlist_sa","78")
cellid:depends("bandlist_sa","79")

cellid:depends("bandlist_nsa","41")
cellid:depends("bandlist_nsa","78")
cellid:depends("bandlist_nsa","79")



cellid.rmempty = true

dataroaming = section:taboption("advanced", Flag, "datarroaming", translate("国际漫游"),"适用于行动网路漫游的数据体验，可能会产生高昂的费用。")
dataroaming.rmempty = true

autofreqlock = section:taboption("advanced", Flag, "autofreqlock", translate("基站自锁"),"适用于固定环境下使用的网路环境自优化，可能会降低或者增加网路延迟。与EARFCN与PCI锁定持久化不兼容")
autofreqlock.rmempty = true


freqlock = section:taboption("advanced", Flag, "freqlock", translate("EARFCN与PCI锁定持久化"),"又称锁小区基站")
freqlock:depends("bandlist_lte","1")
freqlock:depends("bandlist_lte","3")
freqlock:depends("bandlist_lte","5")
freqlock:depends("bandlist_lte","8")
freqlock:depends("bandlist_lte","34")
freqlock:depends("bandlist_lte","38")
freqlock:depends("bandlist_lte","39")
freqlock:depends("bandlist_lte","40")
freqlock:depends("bandlist_lte","41")

freqlock:depends("bandlist_sa","1")
freqlock:depends("bandlist_sa","3")
freqlock:depends("bandlist_sa","5")
freqlock:depends("bandlist_sa","8")
freqlock:depends("bandlist_sa","28")
freqlock:depends("bandlist_sa","41")
freqlock:depends("bandlist_sa","78")
freqlock:depends("bandlist_sa","79")

freqlock:depends("bandlist_nsa","41")
freqlock:depends("bandlist_nsa","78")
freqlock:depends("bandlist_nsa","79")
freqlock.rmempty = true

switchNetwork = section:taboption("advanced", Flag, "switchNetwork", translate("如果断网切换其他网络"),"适用于某些地区，半夜某些5G基站关闭而无法上网")
switchNetwork:depends("freqlock","1")
switchNetwork.rmempty = true
Autoswitchtime = section:taboption("advanced", Value, "Autoswitchtime", translate("时间："),"格式：01-07，也就是凌晨1点到7点自动切换网络")
Autoswitchtime:depends("switchNetwork","1")
Autoswitchtime.rmempty = true
Autoswitchtime.maxlength="5"
Autoswitchtime.default ="00-07"

smode2 = section:taboption("advanced", ListValue, "smode2", translate("网络制式"))
smode2.default = "0"
smode2:value("0", translate("自动"))
smode2:value("1", translate("4G网络"))
smode2:value("2", translate("5G网络"))
smode2:depends("switchNetwork","1")

enable_imei = section:taboption("advanced", Flag, "enable_imei", translate("修改IMEI"))
enable_imei.default = false
enable_imei:depends("simsel", "0")

modify_imei = section:taboption("advanced", Value, "modify_imei", translate("IMEI"))
modify_imei.default = luci.sys.exec("sendat 2 AT+CGSN| grep -oE '[0-9]+'")
modify_imei:depends("enable_imei", "1")
modify_imei.validate = function(self, value)
    if not value:match("^%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d$") then
        return nil, translate("IMEI必须是15位数字")
    end
    return value
end
----------------------------------------native ipv6 set
--adbkey = section:taboption("nativeipv6", DummyValue, "adbkey", translate("模块解锁请求码"))
--adbkey.value = luci.sys.exec("sendat 2 'at+qadbkey?'|grep '+QADBKEY:'|awk -F ' ' {'print $2'}|tr -d '\r\n'")

--adbunlockkey = section:taboption("nativeipv6", Value, "adbunlockkey", translate("ADB解锁码"))

--adb_status = section:taboption("nativeipv6", DummyValue, "adb_status", translate("模块ADB状态"))
--local adb_value = luci.sys.exec("adb devices | awk 'NR>1 {print $1}' | head -n -1")
--adb_status.value = (adb_value ~= "" and adb_value) or "设备ADB连接失败"
--adb_status.description="模块成功启用adb后此处会出现设备标识，请务必看到设备标识后再启用IPV6!"

--enable_native_ipv6 = section:taboption("nativeipv6", Flag, "enable_native_ipv6", translate("启用原生IPV6支持"))
--if adb_value == "" then
--    enable_native_ipv6.readonly = true
--end

--nativeIPV6_status = section:taboption("nativeipv6", DummyValue, "nativeIPV6_status", translate("IPV6状态"))
--local nativeIPV6_status_value = luci.sys.exec("cat /tmp/ipv6prefix")
--nativeIPV6_status.value = (nativeIPV6_status_value ~= "" and nativeIPV6_status_value) or "Native IPV6未使能"

--module_uptime = section:taboption("nativeipv6", DummyValue, "module_uptime", translate("模块运行时间"))
--module_uptime.value = luci.sys.exec("adb shell uptime")



adbkey = section:taboption("nativeipv6", DummyValue, "adbkey", translate("模块解锁请求码"))
local adbkey_value = luci.sys.exec("sendat 2 'at+qadbkey?' | grep '+QADBKEY:' | awk -F ' ' '{print $2}' | tr -d '\r\n'")
luci.sys.exec("logger 'adbkey_value: " .. adbkey_value .. "'")

adbunlockkey = section:taboption("nativeipv6", Value, "adbunlockkey", translate("自动计算的ADB解锁码："))

if adbkey_value ~= "" then
    adbkey.value = adbkey_value
    local adbunlockkey_value = luci.sys.exec("RMUnlock " .. adbkey_value .. " | tr -d '\r\n'")
    luci.sys.exec("logger 'adbunlockkey_value: " .. adbunlockkey_value .. "'")
    adbunlockkey.default = adbunlockkey_value  -- 使用 default 来初始化默认值
else
    adbkey.value = "未获取到解锁请求码"
end

adb_status = section:taboption("nativeipv6", DummyValue, "adb_status", translate("模块ADB状态"))
local adb_value = luci.sys.exec("adb devices | awk 'NR>1 {print $1}' | head -n -1")
adb_status.value = (adb_value ~= "" and adb_value) or "设备ADB连接失败"
adb_status.description = "模块成功启用adb后此处会出现设备标识，请务必看到设备标识后再启用IPV6!"

enable_native_ipv6 = section:taboption("nativeipv6", Flag, "enable_native_ipv6", translate("启用原生IPV6支持"))
if adb_value == "" then
    enable_native_ipv6.readonly = true
end

nativeIPV6_status = section:taboption("nativeipv6", DummyValue, "nativeIPV6_status", translate("IPV6状态"))
local nativeIPV6_status_value = luci.sys.exec("cat /tmp/ipv6prefix")
nativeIPV6_status.value = (nativeIPV6_status_value ~= "" and nativeIPV6_status_value) or "Native IPV6未使能"

module_uptime = section:taboption("nativeipv6", DummyValue, "module_uptime", translate("模块运行时间"))
module_uptime.value = luci.sys.exec("adb shell uptime")






------------------------------------

-- s2 = m:section(TypedSection, "ndis", translate("网络检测"), translate("Ping一个指定地址 失败则重启网络接口 多次尝试无效则会退出检测"))
-- s2.anonymous = true
-- s2.addremove = false

-- en = s2:option(Flag, "pingen", translate("启用"))
-- en.rmempty = false

-- ipaddress= s2:option(Value, "pingaddr", translate("Ping地址"))
-- ipaddress.rmempty=false

-- an = s2:option(Value, "count", translate("检测间隔(秒)"))
-- an.default = "5"
-- an:value("5", "5")
-- an:value("10", "10")
-- an:value("15", "15")
-- an.rmempty=false
------------------------------------


local apply = luci.http.formvalue("cbi.apply")
local sys = require "luci.sys"
local file = io.open("/tmp/modconf.conf", "r")
if apply then
    if file then
        local content = file:read("*all")
        file:close()
        if content and string.find(content, "RM520") then
            io.popen("/usr/share/modem/rm520n.sh &")
        elseif content and string.find(content, "RM500U") then
            io.popen("/usr/share/modem/500U.sh &")  
            sys.call("/sbin/ifup wan")
            sys.call("/sbin/ifup wan6")
        end
    end
end
return m,m2


