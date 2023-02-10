local pack7bit = {}

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

function bitright(x, y)
	return math.floor(x / 2 ^ y)
end

function bitleft(x, y)
	return x * 2 ^ y
end

function pack7bit.pack(udl, txt)
	maxb = math.ceil((tonumber(udl, 16) / 8) * 7)
	udtab = {}
	ii = 1
	jj = 1
	kk = 0
	repeat
		ch = tonumber(txt:sub(jj, jj + 1), 16)
		if ii == 1 then
			udtab[kk + 1] = ch
		elseif ii == 2 then
			udtab[kk] = bitor(bitleft(bitand(ch, 1), 7), udtab[kk])
			udtab[kk + 1] = bitright(bitand(ch, 126), 1)
		elseif ii == 3 then
			udtab[kk] = bitor(bitleft(bitand(ch, 3), 6), udtab[kk])
			udtab[kk + 1] = bitright(bitand(ch, 124), 2)
		elseif ii == 4 then
			udtab[kk] = bitor(bitleft(bitand(ch, 7), 5), udtab[kk])
			udtab[kk + 1] = bitright(bitand(ch, 120), 3)
		elseif ii == 5 then
			udtab[kk] = bitor(bitleft((bitand(ch, 15)), 4), udtab[kk])
			udtab[kk + 1] = bitright(bitand(ch, 112), 4)
		elseif ii == 6 then
			udtab[kk] = bitor(bitleft(bitand(ch, 31), 3), udtab[kk])
			udtab[kk + 1] = bitright(bitand(ch, 96), 5)
		elseif ii == 7 then
			udtab[kk] = bitor(bitleft(bitand(ch, 63), 2), udtab[kk])
			udtab[kk + 1] = bitright(bitand(ch, 64), 6)
		else
			udtab[kk] = bitor(bitleft(ch, 1), udtab[kk])
			ii = 0
			kk = kk - 1
		end
		ii = ii + 1
		jj = jj + 2
		kk = kk + 1
	until jj > #txt
	for jj = 1, maxb do
		udtab[jj] = string.format("%02X", udtab[jj])
	end
	pack7bit["pdu"] = table.concat(udtab)
end
return pack7bit
