module("luci.controller.fancontrol", package.seeall)

local CONFIG = "fancontrol"
local SECTION = "settings"
local TEMP_POINTS = { 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 80, 85, 90, 95, 100, 105, 110 }

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
    h.prepare_content("application/json")
    h.write_json(data)
end

local function formvalue(name)
    return http().formvalue(name)
end

local function uci_get(uci, name, default)
    return uci:get(CONFIG, SECTION, name) or default
end

local function uci_set_if_present(uci, name, value)
    if value ~= nil then
        uci:set(CONFIG, SECTION, name, value)
    end
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

local function read_status()
    local result = {}
    local data = fs().readfile("/var/run/fancontrol.status") or ""
    for key, value in data:gmatch("([%w_]+)=([^\n]*)") do
        result[key] = value
    end
    return result
end

function action_fancontrol()
    require("luci.template").render("fancontrol/main")
end

function action_api()
    local action = formvalue("act")
    local uci = uci_cursor()

    if action == "get_config" then
        write_json({
            enable = uci_get(uci, "enable", "1"),
            mode = tonumber(uci_get(uci, "mode", "1")) or 1,
            temp_file = uci_get(uci, "thermal_file", "/sys/devices/virtual/thermal/thermal_zone0/temp"),
            fan_file = "/var/run/fancontrol.speed",
            fan_rpm_file = uci_get(uci, "fan_rpm_file", "/sys/class/hwmon/hwmon0/fan1_input"),
            max_speed = "100",
            temp_div = uci_get(uci, "temp_div", "1000")
        })
    elseif action == "set_config" then
        uci_set_if_present(uci, "enable", formvalue("enable"))
        uci_set_if_present(uci, "mode", formvalue("mode"))
        uci:commit(CONFIG)
        write_json({ result = "ok" })
    elseif action == "get_status" then
        local daemon_status = read_status()
        local temp_file = uci_get(uci, "thermal_file", "/sys/devices/virtual/thermal/thermal_zone0/temp")
        local fan_file = "/var/run/fancontrol.speed"
        local fan_rpm_file = uci_get(uci, "fan_rpm_file", "/sys/class/hwmon/hwmon0/fan1_input")
        local max_speed = 100
        local temp_div = tonumber(uci_get(uci, "temp_div", "1000"))
        local temp = "--"
        local speed = "--"
        local rpm = "--"

        local t = read_number(temp_file)
        if t and temp_div and temp_div > 0 then
            temp = math.floor((t / temp_div) * 100) / 100
        end

        local s = read_number(fan_file)
        if s and max_speed and max_speed > 0 then
            speed = math.floor(s / max_speed * 100)
        end

        local r = read_number(fan_rpm_file)
        if r and r > 0 then
            rpm = tostring(r)
        end

        local fan_value = rpm
        local fan_unit = "RPM"
        if rpm == "--" then
            fan_value = speed
            fan_unit = "%"
        end

        write_json({
            temp = temp,
            speed = speed,
            rpm = rpm,
            fan_value = fan_value,
            fan_unit = fan_unit,
            backend = daemon_status.backend or "--",
            direction = daemon_status.direction or "--",
            pwm = daemon_status.pwm or "--",
            state = daemon_status.state or "--",
            error = daemon_status.error or ""
        })
    elseif action == "restart" then
        -- Kept for callers from older cached LuCI pages.  The daemon reloads
        -- UCI every second, so no process restart is required.
        write_json({ result = "applied" })
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
        if curve ~= "" then
            uci:set(CONFIG, SECTION, "curve", curve)
            uci:commit(CONFIG)
        end
        write_json({ result = "ok" })
    elseif action == "set_manual_curve" then
        local curve = manual_curve(formvalue("speed"))
        uci:set(CONFIG, SECTION, "curve", curve)
        uci:commit(CONFIG)
        write_json({ result = "ok", curve = curve })
    else
        write_json({ result = "error", message = "unknown action" })
    end
end
