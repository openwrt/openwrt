
module("luci.controller.n2n_v2", package.seeall)

function index()
	if not nixio.fs.access("/etc/config/n2n_v2") then
		return
	end
	
	entry({"admin", "vpn"}, firstchild(), "VPN", 45).dependent = false
	
	local page

	entry({"admin", "vpn", "n2n_v2"}, cbi("n2n_v2"), _("N2N VPN(V2)"), 80).dependent=false
	entry({"admin","vpn","n2n_v2","edge_status"},call("edge_status")).leaf=true
	entry({"admin","vpn","n2n_v2","supernode_status"},call("supernode_status")).leaf=true
end

function edge_status()
  local e={}
  e.running=luci.sys.call("pgrep edge >/dev/null")==0
  luci.http.prepare_content("application/json")
  luci.http.write_json(e)
end

function supernode_status()
  local e={}
  e.running=luci.sys.call("pgrep supernode >/dev/null")==0
  luci.http.prepare_content("application/json")
  luci.http.write_json(e)
end
