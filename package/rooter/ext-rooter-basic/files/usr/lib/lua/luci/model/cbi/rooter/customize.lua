local utl = require "luci.util"

local modemfile = "/etc/config/modem.data"
local modemdata = {}
local count
local tabdata = {}

function process_line(xline, cnt)
	local data = {}
	local pline = xline
	local start = 1
	for i=1,3 do
		s, e = string.find(pline, " ")
		data[i] = string.sub(pline, start, s-1)
		pline = string.sub(pline, e+1)
	end
	data[4] = pline
	modemdata[cnt] = data
end

function read_modem()
	count = 0
	local file = io.open(modemfile, "r")
	if file == nil then
		return
	end
	repeat
		local line = file:read("*line")
		if line == nil then
			break
		end
		if string.len(line) < 5 then
			break
		end
		count = count + 1
		process_line(line, count)
	until 1==0
	file:close()
end

function insert_modem()
	local location = count + 1

	if count > 0 then
		for j=1,count do
			local mdata = modemdata[j]
			if mdata[1] == tabdata[1] and  mdata[2] == tabdata[2] then
				location = j
			end
		end
	end

	if count == 0 then
		count = 1
	else
		if location > count then
			count = count + 1
		end
	end
	modemdata[location] = tabdata
end

function write_modem()
	os.remove(modemfile)
	local file = io.open(modemfile, "w")
	for k=1,count do
		local mdata = modemdata[k]
		for l=1,3 do
			file:write(mdata[l], " ")
		end
		file:write(mdata[4], "\n")
	end
	file:close()
end

function process_tabdata()
	if tabdata[1] ~= "nil" and tabdata[2] ~= "nil" then
		read_modem()
		insert_modem()
		write_modem()
	end
end

m = Map("modem", translate("Custom Modem Support"), translate("Change the Data and Communication Ports used by a Specific Modem"))

m.on_after_commit = function(self)
    -- all written config names are in self.parsechain
	local sobj
	for _, sobj in ipairs(self.children) do
		local sids
		if utl.instanceof(sobj, NamedSection) then
			sids = { sobj.section }
		elseif utl.instanceof(sobj, TypedSection) then
	              sids = sobj:cfgsections()
	       end
		local sid, fld, fln

		if not utl.instanceof(sobj, SimpleSection) then
        		for _, sid in ipairs(sids) do
				for fln, fld in ipairs(sobj.children) do
					local val = fld:formvalue(sid)
					if val == nil or string.len(val) == 0 then
						val = "nil"
					end
					tabdata[fln] = val
				end
            		end
		end
	end
	process_tabdata()
end

--
-- Vid Pid Dataport Commport
--

e = m:section(TypedSection, "new", translate("Modem Ports")) 

a1 = e:option(Value, "vid", translate("Switched Vendor ID :")); 
a1.optional=false;

b1 = e:option(Value, "pid", translate("Switched Product ID :")); 
b1.optional=false; 

p3 = e:option(ListValue, "port", translate("PPP Modem Data Port :"))
p3:value("tty", "default")
p3:value("tty0", "/dev/ttyUSB0")
p3:value("tty1", "/dev/ttyUSB1")
p3:value("tty2", "/dev/ttyUSB2")
p3:value("tty3", "/dev/ttyUSB3")
p3:value("tty4", "/dev/ttyUSB4")
p3:value("tty5", "/dev/ttyUSB5")
p3.default = "tty"

p4 = e:option(ListValue, "comm", translate("Communication Port :"))
p4:value("tty", "default")
p4:value("tty0", "/dev/ttyUSB0")
p4:value("tty1", "/dev/ttyUSB1")
p4:value("tty2", "/dev/ttyUSB2")
p4:value("tty3", "/dev/ttyUSB3")
p4:value("tty4", "/dev/ttyUSB4")
p4:value("tty5", "/dev/ttyUSB5")
p4.default = "tty"

b3 = e:option(DummyValue, "blank", " ");

btn = e:option(Button, "_btn", translate(" "))
btn.inputtitle = translate("Delete Modem Port Database")
btn.inputstyle = "apply"
function btn.write()
	luci.sys.call("/usr/lib/rooter/luci/luaops.sh delete /etc/config/modem.data")
end

m:section(SimpleSection).template = "rooter/custom"

return m