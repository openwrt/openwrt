#!/usr/bin/lua

filepost = "-mac_data.js"
dirname = '/usr/lib/bwmon/data'

function clean()
	nummon = 0
	months = {}
	f = io.popen('/bin/ls ' .. dirname)
	for name in f:lines() do 
		s, e = name:find(filepost)
		if s ~= nil then
			nummon = nummon + 1
			months[nummon] = dirname .. "/" .. name
		end
	end
	f:close()

	count = 1
	if nummon > 0 then
		for i=nummon,1,-1 do
			if count > 3 then
				os.execute("rm -f " .. months[i])
			end
			count = count + 1
		end
	end
end

clean()
dirname = '/tmp/bwmon/data'
clean()