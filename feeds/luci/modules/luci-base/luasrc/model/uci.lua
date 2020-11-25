-- Copyright 2008 Steven Barth <steven@midlink.org>
-- Licensed to the public under the Apache License 2.0.

local os    = require "os"
local util  = require "luci.util"
local table = require "table"


local setmetatable, rawget, rawset = setmetatable, rawget, rawset
local require, getmetatable, assert = require, getmetatable, assert
local error, pairs, ipairs, select = error, pairs, ipairs, select
local type, tostring, tonumber, unpack = type, tostring, tonumber, unpack

-- The typical workflow for UCI is:  Get a cursor instance from the
-- cursor factory, modify data (via Cursor.add, Cursor.delete, etc.),
-- save the changes to the staging area via Cursor.save and finally
-- Cursor.commit the data to the actual config files.
-- LuCI then needs to Cursor.apply the changes so daemons etc. are
-- reloaded.
module "luci.model.uci"

local ERRSTR = {
	"Invalid command",
	"Invalid argument",
	"Method not found",
	"Entry not found",
	"No data",
	"Permission denied",
	"Timeout",
	"Not supported",
	"Unknown error",
	"Connection failed"
}

local session_id = nil

local function call(cmd, args)
	if type(args) == "table" and session_id then
		args.ubus_rpc_session = session_id
	end
	return util.ubus("uci", cmd, args)
end


function cursor()
	return _M
end

function cursor_state()
	return _M
end

function substate(self)
	return self
end


function get_confdir(self)
	return "/etc/config"
end

function get_savedir(self)
	return "/tmp/.uci"
end

function get_session_id(self)
	return session_id
end

function set_confdir(self, directory)
	return false
end

function set_savedir(self, directory)
	return false
end

function set_session_id(self, id)
	session_id = id
	return true
end


function load(self, config)
	return true
end

function save(self, config)
	return true
end

function unload(self, config)
	return true
end


function changes(self, config)
	local rv, err = call("changes", { config = config })

	if type(rv) == "table" and type(rv.changes) == "table" then
		return rv.changes
	elseif err then
		return nil, ERRSTR[err]
	else
		return { }
	end
end


function revert(self, config)
	local _, err = call("revert", { config = config })
	return (err == nil), ERRSTR[err]
end

function commit(self, config)
	local _, err = call("commit", { config = config })
	return (err == nil), ERRSTR[err]
end

function apply(self, rollback)
	local _, err

	if rollback then
		local sys = require "luci.sys"
		local conf = require "luci.config"
		local timeout = tonumber(conf and conf.apply and conf.apply.rollback or 30) or 0

		_, err = call("apply", {
			timeout = (timeout > 30) and timeout or 30,
			rollback = true
		})

		if not err then
			local now = os.time()
			local token = sys.uniqueid(16)

			util.ubus("session", "set", {
				ubus_rpc_session = "00000000000000000000000000000000",
				values = {
					rollback = {
						token   = token,
						session = session_id,
						timeout = now + timeout
					}
				}
			})

			return token
		end
	else
		_, err = call("changes", {})

		if not err then
			if type(_) == "table" and type(_.changes) == "table" then
				local k, v
				for k, v in pairs(_.changes) do
					_, err = call("commit", { config = k })
					if err then
						break
					end
				end
			end
		end

		if not err then
			_, err = call("apply", { rollback = false })
		end
	end

	return (err == nil), ERRSTR[err]
end

function confirm(self, token)
	local is_pending, time_remaining, rollback_sid, rollback_token = self:rollback_pending()

	if is_pending then
		if token ~= rollback_token then
			return false, "Permission denied"
		end

		local _, err = util.ubus("uci", "confirm", {
			ubus_rpc_session = rollback_sid
		})

		if not err then
			util.ubus("session", "set", {
				ubus_rpc_session = "00000000000000000000000000000000",
				values = { rollback = {} }
			})
		end

		return (err == nil), ERRSTR[err]
	end

	return false, "No data"
end

function rollback(self)
	local is_pending, time_remaining, rollback_sid = self:rollback_pending()

	if is_pending then
		local _, err = util.ubus("uci", "rollback", {
			ubus_rpc_session = rollback_sid
		})

		if not err then
			util.ubus("session", "set", {
				ubus_rpc_session = "00000000000000000000000000000000",
				values = { rollback = {} }
			})
		end

		return (err == nil), ERRSTR[err]
	end

	return false, "No data"
end

function rollback_pending(self)
	local rv, err = util.ubus("session", "get", {
		ubus_rpc_session = "00000000000000000000000000000000",
		keys = { "rollback" }
	})

	local now = os.time()

	if type(rv) == "table" and
	   type(rv.values) == "table" and
	   type(rv.values.rollback) == "table" and
	   type(rv.values.rollback.token) == "string" and
	   type(rv.values.rollback.session) == "string" and
	   type(rv.values.rollback.timeout) == "number" and
	   rv.values.rollback.timeout > now
	then
		return true,
			rv.values.rollback.timeout - now,
			rv.values.rollback.session,
			rv.values.rollback.token
	end

	return false, ERRSTR[err]
