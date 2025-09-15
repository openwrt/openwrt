module("luci.controller.Secondsystem", package.seeall)

-- 将公共模块引用移至全局，避免重复加载
local sys = require "luci.sys"
local http = require "luci.http"
local dispatcher = require "luci.dispatcher"

function index()
    -- 主菜单配置，保持不变
    entry({"admin", "system", "Secondsystem"}, 
          alias("admin", "system", "Secondsystem", "settings"), 
          _("官方系统"), 50)
          
    -- 子菜单设置项，保持不变
    entry({"admin", "system", "Secondsystem", "settings"}, 
          template("Secondsystem/settings"), 
          _("Settings"), 10)
          
    -- 修复：为switch条目添加标题和排序
    entry({"admin", "system", "Secondsystem", "switch"}, 
          call("action_switch"), 
          _("Switch System"), 20)  -- 新增标题和排序值20
          
    -- 修复：为reboots条目添加标题和排序
    entry({"admin", "system", "Secondsystem", "reboots"}, 
          call("action_reboots"), 
          _("Reboot System"), 30)  -- 新增标题和排序值30
end

function action_switch()
    local command2 = 'AT+CFUN=1,1'
    local sendat2 = 'sendat 2 "' .. command2 .. '"'
    local confirm = http.formvalue("confirm")
    
    if confirm and confirm == "yes" then
        sys.call("fw_setenv boot_system 0")
        -- sys.call(sendat2)  -- 根据实际需求决定是否启用
        
        -- 修复：先发送HTTP响应再执行重启
        http.write([[
            <!DOCTYPE html>
            <html>
            <head><title>System Rebooting</title></head>
            <body>
                <h1>System is rebooting...</h1>
                <p>Please wait a few minutes and reconnect.</p>
            </body>
            </html>
        ]])
        http.close()  -- 确保响应发送完成
        
        sys.call("reboot")
    else
        -- 修复：使用全局引用的dispatcher简化代码
        http.redirect(dispatcher.build_url("admin", "system", "Secondsystem", "settings"))
    end
end

function action_reboots()
    local command = 'AT+CFUN=1,1'
    local sendat = 'sendat 2 "' .. command .. '"'
    local confirm = http.formvalue("confirm")
    
    if confirm and confirm == "yes" then
        sys.call(sendat)
        
        -- 修复：先发送HTTP响应再执行重启
        http.write([[
            <!DOCTYPE html>
            <html>
            <head><title>System Rebooting</title></head>
            <body>
                <h1>System is rebooting...</h1>
                <p>Please wait a few minutes and reconnect.</p>
            </body>
            </html>
        ]])
        http.close()  -- 确保响应发送完成
        
        sys.call("reboot")
    else
        -- 修复：使用全局引用的dispatcher简化代码
        http.redirect(dispatcher.build_url("admin", "system", "Secondsystem", "settings"))
    end
end
