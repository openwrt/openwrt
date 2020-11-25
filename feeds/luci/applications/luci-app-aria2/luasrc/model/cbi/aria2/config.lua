-- Copyright 2017-2019 Xingwang Liao <kuoruan@gmail.com>
-- Licensed to the public under the MIT License.

local sys  = require "luci.sys"
local util = require "luci.util"

local m, s, o

local function aria2_info()
	if sys.call("command -v aria2c >/dev/null") ~= 0 then
		return nil
	end

	local info = {}
	local line
	for line in util.execi("aria2c -v 2>/dev/null | grep -E '^(aria2 version|Enabled Features)'") do
		if line:match("^aria2 version") then
			local _, _, v = line:find("([%d%.]+)$")
			info.version = v
		elseif line:match("^Enabled Features") then
			info.gzip = line:find("GZip") ~= nil
			info.https = line:find("HTTPS") ~= nil
			info.bt = line:find("BitTorrent") ~= nil
			info.sftp = line:find("SFTP") ~= nil
			info.adns = line:find("Async DNS") ~= nil
			info.cookie = line:find("Firefox3 Cookie") ~= nil
		end
	end

	return info
end

local aria2 = aria2_info()

m = Map("aria2", "%s - %s" % { translate("Aria2"), translate("Settings") },
"<p>%s</p><p>%s</p>" % {
	translate("Aria2 is a lightweight multi-protocol &amp; multi-source, cross platform download utility."),
	translatef("For more information, please visit: %s",
		"<a href=\"https://aria2.github.io\" target=\"_blank\">https://aria2.github.io</a>")
})

if not aria2 then
	m:section(SimpleSection, nil, "<span style=\"color: red;\">%s</span>" %
		translate("Error: Can't find aria2c in PATH, please reinstall aria2."))
	m.reset = false
	m.submit = false
	return m
end

m:append(Template("aria2/settings_header"))

s = m:section(NamedSection, "main", "aria2")
s.addremove = false
s.anonymous = true

s:tab("basic", translate("Basic Options"))

o = s:taboption("basic", Flag, "enabled", translate("Enabled"))
o.rmempty = false

o = s:taboption("basic", ListValue, "user", translate("Run daemon as user"),
	translate("Leave blank to use default user."))
o:value("")
local user
for user in util.execi("cat /etc/passwd | cut -d':' -f1") do
	o:value(user)
end

o = s:taboption("basic", Value, "dir", translate("Download directory"),
	translate("The directory to store the downloaded file. eg. <code>/mnt/sda1</code>"))
o.rmempty = false

o = s:taboption("basic", Value, "config_dir", translate("Config file directory"),
	translate("The directory to store the config file, session file and DHT file."))
o.placeholder = "/var/etc/aria2"

o = s:taboption("basic", Flag, "enable_logging", translate("Enable logging"))
o.rmempty = false

o = s:taboption("basic", Value, "log", translate("Log file"),
	translate("The file name of the log file."))
o:depends("enable_logging", "1")
o.placeholder = "/var/log/aria2.log"

o = s:taboption("basic", ListValue, "log_level", translate("Log level"))
o:depends("enable_logging", "1")
o:value("debug", translate("Debug"))
o:value("info", translate("Info"))
o:value("notice", translate("Notice"))
o:value("warn", translate("Warn"))
o:value("error", translate("Error"))
o.default = "warn"

o = s:taboption("basic", Value, "max_concurrent_downloads", translate("Max concurrent downloads"))
o.placeholder = "5"

s:tab("rpc", translate("RPC Options"))

o = s:taboption("rpc", Flag, "pause", translate("Pause"), translate("Pause download after added."))
o.enabled = "true"
o.disabled = "false"
o.default = "false"

o = s:taboption("rpc", Flag, "pause_metadata", translate("Pause metadata"),
	translate("Pause downloads created as a result of metadata download."))
o.enabled = "true"
o.disabled = "false"
o.default = "false"

o = s:taboption("rpc", Value, "rpc_listen_port", translate("RPC port"))
o.datatype = "range(1024,65535)"
o.placeholder = "6800"

o = s:taboption("rpc", ListValue, "rpc_auth_method", translate("RPC authentication method"))
o:value("none", translate("No Authentication"))
o:value("user_pass", translate("Username & Password"))
o:value("token", translate("Token"))

