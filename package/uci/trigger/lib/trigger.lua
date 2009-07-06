module("uci.trigger", package.seeall)
require("posix")
require("uci")

local path = "/lib/config/trigger"
local triggers = nil
local tmp_cursor = nil

function load_modules()
	if triggers ~= nil then
		return
	end
	triggers = {
		list = {},
		uci = {},
		active = {}
	}
	local modules = posix.glob(path .. "/*.lua")
	if modules == nil then
		return
	end
	local oldpath = package.path
	package.path = path .. "/?.lua"
	for i, v in ipairs(modules) do
		pcall(require(string.gsub(v, path .. "/(%w+)%.lua$", "%1")))
	end
	package.path = oldpath
end

function check_table(table, name)
	if table[name] == nil then
		table[name] = {}
	end
	return table[name]
end

function get_table_val(val, vtype)
	if type(val) == (vtype or "string") then
		return { val }
	elseif type(val) == "table" then
		return val
	end
	return nil
end

function get_name_list(name)
	return get_table_val(name or ".all")
end

function add_trigger_option(list, t)
	local name = get_name_list(t.option)
	for i, n in ipairs(name) do
		option = check_table(list, n)
		table.insert(option, t)
	end
end

function add_trigger_section(list, t)
	local name = get_name_list(t.section)
	for i, n in ipairs(name) do
		section = check_table(list, n)
		add_trigger_option(section, t)
	end
end

function check_insert_triggers(dest, list, tuple)
	if list == nil then
		return
	end
	for i, t in ipairs(list) do
		local add = true
		if type(t.check) == "function" then
			add = t.check(tuple)
		end
		if add then
			dest[t.id] = t
		end
	end
end

function find_section_triggers(tlist, pos, tuple)
	if pos == nil then
		return
	end
	check_insert_triggers(tlist, pos[".all"], tuple)
	if tuple.option then
		check_insert_triggers(tlist, pos[tuple.option], tuple)
	end
end

function check_recursion(name, seen)
	if seen == nil then
		seen = {}
	end
	if seen[name] then
		return nil
	end
	seen[name] = true
	return seen
end


function find_recursive_depends(list, name, seen)
	seen = check_recursion(name, seen)
	if not seen then
		return
	end
	local bt = get_table_val(triggers.list[name].belongs_to) or {}
	for i, n in ipairs(bt) do
		table.insert(list, n)
		find_recursive_depends(list, n, seen)
	end
end

function check_trigger_depth(list, name)
	if name == nil then
		return
	end

	local n = list[name]
	if n == nil then
		return
	end

	list[name] = nil
	return check_trigger_depth(list, n)
end

function find_triggers(tuple)
	local pos = triggers.uci[tuple.package]
	if pos == nil then
		return {}
	end

	local tlist = {}
	find_section_triggers(tlist, pos[".all"], tuple)
	find_section_triggers(tlist, pos[tuple.section[".type"]], tuple)

	for n, t in pairs(tlist) do
		local dep = {}
		find_recursive_depends(dep, t.id)
		for i, depname in ipairs(dep) do
			check_trigger_depth(tlist, depname)
		end
	end

	local nlist = {}
	for n, t in pairs(tlist) do
		if t then
			table.insert(nlist, t)
		end
	end

	return nlist
end

function reset_state()
	assert(io.open("/var/run/uci_trigger", "w")):close()
	if tctx then
		tctx:unload("uci_trigger")
	end
end

function load_state()
	-- make sure the config file exists before we attempt to load it
	-- uci doesn't like loading nonexistent config files
	local f = assert(io.open("/var/run/uci_trigger", "a")):close()

	load_modules()
	triggers.active = {}
	if tctx then
		tctx:unload("uci_trigger")
	else
		tctx = uci.cursor()
	end
	assert(tctx:load("/var/run/uci_trigger"))
	tctx:foreach("uci_trigger", "trigger",
		function(section)
			trigger = triggers.list[section[".name"]]
			if trigger == nil then
				return
			end

			active = {}
			triggers.active[trigger.id] = active

			local s = get_table_val(section["sections"]) or {}
			for i, v in ipairs(s) do
				active[v] = true
			end
		end
	)
end

function get_names(list)
	local slist = {}
	for name, val in pairs(list) do
		if val then
			table.insert(slist, name)
		end
	end
	return slist
end

