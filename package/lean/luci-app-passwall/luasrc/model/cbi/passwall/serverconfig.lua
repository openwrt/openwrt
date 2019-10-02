local d = require "luci.dispatcher"
local ipkg = require("luci.model.ipkg")

local appname = "passwall"

local function is_finded(e)
    return luci.sys.exec("find /usr/*bin -iname " .. e .. " -type f") ~= "" and
               true or false
end

local function is_installed(e) return ipkg.installed(e) end

local ss_encrypt_method = {
    "rc4-md5", "aes-128-cfb", "aes-192-cfb", "aes-256-cfb",
    "aes-128-ctr", "aes-192-ctr", "aes-256-ctr", "bf-cfb", "camellia-128-cfb",
    "camellia-192-cfb", "camellia-256-cfb", "salsa20", "chacha20",
    "chacha20-ietf", -- aead
    "aes-128-gcm", "aes-192-gcm", "aes-256-gcm", "chacha20-ietf-poly1305",
    "xchacha20-ietf-poly1305"
}

local ssr_encrypt_method = {
    "none", "table", "rc2-cfb", "rc4", "rc4-md5", "rc4-md5-6", "aes-128-cfb",
    "aes-192-cfb", "aes-256-cfb", "aes-128-ctr", "aes-192-ctr", "aes-256-ctr",
    "bf-cfb", "camellia-128-cfb", "camellia-192-cfb", "camellia-256-cfb",
    "cast5-cfb", "des-cfb", "idea-cfb", "seed-cfb", "salsa20", "chacha20",
    "chacha20-ietf"
}

local ssr_protocol = {
    "origin", "verify_simple", "verify_deflate", "verify_sha1", "auth_simple",
    "auth_sha1", "auth_sha1_v2", "auth_sha1_v4", "auth_aes128_md5",
    "auth_aes128_sha1", "auth_chain_a", "auth_chain_b", "auth_chain_c",
    "auth_chain_d", "auth_chain_e", "auth_chain_f"
}
local ssr_obfs = {
    "plain", "http_simple", "http_post", "random_head", "tls_simple",
    "tls1.0_session_auth", "tls1.2_ticket_auth"
}

local v2ray_security = {"none", "auto", "aes-128-gcm", "chacha20-poly1305"}

local v2ray_header_type = {
    "none", "srtp", "utp", "wechat-video", "dtls", "wireguard"
}

m = Map(appname, translate("ShadowSocks Server Config"),
        translate("TCP quick open server does not support do not open.") ..
            translate("HAProxy cannot be used with KCP."))
m.redirect = d.build_url("admin", "vpn", "passwall")

s = m:section(NamedSection, arg[1], "servers", "")
s.addremove = false
s.dynamic = false

o = s:option(Value, "remarks", translate("Node Remarks"))
o.default = translate("Node Remarks")
o.rmempty = false

serverType = s:option(ListValue, "server_type", translate("Server Type"))
if is_finded("ss-redir") then
    serverType:value("SS", translate("Shadowsocks Server"))
end
if is_finded("ssr-redir") then
    serverType:value("SSR", translate("ShadowsocksR Server"))
end
if is_installed("v2ray") then
    serverType:value("V2ray", translate("V2ray Server"))
end
if is_installed("brook") or is_finded("brook") then
    serverType:value("Brook", translate("Brook Server"))
end

o = s:option(ListValue, "v2ray_protocol", translate("V2ray Protocol"))
o:value("vmess", translate("Vmess"))
o:depends("server_type", "V2ray")

o.rmempty = false
o = s:option(Value, "server", translate("Server Address (Support Domain Name)"))
o.rmempty = false

o = s:option(Flag, "use_ipv6", translate("Use IPv6"))
o.default = 0

o = s:option(Value, "server_port", translate("Server Port"))
o.datatype = "port"
o.rmempty = false

o = s:option(Value, "password", translate("Password"))
o.password = true
o.rmempty = false
o:depends("server_type", "SS")
o:depends("server_type", "SSR")
o:depends("server_type", "Brook")