o = s:taboption("rpc", Value, "rpc_user", translate("RPC username"))
o:depends("rpc_auth_method", "user_pass")

o = s:taboption("rpc", Value, "rpc_passwd", translate("RPC password"))
o:depends("rpc_auth_method", "user_pass")
o.password  =  true

o = s:taboption("rpc", Value, "rpc_secret", translate("RPC token"))
o:depends("rpc_auth_method", "token")
o.template = "aria2/value_with_btn"
o.btntext = translate("Generate Randomly")
o.btnclick = "randomToken();"

if aria2.https then
	o = s:taboption("rpc", Flag, "rpc_secure", translate("RPC secure"),
		translate("RPC transport will be encrypted by SSL/TLS. The RPC clients must use https"
		.. " scheme to access the server. For WebSocket client, use wss scheme."))
	o.enabled = "true"
	o.disabled = "false"
	o.rmempty = false

	o = s:taboption("rpc", Value, "rpc_certificate", translate("RPC certificate"),
		translate("Use the certificate in FILE for RPC server. The certificate must be either"
		.. " in PKCS12 (.p12, .pfx) or in PEM format.<br/>PKCS12 files must contain the"
		.. " certificate, a key and optionally a chain of additional certificates. Only PKCS12"
		.. " files with a blank import password can be opened!<br/>When using PEM, you have to"
		.. " specify the \"RPC private key\" as well."))
	o:depends("rpc_secure", "true")
	o.datatype = "file"

	o = s:taboption("rpc", Value, "rpc_private_key", translate("RPC private key"),
		translate("Use the private key in FILE for RPC server. The private key must be"
		.. " decrypted and in PEM format."))
	o:depends("rpc_secure", "true")
	o.datatype = "file"
end

o = s:taboption("rpc", Flag, "_use_ws", translate("Use WebSocket"))

o = s:taboption("rpc", Value, "_rpc_url", translate("Json-RPC URL"))
o.template = "aria2/value_with_btn"
o.onmouseover = "this.focus();this.select();"
o.btntext = translate("Show URL")
o.btnclick = "showRPCURL();"

s:tab("http", translate("HTTP/FTP/SFTP Options"))

o = s:taboption("http", Flag, "enable_proxy", translate("Enable proxy"))
o.rmempty = false

o = s:taboption("http", Value, "all_proxy", translate("All proxy"),
	translate("Use a proxy server for all protocols."))
o:depends("enable_proxy", "1")
o.placeholder = "[http://][USER:PASSWORD@]HOST[:PORT]"

o = s:taboption("http", Value, "all_proxy_user", translate("Proxy user"))
o:depends("enable_proxy", "1")

o = s:taboption("http", Value, "all_proxy_passwd", translate("Proxy password"))
o:depends("enable_proxy", "1")
o.password = true

if aria2.https then
	o = s:taboption("http", Flag, "check_certificate", translate("Check certificate"),
		translate("Verify the peer using certificates specified in \"CA certificate\" option."))
	o.enabled = "true"
	o.disabled = "false"
	o.default = "true"
	o.rmempty = false

	o = s:taboption("http", Value, "ca_certificate", translate("CA certificate"),
		translate("Use the certificate authorities in FILE to verify the peers. The certificate"
		.. " file must be in PEM format and can contain multiple CA certificates."))
	o:depends("check_certificate", "true")
	o.datatype = "file"

	o = s:taboption("http", Value, "certificate", translate("Certificate"),
		translate("Use the client certificate in FILE. The certificate must be either in PKCS12"
		.. " (.p12, .pfx) or in PEM format.<br/>PKCS12 files must contain the certificate, a"
		.. " key and optionally a chain of additional certificates. Only PKCS12 files with a"
		.. " blank import password can be opened!<br/>When using PEM, you have to specify the"
		.. " \"Private key\" as well."))
	o.datatype = "file"

	o = s:taboption("http", Value, "private_key", translate("Private key"),
		translate("Use the private key in FILE. The private key must be decrypted and in PEM"
		.. " format. The behavior when encrypted one is given is undefined."))
	o.datatype = "file"
end

