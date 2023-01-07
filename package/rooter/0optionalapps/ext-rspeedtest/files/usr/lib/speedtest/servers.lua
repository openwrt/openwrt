#!/usr/bin/lua

function trim(s)
  return (s:gsub("^%s*(.-)%s*$", "%1"))
end

line = arg[1]
if line == nil then
	return
end

s, e = line:find("url=\"")
if s ~= nil then
	cs, ce = line:find("\"", e+1)
	url = trim(line:sub(e+1, cs-1))
else
	return
end
s, e = line:find("lat=\"")
if s ~= nil then
	cs, ce = line:find("\"", e+1)
	lat = trim(line:sub(e+1, cs-1))
end
s, e = line:find("lon=\"")
if s ~= nil then
	cs, ce = line:find("\"", e+1)
	lon = trim(line:sub(e+1, cs-1))
end
s, e = line:find("name=\"")
if s ~= nil then
	cs, ce = line:find("\"", e+1)
	name = trim(line:sub(e+1, cs-1))
end
s, e = line:find("sponsor=\"")
if s ~= nil then
	cs, ce = line:find("\"", e+1)
	sponsor = trim(line:sub(e+1, cs-1))
end

tfile = io.open("/tmp/server", "w")
tfile:write("URL=\"", url, "\"\n")
tfile:write("LAT=\"", lat, "\"\n")
tfile:write("LON=\"", lon, "\"\n")
tfile:write("NAME=\"", name, "\"\n")
tfile:write("SPONSOR=\"", sponsor, "\"")
tfile:close()