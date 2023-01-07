local utf8togsm = {}

function hasbit(x, p)
	return x % (p + p) >= p
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

function bitleft(x, y)
	return x * 2 ^ y
end

function bitright(x, y)
	return math.floor(x / 2 ^ y)
end

function utf8togsm.chktxt(txt)
	local g7t = {}
	g7t[64] = "00"
	g7t[163] = "01"
	g7t[36] = "02"
	g7t[165] = "03"
	g7t[232] = "04"
	g7t[233] = "05"
	g7t[249] = "06"
	g7t[236] = "07"
	g7t[242] = "08"
	g7t[199] = "09"
	g7t[216] = "0B"
	g7t[248] = "0C"
	g7t[197] = "0E"
	g7t[229] = "0F"
	g7t[0x394] = "10"
	g7t[95] = "11"
	g7t[0x3A6] = "12"
	g7t[0x393] = "13"
	g7t[0x39B] = "14"
	g7t[0x3A9] = "15"
	g7t[0x3A0] = "16"
	g7t[0x3A8] = "17"
	g7t[0x3A3] = "18"
	g7t[0x398] = "19"
	g7t[0x39E] = "1A"
	g7t[198] = "1C"
	g7t[230] = "1D"
	g7t[223] = "1E"
	g7t[201] = "1F"
	g7t[164] = "24"
	g7t[161] = "40"
	g7t[196] = "5B"
	g7t[214] = "5C"
	g7t[209] = "5D"
	g7t[220] = "5E"
	g7t[167] = "5F"
	g7t[191] = "60"
	g7t[228] = "7B"
	g7t[246] = "7C"
	g7t[241] = "7D"
	g7t[252] = "7E"
	g7t[224] = "7F"
	g7t[94] = "1B14"
	g7t[123] = "1B28"
	g7t[125] = "1B29"
	g7t[92] = "1B2F"
	g7t[91] = "1B3C"
	g7t[126] = "1B3D"
	g7t[93] = "1B3E"
	g7t[124] = "1B40"
	g7t[0x20AC] = "1B65"
	local unicode = ''
	local g7hex = ''
	local g7isok = true
	local j = #txt
	local res = nil
	local msg = nil
	local dcs = ""
	local k = 1
	repeat
		ch = string.byte(txt, k, k)
		if ch >= 0xF0 then
			ch = bitleft(bitand(ch, 7), 18)
			ch = bitor(bitleft(bitand(string.byte(txt, k + 1, k + 1), 0x3F), 12), ch)
			ch = bitor(bitleft(bitand(string.byte(txt, k + 2, k + 2), 0x3F), 6), ch)
			ch = bitor(bitand(string.byte(txt, k + 3, k + 3), 0x3F), ch) - 0x10000
			local w1 = bitor(0xD800, bitright(bitand(ch, 0xFFC00), 10))
			local w2 = bitor(0xDC00, bitand(ch, 0x3FF))
			unicode = unicode .. string.format("%04X", w1) .. string.format("%04X", w2)
			g7isok = false
			k = k + 3
		elseif ch >= 0xE0 then
			ch = bitleft(bitand(ch, 0xF), 12)
			ch = bitor(bitleft(bitand(string.byte(txt, k + 1, k + 1), 0x3F), 6), ch)
			ch = bitor(bitand(string.byte(txt, k + 2, k + 2), 0x3F), ch)
			res = g7t[ch]
			if res == nil then
				g7isok = false
			else
				g7hex = g7hex .. res
			end
			unicode = unicode .. string.format("%04X", ch)
			k = k + 2
		elseif ch >= 0xC0 then
			ch = bitleft(bitand(ch, 0x3F), 6)
			ch = bitor(bitand(string.byte(txt, k + 1, k + 1), 0x3F), ch)
			res = g7t[ch]
			if res == nil then
				g7isok = false
			else
				g7hex = g7hex .. res
			end
			unicode = unicode .. string.format("%04X", ch)
			k = k + 1
		elseif ch == 0x60 then
			unicode = unicode .. '0060'
			g7isok = false
		elseif ch <= 0x7F then
			res = g7t[ch]
			if res == nil then
				g7hex = g7hex .. string.format("%02X", ch)
			else
				g7hex = g7hex .. res
			end
			unicode = unicode .. string.format("%04X", ch)
		else
			g7hex = g7hex .. '3F'
			unicode = unicode .. '003F'
		end
		k = k + 1
	until k > j
	if g7isok and #g7hex <= 320 then
		dcs = "00"
		txt = g7hex
	elseif g7isok then
		msg = 'Processed text length = ' .. math.floor(#g7hex / 2) .. ' 7-bit characters.\n'
		msg = msg .. 'Currently ROOter supports 160 maximum per message.'
	elseif #unicode <= 280 then
		dcs = "08"
		txt = unicode
	else
		msg = 'Processed text length = ' .. math.floor(#unicode / 4) .. ' 16-bit Unicode characters.\n'
		msg = msg .. 'Currently ROOter supports 70 maximum per message.'
	end
	utf8togsm["msg"] = msg
	utf8togsm["dcs"] = dcs
	utf8togsm["txt"] = txt
end
return utf8togsm
