-- Copyright 2008 Steven Barth <steven@midlink.org>
-- Copyright 2008-2015 Jo-Philipp Wich <jow@openwrt.org>
-- Licensed to the public under the Apache License 2.0.

local fs = require "nixio.fs"
local sys = require "luci.sys"
local util = require "luci.util"
local http = require "luci.http"
local nixio = require "nixio", require "nixio.util"

module("luci.dispatcher", package.seeall)
context = util.threadlocal()
uci = require "luci.model.uci"
i18n = require "luci.i18n"
_M.fs = fs

-- Index table
local index = nil

-- Fastindex
local fi


function build_url(...)
	local path = {...}
	local url = { http.getenv("SCRIPT_NAME") or "" }

	local p
	for _, p in ipairs(path) do
		if p:match("^[a-zA-Z0-9_%-%.%%/,;]+$") then
			url[#url+1] = "/"
			url[#url+1] = p
		end
	end

	if #path == 0 then
		url[#url+1] = "/"
	end

	return table.concat(url, "")
end

function _ordered_children(node)
	local name, child, children = nil, nil, {}

	for name, child in pairs(node.nodes) do
		children[#children+1] = {
			name  = name,
			node  = child,
			order = child.order or 100
		}
	end

	table.sort(children, function(a, b)
		if a.order == b.order then
			return a.name < b.name
		else
			return a.order < b.order
		end
	end)

	return children
end

local function dependencies_satisfied(node)
	if type(node.file_depends) == "table" then
		for _, file in ipairs(node.file_depends) do
			local ftype = fs.stat(file, "type")
			if ftype == "dir" then
				local empty = true
				for e in (fs.dir(file) or function() end) do
					empty = false
				end
				if empty then
					return false
				end
			elseif ftype == nil then
				return false
			end
		end
	end

	if type(node.uci_depends) == "table" then
		for config, expect_sections in pairs(node.uci_depends) do
			if type(expect_sections) == "table" then
				for section, expect_options in pairs(expect_sections) do
					if type(expect_options) == "table" then
						for option, expect_value in pairs(expect_options) do
							local val = uci:get(config, section, option)
							if expect_value == true and val == nil then
								return false
							elseif type(expect_value) == "string" then
								if type(val) == "table" then
									local found = false
									for _, subval in ipairs(val) do
										if subval == expect_value then
											found = true
										end
									end
									if not found then
										return false
									end
								elseif val ~= expect_value then
									return false
								end
							end
						end
					else
						local val = uci:get(config, section)
						if expect_options == true and val == nil then
							return false
						elseif type(expect_options) == "string" and val ~= expect_options then
							return false
						end
					end
				end
			elseif expect_sections == true then
				if not uci:get_first(config) then
					return false
				end
			end
		end
	end

	return true
end

function node_visible(node)
   if node then
	  return not (
		 (not dependencies_satisfied(node)) or
		 (not node.title or #node.title == 0) or
		 (not node.target or node.hidden == true) or
		 (type(node.target) == "table" and node.target.type == "firstchild" and
		  (type(node.nodes) ~= "table" or not next(node.nodes)))
	  )
   end
   return false
end

function node_childs(node)
	local rv = { }
	if node then
		local _, child
		for _, child in ipairs(_ordered_children(node)) do
			if node_visible(child.node) then
				rv[#rv+1] = child.name
			end
		end
	end
	return rv
end


function error404(message)
	http.status(404, "Not Found")
	message = message or "Not Found"

	local function render()
		local template = require "luci.template"
		template.render("error404")
	end

	if not util.copcall(render) then
		http.prepare_content("text/plain")
		http.write(message)
	end

	return false
end

function error500(message)
	util.perror(message)
	if not context.template_header_sent then
		http.status(500, "Internal Server Error")
		http.prepare_content("text/plain")
		http.write(message)
	else
		require("luci.template")
		if not util.copcall(luci.template.render, "error500", {message=message}) then
			http.prepare_content("text/plain")
			http.write(message)
		end
	end
	return false
end

function httpdispatch(request, prefix)
	http.context.request = request

	local r = {}
	context.request = r

	local pathinfo = http.urldecode(request:getenv("PATH_INFO") or "", true)

	if prefix then
		for _, node in ipairs(prefix) do
			r[#r+1] = node
		end
	end

	local node
	for node in pathinfo:gmatch("[^/%z]+") do
		r[#r+1] = node
	end

	local stat, err = util.coxpcall(function()
		dispatch(context.request)
	end, error500)

	http.close()

	--context._disable_memtrace()
end

local function require_post_security(target, args)
	if type(target) == "table" and target.type == "arcombine" and type(target.targets) == "table" then
		return require_post_security((type(args) == "table" and #args > 0) and target.targets[2] or target.targets[1], args)
	end

	if type(target) == "table" then
		if type(target.post) == "table" then
			local param_name, required_val, request_val

			for param_name, required_val in pairs(target.post) do
				request_val = http.formvalue(param_name)

				if (type(required_val) == "string" and
				    request_val ~= required_val) or
				   (required_val == true and request_val == nil)
				then
					return false
				end
			end

			return true
		end

		return (target.post == true)
	end

	return false
end

function test_post_security()
	if http.getenv("REQUEST_METHOD") ~= "POST" then
		http.status(405, "Method Not Allowed")
		http.header("Allow", "POST")
		return false
	end

	if http.formvalue("token") ~= context.authtoken then
		http.status(403, "Forbidden")
		luci.template.render("csrftoken")
		return false
	end

	return true
end

local function session_retrieve(sid, allowed_users)
	local sdat = util.ubus("session", "get", { ubus_rpc_session = sid })

	if type(sdat) == "table" and
	   type(sdat.values) == "table" and
	   type(sdat.values.token) == "string" and
	   (not allowed_users or
	    util.contains(allowed_users, sdat.values.username))
	then
		uci:set_session_id(sid)
		return sid, sdat.values
	end

	return nil, nil
end

local function session_setup(user, pass, allowed_users)
	if util.contains(allowed_users, user) then
		local login = util.ubus("session", "login", {
			username = user,
			password = pass,
			timeout  = tonumber(luci.config.sauth.sessiontime)
		})

		local rp = context.requestpath
			and table.concat(context.requestpath, "/") or ""

		if type(login) == "table" and
		   type(login.ubus_rpc_session) == "string"
		then
			util.ubus("session", "set", {
				ubus_rpc_session = login.ubus_rpc_session,
				values = { token = sys.uniqueid(16) }
			})

			io.stderr:write("luci: accepted login on /%s for %s from %s\n"
				%{ rp, user, http.getenv("REMOTE_ADDR") or "?" })

			return session_retrieve(login.ubus_rpc_session)
		end

		io.stderr:write("luci: failed login on /%s for %s from %s\n"
			%{ rp, user, http.getenv("REMOTE_ADDR") or "?" })
	end

	return nil, nil
end

function dispatch(request)
	--context._disable_memtrace = require "luci.debug".trap_memtrace("l")
	local ctx = context
	ctx.path = request

	local conf = require "luci.config"
	assert(conf.main,
		"/etc/config/luci seems to be corrupt, unable to find section 'main'")

	local i18n = require "luci.i18n"
	local lang = conf.main.lang or "auto"
	if lang == "auto" then
		local aclang = http.getenv("HTTP_ACCEPT_LANGUAGE") or ""
		for aclang in aclang:gmatch("[%w_-]+") do
			local country, culture = aclang:match("^([a-z][a-z])[_-]([a-zA-Z][a-zA-Z])$")
			if country and culture then
				local cc = "%s_%s" %{ country, culture:lower() }
				if conf.languages[cc] then
					lang = cc
					break
				elseif conf.languages[country] then
					lang = country
					break
				end
			elseif conf.languages[aclang] then
				lang = aclang
				break
			end
		end
	end
	if lang == "auto" then
		lang = i18n.default
	end
	i18n.setlanguage(lang)

	local c = ctx.tree
	local stat
	if not c then
		c = createtree()
	end

	local track = {}
	local args = {}
	ctx.args = args
	ctx.requestargs = ctx.requestargs or args
	local n
	local preq = {}
	local freq = {}

	for i, s in ipairs(request) do
		preq[#preq+1] = s
		freq[#freq+1] = s
		c = c.nodes[s]
		n = i
		if not c then
			break
		end

		util.update(track, c)

		if c.leaf then
			break
		end
	end

	if c and c.leaf then
		for j=n+1, #request do
			args[#args+1] = request[j]
			freq[#freq+1] = request[j]
		end
	end

	ctx.requestpath = ctx.requestpath or freq
	ctx.path = preq

	-- Init template engine
	if (c and c.index) or not track.notemplate then
		local tpl = require("luci.template")
		local media = track.mediaurlbase or luci.config.main.mediaurlbase
		if not pcall(tpl.Template, "themes/%s/header" % fs.basename(media)) then
			media = nil
			for name, theme in pairs(luci.config.themes) do
				if name:sub(1,1) ~= "." and pcall(tpl.Template,
				 "themes/%s/header" % fs.basename(theme)) then
					media = theme
				end
			end
			assert(media, "No valid theme found")
		end

		local function _ifattr(cond, key, val, noescape)
			if cond then
				local env = getfenv(3)
				local scope = (type(env.self) == "table") and env.self
				if type(val) == "table" then
					if not next(val) then
						return ''
					else
						val = util.serialize_json(val)
					end
				end

				val = tostring(val or
					(type(env[key]) ~= "function" and env[key]) or
					(scope and type(scope[key]) ~= "function" and scope[key]) or "")

				if noescape ~= true then
					val = util.pcdata(val)
				end

				return string.format(' %s="%s"', tostring(key), val)
			else
				return ''
			end
		end

		tpl.context.viewns = setmetatable({
		   write       = http.write;
		   include     = function(name) tpl.Template(name):render(getfenv(2)) end;
		   translate   = i18n.translate;
		   translatef  = i18n.translatef;
		   export      = function(k, v) if tpl.context.viewns[k] == nil then tpl.context.viewns[k] = v end end;
		   striptags   = util.striptags;
		   pcdata      = util.pcdata;
		   media       = media;
		   theme       = fs.basename(media);
		   resource    = luci.config.main.resourcebase;
		   ifattr      = function(...) return _ifattr(...) end;
		   attr        = function(...) return _ifattr(true, ...) end;
		   url         = build_url;
		}, {__index=function(tbl, key)
			if key == "controller" then
				return build_url()
			elseif key == "REQUEST_URI" then
				return build_url(unpack(ctx.requestpath))
			elseif key == "FULL_REQUEST_URI" then
				local url = { http.getenv("SCRIPT_NAME") or "", http.getenv("PATH_INFO") }
				local query = http.getenv("QUERY_STRING")
				if query and #query > 0 then
					url[#url+1] = "?"
					url[#url+1] = query
				end
				return table.concat(url, "")
			elseif key == "token" then
				return ctx.authtoken
			else
				return rawget(tbl, key) or _G[key]
			end
		end})
	end

	track.dependent = (track.dependent ~= false)
	assert(not track.dependent or not track.auto,
		"Access Violation\nThe page at '" .. table.concat(request, "/") .. "/' " ..
		"has no parent node so the access to this location has been denied.\n" ..
		"This is a software bug, please report this message at " ..
		"https://github.com/openwrt/luci/issues"
	)

	if track.sysauth and not ctx.authsession then
		local authen = track.sysauth_authenticator
		local _, sid, sdat, default_user, allowed_users

		if type(authen) == "string" and authen ~= "htmlauth" then
			error500("Unsupported authenticator %q configured" % authen)
			return
		end

		if type(track.sysauth) == "table" then
			default_user, allowed_users = nil, track.sysauth
		else
			default_user, allowed_users = track.sysauth, { track.sysauth }
		end

		if type(authen) == "function" then
			_, sid = authen(sys.user.checkpasswd, allowed_users)
		else
			sid = http.getcookie("sysauth")
		end

		sid, sdat = session_retrieve(sid, allowed_users)

		if not (sid and sdat) and authen == "htmlauth" then
			local user = http.getenv("HTTP_AUTH_USER")
			local pass = http.getenv("HTTP_AUTH_PASS")

			if user == nil and pass == nil then
				user = http.formvalue("luci_username")
				pass = http.formvalue("luci_password")
			end

			sid, sdat = session_setup(user, pass, allowed_users)

			if not sid then
				local tmpl = require "luci.template"

				context.path = {}

				http.status(403, "Forbidden")
				http.header("X-LuCI-Login-Required", "yes")
				tmpl.render(track.sysauth_template or "sysauth", {
					duser = default_user,
					fuser = user
				})

				return
			end

			http.header("Set-Cookie", 'sysauth=%s; path=%s; SameSite=Strict; HttpOnly%s' %{
				sid, build_url(), http.getenv("HTTPS") == "on" and "; secure" or ""
			})
			http.redirect(build_url(unpack(ctx.requestpath)))
		end

		if not sid or not sdat then
			http.status(403, "Forbidden")
			http.header("X-LuCI-Login-Required", "yes")
			return
		end

		ctx.authsession = sid
		ctx.authtoken = sdat.token
		ctx.authuser = sdat.username
	end

	if track.cors and http.getenv("REQUEST_METHOD") == "OPTIONS" then
		luci.http.status(200, "OK")
		luci.http.header("Access-Control-Allow-Origin", http.getenv("HTTP_ORIGIN") or "*")
		luci.http.header("Access-Control-Allow-Methods", "GET, POST, OPTIONS")
		return
	end

	if c and require_post_security(c.target, args) then
		if not test_post_security(c) then
			return
		end
	end

	if track.setgroup then
		sys.process.setgroup(track.setgroup)
	end

	if track.setuser then
		sys.process.setuser(track.setuser)
	end

	local target = nil
	if c then
		if type(c.target) == "function" then
			target = c.target
		elseif type(c.target) == "table" then
			target = c.target.target
		end
	end

	if c and (c.index or type(target) == "function") then
		ctx.dispatched = c
		ctx.requested = ctx.requested or ctx.dispatched
	end

	if c and c.index then
		local tpl = require "luci.template"

		if util.copcall(tpl.render, "indexer", {}) then
			return true
		end
	end

	if type(target) == "function" then
		util.copcall(function()
			local oldenv = getfenv(target)
			local module = require(c.module)
			local env = setmetatable({}, {__index=

			function(tbl, key)
				return rawget(tbl, key) or module[key] or oldenv[key]
			end})

			setfenv(target, env)
		end)

		local ok, err
		if type(c.target) == "table" then
			ok, err = util.copcall(target, c.target, unpack(args))
		else
			ok, err = util.copcall(target, unpack(args))
		end
		if not ok then
			error500("Failed to execute " .. (type(c.target) == "function" and "function" or c.target.type or "unknown") ..
			         " dispatcher target for entry '/" .. table.concat(request, "/") .. "'.\n" ..
			         "The called action terminated with an exception:\n" .. tostring(err or "(unknown)"))
		end
	else
		local root = node()
		if not root or not root.target then
			error404("No root node was registered, this usually happens if no module was installed.\n" ..
			         "Install luci-mod-admin-full and retry. " ..
			         "If the module is already installed, try removing the /tmp/luci-indexcache file.")
		else
			error404("No page is registered at '/" .. table.concat(request, "/") .. "'.\n" ..
			         "If this url belongs to an extension, make sure it is properly installed.\n" ..
			         "If the extension was recently installed, try removing the /tmp/luci-indexcache file.")
		end
	end
end

function createindex()
	local controllers = { }
	local base = "%s/controller/" % util.libpath()
	local _, path

	for path in (fs.glob("%s*.lua" % base) or function() end) do
		controllers[#controllers+1] = path
	end

	for path in (fs.glob("%s*/*.lua" % base) or function() end) do
		controllers[#controllers+1] = path
	end

	if indexcache then
		local cachedate = fs.stat(indexcache, "mtime")
		if cachedate then
			local realdate = 0
			for _, obj in ipairs(controllers) do
				local omtime = fs.stat(obj, "mtime")
				realdate = (omtime and omtime > realdate) and omtime or realdate
			end

			if cachedate > realdate and sys.process.info("uid") == 0 then
				assert(
					sys.process.info("uid") == fs.stat(indexcache, "uid")
					and fs.stat(indexcache, "modestr") == "rw-------",
					"Fatal: Indexcache is not sane!"
				)

				index = loadfile(indexcache)()
				return index
			end
		end
	end

	index = {}

	for _, path in ipairs(controllers) do
		local modname = "luci.controller." .. path:sub(#base+1, #path-4):gsub("/", ".")
		local mod = require(modname)
		assert(mod ~= true,
		       "Invalid controller file found\n" ..
		       "The file '" .. path .. "' contains an invalid module line.\n" ..
		       "Please verify whether the module name is set to '" .. modname ..
		       "' - It must correspond to the file path!")

		local idx = mod.index
		assert(type(idx) == "function",
		       "Invalid controller file found\n" ..
		       "The file '" .. path .. "' contains no index() function.\n" ..
		       "Please make sure that the controller contains a valid " ..
		       "index function and verify the spelling!")

		index[modname] = idx
	end

	if indexcache then
		local f = nixio.open(indexcache, "w", 600)
		f:writeall(util.get_bytecode(index))
		f:close()
	end
end

-- Build the index before if it does not exist yet.
function createtree()
	if not index then
		createindex()
	end

	local ctx  = context
	local tree = {nodes={}, inreq=true}

	ctx.treecache = setmetatable({}, {__mode="v"})
	ctx.tree = tree

	local scope = setmetatable({}, {__index = luci.dispatcher})

	for k, v in pairs(index) do
		scope._NAME = k
		setfenv(v, scope)
		v()
	end

	return tree
end

function assign(path, clone, title, order)
	local obj  = node(unpack(path))
	obj.nodes  = nil
	obj.module = nil

	obj.title = title
	obj.order = order

	setmetatable(obj, {__index = _create_node(clone)})

	return obj
end

function entry(path, target, title, order)
	local c = node(unpack(path))

	c.target = target
	c.title  = title
	c.order  = order
	c.module = getfenv(2)._NAME

	return c
end

-- enabling the node.
function get(...)
	return _create_node({...})
end

function node(...)
	local c = _create_node({...})

	c.module = getfenv(2)._NAME
	c.auto = nil

	return c
end

function lookup(...)
	local i, path = nil, {}
	for i = 1, select('#', ...) do
		local name, arg = nil, tostring(select(i, ...))
		for name in arg:gmatch("[^/]+") do
			path[#path+1] = name
		end
	end

	for i = #path, 1, -1 do
		local node = context.treecache[table.concat(path, ".", 1, i)]
		if node and (i == #path or node.leaf) then
			return node, build_url(unpack(path))
		end
	end
end

function _create_node(path)
	if #path == 0 then
		return context.tree
	end

	local name = table.concat(path, ".")
	local c = context.treecache[name]

	if not c then
		local last = table.remove(path)
		local parent = _create_node(path)

		c = {nodes={}, auto=true, inreq=true}

		local _, n
		for _, n in ipairs(path) do
			if context.path[_] ~= n then
				c.inreq = false
				break
			end
		end

		c.inreq = c.inreq and (context.path[#path + 1] == last)

		parent.nodes[last] = c
		context.treecache[name] = c
	end

	return c
end

-- Subdispatchers --

function _find_eligible_node(root, prefix, deep, types, descend)
	local children = _ordered_children(root)

	if not root.leaf and deep ~= nil then
		local sub_path = { unpack(prefix) }

		if deep == false then
			deep = nil
		end

		local _, child
		for _, child in ipairs(children) do
			sub_path[#prefix+1] = child.name

			local res_path = _find_eligible_node(child.node, sub_path,
			                                     deep, types, true)

			if res_path then
				return res_path
			end
		end
	end

	if descend and
	   (not types or
	    (type(root.target) == "table" and
	     util.contains(types, root.target.type)))
	then
		return prefix
	end
end

function _find_node(recurse, types)
	local path = { unpack(context.path) }
	local name = table.concat(path, ".")
	local node = context.treecache[name]

	path = _find_eligible_node(node, path, recurse, types)

	if path then
		dispatch(path)
	else
		require "luci.template".render("empty_node_placeholder")
	end
end

function _firstchild()
	return _find_node(false, nil)
end

function firstchild()
	return { type = "firstchild", target = _firstchild }
end

function _firstnode()
	return _find_node(true, { "cbi", "form", "template", "arcombine" })
end

function firstnode()
	return { type = "firstnode", target = _firstnode }
end

function alias(...)
	local req = {...}
	return function(...)
		for _, r in ipairs({...}) do
			req[#req+1] = r
		end

		dispatch(req)
	end
end

function rewrite(n, ...)
	local req = {...}
	return function(...)
		local dispatched = util.clone(context.dispatched)

		for i=1,n do
			table.remove(dispatched, 1)
		end

		for i, r in ipairs(req) do
			table.insert(dispatched, i, r)
		end

		for _, r in ipairs({...}) do
			dispatched[#dispatched+1] = r
		end

		dispatch(dispatched)
	end
end


local function _call(self, ...)
	local func = getfenv()[self.name]
	assert(func ~= nil,
	       'Cannot resolve function "' .. self.name .. '". Is it misspelled or local?')

	assert(type(func) == "function",
	       'The symbol "' .. self.name .. '" does not refer to a function but data ' ..
	       'of type "' .. type(func) .. '".')

	if #self.argv > 0 then
		return func(unpack(self.argv), ...)
	else
		return func(...)
	end
end

function call(name, ...)
	return {type = "call", argv = {...}, name = name, target = _call}
end

function post_on(params, name, ...)
	return {
		type = "call",
		post = params,
		argv = { ... },
		name = name,
		target = _call
	}
end

function post(...)
	return post_on(true, ...)
end


local _template = function(self, ...)
	require "luci.template".render(self.view)
end

function template(name)
	return {type = "template", view = name, target = _template}
end


local _view = function(self, ...)
	require "luci.template".render("view", { view = self.view })
end

function view(name)
	return {type = "view", view = name, target = _view}
end


local function _cbi(self, ...)
	local cbi = require "luci.cbi"
	local tpl = require "luci.template"
	local http = require "luci.http"

	local config = self.config or {}
	local maps = cbi.load(self.model, ...)

	local state = nil

	local i, res
	for i, res in ipairs(maps) do
		if util.instanceof(res, cbi.SimpleForm) then
			io.stderr:write("Model %s returns SimpleForm but is dispatched via cbi(),\n"
				% self.model)

			io.stderr:write("please change %s to use the form() action instead.\n"
				% table.concat(context.request, "/"))
		end

		res.flow = config
		local cstate = res:parse()
		if cstate and (not state or cstate < state) then
			state = cstate
		end
	end

	local function _resolve_path(path)
		return type(path) == "table" and build_url(unpack(path)) or path
	end

	if config.on_valid_to and state and state > 0 and state < 2 then
		http.redirect(_resolve_path(config.on_valid_to))
		return
	end

	if config.on_changed_to and state and state > 1 then
		http.redirect(_resolve_path(config.on_changed_to))
		return
	end

	if config.on_success_to and state and state > 0 then
		http.redirect(_resolve_path(config.on_success_to))
		return
	end

	if config.state_handler then
		if not config.state_handler(state, maps) then
			return
		end
	end

	http.header("X-CBI-State", state or 0)

	if not config.noheader then
		tpl.render("cbi/header", {state = state})
	end

	local redirect
	local messages
	local applymap   = false
	local pageaction = true
	local parsechain = { }

	for i, res in ipairs(maps) do
		if res.apply_needed and res.parsechain then
			local c
			for _, c in ipairs(res.parsechain) do
				parsechain[#parsechain+1] = c
			end
			applymap = true
		end

		if res.redirect then
			redirect = redirect or res.redirect
		end

		if res.pageaction == false then
			pageaction = false
		end

		if res.message then
			messages = messages or { }
			messages[#messages+1] = res.message
		end
	end

	for i, res in ipairs(maps) do
		res:render({
			firstmap   = (i == 1),
			redirect   = redirect,
			messages   = messages,
			pageaction = pageaction,
			parsechain = parsechain
		})
	end

	if not config.nofooter then
		tpl.render("cbi/footer", {
			flow          = config,
			pageaction    = pageaction,
			redirect      = redirect,
			state         = state,
			autoapply     = config.autoapply,
			trigger_apply = applymap
		})
	end
end

function cbi(model, config)
	return {
		type = "cbi",
		post = { ["cbi.submit"] = true },
		config = config,
		model = model,
		target = _cbi
	}
end


local function _arcombine(self, ...)
	local argv = {...}
	local target = #argv > 0 and self.targets[2] or self.targets[1]
	setfenv(target.target, self.env)
	target:target(unpack(argv))
end

function arcombine(trg1, trg2)
	return {type = "arcombine", env = getfenv(), target = _arcombine, targets = {trg1, trg2}}
end


local function _form(self, ...)
	local cbi = require "luci.cbi"
	local tpl = require "luci.template"
	local http = require "luci.http"

	local maps = luci.cbi.load(self.model, ...)
	local state = nil

	local i, res
	for i, res in ipairs(maps) do
		local cstate = res:parse()
		if cstate and (not state or cstate < state) then
			state = cstate
		end
	end

	http.header("X-CBI-State", state or 0)
	tpl.render("header")
	for i, res in ipairs(maps) do
		res:render()
	end
	tpl.render("footer")
end

function form(model)
	return {
		type = "cbi",
		post = { ["cbi.submit"] = true },
		model = model,
		target = _form
	}
end

translate = i18n.translate

-- This function does not actually translate the given argument but
-- is used by build/i18n-scan.pl to find translatable entries.
function _(text)
	return text
end
