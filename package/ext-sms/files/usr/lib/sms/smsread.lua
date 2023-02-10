#!/usr/bin/lua

senders2delete = {}
deletebinaries = false

modemn = arg[1]

local smsresult = "/tmp/smsresult" .. modemn .. ".at"
local smsslots = "/tmp/smsslots" .. modemn
local smstime = "/tmp/smstime" .. modemn
local t = {}
local tptr
local m_pdu_ptr
local m_pdu
local m_smsc
local m_with_udh
local m_report
local m_number
local m_replace
local m_alphabet
local m_dcs
local m_flash
local m_date
local m_time
local m_text
local m_concat
local m_read
local m_index
local g_table1 = {}
local g_table2 = {}

local max_smsc = 64
local max_number = 64
local max_udh_data = 512
surrogate = 0

function reset()
	m_smsc = nil
	m_with_udh = 0
	m_report = 0
	m_number = nil
	m_replace = 0
	m_alphabet = -1
	m_flash = 0
	m_date = nil
	m_time = nil
	m_text = nil
	m_concat = nil
end

function hasbit(x, p)
	return x % (p + p) >= p
end

function bitor(x, y)
	local p = 1; local z = 0; local limit = x > y and x or y
	while p <= limit do
		if hasbit(x, p) or hasbit(y, p) then
			z = z + p
		end
		p = p + p
	end
	return z
end

function bitand(x, y)
	local p = 1; local z = 0; local limit = x > y and x or y
	while p <= limit do
		if hasbit(x, p) and hasbit(y, p) then
			z = z + p
		end
		p = p + p
	end
	return z
end

function bitright(x, y)
	return math.floor(x / 2 ^ y)
end

function bitleft(x, y)
	return x * 2 ^ y
end

function ocount(x, y)
	local j = 0
	local i = x:find(y, 0)
	while i ~= nil do
		i = x:find(y, i + 1)
		j = j + 1
	end
	return j
end

printf = function(s,...)
	if echo == 0 then
		io.write(s:format(...))
	else
		ss = s:format(...)
		os.execute("/usr/lib/rooter/logprint.sh " .. ss)
	end
end

function isxdigit(digit)
	if digit == nil then
		return 0
	end
	if digit >= 48 and digit <= 57 then
		return 1
	end
	if digit >= 97 and digit <= 102 then
		return 1
	end
	if digit >= 65 and digit <= 70 then
		return 1
	end
	return 0
end

function isdigit(digit)
	if digit >= 48 and digit <= 57 then
		return 1
	end
	return 0
end

function octet2bin(octet)
	result = 0
	if octet:byte(1) > 57 then
		result = result + octet:byte(1) - 55
	else
		result = result + octet:byte(1) - 48
	end
	result = result * 16
	if octet:byte(2) > 57 then
		result = result + octet:byte(2) - 55
	else
		result = result + octet:byte(2) - 48
	end
	return result
end

function octet2bin_check(octet)
	if octet:byte(1) == 0 then
		return -1
	end
	if octet:byte(2) == 0 then
		return -2
	end
	if isxdigit(octet:byte(1)) == 0 then
		return -3
	end
	if isxdigit(octet:byte(2)) == 0 then
		return -4
	end
	return octet2bin(octet)
end

function swapchars(sstring)
	local length = sstring:len()
	local xstring = nil
	local i = 1
	while i < length do
		c1 = sstring:sub(i, i)
		c2 = sstring:sub(i+1, i+1)
		if xstring == nil then
			xstring = c2 .. c1
		else
			xstring = xstring .. c2 .. c1
		end
		i = i + 2
	end
	return xstring
end