o = s:option(ListValue, "ss_encrypt_method", translate("Encrypt Method"))
for a, t in ipairs(ss_encrypt_method) do o:value(t) end
o:depends("server_type", "SS")

o = s:option(ListValue, "ssr_encrypt_method", translate("Encrypt Method"))
for a, t in ipairs(ssr_encrypt_method) do o:value(t) end
o:depends("server_type", "SSR")

o = s:option(ListValue, "v2ray_security", translate("Encrypt Method"))
for a, t in ipairs(v2ray_security) do o:value(t) end
o:depends("server_type", "V2ray")

o = s:option(ListValue, "protocol", translate("Protocol"))
for a, t in ipairs(ssr_protocol) do o:value(t) end
o:depends("server_type", "SSR")

o = s:option(Value, "protocol_param", translate("Protocol_param"))
o:depends("server_type", "SSR")

o = s:option(ListValue, "obfs", translate("Obfs"))
for a, t in ipairs(ssr_obfs) do o:value(t) end
o:depends("server_type", "SSR")

o = s:option(Value, "obfs_param", translate("Obfs_param"))
o:depends("server_type", "SSR")

o = s:option(Value, "timeout", translate("Connection Timeout"))
o.datatype = "uinteger"
o.default = 300
o.rmempty = false
o:depends("server_type", "SS")
o:depends("server_type", "SSR")

o = s:option(ListValue, "fast_open", translate("Fast_open"))
o:value("false")
o:value("true")
o.rmempty = false
o:depends("server_type", "SS")
o:depends("server_type", "SSR")

o = s:option(Flag, "use_kcp", translate("Use Kcptun"),
             "<span style='color:red'>" .. translate(
                 "Please confirm whether the Kcptun is installed. If not, please go to Rule Update download installation.") ..
                 "</span>")
o.default = 0
o:depends("server_type", "SS")
o:depends("server_type", "SSR")
o:depends("server_type", "Brook")

o = s:option(Value, "kcp_server", translate("Kcptun Server"))
o.placeholder = translate("Default:Current Server")
o:depends("use_kcp", "1")

o = s:option(Flag, "kcp_use_ipv6", translate("Use IPv6"))
o.default = 0
o:depends("use_kcp", "1")

o = s:option(Value, "kcp_port", translate("Kcptun Port"))
o.datatype = "port"
o:depends("use_kcp", "1")

o = s:option(TextValue, "kcp_opts", translate("Kcptun Config"), translate(
                 "--crypt aes192 --key abc123 --mtu 1350 --sndwnd 128 --rcvwnd 1024 --mode fast"))
o.placeholder =
    "--crypt aes192 --key abc123 --mtu 1350 --sndwnd 128 --rcvwnd 1024 --mode fast"
o:depends("use_kcp", "1")

o = s:option(Value, "v2ray_VMess_id", translate("ID"))
o.password = true
o.rmempty = false
o:depends("v2ray_protocol", "vmess")

o = s:option(Value, "v2ray_VMess_alterId", translate("Alter ID"))
o.rmempty = false
o:depends("v2ray_protocol", "vmess")

o = s:option(Value, "v2ray_VMess_level", translate("User Level"))
o.default = 1
o:depends("server_type", "V2ray")

o = s:option(ListValue, "v2ray_stream_security",
             translate("Transport Layer Encryption"), translate(
                 'Whether or not transport layer encryption is enabled, the supported options are "none" for unencrypted (default) and "TLS" for using TLS.'))
o:value("none", "none")
o:value("tls", "tls")
o:depends("server_type", "V2ray")

o = s:option(ListValue, "v2ray_transport", translate("Transport"))
o:value("tcp", "TCP")
o:value("mkcp", "mKCP")
o:value("ws", "WebSocket")
o:value("h2", "HTTP/2")
o:value("ds", "DomainSocket")
o:value("quic", "QUIC")
o:depends("server_type", "V2ray")

-- [[ TCP部分 ]]--

-- TCP伪装
o = s:option(ListValue, "v2ray_tcp_guise", translate("Camouflage Type"))
o:depends("v2ray_transport", "tcp")
o:value("none", "none")
o:value("http", "http")