if aria2.gzip then
	o = s:taboption("http", Flag, "http_accept_gzip", translate("HTTP accept gzip"),
		translate("Send <code>Accept: deflate, gzip</code> request header and inflate response"
		.. " if remote server responds with <code>Content-Encoding: gzip</code> or"
		.. " <code>Content-Encoding: deflate</code>."))
	o.enabled = "true"
	o.disabled = "false"
	o.default = "false"
end

o = s:taboption("http", Flag, "http_no_cache", translate("HTTP no cache"),
	translate("Send <code>Cache-Control: no-cache</code> and <code>Pragma: no-cache</code>"
	.. " header to avoid cached content. If disabled, these headers are not sent and you"
	.. " can add Cache-Control header with a directive you like using \"Header\" option."))
o.enabled = "true"
o.disabled = "false"
o.default = "false"

o = s:taboption("http", DynamicList, "header", translate("Header"),
	translate("Append HEADERs to HTTP request header."))

o = s:taboption("http", Value, "connect_timeout", translate("Connect timeout"),
	translate("Set the connect timeout in seconds to establish connection to HTTP/FTP/proxy server." ..
	" After the connection is established, this option makes no effect and \"Timeout\" option is used instead."))
o.datatype = "uinteger"
o.placeholder = "60"

o = s:taboption("http", Value, "timeout", translate("Timeout"))
o.datatype = "uinteger"
o.placeholder = "60"

o = s:taboption("http", Value, "lowest_speed_limit", translate("Lowest speed limit"),
	"%s %s" % {
		translate("Close connection if download speed is lower than or equal to this value(bytes per sec). " ..
	"0 means has no lowest speed limit."),
		translate("You can append K or M.")
	})
o.placeholder = "0"

o = s:taboption("http", Value, "max_connection_per_server", translate("Max connection per server"),
	translate("The maximum number of connections to one server for each download."))
o.datatype = "uinteger"
o.placeholder = "1"

o = s:taboption("http", Value, "split", translate("Max number of split"),
	translate("Download a file using N connections."))
o.datatype = "uinteger"
o.placeholder = "5"

o = s:taboption("http", Value, "min_split_size", translate("Min split size"),
	translate("Don't split less than 2*SIZE byte range. Possible values: 1M-1024M."))
o.placeholder = "20M"

o = s:taboption("http", Value, "max_tries", translate("Max tries"))
o.datatype = "uinteger"
o.placeholder = "5"

o = s:taboption("http", Value, "retry_wait", translate("Retry wait"),
	translate("Set the seconds to wait between retries."))
o.datatype = "uinteger"
o.placeholder = "0"

o = s:taboption("http", Value, "user_agent", translate("User agent"),
	translate("Set user agent for HTTP(S) downloads."))
o.placeholder = "aria2/%s" % { aria2.version and aria2.version or "$VERSION" }