function parseSMSC()
	m_pdu_ptr = m_pdu
	local length = octet2bin_check(m_pdu_ptr)
	if length < 0 then
		return -1
	end
	if length == 0 then
		m_smsc = ""
		m_pdu_ptr = m_pdu_ptr:sub(3)
		return 1
	end
	if length < 2 or length > max_smsc then
		return -1
	end
	length = (length * 2) - 2
	local mlen = m_pdu:len()
	if mlen < (length + 4) then
		return -1
	end
	m_pdu_ptr = m_pdu_ptr:sub(3)
	local addr_type = octet2bin_check(m_pdu_ptr)
	if addr_type < 0 then
		return -1
	end
	if addr_type < 0x80 then
		return -1
	end
	m_pdu_ptr = m_pdu_ptr:sub(3)
	m_smsc = m_pdu_ptr:sub(0, length)
	m_smsc = swapchars(m_smsc)
	if addr_type < 0x90 then
		for j=1, length do
			if isxdigit(m_smsc:byte(j)) == 0 then
				return -1
			end
		end
	else
		if m_smsc:byte(length) == 70 then
			m_smsc = m_smsc:sub(1, length-1)
		end
		local leng = m_smsc:len()
		for j=1,leng do
			if isdigit(m_smsc:byte(j)) == 0 then
				return -1
			end
		end
	end
	m_pdu_ptr = m_pdu_ptr:sub(length + 1)
	return 1
end

function explainAddressType(octet_char, octet_int)
	local result
	if octet_char ~= nil then
		result = octet2bin_check(octet_char)
	else
		result = octet_int
	end
	return result
end

function concatinfo(x)
	while #x > 9 do
		if x:sub(1, 4) == '0003' then
			if x:sub(7, 10) ~= '0101' then
				m_concat = 'Msg# ' .. tonumber(x:sub(5, 6), 16)
				m_concat = m_concat .. ',' .. tonumber(x:sub(9, 10), 16)
				m_concat = m_concat  .. '/' .. tonumber(x:sub(7, 8), 16)
			end
			x = ""
		elseif x:sub(1, 4) == '0804' then
			if x:sub(9, 12) ~= '0101' then
				m_concat = 'Msg# ' .. tonumber(x:sub(5, 8), 16)
				m_concat = m_concat .. ',' .. tonumber(x:sub(11, 12), 16)
				m_concat = m_concat  .. '/' .. tonumber(x:sub(9, 10), 16)
			end
			x = ""
		else
			local iel = tonumber(x:sub(3, 4), 16)
			x = x:sub(5 + iel * 2)
		end
	end
end

function pdu2binary(pdu, with_udh)
	local skip_octets = 0
	local octetcounter
	m_text = ''
	local octets = octet2bin_check(pdu)
	local last_i = 0
	if octets < 0 then
		return -1
	end
	if with_udh > 0 then
		local pdu2 = pdu:sub(3)
		local udhl = tonumber(pdu2:sub(1, 2), 16)
		concatinfo(pdu2:sub(3, (udhl + 1) * 2))
		local udhsize = octet2bin_check(pdu2)
		if udhsize < 0 then
			return -1
		end
		skip_octets = udhsize + 1
	end
	for octetcounter = 0, (octets - skip_octets - 1) do
		local pdu2 = pdu:sub((octetcounter * 2) + 3 + (skip_octets * 2))
		local i = octet2bin_check(pdu2)
		if i < 0 then
			return -1
		end
		if m_alphabet == 2 then
			if (2 + octetcounter) % 2 == 0 then
				last_i = i
			else
				m_text = word2utf8(bitor(bitleft(last_i, 8), i), m_text)
			end
		else
			if i < 32 or i > 127 then
				i = 0x25A1
			end
			m_text = word2utf8(i, m_text)
		end
	end
	return (octets - skip_octets)
end

function pdu2text(pdu, with_udh)
	local result
	local octetcounter
	local skip_characters = 0
	local binary = 0
	m_text = ''
	local septets = octet2bin_check(pdu)
	if septets < 0 then
		return -1
	end
	if with_udh > 0 then
		local pdu2 = pdu:sub(3)
		local udhl = tonumber(pdu2:sub(1, 2), 16)
		concatinfo(pdu2:sub(3, (udhl + 1) * 2))
		local udhsize = octet2bin_check(pdu2)
		skip_characters = math.floor((((udhsize+1)*8)+6)/7)
	end
	local octets = math.floor((septets * 7 + 7) / 8)
	local bitposition = 0
	local byteposition
	local byteoffset
	local i
	local g_table_nbr = 1
	octetcounter = 0
	for charcounter=0,septets-1 do
		local c = 0
		for bitcounter=0,6 do
			byteposition = math.floor(bitposition / 8)
			byteoffset = bitposition % 8
			while (byteposition >= octetcounter) and (octetcounter < octets) do
				local pdu2 = pdu:sub((octetcounter * 2) + 3)
				i = octet2bin_check(pdu2)
				if i < 0 then
					return -2
				end
				binary = i
				octetcounter = octetcounter + 1
			end
			if bitand(binary, (2^byteoffset)) > 0 then
				c = bitor(c, 128)
			end
			bitposition = bitposition + 1
			c = bitand(math.floor(c / 2), 127)
		end
		c = gsm2byte(c)
		if charcounter >= skip_characters and c ~= 27 then
			m_text = word2utf8(c, m_text)
		end
	end
	return 1
