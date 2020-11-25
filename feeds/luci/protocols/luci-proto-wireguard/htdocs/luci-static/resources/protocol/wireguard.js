'use strict';
'require form';
'require network';

function validateBase64(section_id, value) {
	if (value.length == 0)
		return true;

	if (value.length != 44 || !value.match(/^(?:[A-Za-z0-9+\/]{4})*(?:[A-Za-z0-9+\/]{2}==|[A-Za-z0-9+\/]{3}=)?$/))
		return _('Invalid Base64 key string');

	return true;
}

return network.registerProtocol('wireguard', {
	getI18n: function() {
		return _('WireGuard VPN');
	},

	getIfname: function() {
		return this._ubus('l3_device') || this.sid;
	},

	getOpkgPackage: function() {
		return 'wireguard-tools';
	},

	isFloating: function() {
		return true;
	},

	isVirtual: function() {
		return true;
	},

	getDevices: function() {
		return null;
	},

	containsDevice: function(ifname) {
		return (network.getIfnameOf(ifname) == this.getIfname());
	},

	renderFormOptions: function(s) {
		var o, ss;

		// -- general ---------------------------------------------------------------------

		o = s.taboption('general', form.Value, 'private_key', _('Private Key'), _('Required. Base64-encoded private key for this interface.'));
		o.password = true;
		o.validate = validateBase64;
		o.rmempty = false;

		o = s.taboption('general', form.Value, 'listen_port', _('Listen Port'), _('Optional. UDP port used for outgoing and incoming packets.'));
		o.datatype = 'port';
		o.placeholder = _('random');
		o.optional = true;

		o = s.taboption('general', form.DynamicList, 'addresses', _('IP Addresses'), _('Recommended. IP addresses of the WireGuard interface.'));
		o.datatype = 'ipaddr';
		o.optional = true;


		// -- advanced --------------------------------------------------------------------

		o = s.taboption('advanced', form.Value, 'metric', _('Metric'), _('Optional'));
		o.datatype = 'uinteger';
		o.placeholder = '0';
		o.optional = true;

		o = s.taboption('advanced', form.Value, 'mtu', _('MTU'), _('Optional. Maximum Transmission Unit of tunnel interface.'));
		o.datatype = 'range(1280,1420)';
		o.placeholder = '1420';
		o.optional = true;

		o = s.taboption('advanced', form.Value, 'fwmark', _('Firewall Mark'), _('Optional. 32-bit mark for outgoing encrypted packets. Enter value in hex, starting with <code>0x</code>.'));
		o.optional = true;
		o.validate = function(section_id, value) {
			if (value.length > 0 && !value.match(/^0x[a-fA-F0-9]{1,4}$/))
				return _('Invalid hexadecimal value');

			return true;
		};


		// -- peers -----------------------------------------------------------------------

		try {
			s.tab('peers', _('Peers'), _('Further information about WireGuard interfaces and peers at <a href=\'http://wireguard.com\'>wireguard.com</a>.'));
		}
		catch(e) {}

		o = s.taboption('peers', form.SectionValue, '_peers', form.TypedSection, 'wireguard_%s'.format(s.section));
		o.depends('proto', 'wireguard');

		ss = o.subsection;
		ss.anonymous = true;
		ss.addremove = true;
		ss.addbtntitle = _('Add peer');

		ss.renderSectionPlaceholder = function() {
			return E([], [
				E('br'),
				E('em', _('No peers defined yet'))
			]);
		};

		o = ss.option(form.Value, 'description', _('Description'), _('Optional. Description of peer.'));
		o.placeholder = 'My Peer';
		o.datatype = 'string';
		o.optional = true;

		o = ss.option(form.Value, 'public_key', _('Public Key'), _('Required. Base64-encoded public key of peer.'));
		o.validate = validateBase64;
		o.rmempty = false;

		o = ss.option(form.Value, 'preshared_key', _('Preshared Key'), _('Optional. Base64-encoded preshared key. Adds in an additional layer of symmetric-key cryptography for post-quantum resistance.'));
		o.password = true;
		o.validate = validateBase64;
		o.optional = true;

		o = ss.option(form.DynamicList, 'allowed_ips', _('Allowed IPs'), _("Required. IP addresses and prefixes that this peer is allowed to use inside the tunnel. Usually the peer's tunnel IP addresses and the networks the peer routes through the tunnel."));
		o.datatype = 'ipaddr';
		o.rmempty = false;

		o = ss.option(form.Flag, 'route_allowed_ips', _('Route Allowed IPs'), _('Optional. Create routes for Allowed IPs for this peer.'));

		o = ss.option(form.Value, 'endpoint_host', _('Endpoint Host'), _('Optional. Host of peer. Names are resolved prior to bringing up the interface.'));
		o.placeholder = 'vpn.example.com';
		o.datatype = 'host';

		o = ss.option(form.Value, 'endpoint_port', _('Endpoint Port'), _('Optional. Port of peer.'));
		o.placeholder = '51820';
		o.datatype = 'port';

		o = ss.option(form.Value, 'persistent_keepalive', _('Persistent Keep Alive'), _('Optional. Seconds between keep alive messages. Default is 0 (disabled). Recommended value if this device is behind a NAT is 25.'));
		o.datatype = 'range(0,65535)';
		o.placeholder = '0';
	}
});
