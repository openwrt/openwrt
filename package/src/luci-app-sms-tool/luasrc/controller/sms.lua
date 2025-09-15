-- Copyright 2020-2022 Rafa³ Wabik (IceG) - From eko.one.pl forum
-- Licensed to the GNU General Public License v3.0.


	local util = require "luci.util"
	local fs = require "nixio.fs"
	local sys = require "luci.sys"
	local http = require "luci.http"
	local dispatcher = require "luci.dispatcher"
	local http = require "luci.http"
	local sys = require "luci.sys"
	local uci = require "luci.model.uci".cursor()

-- module("luci.controller.modem.sms", package.seeall)
module("luci.controller.sms", package.seeall)
-- I18N = require "luci.i18n"
-- translate = I18N.translate
function index()
	-- entry({"admin", "modem"}, firstchild(), "蜂窝", 30).dependent=false
	-- entry({"admin", "modem", "sms"}, alias("admin", "modem", "sms", "readsms"), translate("SMS Messages"), 10)
	-- entry({"admin", "modem", "sms", "readsms"},template("modem/readsms"),translate("Received Messages"), 10)
 	-- entry({"admin", "modem", "sms", "sendsms"},template("modem/sendsms"),translate("Send Messages"), 20)
	local file = io.open("/tmp/modconf.conf", "r")
	local template_name = "modem/readsms_null"
	if file then
		local content = file:read("*all")
		file:close()
		if content and string.find(content, "RM520") then
			template_name = "modem/readsms"
		elseif content and string.find(content, "RM500U") then
			template_name = "modem/readsms_null"
		end
	end
	entry({"admin", "modem", "readsms"},template(template_name),translate("Received Messages"), 20)
 	entry({"admin", "modem", "sendsms"},template("modem/sendsms"),translate("Send Messages"), 30)
 	-- entry({"admin", "modem", "sms", "ussd"},template("modem/ussd"),translate("USSD Codes"), 30)
	-- entry({"admin", "modem", "sms", "atcommands"},template("modem/atcommands"),translate("AT Commands"), 40)
	if nixio.fs.access("/etc/config/smsconfig") then
        entry({"admin", "modem", "smsconfig"}, cbi("smsconfig"),translate("Configuration"), 50)
    end
	-- entry({"admin", "modem", "smsconfig"},cbi("smsconfig"),translate("Configuration"), 50)
	entry({"admin", "modem", "delete_one"}, call("delete_sms", smsindex), nil).leaf = true
	entry({"admin", "modem", "delete_all"}, call("delete_all_sms"), nil).leaf = true
	-- entry({"admin", "modem", "sms", "run_ussd"}, call("ussd"), nil).leaf = true
	-- entry({"admin", "modem", "sms", "run_at"}, call("at"), nil).leaf = true
	entry({"admin", "modem", "run_sms"}, call("sms"), nil).leaf = true
	entry({"admin", "modem", "readsim"}, call("slots"), nil).leaf = true
	-- entry({"admin", "modem", "countsms"}, call("count_sms"), nil).leaf = true
	-- entry({"admin", "modem", "sms", "user_ussd"}, call("userussd"), nil).leaf = true
	-- entry({"admin", "modem", "sms", "user_atc"}, call("useratc"), nil).leaf = true
	-- entry({"admin", "modem", "sms", "user_phonebook"}, call("userphb"), nil).leaf = true
end


function delete_sms(smsindex)
local devv = tostring(uci:get("sms_tool", "general", "readport"))
local s = smsindex
for d in s:gmatch("%d+") do 
	os.execute("sms_tool -d " .. devv .. " delete " .. d .. "")
end
end

function delete_all_sms()
	local devv = tostring(uci:get("sms_tool", "general", "readport"))
	os.execute("sms_tool -d " .. devv .. " delete all")
end


function sms()
    local devv = tostring(uci:get("sms_tool", "general", "sendport"))
    local sms_code = http.formvalue("scode")

    nr = (string.sub(sms_code, 1, 20))
    msgall = string.sub(sms_code, 21)
    msg = string.gsub(msgall, "\n", " ")
	nr = string.gsub(nr, "%s", "")
    if sms_code then
	    -- local odpall = io.popen("sms_tool -d " .. devv .. " send " .. nr .." '".. msg .."'")
		local odpall = encodeToPDU(nr, msg)
	    local odp =  odpall:read("*a")
	    odpall:close()
        http.write(tostring(odp))
    else
        http.write_json(http.formvalue())
    end

end