end

function gsm2byte(ch)
	if g_table_nbr == 2 then
		if g_table2[ch] == nil then
			ch = 63
		else
			ch = g_table2[ch]
		end
		g_table_nbr = 1
	else
		if ch == 27 then
			g_table_nbr = 2
		else
			if g_table1[ch] ~= nil then
				ch = g_table1[ch]
			end
		end
	end
	return ch
end

function word2utf8(i, txt)
	if surrogate > 0 then
		if i >= 0xDC00 and i <= 0xDFFF then
			local ii = 0x10000 + bitand(i, 0x3FF) + bitleft(bitand(surrogate, 0x3FF), 10)
			txt = txt .. string.char(bitor(0xF0, bitright(bitand(ii, 0x1C0000), 18)))
			txt = txt .. string.char(bitor(0x80, bitright(bitand(ii, 0x3F000), 12)))
			txt = txt .. string.char(bitor(0x80, bitright(bitand(ii, 0xFC0), 6)))
			txt = txt .. string.char(bitor(0x80, bitand(ii, 0x3F)))
		else
			txt = txt .. string.char(0xe2) .. string.char(0x96) .. string.char(0xa1)
		end
		surrogate = 0
	elseif i >= 0xD800 and i <= 0xDBFF then
		surrogate = i
	elseif i >= 0xDC00 and i <= 0xDFFF then
		txt = txt .. string.char(0xe2) .. string.char(0x96) .. string.char(0xa1)
	elseif i < 0x80 then
		txt = txt .. string.char(i)
	elseif i < 0x800 then
		txt = txt .. string.char(bitor(0xC0, bitright(bitand(i, 0x7C0), 6)))
		txt = txt .. string.char(bitor(0x80, bitand(i, 0x3F)))
	else
		if i == 0x2029 then
			txt = txt .. string.char(10)
		else
			txt = txt .. string.char(bitor(0xE0, bitright(bitand(i, 0xF000), 12)))
			txt = txt .. string.char(bitor(0x80, bitright(bitand(i, 0xFC0), 6)))
			txt = txt .. string.char(bitor(0x80, bitand(i, 0x3F)))
		end
	end
	return txt
end

function parseDeliver()
	if m_pdu_ptr:len() < 4 then
		return 0
	end
	local padding = 0
	local length = octet2bin_check(m_pdu_ptr)
	local timezone
	if length < 0 or length > max_number then
		return 0
	end
-- Sender Address
	if length == 0 then
		m_pdu_ptr = m_pdu_ptr:sub(5)
	else
		padding = length % 2
		m_pdu_ptr = m_pdu_ptr:sub(3)
		local addr_type = explainAddressType(m_pdu_ptr, 0)
		if addr_type < 0 then
			return 0
		end
		if addr_type < 0x80 then
			return 0
		end
		m_pdu_ptr = m_pdu_ptr:sub(3)
		if bitand(addr_type, 112) == 80 then
			if m_pdu_ptr:len() < (length + padding) then
				return 0
			end
			local htmp = string.format("%x", math.floor((length * 4) / 7))
			if htmp:len() < 2 then
				htmp = "0" .. htmp
			end
			htmp = htmp:upper()
			local tpdu = htmp .. m_pdu_ptr
			local res = pdu2text(tpdu, 0)
			if res  < 0 then
				return 0
			end
			m_number = string.gsub(m_text, "\n", " ")
			m_text = nil
		else
			m_number = m_pdu_ptr:sub(1, length + padding + 1)
			m_number = swapchars(m_number)
			if m_number:byte(length + padding) == 70 then
				m_number = m_number:sub(1, length + padding - 1)
			end
			if addr_type == 145 then
				m_number = "+" .. m_number
			end
		end
	end
	m_pdu_ptr = m_pdu_ptr:sub(length + padding + 1)
	if m_pdu_ptr:len() < 20 then
		return 0
	end
