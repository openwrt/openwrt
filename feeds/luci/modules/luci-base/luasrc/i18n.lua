-- Copyright 2008 Steven Barth <steven@midlink.org>
-- Licensed to the public under the Apache License 2.0.

local tparser  = require "luci.template.parser"
local util     = require "luci.util"
local tostring = tostring

module "luci.i18n"

i18ndir = util.libpath() .. "/i18n/"
context = util.threadlocal()
default = "en"


function setlanguage(lang)
	local code, subcode = lang:match("^([A-Za-z][A-Za-z])[%-_]([A-Za-z][A-Za-z])$")
	if not (code and subcode) then
		subcode = lang:match("^([A-Za-z][A-Za-z])$")
		if not subcode then
			return nil
		end
	end

	context.parent = code and code:lower()
	context.lang   = context.parent and context.parent.."-"..subcode:lower() or subcode:lower()

	if tparser.load_catalog(context.lang, i18ndir) and
	   tparser.change_catalog(context.lang)
	then
		return context.lang

	elseif context.parent then
		if tparser.load_catalog(context.parent, i18ndir) and
		   tparser.change_catalog(context.parent)
		then
			return context.parent
		end
	end

	return nil
end

function translate(key)
	return tparser.translate(key) or key
end

function translatef(key, ...)
	return tostring(translate(key)):format(...)
end

function dump()
	local rv = {}
	tparser.get_translations(function(k, v) rv[k] = v end)
	return rv
end