end


function foreach(self, config, stype, callback)
	if type(callback) == "function" then
		local rv, err = call("get", {
			config = config,
			type   = stype
		})

		if type(rv) == "table" and type(rv.values) == "table" then
			local sections = { }
			local res = false
			local index = 1

			local _, section
			for _, section in pairs(rv.values) do
				section[".index"] = section[".index"] or index
				sections[index] = section
				index = index + 1
			end

			table.sort(sections, function(a, b)
				return a[".index"] < b[".index"]
			end)

			for _, section in ipairs(sections) do
				local continue = callback(section)
				res = true
				if continue == false then
					break
				end
			end
			return res
		else
			return false, ERRSTR[err] or "No data"
		end
	else
		return false, "Invalid argument"
	end
end

local function _get(self, operation, config, section, option)
	if section == nil then
		return nil
	elseif type(option) == "string" and option:byte(1) ~= 46 then
		local rv, err = call(operation, {
			config  = config,
			section = section,
			option  = option
		})

		if type(rv) == "table" then
			return rv.value or nil
		elseif err then
			return false, ERRSTR[err]
		else
			return nil
		end
	elseif option == nil then
		local values = self:get_all(config, section)
		if values then
			return values[".type"], values[".name"]
		else
			return nil
		end
	else
		return false, "Invalid argument"
	end
end

function get(self, ...)
	return _get(self, "get", ...)
end

function get_state(self, ...)
	return _get(self, "state", ...)
end

function get_all(self, config, section)
	local rv, err = call("get", {
		config  = config,
		section = section
	})

	if type(rv) == "table" and type(rv.values) == "table" then
		return rv.values
	elseif err then
		return false, ERRSTR[err]
	else
		return nil
	end
end

function get_bool(self, ...)
	local val = self:get(...)
	return (val == "1" or val == "true" or val == "yes" or val == "on")
end

function get_first(self, config, stype, option, default)
	local rv = default

	self:foreach(config, stype, function(s)
		local val = not option and s[".name"] or s[option]

		if type(default) == "number" then
			val = tonumber(val)
		elseif type(default) == "boolean" then
			val = (val == "1" or val == "true" or
			       val == "yes" or val == "on")
		end

		if val ~= nil then
			rv = val
			return false
		end
	end)

	return rv
end

function get_list(self, config, section, option)
	if config and section and option then
		local val = self:get(config, section, option)
		return (type(val) == "table" and val or { val })
	end
	return { }
end


function section(self, config, stype, name, values)
	local rv, err = call("add", {
		config = config,
		type   = stype,
		name   = name,
		values = values
	})

	if type(rv) == "table" then
		return rv.section
	elseif err then
		return false, ERRSTR[err]
	else
		return nil
	end
end


function add(self, config, stype)
	return self:section(config, stype)
end

function set(self, config, section, option, ...)
	if select('#', ...) == 0 then
		local sname, err = self:section(config, option, section)
		return (not not sname), err
	else
		local _, err = call("set", {
			config  = config,
			section = section,
			values  = { [option] = select(1, ...) }
		})
		return (err == nil), ERRSTR[err]
	end
end

function set_list(self, config, section, option, value)
	if section == nil or option == nil then
		return false
	elseif value == nil or (type(value) == "table" and #value == 0) then
		return self:delete(config, section, option)
	elseif type(value) == "table" then
		return self:set(config, section, option, value)
	else
		return self:set(config, section, option, { value })
	end
end

function tset(self, config, section, values)
	local _, err = call("set", {
		config  = config,
		section = section,
		values  = values
	})
	return (err == nil), ERRSTR[err]
end

function reorder(self, config, section, index)
	local sections

	if type(section) == "string" and type(index) == "number" then
		local pos = 0

		sections = { }

		self:foreach(config, nil, function(s)
			if pos == index then
				pos = pos + 1
			end

			if s[".name"] ~= section then
				pos = pos + 1
				sections[pos] = s[".name"]
			else
				sections[index + 1] = section
			end
		end)
	elseif type(section) == "table" then
		sections = section
	else
		return false, "Invalid argument"
	end

	local _, err = call("order", {
		config   = config,
		sections = sections
	})

	return (err == nil), ERRSTR[err]
end


function delete(self, config, section, option)
	local _, err = call("delete", {
		config  = config,
		section = section,
		option  = option
	})
	return (err == nil), ERRSTR[err]
end

function delete_all(self, config, stype, comparator)
	local _, err
	if type(comparator) == "table" then
		_, err = call("delete", {
			config = config,
			type   = stype,
			match  = comparator
		})
	elseif type(comparator) == "function" then
		local rv = call("get", {
			config = config,
			type   = stype
		})

		if type(rv) == "table" and type(rv.values) == "table" then
			local sname, section
			for sname, section in pairs(rv.values) do
				if comparator(section) then
					_, err = call("delete", {
						config  = config,
						section = sname
					})
				end
			end
		end
	elseif comparator == nil then
		_, err = call("delete", {
			config  = config,
			type    = stype
		})
	else
		return false, "Invalid argument"
	end

	return (err == nil), ERRSTR[err]
end