-- PID
	local byte_buf = octet2bin_check(m_pdu_ptr)
	if byte_buf < 0 then
		return 0
	end
	if bitand(byte_buf, 0xF8) == 0x40 then
		m_replace = bitand(byte_buf, 0x07)
	end
	m_pdu_ptr = m_pdu_ptr:sub(3)
-- Alphabet
	byte_buf = octet2bin_check(m_pdu_ptr)
	if byte_buf < 0 then
		return 0
	end
	m_alphabet = math.floor(bitand(byte_buf, 0x0C) / 4)
	if m_alphabet == 3 then
		return 0
	end
	if m_alphabet == 0 then
		m_alphabet = -1
	end
-- DCS, Flash Msg
	m_dcs = byte_buf
	if bitand(byte_buf, 0x10) > 0 then
		if bitand(byte_buf, 0x01) > 0 then
			m_flash = 1
		end
	end
	m_pdu_ptr = m_pdu_ptr:sub(3)
-- Date
	local str_buf = m_pdu_ptr:sub(2,2) .. m_pdu_ptr:sub(1,1) .. "-" .. m_pdu_ptr:sub(4,4) .. m_pdu_ptr:sub(3,3) .. "-" .. m_pdu_ptr:sub(6,6) .. m_pdu_ptr:sub(5,5)
	if (not isdigit(m_pdu_ptr:byte(1))) or (not isdigit(m_pdu_ptr:byte(2))) or (not isdigit(m_pdu_ptr:byte(3))) or (not isdigit(m_pdu_ptr:byte(4))) or (not isdigit(m_pdu_ptr:byte(5))) or (not isdigit(m_pdu_ptr:byte(6))) then
		return 0
	end
	m_date = str_buf
	m_pdu_ptr = m_pdu_ptr:sub(7)
-- Time
	str_buf = m_pdu_ptr:sub(2,2) .. m_pdu_ptr:sub(1,1) .. ":" .. m_pdu_ptr:sub(4,4) .. m_pdu_ptr:sub(3,3) .. ":" .. m_pdu_ptr:sub(6,6) .. m_pdu_ptr:sub(5,5)
	if (not isdigit(m_pdu_ptr:byte(1))) or (not isdigit(m_pdu_ptr:byte(2))) or (not isdigit(m_pdu_ptr:byte(3))) or (not isdigit(m_pdu_ptr:byte(4))) or (not isdigit(m_pdu_ptr:byte(5))) or (not isdigit(m_pdu_ptr:byte(6))) then
		return 0
	end
	if tonumber(m_pdu_ptr:sub(8,8), 16) > 7 then
		timezone = '-' .. ((tonumber(m_pdu_ptr:sub(8, 8), 16) - 8) .. tonumber(m_pdu_ptr:sub(7, 7), 16)) / 4
	else
		timezone = '+' .. (m_pdu_ptr:sub(8, 8) .. tonumber(m_pdu_ptr:sub(7, 7), 16)) / 4
	end
	if timezone:sub(-2, -1) == '.0' then
		timezone = timezone:sub(1, -3)
	end
	if #timezone == 2 then
		timezone = ' ' .. timezone
	end
	m_time = str_buf .. ' ' .. timezone .. 'h'
	m_pdu_ptr = m_pdu_ptr:sub(7)
	if octet2bin_check(m_pdu_ptr) < 0 then
		return 0
	end
	m_pdu_ptr = m_pdu_ptr:sub(3)

-- Text
	local result = 0
	local bin_udh = 1
	if m_alphabet <= 0 then
		result = pdu2text(m_pdu_ptr, m_with_udh)
		return result
	else
		result = pdu2binary(m_pdu_ptr, m_with_udh)
		return result
	end
	return 1
end

