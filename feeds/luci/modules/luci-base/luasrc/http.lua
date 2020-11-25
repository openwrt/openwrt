-- Copyright 2008 Steven Barth <steven@midlink.org>
-- Copyright 2010-2018 Jo-Philipp Wich <jo@mein.io>
-- Licensed to the public under the Apache License 2.0.

local util  = require "luci.util"
local coroutine = require "coroutine"
local table = require "table"
local lhttp = require "lucihttp"
local nixio = require "nixio"
local ltn12 = require "luci.ltn12"

local table, ipairs, pairs, type, tostring, tonumber, error =
	table, ipairs, pairs, type, tostring, tonumber, error

module "luci.http"

HTTP_MAX_CONTENT      = 1024*100		-- 100 kB maximum content size

context = util.threadlocal()

Request = util.class()
function Request.__init__(self, env, sourcein, sinkerr)
	self.input = sourcein
	self.error = sinkerr


	-- File handler nil by default to let .content() work
	self.filehandler = nil

	-- HTTP-Message table
	self.message = {
		env = env,
		headers = {},
		params = urldecode_params(env.QUERY_STRING or ""),
	}

	self.parsed_input = false
end

function Request.formvalue(self, name, noparse)
	if not noparse and not self.parsed_input then
		self:_parse_input()
	end

	if name then
		return self.message.params[name]
	else
		return self.message.params
	end
end

