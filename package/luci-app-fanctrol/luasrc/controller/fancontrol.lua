module("luci.controller.fancontrol", package.seeall)

local CONFIG = "fancontrol"
local SECTION = "settings"
local UI_OWNER_FILE = "/var/run/fancontrol.ui-owner"
local TEMP_SOURCE_IDS = { "cpu", "phy", "nvme1", "nvme2" }
local TEMP_SOURCE_ALLOWED = {
    cpu = true,
    phy = true,
    nvme1 = true,
    nvme2 = true
}
local TEMP_POINTS = { 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 80, 85, 90, 95, 100, 105, 110 }
local PRESET_TARGETS = {
    quiet = { option = "curve_silent", mode = 0 },
    balanced = { option = "curve_balanced", mode = 1 },
    performance = { option = "curve_performance", mode = 2 }
}

function index()
    entry({"admin", "system", "fancontrol"}, call("action_fancontrol"), "风扇控制", 90).dependent = true
    entry({"admin", "system", "fancontrol_api"}, call("action_api"), nil).leaf = true
end

local function http()
    return require "luci.http"
end

local function uci_cursor()
    local uci_model = require "luci.model.uci"
    return uci_model.cursor()
end

local function fs()
    return require "nixio.fs"
end

local function write_json(data)
    local h = http()
    h.header("Cache-Control", "no-store, no-cache, must-revalidate")
    h.header("Pragma", "no-cache")
    h.prepare_content("application/json")
    h.write_json(data)
end

local function formvalue(name)
    return http().formvalue(name)
end

local function valid_ui_client(value)
    return type(value) == "string"
        and #value >= 16
        and #value <= 96
        and value:match("^[%w_-]+$") ~= nil
end

local function read_ui_owner()
    local data = fs().readfile(UI_OWNER_FILE)
    return data and data:match("^([%w_-]+)") or nil
end

local function claim_ui(client)
    if not valid_ui_client(client) then
        write_json({ result = "error", message = "invalid ui client" })
        return false
    end
    local nixio_fs = fs()
    local temp_file = UI_OWNER_FILE .. "." .. client
    if not nixio_fs.writefile(temp_file, client .. "\n") then
        write_json({ result = "error", message = "ui ownership write failed" })
        return false
    end
    -- nixio's numeric mode syntax uses decimal octal digits (600 => 0600).
    nixio_fs.chmod(temp_file, 600)
    if not nixio_fs.rename(temp_file, UI_OWNER_FILE) then
        nixio_fs.remove(temp_file)
        write_json({ result = "error", message = "ui ownership commit failed" })
        return false
    end
    write_json({ result = "ok", client = client })
    return true
end

local function ui_owner_matches()
    local client = formvalue("client")
    return valid_ui_client(client) and read_ui_owner() == client
end

local function require_ui_owner(uci)
    if not ui_owner_matches() then
        if uci then uci:revert(CONFIG) end
        write_json({ result = "taken_over" })
        return false
    end
    return true
end

local function uci_get(uci, name, default)
    return uci:get(CONFIG, SECTION, name) or default
end

local function uci_set_if_present(uci, name, value)
    if value ~= nil then
        return uci:set(CONFIG, SECTION, name, value)
    end
    return true
end

local function write_uci_error(uci, message)
    uci:revert(CONFIG)
    write_json({ result = "error", message = message or "uci write failed" })
end

local function clamp_number(value, min, max)
    value = tonumber(value)
    if not value then return nil end
    if value < min then return min end
    if value > max then return max end
    return value
end

