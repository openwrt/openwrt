#!/usr/bin/lua
local rs = require "luars232"

-- port_name = "/dev/ttyUSB0"
port_name = "/dev/ttyS1"

out = io.stderr
	
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

function dumpReply(reply)
	io.write("Reply: ")
	for i,v in ipairs(reply) do
		io.write(string.format(" %02x", v))
	end
	io.write("\n");
end
	
function getStatus(pCon)
	local cmd = {0x20, 0x01}
	local reply = sendCommand(pCon, cmd)
	if not reply then return(nil) end
	-- returns status, PoEExtVersion, PoEVersion, state2
	return({reply[5], reply[6], reply[7], reply[10]})
end

function disablePort(pCon, port)
	local cmd = {0x00, port, port, 0x00}
	-- disable command is always sent twice
	sendCommand(pCon, cmd)
	sendCommand(pCon, cmd)
end

function enablePort(pCon, port)
	local cmd = {0x00, port, port, 0x01}
	sendCommand(pCon, cmd)
end

function setPortRelPrio(pCon, port, prio)
	local cmd = {0x1d, 0x00, port, prio}
	sendCommand(pCon, cmd)
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

function setPowerLowAction(pCon, disableNext)
	local cmd = {0x17, 0x00}
	if disableNext then
		table.insert(cmd, 0x04)
	else
		table.insert(cmd, 0x02)
	end
	sendCommand(pCon, cmd)
end
	
function getPowerStat(pCon)
	local cmd = {0x23, 0x01}
	local reply = sendCommand(pCon, cmd)
	if not reply then return(nil) end
	local watts = (reply[3] * 256 + reply[4]) / 10.0
	return(watts)
end

function getPortPower(pCon, port)
	local cmd = {0x30, 0x01, port}
	local reply = sendCommand(pCon, cmd)
	if not reply then return(nil) end
	local watts = (reply[10] * 256 + reply[11]) / 10.0
	local mamps = reply[6] * 256 + reply[7]
	return({watts, mamps})
end

function getPortOverview(pCon)
	local cmd = {0x2a, 0x01, 0x00}
	local reply = sendCommand(pCon, cmd)
	if not reply then return(nil) end
	local s = { }
	for i = 4, 11 do
		if reply[i] == 0x10 then
			s[i-3] = "off"
		end
		if reply[i] == 0x11 then
			s[i-3] = "enabled"
		end 
		if reply[i] > 0x11 then
			s[i-3] = "active"
		end
	end
	return(s)
end

-- Priority for power: 3: High, 2: Normal, 1: Low?
function setPortPriority(pCon, port, prio)
	local cmd = {0x1a, port, port, prio}
	local reply = sendCommand(pCon, cmd)
	if not reply then return(nil) end
	return(unpack(reply, 4, 11))
end

function getPortPowerLimits(pCon, port)
	local cmd = {0x26, 0x01, port}
	local reply = sendCommand(pCon, cmd)
	if not reply then return(nil) end
	return(reply)
end

function startupPoE(pCon)
	local reply = nil
	reply = getStatus(pCon)
	
	
	setGlobalPowerBudget(pCon, 0, 0)
	setPowerLowAction(pCon, nil)
	-- do something unknown
	sendCommand(pCon, {0x06, 0x00, 0x01})
	for i = 0, 7 do
		disablePort(pCon, i)
	end
	-- do something unknown
	sendCommand(pCon, {0x02, 0x00, 0x01})

	for i = 0, 7 do
		disablePort(pCon, i)
	end
	-- do something unknown
	sendCommand(pCon, {0x02, 0x00, 0x01})

	for i = 0, 7 do
		setPortRelPrio(pCon, i, 7-i)
	end
	-- use monitor command 25
	sendCommand(pCon, {0x25, 0x01})
	
	setGlobalPowerBudget(pCon, 65.0, 7.0)
	getPowerStat(pCon)
	-- -> 23 01 00 00 02 44 00 02 ff ff 00 6a
	
	-- Set 4 unknown port properties:
	for i = 0, 7 do
		sendCommand(pCon, {0x11, i, i, 0x01})
		sendCommand(pCon, {0x13, i, i, 0x02})
		sendCommand(pCon, {0x15, i, i, 0x01})
		sendCommand(pCon, {0x10, i, i, 0x03})
	end
	for i = 0, 7 do
		enablePort(pCon, i)
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
