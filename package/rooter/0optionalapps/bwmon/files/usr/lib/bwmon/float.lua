#!/usr/bin/lua

val = arg[1]

retval = val * 1000

local tfile = io.open("/tmp/float", "w")
tfile:write("SPEED=\"", retval, "\"")
tfile:close()