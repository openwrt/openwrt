#!/usr/bin/lua

mfile = "/tmp/sysinfo/model"
echo = 1
model = {}
gpio = {}
gpio2 = {}
gpio3 = {}
gpio4 = {}
gpioname = {}
gpioname2 = {}
gpioname3 = {}
gpioname4 = {}

pin = nil
pin2 = nil

model[1] = "10u"
gpio[1] = 18
model[2] = "11u"
gpio[2] = 8
model[3] = "13u"
gpio[3] = 18
model[4] = "mr3020"
gpio[4] = 8
model[5] = "3040"
gpio[5] = 18
model[6] = "3220"
gpio[6] = 6
model[7] = "3420"
gpio[7] = 6
model[8] = "wdr3500"
gpio[8] = 12
gpioname[8] = "tp-link:power:usb"
model[9] = "wdr3600"
gpio[9] = 22
gpioname[9] = "tp-link:power:usb1"
gpio2[9] = 21
gpioname2[9] = "tp-link:power:usb2"
model[10] = "wdr4300"
gpio[10] = 22
gpioname[10] = "tp-link:power:usb1"
gpio2[10] = 21
gpioname2[10] = "tp-link:power:usb2"
model[11] = "wdr4310"
gpio[11] = 22
gpioname2[11] = "tp-link:power:usb2"
gpioname[11] = "tp-link:power:usb1"
gpio2[11] = 21
model[12] = "wdr4900"
gpio[12] = 10
model[13] = "703n"
gpio[13] = 8
model[14] = "710n"
gpio[14] = 8
model[15] = "720"
gpio[15] = 8
model[16] = "842"
gpio[16] = 6
gpioname[16] = "tp-link:power:usb"
model[17] = "1043"
gpio[17] = 21
gpioname[17] = "tp-link:power:usb"
model[18] = "4530"
gpio[18] = 22
model[19] = "archer"
gpio[19] = 22
gpio2[19] = 21
gpioname2[19] = "tp-link:power:usb2"
gpioname[19] = "tp-link:power:usb1"
model[20] = "ar150"
gpio[20] = 6
model[21] = "domino"
gpio[21] = 6
model[22] = "300a"
gpio[22] = 0
model[23] = "mt300n"
gpio[23] = 0
model[24] = "ar750s"
gpio[24] = 7
model[25] = "oolite"
gpio[25] = 18
model[26] = "7800"
gpio[26] = 15
gpio2[26] = 16
model[27] = "m11g"
gpio[27] = 9
gpioname[27] = "gpio9"
model[28] = "m33g"
gpio[28] = 9
gpio2[28] = 10
gpio3[28] = 11
gpio4[28] = 12
gpioname[28] = "pcie0_power"
gpioname2[28] = "pcie1_power"
gpioname3[28] = "pcie2_power"
gpioname4[28] = "usb_power"
model[29] = "rbsxtr"
gpio[29] = 13
model[30] = "ap147"
gpio[30] = 13
model[31] = "gigamod"
gpio[31] = 16
gpioname[31] = "power_usb"
model[32] = "turbomod"
gpio[32] = 17
gpioname[32] = "power_usb"
model[33] = "mk01"
gpio[33] = 6

numodel = 33

local file = io.open(mfile, "r")
if file == nil then
	return
end

name = nil
name2 = nil
line = file:read("*line")
file:close()
line = line:lower()

for i=1,numodel do
	start, ends = line:find(model[i])
	if start ~= nil then
		if model[i] == "3420" then
			start, ends = line:find("v1")
			if start ~= nil then
				pin = gpio[i]
				pin2 = nil
			else
				pin = 4
				pin2 = nil
			end
		elseif model[i] == "3220" then
			start, ends = line:find("v1")
			if start ~= nil then
				pin = gpio[i]
				pin2 = nil
			else
				pin = 8
				pin2 = nil
			end
		elseif model[i] == "1043" then
			start, ends = line:find("v2")
			if start ~= nil then
				pin = gpio[i]
				pin2 = nil
				name = gpioname[i]
				name2 = nil
			end
		elseif model[i] == "842" then
			start, ends = line:find("v3")
			if start == nil then
				start, ends = line:find("v2")
				if start == nil then
					pin = gpio[i]
					pin2 = gpio2[i]
					name = gpioname[i]
					name2 = gpioname2[i]
				else
					pin = 4
					pin2 = nil
					name = gpioname[i]
					name2 = nil
				end
			end
		elseif model[i] == "archer" then
			start, ends = line:find("c20")
			if start == nil then
				pin = gpio[i]
				pin2 = gpio2[i]
				name = gpioname[i]
				name2 = gpioname2[i]
			end
		elseif model[i] == "mt300n" then
			start, ends = line:find("v2")
			if start ~= nil then
				pin = 11
				pin2 = nil
				name = "usb"
				name2 = nil
			else
				pin = gpio[i]
				pin2 = nil
				name = gpioname[i]
				name2 = nil
			end
		else
			pin = gpio[i]
			pin2 = gpio2[i]
			name = gpioname[i]
			name2 = gpioname2[i]
		end
		break
	end
end

if pin ~= nil then
	local tfile = io.open("/tmp/gpiopin", "w")
	if pin2 ~= nil then
		tfile:write("GPIOPIN=\"", pin, "\"\n")
		tfile:write("GPIOPIN2=\"", pin2, "\"")
	else
		tfile:write("GPIOPIN=\"", pin, "\"")
	end
	tfile:close()
end
if name ~= nil then
	local tfile = io.open("/tmp/gpioname", "w")
	if name2 ~= nil then
		tfile:write("GPIONAME=\"", name, "\"\n")
		tfile:write("GPIONAME2=\"", name2, "\"")
	else
		tfile:write("GPIONAME=\"", name, "\"")
	end
	tfile:close()
else
        os.remove("/tmp/gpioname")
end
