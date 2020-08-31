#!/usr/bin/lua
local rs = require "luars232"

-- port_name = "/dev/ttyUSB0"
port_name = "/dev/ttyS1"

out = io.stderr
-- sequence number
nseq = 0
	
function initSerial(p)
	local e, p = rs.open(p)
	if e ~= rs.RS232_ERR_NOERROR then
		-- handle error
		out:write(string.format("can't open serial port '%s', error: '%s'\n",
				port_name, rs.error_tostring(e)))
		return
	end

	assert(p:set_baud_rate(rs.RS232_BAUD_19200) == rs.RS232_ERR_NOERROR)
	assert(p:set_data_bits(rs.RS232_DATA_8) == rs.RS232_ERR_NOERROR)
	assert(p:set_parity(rs.RS232_PARITY_NONE) == rs.RS232_ERR_NOERROR)
	assert(p:set_stop_bits(rs.RS232_STOP_1) == rs.RS232_ERR_NOERROR)
	assert(p:set_flow_control(rs.RS232_FLOW_OFF)  == rs.RS232_ERR_NOERROR)

	out:write(string.format("OK, port open with values '%s'\n", tostring(p)))

	return p
end

function receive(pCon)
--	local reply = {0x20, 0x01, 0x00, 0x08, 0x00, 0xe1, 0x21, 0x18, 0x01, 0x02, 0x21, 0x67}
	local reply = {}
	local retries = 0

	while table.getn(reply) < 12 and retries < 4 do
		-- Read up to 12 byte response, timeout 400ms
		err, data_read, size = pCon:read(12, 400)
		assert(err == rs.RS232_ERR_NOERROR)
--		io.write(string.format("-> [%2d]:", string.len(data_read)))
		for i = 1, string.len(data_read) do
			table.insert(reply, string.byte(string.sub(data_read, i, i)))
--			io.write(string.format(" %02x", reply[i]))
		end
--		io.write("\n")
		retries = retries + 1
	end
	if table.getn(reply) ~= 12 then
		return(nil)
	end
	local sum = 0
	for i = 1, 11 do
		sum = sum + reply[i]
	end
	if sum % 256 ~= reply[12] then
		print ("Checksum error!")
		return(nil)
	end
	return(reply)
end

function sendCommand(pCon, cmd)
	cmd[2] = nseq
	nseq = nseq + 1

	while table.getn(cmd) < 11 do
		table.insert(cmd, 0xff)
	end
	local sum = 0
	for i,v in ipairs(cmd) do
		sum = sum + v
	end
	table.insert(cmd, sum % 256)
	local c_string = ""
	for i = 1, 12 do
		c_string = c_string .. string.char(cmd[i])
	end
	err, len_written = pCon:write(c_string)
	assert(err == rs.RS232_ERR_NOERROR)

	local reply = receive(pCon)
	if reply then
		if (reply[1] == cmd[1] and reply[2] == cmd[2]) then
--			io.write("valid: ")
--			dumpReply(reply)
			return(reply)
		end
	end
	return(nil)
end

sendAllPorts(pCon, command, value)
	cmd = {command, 0x00, 0, value, 1, value, 2, value, 3, value}
	sendCommand(pCon, cmd)

	for i = 4, 7 do cmd[(2 * (i-4)) + 3] = i end
	return(sendCommand(pCon, cmd))
end



function dumpReply(reply)
	io.write("Reply: ")
	for i,v in ipairs(reply) do
		io.write(string.format(" %02x", v))
	end
	io.write("\n");
end
	
function getStatus(pCon)
	local cmd = {0x20, 0x00}
	local reply = sendCommand(pCon, cmd)
	if not reply then return(nil) end
	return(reply)
end

function disablePort(pCon, port)
	local cmd = {0x00, 0x00, port, 0x00}
	-- disable command is always sent twice
	sendCommand(pCon, cmd)
	sendCommand(pCon, cmd)
end

function enablePort(pCon, port)
	local cmd = {0x00, 0x00, port, 0x01}
	sendCommand(pCon, cmd)
end

function portPowerOff(pCon, port)
	local cmd = {0x03, 0x00, port, 0x00}
	sendCommand(pCon, cmd)
end

function portPowerOn(pCon, port)
	local cmd = {0x03, 0x00, port, 0x01}
	sendCommand(pCon, cmd)
end


function setPortRelPrio(pCon, port, prio)
	local cmd = {0x1d, 0x00, port, prio}
	sendCommand(pCon, cmd)
end

-- set consumption based or classification based accounting
function setConsumptionBasedAccounting(pCon, enabled)
	local cmd = {}
	if (enabled) then
		cmd = {0x17 0x00 0x02}
	else
		cmd = {0x17 0x00 0x01}
	sendCommand(pCon, cmd)