if aria2.bt then
	s:tab("bt", translate("BitTorrent Options"))

	o = s:taboption("bt", Flag, "enable_dht", translate("IPv4 <abbr title=\"Distributed Hash Table\">DHT</abbr> enabled"),
		"%s %s" % {
			translate("Enable IPv4 DHT functionality. It also enables UDP tracker support."),
			translate("This option will be ignored if a private flag is set in a torrent.")
		})
	o.enabled = "true"
	o.disabled = "false"
	o.default = "true"
	o.rmempty = false

	o = s:taboption("bt", Flag, "enable_dht6", translate("IPv6 <abbr title=\"Distributed Hash Table\">DHT</abbr> enabled"),
		"%s %s" % {
			translate("Enable IPv6 DHT functionality."),
			translate("This option will be ignored if a private flag is set in a torrent.")
		})
	o.enabled = "true"
	o.disabled = "false"

	o = s:taboption("bt", Flag, "bt_enable_lpd", translate("<abbr title=\"Local Peer Discovery\">LPD</abbr> enabled"),
		"%s %s" % {
			translate("Enable Local Peer Discovery."),
			translate("This option will be ignored if a private flag is set in a torrent.")
		})
	o.enabled = "true"
	o.disabled = "false"
	o.default = "false"

	o = s:taboption("bt", Flag, "enable_peer_exchange", translate("Enable peer exchange"),
		"%s %s" % {
			translate("Enable Peer Exchange extension."),
			translate("This option will be ignored if a private flag is set in a torrent.")
		})
	o.enabled = "true"
	o.disabled = "false"
	o.default = "true"
	o.rmempty = false

	o = s:taboption("bt", Flag, "bt_save_metadata", translate("Sava metadata"),
		translate("Save meta data as \".torrent\" file. This option has effect only when BitTorrent"
		.. " Magnet URI is used. The file name is hex encoded info hash with suffix \".torrent\"."))
	o.enabled = "true"
	o.disabled = "false"
	o.default = "false"

	o = s:taboption("bt", Flag, "bt_remove_unselected_file", translate("Remove unselected file"),
		translate("Removes the unselected files when download is completed in BitTorrent. Please"
		.. " use this option with care because it will actually remove files from your disk."))
	o.enabled = "true"
	o.disabled = "false"
	o.default = "false"

	o = s:taboption("bt", Flag, "bt_seed_unverified", translate("Seed unverified"),
		translate("Seed previously downloaded files without verifying piece hashes."))
	o.enabled = "true"
	o.disabled = "false"
	o.default = "false"

	o = s:taboption("bt", Value, "listen_port", translate("BitTorrent listen port"),
		translate("Set TCP port number for BitTorrent downloads. Accept format: \"6881,6885\","
		.. " \"6881-6999\" and \"6881-6889,6999\". Make sure that the specified ports are open"
		.. " for incoming TCP traffic."))
	o.placeholder = "6881-6999"

	o = s:taboption("bt", Value, "dht_listen_port", translate("DHT Listen port"),
		translate("Set UDP listening port used by DHT(IPv4, IPv6) and UDP tracker. Make sure that the "
		.. "specified ports are open for incoming UDP traffic."))
	o:depends("enable_dht", "true")
	o:depends("enable_dht6", "true")
	o.placeholder = "6881-6999"

	o = s:taboption("bt", ListValue, "follow_torrent", translate("Follow torrent"))
	o:value("true", translate("True"))
	o:value("false", translate("False"))
	o:value("mem", translate("Keep in memory"))

	o = s:taboption("bt", Value, "max_overall_upload_limit", translate("Max overall upload limit"),
		"%s %s" % {
			translate("Set max overall upload speed in bytes/sec. 0 means unrestricted."),
			translate("You can append K or M.")
		})
	o.placeholder = "0"

	o = s:taboption("bt", Value, "max_upload_limit", translate("Max upload limit"),
		"%s %s" % {
			translate("Set max upload speed per each torrent in bytes/sec. 0 means unrestricted."),
			translate("You can append K or M.")
		})
	o.placeholder = "0"

	o = s:taboption("bt", Value, "bt_max_open_files", translate("Max open files"),
		translate("Specify maximum number of files to open in multi-file BitTorrent download globally."))
	o.datatype = "uinteger"
	o.placeholder = "100"

	o = s:taboption("bt", Value, "bt_max_peers", translate("Max peers"),
		translate("Specify the maximum number of peers per torrent, 0 means unlimited."))
	o.datatype = "uinteger"
	o.placeholder = "55"

	o = s:taboption("bt", Value, "bt_request_peer_speed_limit", translate("Request peer speed limit"),
		"%s %s" % {
			translate("If the whole download speed of every torrent is lower than SPEED, aria2"
			.. " temporarily increases the number of peers to try for more download speed."
			.. " Configuring this option with your preferred download speed can increase your"
			.. " download speed in some cases."),
			translate("You can append K or M.")
		})
	o.placeholder = "50K"

	o = s:taboption("bt", Value, "bt_stop_timeout", translate("Stop timeout"),
		translate("Stop BitTorrent download if download speed is 0 in consecutive N seconds. If 0 is"
		.. " given, this feature is disabled."))
	o.datatype = "uinteger"
	o.placeholder = "0"

	o = s:taboption("bt", Value, "peer_id_prefix", translate("Prefix of peer ID"),
		translate("Specify the prefix of peer ID. The peer ID in BitTorrent is 20 byte length."
		.. " If more than 20 bytes are specified, only first 20 bytes are used. If less than 20"
		.. " bytes are specified, random byte data are added to make its length 20 bytes."))
	o.placeholder = "A2-%s-" % {
		aria2.version and string.gsub(aria2.version, "%.", "-") or "$MAJOR-$MINOR-$PATCH"
	}

	o = s:taboption("bt", Value, "seed_ratio", translate("Seed ratio"),
		translate("Specify share ratio. Seed completed torrents until share ratio reaches RATIO."
		.. " You are strongly encouraged to specify equals or more than 1.0 here. Specify 0.0 if"
		.. " you intend to do seeding regardless of share ratio."))
	o.datatype = "ufloat"
	o.placeholder = "1.0"

	o = s:taboption("bt", Value, "seed_time", translate("Seed time"),
		translate("Specify seeding time in minutes. If \"Seed ratio\" option is"
		.. " specified along with this option, seeding ends when at least one of the conditions"
		.. " is satisfied. Specifying 0 disables seeding after download completed."))
	o.datatype = "ufloat"

	o = s:taboption("bt", DynamicList, "bt_tracker", translate("Additional BT tracker"),
		translate("List of additional BitTorrent tracker's announce URI."))
	o.placeholder = "http://tracker.example.com/announce"
