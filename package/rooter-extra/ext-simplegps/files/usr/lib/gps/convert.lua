#!/usr/bin/lua

ldeg = arg[1]
lmin = arg[2]
lsecd = arg[3]
lhemi = arg[4]
sect = arg[5]

lmin = lmin / 60
if sect == nil then
	lsecd = (lsecd * 6) /1000
end
lsec = lsecd / 3600
ldegree = ldeg + lmin + lsec
if lhemi == "S" then
	ldegree = -ldegree
end
if lhemi == "W" then
	ldegree = -ldegree
end

sdeg = string.format("%.4f",ldegree)

local tfile = io.open("/tmp/latlon", "w")
tfile:write("CONVERT=\"", sdeg, "\"")
tfile:close()