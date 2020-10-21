-- Copyright 2008 Steven Barth <steven@midlink.org>
-- Copyright 2008-2015 Jo-Philipp Wich <jow@openwrt.org>
-- Licensed to the public under the Apache License 2.0.

local fs = require "nixio.fs"
local sys = require "luci.sys"
local util = require "luci.util"
local xml = require "luci.xml"
local http = require "luci.http"
local nixio = require "nixio", require "nixio.util"

module("luci.dispatcher", package.seeall)
context = util.threadlocal()
uci = require "luci.model.uci"
i18n = require "luci.i18n"
_M.fs = fs

-- Index table
local index = nil

function fork_exec(command)
	local pid = nixio.fork()
	if pid > 0 then
		return
	elseif pid == 0 then
		-- change to root dir
		nixio.chdir("/")

		-- patch stdin, out, err to /dev/null
		local null = nixio.open("/dev/null", "w+")
		if null then
			nixio.dup(null, nixio.stderr)
			nixio.dup(null, nixio.stdout)
			nixio.dup(null, nixio.stdin)
			if null:fileno() > 2 then
				null:close()
			end
		end

		-- replace with target command
		nixio.exec("/bin/sh", "-c", command)
	end
end

local function check_fs_depends(spec)
	local fs = require "nixio.fs"

	for path, kind in pairs(spec) do
		if kind == "directory" then
			local empty = true
			for entry in (fs.dir(path) or function() end) do
				empty = false
				break
			end
			if empty then
				return false
			end
		elseif kind == "executable" then
			if fs.stat(path, "type") ~= "reg" or not fs.access(path, "x") then
				return false
			end
		elseif kind == "file" then
			if fs.stat(path, "type") ~= "reg" then
				return false
			end
		end
	end

	return true
end

local function check_uci_depends_options(conf, s, opts)
	local uci = require "luci.model.uci"

	if type(opts) == "string" then
		return (s[".type"] == opts)
	elseif opts == true then
		for option, value in pairs(s) do
			if option:byte(1) ~= 46 then
				return true
			end
		end
	elseif type(opts) == "table" then
		for option, value in pairs(opts) do
			local sval = s[option]
			if type(sval) == "table" then
				local found = false
				for _, v in ipairs(sval) do
					if v == value then
						found = true
						break
					end
				end
				if not found then
					return false
				end
			elseif value == true then
				if sval == nil then
					return false
				end
			else
				if sval ~= value then
					return false
				end
			end
		end
	end

	return true
end

local function check_uci_depends_section(conf, sect)
	local uci = require "luci.model.uci"

	for section, options in pairs(sect) do
		local stype = section:match("^@([A-Za-z0-9_%-]+)$")
		if stype then
			local found = false
			uci:foreach(conf, stype, function(s)
				if check_uci_depends_options(conf, s, options) then
					found = true
					return false
				end
			end)
			if not found then
				return false
			end
		else
			local s = uci:get_all(conf, section)
			if not s or not check_uci_depends_options(conf, s, options) then
				return false
			end
		end
	end

	return true
end

local function check_uci_depends(conf)
	local uci = require "luci.model.uci"

	for config, values in pairs(conf) do
		if values == true then
			local found = false
			uci:foreach(config, nil, function(s)
				found = true
				return false
			end)
			if not found then
				return false
			end
		elseif type(values) == "table" then
			if not check_uci_depends_section(config, values) then
				return false
			end
		end
	end

	return true
end

local function check_acl_depends(require_groups, groups)
	if type(require_groups) == "table" and #require_groups > 0 then
		local writable = false

		for _, group in ipairs(require_groups) do
			local read = false
			local write = false
			if type(groups) == "table" and type(groups[group]) == "table" then
				for _, perm in ipairs(groups[group]) do
					if perm == "read" then
						read = true
					elseif perm == "write" then
						write = true
					end
				end
			end
			if not read and not write then
				return nil
			elseif write then
				writable = true
			end
		end

		return writable
	end

	return true