local function normalize_curve(curve)
    local points = {}
    if curve then
        for temp, speed in tostring(curve):gmatch("(%d+)%s*:%s*(%d+)") do
            temp = clamp_number(temp, 0, 130)
            speed = clamp_number(speed, 0, 100)
            if temp and speed then
                points[#points + 1] = { temp = math.floor(temp), speed = math.floor(speed) }
            end
        end
    end

    table.sort(points, function(a, b) return a.temp < b.temp end)

    local deduped = {}
    local last_temp = nil
    for _, point in ipairs(points) do
        if point.temp ~= last_temp then
            deduped[#deduped + 1] = point
            last_temp = point.temp
        end
    end

    if #deduped == 0 then return "" end

    local function speed_at(temp)
        if temp <= deduped[1].temp then return deduped[1].speed end
        if temp >= deduped[#deduped].temp then return deduped[#deduped].speed end

        for i = 2, #deduped do
            if temp <= deduped[i].temp then
                local left = deduped[i - 1]
                local right = deduped[i]
                local ratio = (temp - left.temp) / (right.temp - left.temp)
                return math.floor(left.speed + (right.speed - left.speed) * ratio + 0.5)
            end
        end

        return deduped[#deduped].speed
    end

    local result = {}
    for _, temp in ipairs(TEMP_POINTS) do
        local speed = math.floor(clamp_number(speed_at(temp), 0, 100) or 0)
        result[#result + 1] = tostring(temp) .. ":" .. tostring(speed)
    end

    return table.concat(result, ",")
end

local function manual_curve(speed)
    speed = clamp_number(speed, 0, 100) or 0
    speed = math.floor(speed)

    local result = {}
    for _, temp in ipairs(TEMP_POINTS) do
        result[#result + 1] = tostring(temp) .. ":" .. tostring(speed)
    end
    return table.concat(result, ",")
end

local function normalize_temp_sources(value)
    if value == nil then return nil end
    if type(value) ~= "string" or not TEMP_SOURCE_ALLOWED[value] then
        return nil
    end
    return value
end

local function canonical_temp_source(value)
    if type(value) ~= "string" then return "cpu" end
    local seen = {}
    for token in value:gmatch("%S+") do
        if TEMP_SOURCE_ALLOWED[token] then seen[token] = true end
    end
    for _, source_id in ipairs(TEMP_SOURCE_IDS) do
        if seen[source_id] then return source_id end
    end
    return "cpu"
end

local function read_number(file)
    if not file or file == "" then
        return nil
    end

    local nixio_fs = fs()
    if not nixio_fs.access(file) then
        return nil
    end

    local data = nixio_fs.readfile(file)
    if not data then
        return nil
    end

    return tonumber(data:match("[-%d%.]+"))
end

local function notify_daemon()
    local nixio_fs = fs()
    local raw_pid = nixio_fs.readfile("/var/run/fancontrol.pid")
    local pid = raw_pid and tonumber(raw_pid:match("^%s*(%d+)%s*$")) or nil

    if not pid or pid < 2 or pid ~= math.floor(pid) then
        return false
    end

    -- Refuse to signal a reused PID. The daemon is an ash process, so /proc
    -- points at BusyBox; the script path in cmdline is the reliable identity.
    local cmdline = nixio_fs.readfile("/proc/" .. tostring(pid) .. "/cmdline")
    if not cmdline or not cmdline:find("/usr/bin/fancontrol", 1, true) then
        return false
    end

    return require("nixio").kill(pid, 1) and true or false
end

local function status_payload()
    -- The daemon already publishes a compact atomic status snapshot once per
    -- control cycle.  Prefer it so the one-second UI poll does not create a
    -- UCI cursor and reread several configuration values on every request.
    local nixio_fs = fs()
    local runtime = nixio_fs.readfile("/var/run/fancontrol.status")
    if runtime then
        local raw_temp = tonumber(runtime:match("temperature=([-%d%.]+)"))
        local raw_speed = tonumber(runtime:match("speed=([-%d%.]+)"))
        if raw_temp and raw_speed then
            local function runtime_value(name)
                return runtime:match("\n" .. name .. "=([^\r\n]*)")
                    or runtime:match("^" .. name .. "=([^\r\n]*)")
            end
            local function runtime_source(source_id)
                local state = runtime_value("source_" .. source_id .. "_state") or "missing"
                local raw_value = runtime_value("source_" .. source_id .. "_temp")
                local value = tonumber(raw_value)
                if state ~= "ok" and state ~= "stale" and state ~= "missing" and state ~= "error" then
                    state = "error"
                end
                if state ~= "ok" or not value or value < -40000 or value > 150000 then
                    value = nil
                else
                    value = math.floor((value / 1000) * 100) / 100
                end
                return { id = source_id, state = state, temp = value }
            end
            local sample_uptime = tonumber(runtime_value("sample_uptime"))
            local uptime_raw = nixio_fs.readfile("/proc/uptime")
            local current_uptime = uptime_raw and tonumber(uptime_raw:match("^(%d+%.?%d*)")) or nil
            local snapshot_fresh = sample_uptime and current_uptime and sample_uptime >= 0 and
                current_uptime >= sample_uptime and current_uptime - sample_uptime <= 3
            local runtime_control_source = runtime_value("control_source")
            if not TEMP_SOURCE_ALLOWED[runtime_control_source or ""] then
                runtime_control_source = nil
            end

            -- --off/--stop update the dedicated speed file but intentionally leave
            -- the last status snapshot in place.  Prefer that live speed so a
            -- disabled controller is shown as 0% instead of a stale value.
            raw_speed = read_number("/var/run/fancontrol.speed") or raw_speed
            if math.abs(raw_temp) >= 1000 then
                raw_temp = raw_temp / 1000
            end
            local temp = runtime_control_source and math.floor(raw_temp * 100) / 100 or "--"
            local speed = math.floor(clamp_number(raw_speed, 0, 100) or 0)
            if snapshot_fresh then return {
                temp = temp,
                speed = speed,
                rpm = "--",
                fan_value = speed,
                fan_unit = "%",
                control_source = runtime_control_source,
                sources = {
                    runtime_source("cpu"),
                    runtime_source("phy"),
                    runtime_source("nvme1"),
                    runtime_source("nvme2")
                }
            } end
        end
    end

    local uci = uci_cursor()
    local temp_file = uci_get(uci, "thermal_file", "/sys/devices/virtual/thermal/thermal_zone0/temp")
    local fan_file = "/var/run/fancontrol.speed"
    local fan_rpm_file = uci_get(uci, "fan_rpm_file", "/sys/class/hwmon/hwmon0/fan1_input")
    local temp_div = tonumber(uci_get(uci, "temp_div", "1000"))
    local temp = "--"
    local speed = "--"
    local rpm = "--"

    local t = read_number(temp_file)
    if t and temp_div and temp_div > 0 then
        temp = math.floor((t / temp_div) * 100) / 100
    end

    local s = read_number(fan_file)
    if s then
        speed = math.floor(clamp_number(s, 0, 100) or 0)
    end

    local r = read_number(fan_rpm_file)
    if r and r > 0 then
        rpm = tostring(r)
    end

    return {
        temp = temp,
        speed = speed,
        rpm = rpm,
        fan_value = rpm ~= "--" and rpm or speed,
        fan_unit = rpm ~= "--" and "RPM" or "%",
        control_source = nil,
        sources = {
            { id = "cpu", state = "missing", temp = nil },
            { id = "phy", state = "missing", temp = nil },
            { id = "nvme1", state = "missing", temp = nil },
            { id = "nvme2", state = "missing", temp = nil }
        }
    }
end

function action_fancontrol()
    require("luci.template").render("fancontrol/main")
end

function action_api()
    local action = formvalue("act")
    if action == "claim_ui" then
        claim_ui(formvalue("client"))
        return
    end
    if not require_ui_owner() then
        return
    end
    if action == "get_status" then
        write_json(status_payload())
        return
    end

    local uci = uci_cursor()

    if action == "get_config" then
        write_json({
            enable = uci_get(uci, "enable", "1"),
            mode = tonumber(uci_get(uci, "mode", "1")) or 1,
            temp_sources = canonical_temp_source(uci_get(uci, "temp_sources", "cpu")),
            curve_silent = uci_get(uci, "curve_silent", ""),
            curve_balanced = uci_get(uci, "curve_balanced", ""),
            curve_performance = uci_get(uci, "curve_performance", ""),
            temp_file = uci_get(uci, "thermal_file", "/sys/devices/virtual/thermal/thermal_zone0/temp"),
            fan_file = "/var/run/fancontrol.speed",
            fan_rpm_file = uci_get(uci, "fan_rpm_file", "/sys/class/hwmon/hwmon0/fan1_input"),
            max_speed = "100",
            temp_div = uci_get(uci, "temp_div", "1000")
        })
    elseif action == "set_config" then
        local enable = formvalue("enable")
        local mode = formvalue("mode")
        local requested_temp_sources = formvalue("temp_sources")
        local temp_sources = requested_temp_sources and normalize_temp_sources(requested_temp_sources) or nil
        if enable ~= nil and enable ~= "0" and enable ~= "1" then
            write_json({ result = "error", message = "invalid enable value" })
            return
        end
        if mode ~= nil and mode ~= "0" and mode ~= "1" and mode ~= "2" then
            write_json({ result = "error", message = "invalid mode value" })
            return
        end
        if requested_temp_sources ~= nil and not temp_sources then
            write_json({ result = "error", message = "invalid temperature sources" })
            return
        end

        local ok, err = uci_set_if_present(uci, "enable", enable)
        if not ok then write_uci_error(uci, err); return end
        ok, err = uci_set_if_present(uci, "mode", mode)
        if not ok then write_uci_error(uci, err); return end
        ok, err = uci_set_if_present(uci, "temp_sources", temp_sources)
        if not ok then write_uci_error(uci, err); return end
        if not require_ui_owner(uci) then return end
        ok, err = uci:commit(CONFIG)
        if not ok then write_uci_error(uci, err); return end
        local applied = notify_daemon()
        write_json({ result = "ok", temp_sources = temp_sources, applied = applied })
    elseif action == "sync_service" then
        local rc
        if not require_ui_owner() then return end
        -- The daemon stays alive in both switch states: enable=0 holds 0%,
        -- while an actual daemon/service exit retains full-speed fail-safe.
        rc = require("luci.sys").call(
            "/etc/init.d/fancontrol enable >/dev/null 2>&1 && " ..
            "(/etc/init.d/fancontrol running >/dev/null 2>&1 || " ..
            "(/etc/init.d/fancontrol start >/dev/null 2>&1 && " ..
            "/etc/init.d/fancontrol running >/dev/null 2>&1))"
        )
        if rc == 0 then
            write_json({ result = "ok" })
        else
            write_json({ result = "error", message = "fancontrol service sync failed" })
        end
    elseif action == "restart" then
        local rc
        if not require_ui_owner() then return end
        rc = require("luci.sys").call(
            "/etc/init.d/fancontrol enable >/dev/null 2>&1 && " ..
            "/etc/init.d/fancontrol restart >/dev/null 2>&1 && " ..
            "sleep 1 && /etc/init.d/fancontrol running >/dev/null 2>&1"
        )
        if rc == 0 then
            write_json({ result = "ok" })
        else
            write_json({ result = "error", message = "fancontrol service action failed" })
        end
    elseif action == "get_curve" then
        local mode = tonumber(formvalue("mode") or "-1")
        local curve = ""
        if mode == 0 then
            curve = uci_get(uci, "curve_silent", "")
        elseif mode == 1 then
            curve = uci_get(uci, "curve_balanced", "")
        elseif mode == 2 then
            curve = uci_get(uci, "curve_performance", "")
        else
            curve = uci_get(uci, "curve", "")
        end
        write_json({ curve = curve })
    elseif action == "set_curve" then
        local curve = normalize_curve(formvalue("curve"))
        local preset_name = formvalue("preset")
        local preset = preset_name and PRESET_TARGETS[preset_name] or nil
        local enable = formvalue("enable")
        local requested_mode = formvalue("mode")

        if curve == "" then
            write_json({ result = "error", message = "invalid curve" })
            return
        end
        if preset_name and not preset then
            write_json({ result = "error", message = "invalid preset" })
            return
        end
        if enable ~= nil and enable ~= "0" and enable ~= "1" then
            write_json({ result = "error", message = "invalid enable value" })
            return
        end
        if requested_mode ~= nil and requested_mode ~= "0" and requested_mode ~= "1" and requested_mode ~= "2" then
            write_json({ result = "error", message = "invalid mode value" })
            return
        end

        local ok, err = uci:set(CONFIG, SECTION, "curve", curve)
        if not ok then write_uci_error(uci, err); return end
        if enable ~= nil then
            ok, err = uci:set(CONFIG, SECTION, "enable", enable)
            if not ok then write_uci_error(uci, err); return end
        end
        if preset then
            ok, err = uci:set(CONFIG, SECTION, preset.option, curve)
            if not ok then write_uci_error(uci, err); return end
            ok, err = uci:set(CONFIG, SECTION, "mode", tostring(preset.mode))
            if not ok then write_uci_error(uci, err); return end
        elseif requested_mode ~= nil then
            ok, err = uci:set(CONFIG, SECTION, "mode", requested_mode)
            if not ok then write_uci_error(uci, err); return end
        end
        if not require_ui_owner(uci) then return end
        ok, err = uci:commit(CONFIG)
        if not ok then write_uci_error(uci, err); return end
        notify_daemon()
        write_json({
            result = "ok",
            curve = curve,
            preset = preset_name,
            mode = preset and preset.mode or tonumber(requested_mode or uci_get(uci, "mode", "1")) or 1
        })
    elseif action == "set_manual_curve" then
        local curve = manual_curve(formvalue("speed"))
        local ok, err = uci:set(CONFIG, SECTION, "curve", curve)
        if not ok then write_uci_error(uci, err); return end
        if not require_ui_owner(uci) then return end
        ok, err = uci:commit(CONFIG)
        if not ok then write_uci_error(uci, err); return end
        notify_daemon()
        write_json({ result = "ok", curve = curve })
    else
        write_json({ result = "error", message = "unknown action" })
    end
end