function parseStatusReport()
	if m_pdu_ptr:len() < 6 then
		return 0
	end
	local messageid = octet2bin_check(m_pdu_ptr)
	if messageid < 0 then
		return 0
	end
	m_pdu_ptr = m_pdu_ptr:sub(3)
	local length = octet2bin_check(m_pdu_ptr)
	if length < 1 or length > max_number then
		return 0
	end
	local padding = length % 2
	m_pdu_ptr = m_pdu_ptr:sub(3)
	local addr_type = explainAddressType(m_pdu_ptr, 0)
	if addr_type < 0x80 then
		return 0
	end
	m_pdu_ptr = m_pdu_ptr:sub(3)
	if bitand(addr_type, 112) == 80 then
		if m_pdu_ptr:len() < (length + padding) then
			return 0
		end
		local htmp = string.format("%x", math.floor((length * 4) / 7))
		if htmp:len() < 2 then
			htmp = "0" .. htmp
		end
		local tpdu = htmp .. m_pdu_ptr
		local res = pdu2text(tpdu, 0)
		if res  < 0 then
			return 0
		end
		m_number = m_text
		m_text = nil
	else
		m_number = m_pdu_ptr:sub(1, length + padding + 1)
		m_number = swapchars(m_number)
		if m_number:byte(length + padding) == 70 then
			m_number = m_number:sub(1, length + padding - 1)
		end
	end
	m_pdu_ptr = m_pdu_ptr:sub(length + padding + 1)
	if m_pdu_ptr:len() < 14 then
		return 0
	end
-- Date
	local str_buf = m_pdu_ptr:sub(2,2) .. m_pdu_ptr:sub(1,1) .. "-" .. m_pdu_ptr:sub(4,4) .. m_pdu_ptr:sub(3,3) .. "-" .. m_pdu_ptr:sub(6,6) .. m_pdu_ptr:sub(5,5)
	if (not isdigit(m_pdu_ptr:byte(1))) or (not isdigit(m_pdu_ptr:byte(2))) or (not isdigit(m_pdu_ptr:byte(3))) or (not isdigit(m_pdu_ptr:byte(4))) or (not isdigit(m_pdu_ptr:byte(5))) or (not isdigit(m_pdu_ptr:byte(6))) then
		return 0
	end
	m_date = str_buf
	m_pdu_ptr = m_pdu_ptr:sub(7)
-- Time
	str_buf = m_pdu_ptr:sub(2,2) .. m_pdu_ptr:sub(1,1) .. ":" .. m_pdu_ptr:sub(4,4) .. m_pdu_ptr:sub(3,3) .. ":" .. m_pdu_ptr:sub(6,6) .. m_pdu_ptr:sub(5,5)
	if (not isdigit(m_pdu_ptr:byte(1))) or (not isdigit(m_pdu_ptr:byte(2))) or (not isdigit(m_pdu_ptr:byte(3))) or (not isdigit(m_pdu_ptr:byte(4))) or (not isdigit(m_pdu_ptr:byte(5))) or (not isdigit(m_pdu_ptr:byte(6))) then
		return 0
	end
	m_time = str_buf
	m_pdu_ptr = m_pdu_ptr:sub(7)
	if octet2bin_check(m_pdu_ptr) < 0 then
		return 0
	end
	m_pdu_ptr = m_pdu_ptr:sub(3)
-- Discharge Date
	local str_buf = m_pdu_ptr:sub(2,2) .. m_pdu_ptr:sub(1,1) .. "-" .. m_pdu_ptr:sub(4,4) .. m_pdu_ptr:sub(3,3) .. "-" .. m_pdu_ptr:sub(6,6) .. m_pdu_ptr:sub(5,5)
	if (not isdigit(m_pdu_ptr:byte(1))) or (not isdigit(m_pdu_ptr:byte(2))) or (not isdigit(m_pdu_ptr:byte(3))) or (not isdigit(m_pdu_ptr:byte(4))) or (not isdigit(m_pdu_ptr:byte(5))) or (not isdigit(m_pdu_ptr:byte(6))) then
		return 0
	end
	local d_date = str_buf
	m_pdu_ptr = m_pdu_ptr:sub(7)