end
		
function setGlobalOptions(pCon, preAlloc, powerDelay)
	local cmd = {0x0b, 0x00, 0x00, 0x00, 0x00}
	if preAlloc then cmd[3] = 0x01 end
	if not powerDelay then cmd[4] = 0x01 end
	sendCommand(pCon, cmd)
end

function getGlobalOptions(pCon)
	local cmd = {0x2b, 0x00}
	-- Defaults are 2b sq aa 00 01 00 00 01 01 ff 00 cc
	local reply = sendCommand(pCon, cmd)
	if not reply then return(nil) end
	
	return({reply[3], reply[4], reply[5]})
end

function setGlobalPowerBudget(pCon, maxPower, guard)
	-- maxPower and guard Watts
	local cmd = {0x18, 0x01, 0x00}
	table.insert(cmd, math.floor(maxPower * 10 / 256))
	table.insert(cmd, math.floor(maxPower * 10) % 256)
	table.insert(cmd, math.floor(guard * 10 / 256))
	table.insert(cmd, math.floor(guard * 10) % 256)
	sendCommand(pCon, cmd)
end

function getPowerStat(pCon)
	local cmd = {0x23, 0x01}
	local reply = sendCommand(pCon, cmd)
	if not reply then return(nil) end
	local watts = (reply[3] * 256 + reply[4]) / 10.0
	return(watts)
end

-- identical between Zyxel and DLink
function getPortPower(pCon, port)
	local cmd = {0x30, 0x00, port}
	local reply = sendCommand(pCon, cmd)
	if not reply then return(nil) end
	local watts = (reply[10] * 256 + reply[11]) / 10.0
	local mamps = reply[6] * 256 + reply[7]
	return({watts, mamps})
end

function getPortStatus(pCon, port)
	local cmd = {0x25, 0x00, port}
	-- 25 sq pp 00 00 02 01 02 00 (disabled)
	-- 25 sq pp 01 00 02 01 02 00 (enabled)
	-- 25 sq pp 01 00 05 01 02 00 (enabled, wideRange)
	local reply = sendCommand(pCon, cmd)
	if not reply then return(nil) end
	local enabled = false
	if reply[4] == 0x01 then enabled = true end
	local range = reply[6]
	return({enabled, range})
end

function getPortState(pCon, port)
	local cmd = {0x21, 0x00, port}
	-- enabled: 21 sq pp 01 01 00 00 00 00 00 03 ch
	-- active:  21 sq pp 02 03 03 01 00 00 01 03
	local reply = sendCommand(pCon, cmd)
	if not reply then return(nil) end
	return(reply)
end

function getPortOverview(pCon)
	local cmd = {0x28, 0x01, 0x00, 0x01, 0x01, 0x01, 0x02, 0x02, 0x03, 0x01}
	local reply = sendCommand(pCon, cmd)
	if not reply then return(nil) end
	cmd = {0x28, 0x01, 0x04, 0x01, 0x05, 0x01, 0x06, 0x02, 0x07, 0x01}
	local reply2 = sendCommand(pCon, cmd)
	if not reply then return(nil) end
	for k,v in pairs(reply2) do 
		if k > 2 then
			reply[k+8] = v 
		end
	end

	local s = { }
	for i = 1, 8 do
		if reply[2*i+2] == 0x10 then
			s[i] = "off"
		end
		if reply[2*i+2] == 0x11 then
			s[i] = "enabled"
		end 
		if reply[2*i+2] > 0x11 then
			s[i] = "active"
		end
	end
	return(s)
end

function setWideRange(pCon, port, enabled)
	local range = 0x02
	if enabled then range = 0x05 end
	local cmd = {0x1a, 0x00, port, range}
	local reply = sendCommand(pCon, cmd)
	return(reply)
end

-- Priority for power: 3: critical 2: High, 1: Normal, 0: Low
function setPortPriority(pCon, port, prio)
	local cmd = {0x1a, 0x00, port, prio}
	local reply = sendCommand(pCon, cmd)
	if not reply then return(nil) end
	return(unpack(reply, 4, 11))
end

function getPortPrio(pCon, port)
	local cmd = {0x26, 0x00, port}
	local reply = sendCommand(pCon, cmd)
	if not reply then return(nil) end
	return(reply[7])
end

function getPortPowerLimits(pCon, port) -- ?????
	local cmd = {0x26, 0x01, port}
	local reply = sendCommand(pCon, cmd)
	if not reply then return(nil) end
	return(answer)
end

