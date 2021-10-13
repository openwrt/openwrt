-- SPDX-License-Identifier: GPL-2.0-only
-- Copyright (c) Facebook, Inc. and its affiliates.

module("luci.controller.fbwifi", package.seeall)

sys = require "luci.sys"
ut = require "luci.util"

function index()
    entry({"admin", "network", "fbwifi"}, template("fbwifi"), "Facebook Wi-Fi", 90).dependent=false
end