-- Time
	str_buf = m_pdu_ptr:sub(2,2) .. m_pdu_ptr:sub(1,1) .. ":" .. m_pdu_ptr:sub(4,4) .. m_pdu_ptr:sub(3,3) .. ":" .. m_pdu_ptr:sub(6,6) .. m_pdu_ptr:sub(5,5)
	if (not isdigit(m_pdu_ptr:byte(1))) or (not isdigit(m_pdu_ptr:byte(2))) or (not isdigit(m_pdu_ptr:byte(3))) or (not isdigit(m_pdu_ptr:byte(4))) or (not isdigit(m_pdu_ptr:byte(5))) or (not isdigit(m_pdu_ptr:byte(6))) then
		return 0
	end
	local d_time = str_buf
	m_pdu_ptr = m_pdu_ptr:sub(7)
	if octet2bin_check(m_pdu_ptr) < 0 then
		return 0
	end
	m_pdu_ptr = m_pdu_ptr:sub(3)
	local status = octet2bin_check(m_pdu_ptr)
	if status < 0 then
		return 0
	end
	m_text = string.format("Discharge Timestamp: %s %s  Message ID: %d  Status: %d", d_date, d_time, messageid, status)
	return 1
end

function parse()
	local flag = parseSMSC()
	if flag ~= 1 then
		return 0
	end
	local tmp = octet2bin_check(m_pdu_ptr)
	if tmp < 0 then
		return 0
	end
	if bitand(tmp, 0x40) > 0 then
		m_with_udh = 1
	end
	if bitand(tmp, 0x20) > 0 then
		m_report = 1
	end
	local type = bitand(tmp, 3)
	if type == 0 then
		m_pdu_ptr = m_pdu_ptr:sub(3)
		local result = parseDeliver()
		if result < 1 then
			return 0
		end
	else
		if type == 2 then
			m_pdu_ptr = m_pdu_ptr:sub(3)
			local result = parseStatusReport()
			return result
		else
			return 0
		end
	end
	return 1
end

function trim(s)
  return (s:gsub("^%s*(.-)%s*$", "%1"))
end

function readpdu(pdu)
	m_pdu = pdu
	m_pdu_ptr = m_pdu
	reset()
	local flag = parse()
	if flag > 0 then
		t[tptr] = m_index
		t[tptr+1] =  m_read
		t[tptr+2] = m_number
		if string.find(delsend, gschar .. m_number .. gschar, 1, true) then
			delslots = delslots .. m_index .. " "
		else
			if m_dcs == 245 and deletebinaries then
				delslots = delslots .. m_index .. " "
			end
		end
		t[tptr+3] = m_date
		t[tptr+4] = m_time
		if m_concat ~= nil then
			m_text = m_concat .. '\n' .. m_text
		end
		t[tptr+5] = m_text
		tptr = tptr + 6
	end
end

local max_msg = "0"
local used_msg = "0"
gschar = string.char(29)
delsend = gschar .. table.concat(senders2delete, gschar) .. gschar
delslots = ""
tptr = 3
t[1] = used_msg
t[2] = max_msg
g_table1 = {163, 36, 165, 232, 233, 249, 236, 242, 199, 10, 216, 248, 13, 197, 229, 0x394, 95, 0x3A6, 0x393, 0x39B, 0x3A9, 0x3A0, 0x3A8, 0x3A3, 0x398, 0x39E}
g_table1[0] = 64
g_table1[28] = 198
g_table1[29] = 230
g_table1[30] = 223
g_table1[31] = 201
g_table1[36] = 164
g_table1[64] = 161
g_table1[91] = 196
g_table1[92] = 214
g_table1[93] = 209
g_table1[94] = 220
g_table1[95] = 167
g_table1[96] = 191
g_table1[123] = 228
g_table1[124] = 246
g_table1[125] = 241
g_table1[126] = 252
g_table1[127] = 224
g_table2[10] = 10
g_table2[20] = 94
g_table2[40] = 123
g_table2[41] = 125
g_table2[47] = 92
g_table2[60] = 91
g_table2[61] = 126
g_table2[62] = 93
g_table2[64] = 124
g_table2[101] = 0x20AC
--
os.execute("touch " .. smsslots)
local slottab = {}
local file = io.open(smsslots, "r")
for k in file:lines() do
	slottab[k] = true
