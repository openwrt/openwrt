'use strict';
'require rpc';
'require form';
'require network';

var callGetCertificateFiles = rpc.declare({
	object: 'luci.openconnect',
	method: 'getCertificates',
	params: [ 'interface' ],
	expect: { '': {} }
});

var callSetCertificateFiles = rpc.declare({
	object: 'luci.openconnect',
	method: 'setCertificates',
	params: [ 'interface', 'user_certificate', 'user_privatekey', 'ca_certificate' ],
	expect: { '': {} }
});

network.registerPatternVirtual(/^vpn-.+$/);

function sanitizeCert(s) {
	if (typeof(s) != 'string')
		return null;

	s = s.trim();

	if (s == '')
		return null;

	s = s.replace(/\r\n?/g, '\n');

	if (!s.match(/\n$/))
		s += '\n';

	return s;
}

function validateCert(priv, section_id, value) {
	var beg = priv ? /^-----BEGIN RSA PRIVATE KEY-----$/ : /^-----BEGIN CERTIFICATE-----$/,
	    end = priv ? /^-----END RSA PRIVATE KEY-----$/ : /^-----END CERTIFICATE-----$/,
	    lines = value.trim().split(/[\r\n]/),
	    start = false,
	    i;

	if (value === null || value === '')
		return true;

	for (i = 0; i < lines.length; i++) {
		if (lines[i].match(beg))
			start = true;
		else if (start && !lines[i].match(/^(?:[A-Za-z0-9+\/]{4})*(?:[A-Za-z0-9+\/]{2}==|[A-Za-z0-9+\/]{3}=)?$/))
			break;
	}

	if (!start || i < lines.length - 1 || !lines[i].match(end))
		return _('This does not look like a valid PEM file');

	return true;
}

return network.registerProtocol('openconnect', {
	getI18n: function() {
		return _('OpenConnect (CISCO AnyConnect)');
	},

	getIfname: function() {
		return this._ubus('l3_device') || 'vpn-%s'.format(this.sid);
	},

	getOpkgPackage: function() {
		return 'openconnect';
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
		var dev = this.getDevice().getName(),
		    certLoadPromise = null,
		    o;

		o = s.taboption('general', form.Value, 'server', _('VPN Server'));
		o.datatype = 'host(0)';

		o = s.taboption('general', form.Value, 'port', _('VPN Server port'));
		o.placeholder = '443';
		o.datatype    = 'port';

		s.taboption('general', form.Value, 'serverhash', _("VPN Server's certificate SHA1 hash"));
		s.taboption('general', form.Value, 'authgroup', _('Auth Group'));
		s.taboption("general", form.Value, "username", _("Username"));

		o = s.taboption('general', form.Value, 'password', _('Password'));
		o.password = true;

		o = s.taboption('general', form.Value, 'password2', _('Password2'));
		o.password = true;

		o = s.taboption('general', form.TextValue, 'usercert', _('User certificate (PEM encoded)'));
		o.rows = 10;
		o.monospace = true;
		o.validate = L.bind(validateCert, o, false);
		o.load = function(section_id) {
			certLoadPromise = certLoadPromise || callGetCertificateFiles(section_id);
			return certLoadPromise.then(function(certs) { return certs.user_certificate });
		};
		o.write = function(section_id, value) {
			return callSetCertificateFiles(section_id, sanitizeCert(value), null, null);
		};

		o = s.taboption('general', form.TextValue, 'userkey', _('User key (PEM encoded)'));
		o.rows = 10;
		o.monospace = true;
		o.validate = L.bind(validateCert, o, true);
		o.load = function(section_id) {
			certLoadPromise = certLoadPromise || callGetCertificateFiles(section_id);
			return certLoadPromise.then(function(certs) { return certs.user_privatekey });
		};
		o.write = function(section_id, value) {
			return callSetCertificateFiles(section_id, null, sanitizeCert(value), null);
		};

		o = s.taboption('general', form.TextValue, 'ca', _('CA certificate; if empty it will be saved after the first connection.'));
		o.rows = 10;
		o.monospace = true;
		o.validate = L.bind(validateCert, o, false);
		o.load = function(section_id) {
			certLoadPromise = certLoadPromise || callGetCertificateFiles(section_id);
			return certLoadPromise.then(function(certs) { return certs.ca_certificate });
		};
		o.write = function(section_id, value) {
			return callSetCertificateFiles(section_id, null, null, sanitizeCert(value));
		};

		o = s.taboption('advanced', form.Flag, 'defaultroute', _('Default gateway'), _('If unchecked, no default route is configured'));
		o.default = o.enabled;

		o = s.taboption('advanced', form.Value, 'metric', _('Use gateway metric'));
		o.placeholder = '0';
		o.datatype    = 'uinteger';
		o.depends('defaultroute', '1');

		o = s.taboption('advanced', form.Value, 'mtu', _('Override MTU'));
		o.optional = true;
		o.placeholder = 1406;
		o.datatype = 'range(68, 9200)';
	}
});