end

s:tab("advance", translate("Advanced Options"))

o = s:taboption("advance", Flag, "disable_ipv6", translate("IPv6 disabled"),
	translate("Disable IPv6. This is useful if you have to use broken DNS and want to avoid terribly"
	.. " slow AAAA record lookup."))
o.enabled = "true"
o.disabled = "false"
o.default = "false"

o = s:taboption("advance", Value, "auto_save_interval", translate("Auto save interval"),
	translate("Save a control file(*.aria2) every N seconds. If 0 is given, a control file is not"
	.. " saved during download."))
o.datatype = "range(0, 600)"
o.placeholder = "60"

o = s:taboption("advance", Value, "save_session_interval", translate("Save session interval"),
	translate("Save error/unfinished downloads to session file every N seconds. If 0 is given, file"
	.. " will be saved only when aria2 exits."))
o.datatype = "uinteger"
o.placeholder = "0"

o = s:taboption("advance", Value, "disk_cache", translate("Disk cache"),
	"%s %s" % {
		translate("Enable disk cache (in bytes), set 0 to disabled."),
		translate("You can append K or M.")
	})
o.placeholder = "16M"

o = s:taboption("advance", ListValue, "file_allocation", translate("File allocation"),
	translate("Specify file allocation method. If you are using newer file systems such as ext4"
	.. " (with extents support), btrfs, xfs or NTFS(MinGW build only), \"falloc\" is your best choice."
	.. " It allocates large(few GiB) files almost instantly, but it may not be available if your system"
	.. " doesn't have posix_fallocate(3) function. Don't use \"falloc\" with legacy file systems such as"
	.. " ext3 and FAT32 because it takes almost same time as \"prealloc\" and it blocks aria2 entirely"
	.. " until allocation finishes."))
o:value("none", translate("None"))
o:value("prealloc", translate("prealloc"))
o:value("trunc", translate("trunc"))
o:value("falloc", translate("falloc"))
o.default = "prealloc"

o = s:taboption("advance", Flag, "force_save", translate("Force save"),
	translate("Save download to session file even if the download is completed or removed."
	.. " This option also saves control file in that situations. This may be useful to save"
	.. " BitTorrent seeding which is recognized as completed state."))
o.enabled = "true"
o.disabled = "false"
o.default = "false"

o = s:taboption("advance", Value, "max_overall_download_limit", translate("Max overall download limit"),
	"%s %s" % {
		translate("Set max overall download speed in bytes/sec. 0 means unrestricted."),
		translate("You can append K or M.")
	})
o.placeholder = "0"

o = s:taboption("advance", Value, "max_download_limit", translate("Max download limit"),
	"%s %s" % {
		translate("Set max download speed per each download in bytes/sec. 0 means unrestricted."),
		translate("You can append K or M.")
	})
o.placeholder = "0"

s = m:section(NamedSection, "main", "aria2", translate("Extra Settings"),
	translate("Settings in this section will be added to config file."))
s.addremove = false
s.anonymous = true

o = s:option(DynamicList, "extra_setting", translate("Settings list"),
	translate("List of extra settings. Format: option=value, eg. <code>netrc-path=/tmp/.netrc</code>."))
o.placeholder = "option=value"

return m