function Request.formvaluetable(self, prefix)
	local vals = {}
	prefix = prefix and prefix .. "." or "."

	if not self.parsed_input then
		self:_parse_input()
	end

	local void = self.message.params[nil]
	for k, v in pairs(self.message.params) do
		if k:find(prefix, 1, true) == 1 then
			vals[k:sub(#prefix + 1)] = tostring(v)
		end
	end

	return vals
end

function Request.content(self)
	if not self.parsed_input then
		self:_parse_input()
	end

	return self.message.content, self.message.content_length
end

function Request.getcookie(self, name)
	return lhttp.header_attribute("cookie; " .. (self:getenv("HTTP_COOKIE") or ""), name)
end

function Request.getenv(self, name)
	if name then
		return self.message.env[name]
	else
		return self.message.env
	end
end

function Request.setfilehandler(self, callback)
	self.filehandler = callback

	if not self.parsed_input then
		return
	end

	-- If input has already been parsed then uploads are stored as unlinked
	-- temporary files pointed to by open file handles in the parameter
	-- value table. Loop all params, and invoke the file callback for any
	-- param with an open file handle.
	local name, value
	for name, value in pairs(self.message.params) do
		if type(value) == "table" then
			while value.fd do
				local data = value.fd:read(1024)
				local eof = (not data or data == "")

				callback(value, data, eof)

				if eof then
					value.fd:close()
					value.fd = nil
				end
			end
		end
	end
end

function Request._parse_input(self)
	parse_message_body(
		 self.input,
		 self.message,
		 self.filehandler
	)
	self.parsed_input = true
end

function close()
	if not context.eoh then
		context.eoh = true
		coroutine.yield(3)
	end

	if not context.closed then
		context.closed = true
		coroutine.yield(5)
	end
end

function content()
	return context.request:content()
end

function formvalue(name, noparse)
	return context.request:formvalue(name, noparse)
end

function formvaluetable(prefix)
	return context.request:formvaluetable(prefix)
end

function getcookie(name)
	return context.request:getcookie(name)
end

-- or the environment table itself.
function getenv(name)
	return context.request:getenv(name)
end

function setfilehandler(callback)
	return context.request:setfilehandler(callback)
end

function header(key, value)
	if not context.headers then
		context.headers = {}
	end
	context.headers[key:lower()] = value
	coroutine.yield(2, key, value)
end

function prepare_content(mime)
	if not context.headers or not context.headers["content-type"] then
		if mime == "application/xhtml+xml" then
			if not getenv("HTTP_ACCEPT") or
			  not getenv("HTTP_ACCEPT"):find("application/xhtml+xml", nil, true) then
				mime = "text/html; charset=UTF-8"
			end
			header("Vary", "Accept")
		end
		header("Content-Type", mime)
	end
end

function source()
	return context.request.input
end

function status(code, message)
	code = code or 200
	message = message or "OK"
	context.status = code
	coroutine.yield(1, code, message)
end

-- This function is as a valid LTN12 sink.
-- If the content chunk is nil this function will automatically invoke close.
function write(content, src_err)
	if not content then
		if src_err then
			error(src_err)
		else
			close()
		end
		return true
	elseif #content == 0 then
		return true
	else
		if not context.eoh then
			if not context.status then
				status()
			end
			if not context.headers or not context.headers["content-type"] then
				header("Content-Type", "text/html; charset=utf-8")
			end
			if not context.headers["cache-control"] then
				header("Cache-Control", "no-cache")
				header("Expires", "0")
			end
			if not context.headers["x-frame-options"] then
				header("X-Frame-Options", "SAMEORIGIN")
			end
			if not context.headers["x-xss-protection"] then
				header("X-XSS-Protection", "1; mode=block")
			end
			if not context.headers["x-content-type-options"] then
				header("X-Content-Type-Options", "nosniff")
			end

			context.eoh = true
			coroutine.yield(3)
		end
		coroutine.yield(4, content)
		return true
	end
end

function splice(fd, size)
	coroutine.yield(6, fd, size)
end

function redirect(url)
	if url == "" then url = "/" end
	status(302, "Found")
	header("Location", url)
	close()
end

function build_querystring(q)
	local s, n, k, v = {}, 1, nil, nil

	for k, v in pairs(q) do
		s[n+0] = (n == 1) and "?" or "&"
		s[n+1] = util.urlencode(k)
		s[n+2] = "="
		s[n+3] = util.urlencode(v)
		n = n + 4
	end

	return table.concat(s, "")
end

urldecode = util.urldecode

urlencode = util.urlencode

function write_json(x)
	util.serialize_json(x, write)
end

-- from given url or string. Returns a table with urldecoded values.
-- Simple parameters are stored as string values associated with the parameter
-- name within the table. Parameters with multiple values are stored as array
-- containing the corresponding values.
function urldecode_params(url, tbl)
	local parser, name
	local params = tbl or { }

	parser = lhttp.urlencoded_parser(function (what, buffer, length)
		if what == parser.TUPLE then
			name, value = nil, nil
		elseif what == parser.NAME then
			name = lhttp.urldecode(buffer)
		elseif what == parser.VALUE and name then
			params[name] = lhttp.urldecode(buffer) or ""
		end

		return true
	end)

	if parser then
		parser:parse((url or ""):match("[^?]*$"))
		parser:parse(nil)
	end

	return params
end

-- separated by "&". Tables are encoded as parameters with multiple values by
-- repeating the parameter name with each value.
function urlencode_params(tbl)
	local k, v
	local n, enc = 1, {}
	for k, v in pairs(tbl) do
		if type(v) == "table" then
			local i, v2
			for i, v2 in ipairs(v) do
				if enc[1] then
					enc[n] = "&"
					n = n + 1
				end

				enc[n+0] = lhttp.urlencode(k)
				enc[n+1] = "="
				enc[n+2] = lhttp.urlencode(v2)
				n = n + 3
			end
		else
			if enc[1] then
				enc[n] = "&"
				n = n + 1
			end

			enc[n+0] = lhttp.urlencode(k)
			enc[n+1] = "="
			enc[n+2] = lhttp.urlencode(v)
			n = n + 3
		end
	end

	return table.concat(enc, "")
end

-- Content-Type. Stores all extracted data associated with its parameter name
-- in the params table within the given message object. Multiple parameter
-- values are stored as tables, ordinary ones as strings.
-- If an optional file callback function is given then it is fed with the
-- file contents chunk by chunk and only the extracted file name is stored
-- within the params table. The callback function will be called subsequently
-- with three arguments:
--  o Table containing decoded (name, file) and raw (headers) mime header data
--  o String value containing a chunk of the file data
--  o Boolean which indicates whether the current chunk is the last one (eof)
function mimedecode_message_body(src, msg, file_cb)
	local parser, header, field
	local len, maxlen = 0, tonumber(msg.env.CONTENT_LENGTH or nil)

	parser, err = lhttp.multipart_parser(msg.env.CONTENT_TYPE, function (what, buffer, length)
		if what == parser.PART_INIT then
			field = { }

		elseif what == parser.HEADER_NAME then
			header = buffer:lower()

		elseif what == parser.HEADER_VALUE and header then
			if header:lower() == "content-disposition" and
			   lhttp.header_attribute(buffer, nil) == "form-data"
			then
				field.name = lhttp.header_attribute(buffer, "name")
				field.file = lhttp.header_attribute(buffer, "filename")
				field[1] = field.file
			end

			if field.headers then
				field.headers[header] = buffer
			else
				field.headers = { [header] = buffer }
			end

		elseif what == parser.PART_BEGIN then
			return not field.file

		elseif what == parser.PART_DATA and field.name and length > 0 then
			if field.file then
				if file_cb then
					file_cb(field, buffer, false)
					msg.params[field.name] = msg.params[field.name] or field
				else
					if not field.fd then
						field.fd = nixio.mkstemp(field.name)
					end

					if field.fd then
						field.fd:write(buffer)
						msg.params[field.name] = msg.params[field.name] or field
					end
				end
			else
				field.value = buffer
			end

		elseif what == parser.PART_END and field.name then
			if field.file and msg.params[field.name] then
				if file_cb then
					file_cb(field, "", true)
				elseif field.fd then
					field.fd:seek(0, "set")
				end
			else
				local val = msg.params[field.name]

				if type(val) == "table" then
					val[#val+1] = field.value or ""
				elseif val ~= nil then
					msg.params[field.name] = { val, field.value or "" }
				else
					msg.params[field.name] = field.value or ""
				end
			end

			field = nil

		elseif what == parser.ERROR then
			err = buffer
		end

		return true
	end, HTTP_MAX_CONTENT)

	return ltn12.pump.all(src, function (chunk)
		len = len + (chunk and #chunk or 0)

		if maxlen and len > maxlen + 2 then
			return nil, "Message body size exceeds Content-Length"
		end

		if not parser or not parser:parse(chunk) then
			return nil, err
		end

		return true
	end)
end

-- Content-Type. Stores all extracted data associated with its parameter name
-- in the params table within the given message object. Multiple parameter
-- values are stored as tables, ordinary ones as strings.
function urldecode_message_body(src, msg)
	local err, name, value, parser
	local len, maxlen = 0, tonumber(msg.env.CONTENT_LENGTH or nil)

	parser = lhttp.urlencoded_parser(function (what, buffer, length)
		if what == parser.TUPLE then
			name, value = nil, nil
		elseif what == parser.NAME then
			name = lhttp.urldecode(buffer, lhttp.DECODE_PLUS)
		elseif what == parser.VALUE and name then
			local val = msg.params[name]

			if type(val) == "table" then
				val[#val+1] = lhttp.urldecode(buffer, lhttp.DECODE_PLUS) or ""
			elseif val ~= nil then
				msg.params[name] = { val, lhttp.urldecode(buffer, lhttp.DECODE_PLUS) or "" }
			else
				msg.params[name] = lhttp.urldecode(buffer, lhttp.DECODE_PLUS) or ""
			end
		elseif what == parser.ERROR then
			err = buffer
		end

		return true
	end, HTTP_MAX_CONTENT)

	return ltn12.pump.all(src, function (chunk)
		len = len + (chunk and #chunk or 0)

		if maxlen and len > maxlen + 2 then
			return nil, "Message body size exceeds Content-Length"
		elseif len > HTTP_MAX_CONTENT then
			return nil, "Message body size exceeds maximum allowed length"
		end

		if not parser or not parser:parse(chunk) then
			return nil, err
		end

		return true
	end)
end

-- This function will examine the Content-Type within the given message object
-- to select the appropriate content decoder.
-- Currently the application/x-www-urlencoded and application/form-data
-- mime types are supported. If the encountered content encoding can't be
-- handled then the whole message body will be stored unaltered as "content"
-- property within the given message object.
function parse_message_body(src, msg, filecb)
	if msg.env.CONTENT_LENGTH or msg.env.REQUEST_METHOD == "POST" then
		local ctype = lhttp.header_attribute(msg.env.CONTENT_TYPE, nil)

		-- Is it multipart/mime ?
		if ctype == "multipart/form-data" then
			return mimedecode_message_body(src, msg, filecb)

		-- Is it application/x-www-form-urlencoded ?
		elseif ctype == "application/x-www-form-urlencoded" then
			return urldecode_message_body(src, msg)

		end

		-- Unhandled encoding
		-- If a file callback is given then feed it chunk by chunk, else
		-- store whole buffer in message.content
		local sink

		-- If we have a file callback then feed it
		if type(filecb) == "function" then
			local meta = {
				name = "raw",
				encoding = msg.env.CONTENT_TYPE
			}
			sink = function( chunk )
				if chunk then
					return filecb(meta, chunk, false)
				else
					return filecb(meta, nil, true)
				end
			end
		-- ... else append to .content
		else
			msg.content = ""
			msg.content_length = 0

			sink = function( chunk )
				if chunk then
					if ( msg.content_length + #chunk ) <= HTTP_MAX_CONTENT then
						msg.content        = msg.content        .. chunk
						msg.content_length = msg.content_length + #chunk
						return true
					else
						return nil, "POST data exceeds maximum allowed length"
					end
				end
				return true
			end
		end

		-- Pump data...
		while true do
			local ok, err = ltn12.pump.step( src, sink )

			if not ok and err then
				return nil, err
			elseif not ok then -- eof
				return true
			end
		end

		return true
	end

	return false
end