function encodeToPDU(phoneNumber, message)
    local smsc=""
    local function TONGen(input, isPhonenum)
        local TONBegin = "91"
        local orinInput = input
        if #input % 2 == 1 then
            input = input .. 'F'
        end
        -- 交换数位
        local transformed = {}
        for i = 1, #input, 2 do
            local firstChar = input:sub(i, i)
            local secondChar = input:sub(i + 1, i + 1)
            transformed[#transformed + 1] = secondChar
            transformed[#transformed + 1] = firstChar
        end
        local TONStr = TONBegin .. table.concat(transformed)
        local TONLength = 0
        if (isPhonenum == false) then
            TONLength = string.len(TONStr) / 2
        else
            TONLength = string.format("%02X", string.len(orinInput))
        end
        if (string.len(TONLength) < 2) then --当短信中心号码过短时，最开头需要补0
            TONLength = "0" .. TONLength
        end
        return TONLength .. TONStr
    end

    local function splitMessage(msg,subLen)
        local segments = {}
        local len = string.len(msg)
        local i = 1
        while i <= len do
            local segment = msg:sub(i,i+subLen-1)
            segments[#segments + 1] = segment
            i = i + subLen
        end
        return segments
    end
    
    local function generateRandomInt8()
        math.randomseed(os.time())
        local randomInt8 = math.random(0, 255)
        return randomInt8
    end
        

    local SCA=TONGen(smsc, false)
    local MTI0='1'
    local MTI1='0'
    local RD='0'
    local VPF0='0'
    local VPF1='0'
    local SR='0'
    local UDHI='0'
    local RP='0'
    local pdu
    local TPMR = "00" -- TP-MR 消息基准
    local phoneNumEncode = TONGen(phoneNumber, true) --DA
    local TPPID = "00" -- TP-PID
    local TPDCS = "08" -- TP-DCS
    local MSG = encodeToUCS2(message) 
    local sendLimit=60*4 ---单条短信字符限制
  
        if string.len(MSG) >= sendLimit then
            UDHI='1'
        end
        local PDUType  = RP .. UDHI .. SR .. VPF1 .. VPF0 .. RD .. MTI1 .. MTI0 -- PDU-Type 

        local decimalValue = tonumber(PDUType, 2)  -- 将二进制字符串转换为十进制数
        PDUType = string.format("%02X", decimalValue)  -- 将十进制数转换为十六进制字符串    
        if (string.len(smsc) == 0) then
            pdu = "00" .. PDUType  .. TPMR .. phoneNumEncode
        else
            pdu = SCA .. PDUType  .. TPMR .. phoneNumEncode
        end

    local sendList={}
    if string.len(MSG) <= sendLimit then
        local MSGLen = string.format("%02X", string.len(MSG) / 2)
        local AllMsgLen = 7 + string.len(phoneNumEncode) / 2 + string.len(MSG) / 2 - 2
        pdu = AllMsgLen .. " " .. pdu .. TPPID .. TPDCS .. MSGLen .. MSG
        sendList[#sendList + 1]=pdu
    else
        local RefSeq=generateRandomInt8()
        local segments = splitMessage(MSG,sendLimit)
        for i, segment in ipairs(segments) do
            local UDHIHeader = string.format("05%02X%02X%02X%02X%02X", 0,3,RefSeq,#segments,i) -- 长短信的UDHI头 --05 00 03 85 03 02.
            local MSGLen = string.format("%02X", string.len(segment) / 2 + 6)
            segment=UDHIHeader .. segment
            local AllMsgLen = 7 + string.len(phoneNumEncode) / 2 + string.len(segment) / 2 - 2 
            local currentPdu = AllMsgLen .. " " .. pdu  .. TPPID .. TPDCS .. MSGLen .. segment
            sendList[#sendList + 1]=currentPdu
        end
    end
    local file = io.open("/tmp/sms.log", "a") -- 打开文件以追加内容
    if file then
        file:write(table.concat(sendList),"\n") -- 写入内容到文件，以空格分隔并在结尾加入换行符
        file:close() -- 关闭文件
    end

    for i, segment in ipairs(sendList) do
        local odpall = io.popen("/usr/share/modem/mopdu " .. segment)
        os.execute("sleep " .. 2) --需要一点等待的时间
        
    end
    return odpall

end

function encodeToUCS2(text)
    local ucs2 = {}
    local index = 1
    local length = string.len(text)

    while index <= length do
        local byte1 = string.byte(text, index)

        if byte1 < 128 then
            ucs2[#ucs2 + 1] = string.format("%04X", byte1)
            index = index + 1
        elseif byte1 >= 192 and byte1 < 224 then
            local byte2 = string.byte(text, index + 1)
            ucs2[#ucs2 + 1] = string.format("%04X", (byte1 - 192) * 64 + (byte2 - 128))
            index = index + 2
        elseif byte1 >= 224 then
            local byte2 = string.byte(text, index + 1)
            local byte3 = string.byte(text, index + 2)
            ucs2[#ucs2 + 1] = string.format("%04X", (byte1 - 224) * 4096 + (byte2 - 128) * 64 + (byte3 - 128))
            index = index + 3
        else
            return nil
        end
    end

    return table.concat(ucs2)
end


function slots()
	local sim = { }
	local devv = tostring(uci:get("sms_tool", "general", "readport"))
	-- local led = tostring(uci:get("sms_tool", "general", "smsled"))
	-- local dsled = tostring(uci:get("sms_tool", "general", "ledtype"))
	-- local ln = tostring(uci:get("sms_tool", "general", "lednotify"))

	local smsmem = tostring(uci:get("sms_tool", "general", "storage"))

	local statusb = luci.util.exec("sms_tool -s" .. smsmem .. " -d ".. devv .. " status")
	local usex = string.sub (statusb, 23, 27)
	local max = statusb:match('[^: ]+$')
	sim["use"] = string.match(usex, '%d+')
	local smscount = string.match(usex, '%d+')
	-- if ln == "1" then
    --   		luci.sys.call("echo " .. smscount .. " > /etc/config/sms_count")
	-- 	if dsled == "S" then
	-- 	luci.util.exec("/etc/init.d/led restart")
	-- 	end
	-- 	if dsled == "D" then
	-- 	luci.sys.call("echo 0 > '/sys/class/leds/" .. led .. "/brightness'")
	-- 	end
 	-- end
	sim["all"] = string.match(max, '%d+')
	luci.http.prepare_content("application/json")
	luci.http.write_json(sim)
end