function check_cancel(name, seen)
	local t = triggers.list[name]
	local dep = get_table_val(t.belongs_to)
	seen = check_recursion(name, seen)

	if not t or not dep or not seen then
		return false
	end

	for i, v in ipairs(dep) do
		-- only cancel triggers for all sections
		-- if both the current and the parent trigger
		-- are per-section
		local section_only = false
		if t.section_only then
			local tdep = triggers.list[v]
			if tdep then
				section_only = tdep.section_only
			end
		end

		if check_cancel(v, seen) then
			return true
		end
		if triggers.active[v] then
			if section_only then
				for n, active in pairs(triggers.active[v]) do
					triggers.active[name][n] = false
				end
			else
				return true
			end
		end
	end
	return false
end

-- trigger api functions

function add(ts)
	for i,t in ipairs(ts) do
		triggers.list[t.id] = t
		match = {}
		if t.package then
			local package = check_table(triggers.uci, t.package)
			add_trigger_section(package, t)
			triggers.list[t.id] = t
		end
	end
end

function save_trigger(name)
	if triggers.active[name] then
		local slist = get_names(triggers.active[name])
		if #slist > 0 then
			tctx:set("uci_trigger", name, "sections", slist)
		end
	else
		tctx:delete("uci_trigger", name)
	end
end

function set(data, cursor)
	assert(data ~= nil)
	if cursor == nil then
		cursor = tmp_cursor or uci.cursor()
		tmp_cursor = uci.cursor
	end

	local tuple = {
		package = data[1],
		section = data[2],
		option = data[3],
		value = data[4]
	}
	assert(cursor:load(tuple.package))

	load_state()
	local section = cursor:get_all(tuple.package, tuple.section)
	if (section == nil) then
		if option ~= nil then
			return
		end
		section = {
			[".type"] = value
		}
		if tuple.section == nil then
			tuple.section = ""
			section[".anonymous"] = true
		end
		section[".name"] = tuple.section
	end
	tuple.section = section

	local ts = find_triggers(tuple)
	for i, t in ipairs(ts) do
		local active = triggers.active[t.id]
		if not active then
			active = {}
			triggers.active[t.id] = active
			tctx:set("uci_trigger", t.id, "trigger")
		end
		if section[".name"] then
			active[section[".name"]] = true
		end
		save_trigger(t.id)
	end
	tctx:save("uci_trigger")
end

function get_description(trigger, sections)
	if not trigger.title then
		return trigger.id
	end
	local desc = trigger.title
	if trigger.section_only and sections and #sections > 0 then
		desc = desc .. " (" .. table.concat(sections, ", ") .. ")"
	end
	return desc
end

function get_active()
	local slist = {}

	if triggers == nil then
		load_state()
	end
	for name, val in pairs(triggers.active) do
		if val and not check_cancel(name) then
			local sections = {}
			for name, active in pairs(triggers.active[name]) do
				if active then
					table.insert(sections, name)
				end
			end
			table.insert(slist, { triggers.list[name], sections })
		end
	end
	return slist
end

function set_active(trigger, sections)
	if triggers == nil then
		load_state()
	end
	if not triggers.list[trigger] then
		return
	end
	if triggers.active[trigger] == nil then
		tctx:set("uci_trigger", trigger, "trigger")
		triggers.active[trigger] = {}
	end
	local active = triggers.active[trigger]
	if triggers.list[trigger].section_only or sections ~= nil then
		for i, t in ipairs(sections) do
			triggers.active[trigger][t] = true
		end
	end
	save_trigger(trigger)
	tctx:save("uci_trigger")
end

function clear_active(trigger, sections)
	if triggers == nil then
		load_state()
	end
	if triggers.list[trigger] == nil or triggers.active[trigger] == nil then
		return
	end
	local active = triggers.active[trigger]
	if not triggers.list[trigger].section_only or sections == nil then
		triggers.active[trigger] = nil
	else
		for i, t in ipairs(sections) do
			triggers.active[trigger][t] = false
		end
	end
	save_trigger(trigger)
	tctx:save("uci_trigger")
end

function run(ts)
	if ts == nil then
		ts = get_active()
	end
	for i, t in ipairs(ts) do
		local trigger = t[1]
		local sections = t[2]
		local actions = get_table_val(trigger.action, "function") or {}
		for ai, a in ipairs(actions) do
			if not trigger.section_only then
				sections = { "" }
			end
			for si, s in ipairs(sections) do
				if a(s) then
					tctx:delete("uci_trigger", trigger.id)
					tctx:save("uci_trigger")
				end
			end
		end
	end
end

-- helper functions

function system_command(arg)
	local cmd = arg
	return function(arg)
		return os.execute(cmd:format(arg)) == 0
	end
end

function service_restart(arg)
	return system_command("/etc/init.d/" .. arg .. " restart")
end