end

local function check_depends(spec)
	if type(spec.depends) ~= "table" then
		return true
	end

	if type(spec.depends.fs) == "table" then
		local satisfied = false
		local alternatives = (#spec.depends.fs > 0) and spec.depends.fs or { spec.depends.fs }
		for _, alternative in ipairs(alternatives) do
			if check_fs_depends(alternative) then
				satisfied = true
				break
			end
		end
		if not satisfied then
			return false
		end
	end

	if type(spec.depends.uci) == "table" then
		local satisfied = false
		local alternatives = (#spec.depends.uci > 0) and spec.depends.uci or { spec.depends.uci }
		for _, alternative in ipairs(alternatives) do
			if check_uci_depends(alternative) then
				satisfied = true
				break
			end
		end
		if not satisfied then
			return false
		end
	end

	return true
end

local function target_to_json(target, module)
	local action

	if target.type == "call" then
		action = {
			["type"] = "call",
			["module"] = module,
			["function"] = target.name,
			["parameters"] = target.argv
		}
	elseif target.type == "view" then
		action = {
			["type"] = "view",
			["path"] = target.view
		}
	elseif target.type == "template" then
		action = {
			["type"] = "template",
			["path"] = target.view
		}
	elseif target.type == "cbi" then
		action = {
			["type"] = "cbi",
			["path"] = target.model,
			["config"] = target.config
		}
	elseif target.type == "form" then
		action = {
			["type"] = "form",
			["path"] = target.model
		}
	elseif target.type == "firstchild" then
		action = {
			["type"] = "firstchild"
		}
	elseif target.type == "firstnode" then
		action = {
			["type"] = "firstchild",
			["recurse"] = true
		}
	elseif target.type == "arcombine" then
		if type(target.targets) == "table" then
			action = {
				["type"] = "arcombine",
				["targets"] = {
					target_to_json(target.targets[1], module),
					target_to_json(target.targets[2], module)
				}
			}
		end
	elseif target.type == "alias" then
		action = {
			["type"] = "alias",
			["path"] = table.concat(target.req, "/")
		}
	elseif target.type == "rewrite" then
		action = {
			["type"] = "rewrite",
			["path"] = table.concat(target.req, "/"),
			["remove"] = target.n
		}
	end

	if target.post and action then
		action.post = target.post
	end

	return action
end

local function tree_to_json(node, json)
	local fs = require "nixio.fs"
	local util = require "luci.util"

	if type(node.nodes) == "table" then
		for subname, subnode in pairs(node.nodes) do
			local spec = {
				title = xml.striptags(subnode.title),
				order = subnode.order
			}

			if subnode.leaf then
				spec.wildcard = true
			end

			if subnode.cors then
				spec.cors = true
			end

			if subnode.setuser then
				spec.setuser = subnode.setuser
			end

			if subnode.setgroup then
				spec.setgroup = subnode.setgroup
			end

			if type(subnode.target) == "table" then
				spec.action = target_to_json(subnode.target, subnode.module)
			end

			if type(subnode.file_depends) == "table" then
				for _, v in ipairs(subnode.file_depends) do
					spec.depends = spec.depends or {}
					spec.depends.fs = spec.depends.fs or {}

					local ft = fs.stat(v, "type")
					if ft == "dir" then
						spec.depends.fs[v] = "directory"
					elseif v:match("/s?bin/") then
						spec.depends.fs[v] = "executable"
					else
						spec.depends.fs[v] = "file"
					end
				end
			end

			if type(subnode.uci_depends) == "table" then
				for k, v in pairs(subnode.uci_depends) do
					spec.depends = spec.depends or {}
					spec.depends.uci = spec.depends.uci or {}
					spec.depends.uci[k] = v
				end
			end

			if type(subnode.acl_depends) == "table" then
				for _, acl in ipairs(subnode.acl_depends) do
					spec.depends = spec.depends or {}
					spec.depends.acl = spec.depends.acl or {}
					spec.depends.acl[#spec.depends.acl + 1] = acl
				end
			end

			if (subnode.sysauth_authenticator ~= nil) or
			   (subnode.sysauth ~= nil and subnode.sysauth ~= false)
			then
				if subnode.sysauth_authenticator == "htmlauth" then
					spec.auth = {
						login = true,
						methods = { "cookie:sysauth" }
					}
				elseif subname == "rpc" and subnode.module == "luci.controller.rpc" then
					spec.auth = {
						login = false,
						methods = { "query:auth", "cookie:sysauth" }
					}
				elseif subnode.module == "luci.controller.admin.uci" then
					spec.auth = {
						login = false,
						methods = { "param:sid" }
					}
				end
			elseif subnode.sysauth == false then
				spec.auth = {}
			end

			if not spec.action then
				spec.title = nil
			end

			spec.satisfied = check_depends(spec)
			json.children = json.children or {}
			json.children[subname] = tree_to_json(subnode, spec)
		end
	end

	return json
end

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


function error404(message)
	http.status(404, "Not Found")
	message = message or "Not Found"

	local function render()
		local template = require "luci.template"
		template.render("error404", {message=message})
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

local function determine_request_language()
	local conf = require "luci.config"
	assert(conf.main, "/etc/config/luci seems to be corrupt, unable to find section 'main'")

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
end

function httpdispatch(request, prefix)
	http.context.request = request

	local r = {}
	context.request = r

	local fs = require "nixio.fs"
	local default_path_info = fs.access("/etc/config/wizard") and "admin/initsetup" or ""

	local pathinfo = http.urldecode(request:getenv("PATH_INFO") or default_path_info, true)

	if prefix then
		for _, node in ipairs(prefix) do
			r[#r+1] = node
		end
	end

	local node
	for node in pathinfo:gmatch("[^/%z]+") do
		r[#r+1] = node
	end

	determine_request_language()

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
	local sacl = util.ubus("session", "access", { ubus_rpc_session = sid })

	if type(sdat) == "table" and
	   type(sdat.values) == "table" and
	   type(sdat.values.token) == "string" and
	   (not allowed_users or
	    util.contains(allowed_users, sdat.values.username))
	then
		uci:set_session_id(sid)
		return sid, sdat.values, type(sacl) == "table" and sacl or {}
	end

	return nil, nil, nil
end

local function session_setup(user, pass)
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
			%{ rp, user or "?", http.getenv("REMOTE_ADDR") or "?" })

		return session_retrieve(login.ubus_rpc_session)
	end

	io.stderr:write("luci: failed login on /%s for %s from %s\n"
		%{ rp, user or "?", http.getenv("REMOTE_ADDR") or "?" })
end

local function check_authentication(method)
	local auth_type, auth_param = method:match("^(%w+):(.+)$")
	local sid, sdat

	if auth_type == "cookie" then
		sid = http.getcookie(auth_param)
	elseif auth_type == "param" then
		sid = http.formvalue(auth_param)
	elseif auth_type == "query" then
		sid = http.formvalue(auth_param, true)
	end

	return session_retrieve(sid)
end

local function get_children(node)
	local children = {}

	if not node.wildcard and type(node.children) == "table" then
		for name, child in pairs(node.children) do
			children[#children+1] = {
				name  = name,
				node  = child,
				order = child.order or 1000
			}
		end

		table.sort(children, function(a, b)
			if a.order == b.order then
				return a.name < b.name
			else
				return a.order < b.order
			end
		end)
	end

	return children
end

local function find_subnode(root, prefix, recurse, descended)
	local children = get_children(root)

	if #children > 0 and (not descended or recurse) then
		local sub_path = { unpack(prefix) }

		if recurse == false then
			recurse = nil
		end

		for _, child in ipairs(children) do
			sub_path[#prefix+1] = child.name

			local res_path = find_subnode(child.node, sub_path, recurse, true)

			if res_path then
				return res_path
			end
		end
	end

	if descended then
		if not recurse or
		   root.action.type == "cbi" or
		   root.action.type == "form" or
		   root.action.type == "view" or
		   root.action.type == "template" or
		   root.action.type == "arcombine"
		then
			return prefix
		end
	end
end

local function merge_trees(node_a, node_b)
	for k, v in pairs(node_b) do
		if k == "children" then
			node_a.children = node_a.children or {}

			for name, spec in pairs(v) do
				node_a.children[name] = merge_trees(node_a.children[name] or {}, spec)
			end
		else
			node_a[k] = v
		end
	end

	if type(node_a.action) == "table" and
	   node_a.action.type == "firstchild" and
	   node_a.children == nil
	then
		node_a.satisfied = false
	end

	return node_a
end

local function apply_tree_acls(node, acl)
	if type(node.children) == "table" then
		for _, child in pairs(node.children) do
			apply_tree_acls(child, acl)
		end
	end

	local perm
	if type(node.depends) == "table" then
		perm = check_acl_depends(node.depends.acl, acl["access-group"])
	else
		perm = true
	end

	if perm == nil then
		node.satisfied = false
	elseif perm == false then
		node.readonly = true
	end
end

function menu_json(acl)
	local tree = context.tree or createtree()
	local lua_tree = tree_to_json(tree, {
		action = {
			["type"] = "firstchild",
			["recurse"] = true
		}
	})

	local json_tree = createtree_json()
	local menu_tree = merge_trees(lua_tree, json_tree)

	if acl then
		apply_tree_acls(menu_tree, acl)
	end

	return menu_tree
end

local function init_template_engine(ctx)
	local tpl = require "luci.template"
	local media = luci.config.main.mediaurlbase

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
				val = xml.pcdata(val)
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
		striptags   = xml.striptags;
		pcdata      = xml.pcdata;
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

	return tpl
end

function dispatch(request)
	--context._disable_memtrace = require "luci.debug".trap_memtrace("l")
	local ctx = context

	local auth, cors, suid, sgid
	local menu = menu_json()
	local page = menu

	local requested_path_full = {}
	local requested_path_node = {}
	local requested_path_args = {}

	local required_path_acls = {}

	for i, s in ipairs(request) do
		if type(page.children) ~= "table" or not page.children[s] then
			page = nil
			break
		end

		if not page.children[s].satisfied then
			page = nil
			break
		end

		page = page.children[s]
		auth = page.auth or auth
		cors = page.cors or cors
		suid = page.setuser or suid
		sgid = page.setgroup or sgid

		if type(page.depends) == "table" and type(page.depends.acl) == "table" then
			for _, group in ipairs(page.depends.acl) do
				local found = false
				for _, item in ipairs(required_path_acls) do
					if item == group then
						found = true
						break
					end
				end
				if not found then
					required_path_acls[#required_path_acls + 1] = group
				end
			end
		end

		requested_path_full[i] = s
		requested_path_node[i] = s

		if page.wildcard then
			for j = i + 1, #request do
				requested_path_args[j - i] = request[j]
				requested_path_full[j] = request[j]
			end
			break
		end
	end

	local tpl = init_template_engine(ctx)

	ctx.args = requested_path_args
	ctx.path = requested_path_node
	ctx.dispatched = page

	ctx.requestpath = ctx.requestpath or requested_path_full
	ctx.requestargs = ctx.requestargs or requested_path_args
	ctx.requested = ctx.requested or page

	if type(auth) == "table" and type(auth.methods) == "table" and #auth.methods > 0 then
		local sid, sdat, sacl
		for _, method in ipairs(auth.methods) do
			sid, sdat, sacl = check_authentication(method)

			if sid and sdat and sacl then
				break
			end
		end

		if fs.access("/tmp/factory_auth") then
			local mac = http.formvalue("luci_mac")
			local sn = http.formvalue("luci_sn")
			if mac and sn then
				local ok = os.execute("mt7628factory.sh set_mac_sn " .. mac .. " " .. sn .. " >/dev/null 2>&1") == 0
				if ok then
					os.execute("rm -f /tmp/factory_auth")
					tpl.render("applyreboot", {
						title = luci.i18n.translate("Reboot..."),
						msg   = luci.i18n.translate("Good, MAC/SN Verify OK, Please wait reboot"),
						addr  = nil
					})
					fork_exec("sleep 1; system_reset -y -r")
					return
				end
			end

			return tpl.render("factory_auth", {dmac = mac, dsn = sn, fmac = mac})
		end

		if not (sid and sdat and sacl) and auth.login then
			local user = http.getenv("HTTP_AUTH_USER")
			local pass = http.getenv("HTTP_AUTH_PASS")

			if user == nil and pass == nil then
				user = http.formvalue("luci_username")
				pass = http.formvalue("luci_password")
			end

			if user and pass then
				sid, sdat, sacl = session_setup(user, pass)
			end

			if not sid then
				context.path = {}

				http.status(403, "Forbidden")
				http.header("X-LuCI-Login-Required", "yes")

				local scope = { duser = "root", fuser = user }
				local ok, res = util.copcall(tpl.render_string, [[<% include("themes/" .. theme .. "/sysauth") %>]], scope)
				if ok then
					return res
				end
				return tpl.render("sysauth", scope)
			end

			http.header("Set-Cookie", 'sysauth=%s; path=%s; SameSite=Strict; HttpOnly%s' %{
				sid, build_url(), http.getenv("HTTPS") == "on" and "; secure" or ""
			})

			local lang = http.formvalue("luci_lang")
			if lang then
				local uci = (require "luci.model.uci").cursor()
				local orig_lang = uci:get("luci", "main", "lang")
				if lang ~= orig_lang then
					uci:set("luci", "main", "lang", lang)
					uci:commit("luci")
				end
			end

			http.redirect(build_url(unpack(ctx.requestpath)))
			return
		end

		if not sid or not sdat or not sacl then
			http.status(403, "Forbidden")
			http.header("X-LuCI-Login-Required", "yes")
			return
		end

		ctx.authsession = sid
		ctx.authtoken = sdat.token
		ctx.authuser = sdat.username
		ctx.authacl = sacl
	end

	if #required_path_acls > 0 then
		local perm = check_acl_depends(required_path_acls, ctx.authacl and ctx.authacl["access-group"])
		if perm == nil then
			http.status(403, "Forbidden")
			return
		end

		page.readonly = not perm
	end

	local action = (page and type(page.action) == "table") and page.action or {}

	if action.type == "arcombine" then
		action = (#requested_path_args > 0) and action.targets[2] or action.targets[1]
	end

	if cors and http.getenv("REQUEST_METHOD") == "OPTIONS" then
		luci.http.status(200, "OK")
		luci.http.header("Access-Control-Allow-Origin", http.getenv("HTTP_ORIGIN") or "*")
		luci.http.header("Access-Control-Allow-Methods", "GET, POST, OPTIONS")
		return
	end

	if require_post_security(action) then
		if not test_post_security() then
			return
		end
	end

	if sgid then
		sys.process.setgroup(sgid)
	end

	if suid then
		sys.process.setuser(suid)
	end

	if action.type == "view" then
		tpl.render("view", { view = action.path })

	elseif action.type == "call" then
		local ok, mod = util.copcall(require, action.module)
		if not ok then
			error500(mod)
			return
		end

		local func = mod[action["function"]]

		assert(func ~= nil,
		       'Cannot resolve function "' .. action["function"] .. '". Is it misspelled or local?')

		assert(type(func) == "function",
		       'The symbol "' .. action["function"] .. '" does not refer to a function but data ' ..
		       'of type "' .. type(func) .. '".')

		local argv = (type(action.parameters) == "table" and #action.parameters > 0) and { unpack(action.parameters) } or {}
		for _, s in ipairs(requested_path_args) do
			argv[#argv + 1] = s
		end

		local ok, err = util.copcall(func, unpack(argv))
		if not ok then
			error500(err)
		end

	elseif action.type == "firstchild" then
		local sub_request = find_subnode(page, requested_path_full, action.recurse)
		if sub_request then
			dispatch(sub_request)
		else
			tpl.render("empty_node_placeholder", getfenv(1))
		end

	elseif action.type == "alias" then
		local sub_request = {}
		for name in action.path:gmatch("[^/]+") do
			sub_request[#sub_request + 1] = name
		end

		for _, s in ipairs(requested_path_args) do
			sub_request[#sub_request + 1] = s
		end

		dispatch(sub_request)

	elseif action.type == "rewrite" then
		local sub_request = { unpack(request) }
		for i = 1, action.remove do
			table.remove(sub_request, 1)
		end

		local n = 1
		for s in action.path:gmatch("[^/]+") do
			table.insert(sub_request, n, s)
			n = n + 1
		end

		for _, s in ipairs(requested_path_args) do
			sub_request[#sub_request + 1] = s
		end

		dispatch(sub_request)

	elseif action.type == "template" then
		tpl.render(action.path, getfenv(1))

	elseif action.type == "cbi" then
		_cbi({ config = action.config, model = action.path }, unpack(requested_path_args))

	elseif action.type == "form" then
		_form({ model = action.path }, unpack(requested_path_args))

	else
		local root = find_subnode(menu, {}, true)
		if not root then
			error404("No root node was registered, this usually happens if no module was installed.\n" ..
			         "Install luci-mod-admin-full and retry. " ..
			         "If the module is already installed, try removing the /tmp/luci-indexcache file.")
		else
			error404("No page is registered at '/" .. table.concat(requested_path_full, "/") .. "'.\n" ..
			         "If this url belongs to an extension, make sure it is properly installed.\n" ..
			         "If the extension was recently installed, try removing the /tmp/luci-indexcache file.")
		end
	end
end

local function hash_filelist(files)
	local fprint = {}
	local n = 0

	for i, file in ipairs(files) do
		local st = fs.stat(file)
		if st then
			fprint[n + 1] = '%x' % st.ino
			fprint[n + 2] = '%x' % st.mtime
			fprint[n + 3] = '%x' % st.size
			n = n + 3
		end
	end

	return nixio.crypt(table.concat(fprint, "|"), "$1$"):sub(5):gsub("/", ".")
end

local function read_cachefile(file, reader)
	local euid = sys.process.info("uid")
	local fuid = fs.stat(file, "uid")
	local mode = fs.stat(file, "modestr")

	if euid ~= fuid or mode ~= "rw-------" then
		return nil
	end

	return reader(file)
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

	local cachefile

	if indexcache then
		cachefile = "%s.%s.lua" %{ indexcache, hash_filelist(controllers) }

		local res = read_cachefile(cachefile, function(path) return loadfile(path)() end)
		if res then
			index = res
			return res
		end

		for file in (fs.glob("%s.*.lua" % indexcache) or function() end) do
			fs.unlink(file)
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
		if type(idx) == "function" then
			index[modname] = idx
		end
	end

	if cachefile then
		local f = nixio.open(cachefile, "w", 600)
		f:writeall(util.get_bytecode(index))
		f:close()
	end
end

function createtree_json()
	local json = require "luci.jsonc"
	local tree = {}

	local schema = {
		action = "table",
		auth = "table",
		cors = "boolean",
		depends = "table",
		order = "number",
		setgroup = "string",
		setuser = "string",
		title = "string",
		wildcard = "boolean"
	}

	local files = {}
	local cachefile

	for file in (fs.glob("/usr/share/luci/menu.d/*.json") or function() end) do
		files[#files+1] = file
	end

	if indexcache then
		cachefile = "%s.%s.json" %{ indexcache, hash_filelist(files) }

		local res = read_cachefile(cachefile, function(path) return json.parse(fs.readfile(path) or "") end)
		if res then
			return res
		end

		for file in (fs.glob("%s.*.json" % indexcache) or function() end) do
			fs.unlink(file)
		end
	end

	for _, file in ipairs(files) do
		local data = json.parse(fs.readfile(file) or "")
		if type(data) == "table" then
			for path, spec in pairs(data) do
				if type(spec) == "table" then
					local node = tree

					for s in path:gmatch("[^/]+") do
						if s == "*" then
							node.wildcard = true
							break
						end

						node.children = node.children or {}
						node.children[s] = node.children[s] or {}
						node = node.children[s]
					end

					if node ~= tree then
						for k, t in pairs(schema) do
							if type(spec[k]) == t then
								node[k] = spec[k]
							end
						end

						node.satisfied = check_depends(spec)
					end
				end
			end
		end
	end

	if cachefile then
		local f = nixio.open(cachefile, "w", 600)
		f:writeall(json.stringify(tree))
		f:close()
	end

	return tree
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

		parent.nodes[last] = c
		context.treecache[name] = c
	end

	return c
end

-- Subdispatchers --

function firstchild()
	return { type = "firstchild" }
end

function firstnode()
	return { type = "firstnode" }
end

function alias(...)
	return { type = "alias", req = { ... } }
end

function rewrite(n, ...)
	return { type = "rewrite", n = n, req = { ... } }
end

function call(name, ...)
	return { type = "call", argv = {...}, name = name }
end

function post_on(params, name, ...)
	return {
		type = "call",
		post = params,
		argv = { ... },
		name = name
	}
end

function post(...)
	return post_on(true, ...)
end


function template(name)
	return { type = "template", view = name }
end

function view(name)
	return { type = "view", view = name }
end


function _cbi(self, ...)
	local cbi = require "luci.cbi"
	local tpl = require "luci.template"
	local http = require "luci.http"
	local util = require "luci.util"

	local config = self.config or {}
	local maps = cbi.load(self.model, ...)

	local state = nil

	local function has_uci_access(config, level)
		local rv = util.ubus("session", "access", {
			ubus_rpc_session = context.authsession,
			scope = "uci", object = config,
			["function"] = level
		})

		return (type(rv) == "table" and rv.access == true) or false
	end

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
	local writable   = false

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
		local is_readable_map = has_uci_access(res.config, "read")
		local is_writable_map = has_uci_access(res.config, "write")

		writable = writable or is_writable_map

		res:render({
			firstmap   = (i == 1),
			redirect   = redirect,
			messages   = messages,
			pageaction = pageaction,
			parsechain = parsechain,
			readable   = is_readable_map,
			writable   = is_writable_map
		})
	end

	if not config.nofooter then
		tpl.render("cbi/footer", {
			flow          = config,
			pageaction    = pageaction,
			redirect      = redirect,
			state         = state,
			autoapply     = config.autoapply,
			trigger_apply = applymap,
			writable      = writable
		})
	end
end

function cbi(model, config)
	return {
		type = "cbi",
		post = { ["cbi.submit"] = true },
		config = config,
		model = model
	}
end


function arcombine(trg1, trg2)
	return {
		type = "arcombine",
		env = getfenv(),
		targets = {trg1, trg2}
	}
end


function _form(self, ...)
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
		type = "form",
		post = { ["cbi.submit"] = true },
		model = model
	}
end

translate = i18n.translate

-- This function does not actually translate the given argument but
-- is used by build/i18n-scan.pl to find translatable entries.
function _(text)
	return text
end
