
local fs	= require "nixio.fs"
local type  = type

module "luci.openclash"

--- LuCI filesystem library.

function isdirectory(dirname)
	return fs.stat(dirname, "type") == "dir"
end
