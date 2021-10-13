-- SPDX-License-Identifier: GPL-2.0-only
-- Copyright (c) Facebook, Inc. and its affiliates.
--
-- FBWIFI Lua library
-- function table
local fbwifi = {}

local http = require("ssl.https")
local json = require("cjson")
local log = require("posix.syslog")
local uci = require("uci")

function fbwifi.gateway_token()
	token = uci.get("fbwifi.main.gateway_token")
	if token and string.len(token) > 0 then
		return token
	else
		log.syslog( log.LOG_WARNING, "[fbwifi] UCI option fbwifi.main.gateway_token is missing" )
		return nil
	end 
end

function fbwifi.validate_token( token )

	local valid = false

	if string.len(token or '' ) > 0 then

	        GATEWAY_TOKEN = fbwifi.gateway_token()

	        URL="https://api.fbwifi.com/v2.0/token"
	        BODY="token="..token
	        body, code, headers = http.request(URL.."?access_token="..GATEWAY_TOKEN, BODY)

	        if code==200 then
	                valid = true
	        else
	                log.syslog(log.LOG_WARNING, "[fbwifi] validate_token:"..body)
	        end

	end

	return valid
end

local mac_to_purge=''
function remove_client_by_mac(client)
	state = uci.cursor("/var/state", "/tmp/fbwifi")

	for key, value in pairs(client) do
		if
			key == 'mac' and
			value == mac_to_purge
		then
			log.syslog(log.LOG_INFO, string.format("[fbwifi] Purging DB entry %s for MAC %s", client['.name'] or 'unknown', mac_to_purge) )
			state:delete("fbwifi", client['.name'])
			return
		end
	end
end

function fbwifi.instate_client_rule( token, client_mac )

	log.syslog(log.LOG_INFO, "[fbwifi] Validating client "..client_mac)

	state = uci.cursor("/var/state", "/tmp/fbwifi")
	state_name = "token_" .. token

	RULE_COND="iptables -w -L FBWIFI_CLIENT_TO_INTERNET -t mangle | grep -i -q \"%s\""
	RULE_FMT="iptables -w -t mangle -%s FBWIFI_CLIENT_TO_INTERNET -m mac --mac-source \"%s\" -j MARK --set-mark 0xfb"
	local RULE

	log.syslog(log.LOG_INFO, string.format("[fbwifi] Cleaning DB for MAC %s", client_mac) )
	mac_to_purge = client_mac
	state:foreach("fbwifi", "client", remove_client_by_mac)
	
		
	log.syslog(log.LOG_INFO, string.format("[fbwifi] Adding DB entry %s for MAC %s", state_name, client_mac) )
	state:set("fbwifi", state_name, "client")
	state:set("fbwifi", state_name, "token", token)
	state:set("fbwifi", state_name, "mac", client_mac)
	state:set("fbwifi", state_name, "authenticated", "true")
				
	-- verify a rule exists for the given client MAC, 
	--   OR install it
	RULE=string.format(RULE_COND.." || "..RULE_FMT, client_mac, "A", client_mac)

	log.syslog(log.LOG_INFO, string.format( "[fbwifi] Opening iptables for %s", client_mac ) )
	res = os.execute(RULE)
	if res ~= 0 then 
		log.syslog(log.LOG_WARNING, string.format( "[fbwifi] Failed to update iptables (%s)", res ) )
	end
	log.syslog(log.LOG_INFO, "[fbwifi] "..RULE)

	state:save('fbwifi')
	state:commit('fbwifi')
end

function fbwifi.revoke_client_rule( token )
        
        if (token == nil) then
                log.syslog(log.LOG_INFO, "[fbwifi] Invalidating token, but token is Nil")
                return
        end

	log.syslog(log.LOG_INFO, string.format( "[fbwifi] Invalidating token (%s)", token) )

	state = uci.cursor("/var/state", "/tmp/fbwifi")
	state_name = "token_" .. token
	
	client_mac = state:get("fbwifi", state_name, "mac")

	if client_mac then
		RULE_COND="iptables -w -L FBWIFI_CLIENT_TO_INTERNET -t mangle | grep -i -q \"%s\""
		RULE_FMT="iptables -w -t mangle -%s FBWIFI_CLIENT_TO_INTERNET -m mac --mac-source \"%s\" -j MARK --set-mark 0xfb"

		-- verify a rule exists for the given client MAC, 
		--  AND delete it
		RULE=string.format(RULE_COND.." && "..RULE_FMT, client_mac, "D", client_mac)

		res = os.execute(RULE)
		if res ~= 0 then 
			log.syslog(log.LOG_WARNING, string.format( "[fbwifi] Failed to update iptables (%s)", res ) )
		end
		log.syslog(log.LOG_INFO, "[fbwifi] "..RULE)

		state:delete("fbwifi", state_name)
		state:save('fbwifi')
		state:commit('fbwifi')
	else
		log.syslog(log.LOG_WARNING, string.format( "[fbwifi] Client MAC not found in DB (%s)", state_name ) )
	end
end

function fbwifi.reset()

	local success = false
        GATEWAY_TOKEN = fbwifi.gateway_token()
        URL="https://api.fbwifi.com/v2.0/gateway/reset"
	BODY="{}"
        body, code, headers = http.request(URL.."?access_token="..GATEWAY_TOKEN, BODY)

        if code==200 then
                log.syslog(log.LOG_INFO, "[fbwifi] Reset committed")
                success = true
        else
                log.syslog(log.LOG_WARNING, "[fbwifi] Reset failed : "..body)
        end

	return success
end

--
-- Return the function table to the host script
--
return fbwifi