-- 00: 802.3at 02: 802.3af 
function setPortType(pCon, pType)
	local cmd = {0x1c, 0x00, port, pType}
	local reply = sendCommand(pCon, cmd)
	if not reply then return(nil) end
	return(answer)
end

function startupPoE(pCon)
	local reply = nil

	print("Getting status")
	reply = getStatus(pCon)
	dumpReply(reply)

	print("Getting status")
	reply = getStatus(pCon)
	dumpReply(reply)

	sendCommand(pCon, {0x07, 0x00, 0x02})
	setConsumptionBasedAccounting(pCon, true) -- 0x17
	sendCommand(pCon, {0x2b})
	setGlobalOptions(pCon, false, true) -- 0x0b

	sendCommand(pCon, {0x0a, 0x00, 0xaa, 0x00, 0x01})
	sendCommand(pCon, {0x41, 0x00, 0x01, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x41})

	for i = 0, 7 do
		sendCommand(pCon, {0x18, 0x00, i, 0x03, 0x02, 0x00, 0x00})
	end

	-- the following sets unknown packed per-port properties for the ports
	-- in the sequence 3, 0, 1, 2, 7, 4, 5, 6
	-- 1d-command is rel-prio on DLink, see above...
	sendCommand(pCon, {0x1d, 0x00, 0x03, 0x03, 0x00, 0x00, 0x01, 0x01, 0x02, 0x02})
	
	sendCommand(pCon, {0x1c, 0x00, 0x03, 0x03, 0x00, 0x03, 0x01, 0x03, 0x02, 0x03})
	
	sendCommand(pCon, {0x15, 0x00, 0x03, 0x01, 0x00, 0x01, 0x01, 0x01, 0x02, 0x01})

	sendCommand(pCon, {0x1d, 0x00, 0x07, 0x07, 0x04, 0x04, 0x05, 0x05, 0x06, 0x06})

	sendCommand(pCon, {0x1c, 0x00, 0x07, 0x03, 0x04, 0x03, 0x05, 0x03, 0x06, 0x03})

	sendCommand(pCon, {0x15, 0x00, 0x07, 0x01, 0x04, 0x01, 0x05, 0x01, 0x06, 0x01})

	-- global comannds again
	sendCommand(pCon, {0x02, 0x00, 0x01})

	-- monitoring command
	sendCommand(pCon, {0x27, 0x00, 0x00})
	-- > 0x27 sq 0x02 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00

	-- packed per-port options, sequence now 0,1 ... 6, 7
	sendAllPorts(pCon, 0x28, 0x01)

	for i = 0, 7 do
		sendCommand(pCon, {0x22, 0x00, 0x00, i})
		-- > 22 sq 00 00 00 00 00 00
	end

	-- Now set defaults for class based accounting and prealloc, power up delay
	setConsumptionBasedAccounting(pCon, false)
	setGlobalOptions(pCon, true, true)

	-- per-port options
	sendAllPorts(pCon, 0x1c, 0x00)
	sendAllPorts(pCon, 0x1a, 0x00)
	sendAllPorts(pCon, 0x15, 0x01)
	sendAllPorts(pCon, 0x16, 0x9c)

	sendCommand(pCon, {0x25, 0x00, 0x00})

	for i = 0, 7 do
		sendCommand(pCon, {0x00, 0x00, i, 0x00})
		sendCommand(pCon, {0x03, 0x00, i, 0x01})
		sendCommand(pCon, {0x25, 0x00, i})
	end

	sendCommand(pCon, {0x10, 0x00, 0x7f, 0x02})
	getGlobalOptions(pCon)
	--> 2b sq aa 00 00 00 00 01 01 ff 00 cc

	setGlobalOptions(pCon, false, true)
	getGlobalOptions(pCon)
	--> 2b sq aa 00 00 00 00 01 01 ff 00 cc
	setGlobalOptions(pCon, true, true)
	

	for i = 0, 7 do
		enablePort(i)
		portPowerOn(i)
	end

end

local p = initSerial(port_name)
-- startupPoE(p)
local s = { }
while 1 do
	reply = getStatus(p)
--	dumpReply(reply)
	io.write(string.format("Total power: %f W\n", getPowerStat(p)))
	s = getPortOverview(p)
	io.write("Port state: ")
	for i = 1, 8 do
		io.write(string.format("%d: %s  ", i, s[i]))
	end
	io.write("\n")
	for i = 1, 8 do
		if s[i] == "active" then
			local r = getPortPower(p, i-1)
			io.write(string.format("Port %d: %f Watt, %d mA\n", i, r[1], r[2]))
		end
	end
	io.write("\n")
	os.execute("sleep " .. tonumber(2))
end