-- HTTP域名
o = s:option(DynamicList, "v2ray_tcp_guise_http_host", translate("HTTP Host"))
o:depends("v2ray_tcp_guise", "http")

-- HTTP路径
o = s:option(DynamicList, "v2ray_tcp_guise_http_path", translate("HTTP Path"))
o:depends("v2ray_tcp_guise", "http")

-- [[ mKCP部分 ]]--

o = s:option(ListValue, "v2ray_mkcp_guise", translate("Camouflage Type"),
             translate(
                 '<br>none: default, no masquerade, data sent is packets with no characteristics.<br>srtp: disguised as an SRTP packet, it will be recognized as video call data (such as FaceTime).<br>utp: packets disguised as uTP will be recognized as bittorrent downloaded data.<br>wechat-video: packets disguised as WeChat video calls.<br>dtls: disguised as DTLS 1.2 packet.<br>wireguard: disguised as a WireGuard packet. (not really WireGuard protocol)'))
for a, t in ipairs(v2ray_header_type) do o:value(t) end
o:depends("v2ray_transport", "mkcp")

o = s:option(Value, "v2ray_mkcp_mtu", translate("KCP MTU"))
o:depends("v2ray_transport", "mkcp")

o = s:option(Value, "v2ray_mkcp_tti", translate("KCP TTI"))
o:depends("v2ray_transport", "mkcp")

o =
    s:option(Value, "v2ray_mkcp_uplinkCapacity", translate("KCP uplinkCapacity"))
o:depends("v2ray_transport", "mkcp")

o = s:option(Value, "v2ray_mkcp_downlinkCapacity",
             translate("KCP downlinkCapacity"))
o:depends("v2ray_transport", "mkcp")

o = s:option(Flag, "v2ray_mkcp_congestion", translate("KCP Congestion"))
o:depends("v2ray_transport", "mkcp")

o =
    s:option(Value, "v2ray_mkcp_readBufferSize", translate("KCP readBufferSize"))
o:depends("v2ray_transport", "mkcp")

o = s:option(Value, "v2ray_mkcp_writeBufferSize",
             translate("KCP writeBufferSize"))
o:depends("v2ray_transport", "mkcp")

-- [[ WebSocket部分 ]]--

o = s:option(Value, "v2ray_ws_host", translate("WebSocket Host"))
o:depends("v2ray_transport", "ws")

o = s:option(Value, "v2ray_ws_path", translate("WebSocket Path"))
o:depends("v2ray_transport", "ws")

-- [[ HTTP/2部分 ]]--

o = s:option(DynamicList, "v2ray_h2_host", translate("HTTP/2 Host"))
o:depends("v2ray_transport", "h2")

o = s:option(Value, "v2ray_h2_path", translate("HTTP/2 Path"))
o:depends("v2ray_transport", "h2")

-- [[ DomainSocket部分 ]]--

o = s:option(Value, "v2ray_ds_path", "Path", translate(
                 "A legal file path. This file must not exist before running V2Ray."))
o:depends("v2ray_transport", "ds")

-- [[ QUIC部分 ]]--
o = s:option(ListValue, "v2ray_quic_security", translate("Encrypt Method"))
o:value("none")
o:value("aes-128-gcm")
o:value("chacha20-poly1305")
o:depends("v2ray_transport", "quic")

o = s:option(Value, "v2ray_quic_key",
             translate("Encrypt Method") .. translate("Key"))
o:depends("v2ray_transport", "quic")

o = s:option(ListValue, "v2ray_quic_guise", translate("Camouflage Type"))
for a, t in ipairs(v2ray_header_type) do o:value(t) end
o:depends("v2ray_transport", "quic")

-- [[ 其它 ]]--

o = s:option(Flag, "v2ray_mux", translate("Mux"))
o:depends("server_type", "V2ray")

o = s:option(Value, "v2ray_mux_concurrency", translate("Mux Concurrency"))
o.default = 8
o:depends("v2ray_mux", "1")

-- o = s:option(Flag, "v2ray_insecure", translate("allowInsecure"))
-- o:depends("server_type", "V2ray")

return m