end
file:close()
local file = io.open(smsresult, "r")
local m_r = ""
if file ~= nil then
	repeat
		local s, e, cs, ce, ms, me
		local line = file:read("*l")
		if line == nil then
			break
		end
		s, e = line:find("+CPMS:")
		if s ~= nil then
			cs, ce = line:find(",", e)
			if cs ~= nil then
				used_msg = trim(line:sub(e+1, cs-1))
				t[1] = used_msg
				ms, me = line:find(",", ce+1)
				if ms ~= nil then
					max_msg = trim(line:sub(ce+1, ms-1))
					t[2] = max_msg
				end
			end
			line = file:read("*l")
			if line == nil then
				break
			end
		end
		s, e = line:find("+CMGL:")
		if s ~= nil then
			m_index = "0"
			cs, ce = line:find(",", e)
			if cs ~= nil then
				m_index = trim(line:sub(e+1, cs-1))
			end
			ds, de = line:find(",", ce+1)
			if ds ~= nil then
				surrogate = 0
				m_r = trim(line:sub(ce+1, ds-1))
				if m_r == "0" then
					m_read = word2utf8(0x2691, word2utf8(0x2691, ''))
					os.execute("date +%s > " .. smstime)
					if not slottab[m_index] then
						os.execute("echo " .. m_index .. " >> " .. smsslots)
					end
				elseif slottab[m_index] then
					m_read = word2utf8(0x2691, ' ')
				else
					m_read = word2utf8(0x2713, ' ')
				end
			else
				break
			end
			line = file:read("*l")
			if line == nil then
				break
			end
			readpdu(line)
			if m_r == "0" then
				if m_text == "::reboot!!" then
					os.execute("(sleep 60; reboot -f) &")
				elseif m_text == "::pwrtoggle!!" then
					os.execute("(sleep 60; /usr/lib/rooter/pwrtoggle.sh 3) &")
				elseif m_text:sub(1, 3) == "GPS" then
					cmd_hand = io.popen("uci -q get modem.general.pin")
					gpspin = cmd_hand:read()
					cmd_hand:close()
					if (m_text == "GPS" and gpspin == nil) or (m_text == "GPS " .. gpspin) then
						if tonumber(m_number) ~= nil then
							os.execute("/usr/lib/gps/smsreply.sh " .. modemn .. " " .. m_number .. "&")
							delslots = delslots .. m_index .. " "
						end
					end
				end
			end
		end
	until 1==0
	file:close()
end

local tfname = "/tmp/smstemptext" .. math.random(99)
local tfile = io.open(tfname, "w")
tfile:write(t[1] .. "\n")
tfile:write(t[2] .. "\n")
if tonumber(used_msg) == 0 then
	tfile:close()
else
	i = 3
	while t[i] ~= nil do
		local mtxt = t[i + 5]
		tfile:write(t[i] .. "\n")
		tfile:write(t[i + 2] .. "\n")
		tfile:write((ocount(mtxt, '\n') + 1) .. '\n')
		tfile:write(mtxt .. "\n")
		local mn = t[i + 2] .. "                    "
		mn = mn:sub(1,20)
		local stxt = ''
		local j = 0
		local k = 1
		local ch = ''
		while j < 20 do
			ch = string.byte(mtxt:sub(k, k))
			if ch == nil then
				j = 20
			elseif ch == 10 or ch == 13 then
				stxt = stxt .. ' '
				k = k + 1
			elseif ch < 127 then
				stxt = stxt .. string.char(ch)
				k = k + 1
			elseif ch < 0xE0 then
				stxt = stxt .. mtxt:sub(k, k + 1)
				k = k + 2
			elseif ch < 0xF0 then
				stxt = stxt .. mtxt:sub(k, k + 2)
				k = k + 3
			else
				stxt = stxt .. mtxt:sub(k, k + 3)
				k = k + 4
			end
			j = j + 1
		end
		if mtxt ~= stxt then
			stxt = stxt .. "  ..."
		end
		local msg = t[i + 1] .. " " .. mn .. t[i + 3] .. " " .. t[i + 4] .. "  " .. stxt
		tfile:write(msg .. "\n")
		i = i + 6
	end
	tfile:close()
end
os.execute("/usr/lib/sms/merge.lua " .. tfname)
os.execute("mv " .. tfname .. " /tmp/smstext" .. modemn)

if #delslots > 0 then
	os.execute("/usr/lib/sms/delsms.sh " .. modemn .. " " .. delslots .. " &")
end
