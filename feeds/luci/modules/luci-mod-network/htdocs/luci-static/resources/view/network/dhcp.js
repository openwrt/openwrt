'use strict';
'require rpc';
'require uci';
'require form';
'require validation';

var callHostHints, callDUIDHints, callDHCPLeases, CBILeaseStatus, CBILease6Status;

callHostHints = rpc.declare({
	object: 'luci-rpc',
	method: 'getHostHints',
	expect: { '': {} }
});

callDUIDHints = rpc.declare({
	object: 'luci-rpc',
	method: 'getDUIDHints',
	expect: { '': {} }
});

callDHCPLeases = rpc.declare({
	object: 'luci-rpc',
	method: 'getDHCPLeases',
	expect: { '': {} }
});

CBILeaseStatus = form.DummyValue.extend({
	renderWidget: function(section_id, option_id, cfgvalue) {
		return E([
			E('h4', _('Active DHCP Leases')),
			E('div', { 'id': 'lease_status_table', 'class': 'table' }, [
				E('div', { 'class': 'tr table-titles' }, [
					E('div', { 'class': 'th' }, _('Hostname')),
					E('div', { 'class': 'th' }, _('IPv4-Address')),
					E('div', { 'class': 'th' }, _('MAC-Address')),
					E('div', { 'class': 'th' }, _('Leasetime remaining'))
				]),
				E('div', { 'class': 'tr placeholder' }, [
					E('div', { 'class': 'td' }, E('em', _('Collecting data...')))
				])
			])
		]);
	}
});

CBILease6Status = form.DummyValue.extend({
	renderWidget: function(section_id, option_id, cfgvalue) {
		return E([
			E('h4', _('Active DHCPv6 Leases')),
			E('div', { 'id': 'lease6_status_table', 'class': 'table' }, [
				E('div', { 'class': 'tr table-titles' }, [
					E('div', { 'class': 'th' }, _('Host')),
					E('div', { 'class': 'th' }, _('IPv6-Address')),
					E('div', { 'class': 'th' }, _('DUID')),
					E('div', { 'class': 'th' }, _('Leasetime remaining'))
				]),
				E('div', { 'class': 'tr placeholder' }, [
					E('div', { 'class': 'td' }, E('em', _('Collecting data...')))
				])
			])
		]);
	}
});

function validateHostname(sid, s) {
	if (s.length > 256)
		return _('Expecting: %s').format(_('valid hostname'));

	var labels = s.replace(/^\.+|\.$/g, '').split(/\./);

	for (var i = 0; i < labels.length; i++)
		if (!labels[i].match(/^[a-z0-9_](?:[a-z0-9-]{0,61}[a-z0-9])?$/i))
			return _('Expecting: %s').format(_('valid hostname'));

	return true;
}

function validateAddressList(sid, s) {
	if (s == null || s == '')
		return true;

	var m = s.match(/^\/(.+)\/$/),
	    names = m ? m[1].split(/\//) : [ s ];

	for (var i = 0; i < names.length; i++) {
		var res = validateHostname(sid, names[i]);

		if (res !== true)
			return res;
	}

	return true;
}

function validateServerSpec(sid, s) {
	if (s == null || s == '')
		return true;

	var m = s.match(/^\/(.+)\/(.*)$/);
	if (!m)
		return _('Expecting: %s').format(_('valid hostname'));

	var res = validateAddressList(sid, m[1]);
	if (res !== true)
		return res;

	if (m[2] == '' || m[2] == '#')
		return true;

	// ipaddr%scopeid#srvport@source@interface#srcport

	m = m[2].match(/^([0-9a-f:.]+)(?:%[^#@]+)?(?:#(\d+))?(?:@([0-9a-f:.]+)(?:@[^#]+)?(?:#(\d+))?)?$/);

	if (!m)
		return _('Expecting: %s').format(_('valid IP address'));
	else if (validation.parseIPv4(m[1]) && m[3] != null && !validation.parseIPv4(m[3]))
		return _('Expecting: %s').format(_('valid IPv4 address'));
	else if (validation.parseIPv6(m[1]) && m[3] != null && !validation.parseIPv6(m[3]))
		return _('Expecting: %s').format(_('valid IPv6 address'));
	else if ((m[2] != null && +m[2] > 65535) || (m[4] != null && +m[4] > 65535))
		return _('Expecting: %s').format(_('valid port value'));

	return true;
}

return L.view.extend({
	load: function() {
		return Promise.all([
			callHostHints(),
			callDUIDHints()
		]);
	},

	render: function(hosts_duids) {
		var has_dhcpv6 = L.hasSystemFeature('dnsmasq', 'dhcpv6') || L.hasSystemFeature('odhcpd'),
		    hosts = hosts_duids[0],
		    duids = hosts_duids[1],
		    m, s, o, ss, so;

		m = new form.Map('dhcp', _('DHCP and DNS'), _('Dnsmasq is a combined <abbr title="Dynamic Host Configuration Protocol">DHCP</abbr>-Server and <abbr title="Domain Name System">DNS</abbr>-Forwarder for <abbr title="Network Address Translation">NAT</abbr> firewalls'));

		s = m.section(form.TypedSection, 'dnsmasq', _('Server Settings'));
		s.anonymous = true;
		s.addremove = false;

		s.tab('general', _('General Settings'));
		s.tab('files', _('Resolv and Hosts Files'));
		s.tab('tftp', _('TFTP Settings'));
		s.tab('advanced', _('Advanced Settings'));
		s.tab('leases', _('Static Leases'));

		s.taboption('general', form.Flag, 'domainneeded',
			_('Domain required'),
			_('Don\'t forward <abbr title="Domain Name System">DNS</abbr>-Requests without <abbr title="Domain Name System">DNS</abbr>-Name'));

		s.taboption('general', form.Flag, 'authoritative',
			_('Authoritative'),
			_('This is the only <abbr title="Dynamic Host Configuration Protocol">DHCP</abbr> in the local network'));


		s.taboption('files', form.Flag, 'readethers',
			_('Use <code>/etc/ethers</code>'),
			_('Read <code>/etc/ethers</code> to configure the <abbr title="Dynamic Host Configuration Protocol">DHCP</abbr>-Server'));

		s.taboption('files', form.Value, 'leasefile',
			_('Leasefile'),
			_('file where given <abbr title="Dynamic Host Configuration Protocol">DHCP</abbr>-leases will be stored'));

		s.taboption('files', form.Flag, 'noresolv',
			_('Ignore resolve file')).optional = true;

		o = s.taboption('files', form.Value, 'resolvfile',
			_('Resolve file'),
			_('local <abbr title="Domain Name System">DNS</abbr> file'));

		o.depends('noresolv', '0');
		o.placeholder = '/tmp/resolv.conf.auto';
		o.optional = true;


		s.taboption('files', form.Flag, 'nohosts',
			_('Ignore <code>/etc/hosts</code>')).optional = true;

		s.taboption('files', form.DynamicList, 'addnhosts',
			_('Additional Hosts files')).optional = true;

		o = s.taboption('advanced', form.Flag, 'quietdhcp',
			_('Suppress logging'),
			_('Suppress logging of the routine operation of these protocols'));
		o.optional = true;

		o = s.taboption('advanced', form.Flag, 'sequential_ip',
			_('Allocate IP sequentially'),
			_('Allocate IP addresses sequentially, starting from the lowest available address'));
		o.optional = true;

		o = s.taboption('advanced', form.Flag, 'boguspriv',
			_('Filter private'),
			_('Do not forward reverse lookups for local networks'));
		o.default = o.enabled;

		s.taboption('advanced', form.Flag, 'filterwin2k',
			_('Filter useless'),
			_('Do not forward requests that cannot be answered by public name servers'));


		s.taboption('advanced', form.Flag, 'localise_queries',
			_('Localise queries'),
			_('Localise hostname depending on the requesting subnet if multiple IPs are available'));

		if (L.hasSystemFeature('dnsmasq', 'dnssec')) {
			o = s.taboption('advanced', form.Flag, 'dnssec',
				_('DNSSEC'));
			o.optional = true;

			o = s.taboption('advanced', form.Flag, 'dnsseccheckunsigned',
				_('DNSSEC check unsigned'),
				_('Requires upstream supports DNSSEC; verify unsigned domain responses really come from unsigned domains'));
			o.optional = true;
		}

		s.taboption('general', form.Value, 'local',
			_('Local server'),
			_('Local domain specification. Names matching this domain are never forwarded and are resolved from DHCP or hosts files only'));

		s.taboption('general', form.Value, 'domain',
			_('Local domain'),
			_('Local domain suffix appended to DHCP names and hosts file entries'));

		s.taboption('advanced', form.Flag, 'expandhosts',
			_('Expand hosts'),
			_('Add local domain suffix to names served from hosts files'));

		s.taboption('advanced', form.Flag, 'nonegcache',
			_('No negative cache'),
			_('Do not cache negative replies, e.g. for not existing domains'));

		s.taboption('advanced', form.Value, 'serversfile',
			_('Additional servers file'),
			_('This file may contain lines like \'server=/domain/1.2.3.4\' or \'server=1.2.3.4\' for domain-specific or full upstream <abbr title="Domain Name System">DNS</abbr> servers.'));

		s.taboption('advanced', form.Flag, 'strictorder',
			_('Strict order'),
			_('<abbr title="Domain Name System">DNS</abbr> servers will be queried in the order of the resolvfile')).optional = true;

		s.taboption('advanced', form.Flag, 'allservers',
			_('All Servers'),
			_('Query all available upstream <abbr title="Domain Name System">DNS</abbr> servers')).optional = true;

		o = s.taboption('advanced', form.DynamicList, 'bogusnxdomain', _('Bogus NX Domain Override'),
			_('List of hosts that supply bogus NX domain results'));

		o.optional = true;
		o.placeholder = '67.215.65.132';


		s.taboption('general', form.Flag, 'logqueries',
			_('Log queries'),
			_('Write received DNS requests to syslog')).optional = true;

		o = s.taboption('general', form.DynamicList, 'server', _('DNS forwardings'),
			_('List of <abbr title="Domain Name System">DNS</abbr> servers to forward requests to'));

		o.optional = true;
		o.placeholder = '/example.org/10.1.2.3';
		o.validate = validateServerSpec;


		o = s.taboption('general', form.Flag, 'rebind_protection',
			_('Rebind protection'),
			_('Discard upstream RFC1918 responses'));

		o.rmempty = false;


		o = s.taboption('general', form.Flag, 'rebind_localhost',
			_('Allow localhost'),
			_('Allow upstream responses in the 127.0.0.0/8 range, e.g. for RBL services'));

		o.depends('rebind_protection', '1');


		o = s.taboption('general', form.DynamicList, 'rebind_domain',
			_('Domain whitelist'),
			_('List of domains to allow RFC1918 responses for'));
		o.optional = true;

		o.depends('rebind_protection', '1');
		o.placeholder = 'ihost.netflix.com';
		o.validate = validateAddressList;


		o = s.taboption('advanced', form.Value, 'port',
			_('<abbr title="Domain Name System">DNS</abbr> server port'),
			_('Listening port for inbound DNS queries'));

		o.optional = true;
		o.datatype = 'port';
		o.placeholder = 53;


		o = s.taboption('advanced', form.Value, 'queryport',
			_('<abbr title="Domain Name System">DNS</abbr> query port'),
			_('Fixed source port for outbound DNS queries'));

		o.optional = true;
		o.datatype = 'port';
		o.placeholder = _('any');


		o = s.taboption('advanced', form.Value, 'dhcpleasemax',
			_('<abbr title="maximal">Max.</abbr> <abbr title="Dynamic Host Configuration Protocol">DHCP</abbr> leases'),
			_('Maximum allowed number of active DHCP leases'));

		o.optional = true;
		o.datatype = 'uinteger';
		o.placeholder = _('unlimited');


		o = s.taboption('advanced', form.Value, 'ednspacket_max',
			_('<abbr title="maximal">Max.</abbr> <abbr title="Extension Mechanisms for Domain Name System">EDNS0</abbr> packet size'),
			_('Maximum allowed size of EDNS.0 UDP packets'));

		o.optional = true;
		o.datatype = 'uinteger';
		o.placeholder = 1280;


		o = s.taboption('advanced', form.Value, 'dnsforwardmax',
			_('<abbr title="maximal">Max.</abbr> concurrent queries'),
			_('Maximum allowed number of concurrent DNS queries'));

		o.optional = true;
		o.datatype = 'uinteger';
		o.placeholder = 150;

		o = s.taboption('advanced', form.Value, 'cachesize',
			_('Size of DNS query cache'),
			_('Number of cached DNS entries (max is 10000, 0 is no caching)'));
		o.optional = true;
		o.datatype = 'range(0,10000)';
		o.placeholder = 150;

		s.taboption('tftp', form.Flag, 'enable_tftp',
			_('Enable TFTP server')).optional = true;

		o = s.taboption('tftp', form.Value, 'tftp_root',
			_('TFTP server root'),
			_('Root directory for files served via TFTP'));

		o.optional = true;
		o.depends('enable_tftp', '1');
		o.placeholder = '/';


		o = s.taboption('tftp', form.Value, 'dhcp_boot',
			_('Network boot image'),
			_('Filename of the boot image advertised to clients'));

		o.optional = true;
		o.depends('enable_tftp', '1');
		o.placeholder = 'pxelinux.0';

		o = s.taboption('general', form.Flag, 'localservice',
			_('Local Service Only'),
			_('Limit DNS service to subnets interfaces on which we are serving DNS.'));
		o.optional = false;
		o.rmempty = false;

		o = s.taboption('general', form.Flag, 'nonwildcard',
			_('Non-wildcard'),
			_('Bind dynamically to interfaces rather than wildcard address (recommended as linux default)'));
		o.default = o.enabled;
		o.optional = false;
		o.rmempty = true;

		o = s.taboption('general', form.DynamicList, 'interface',
			_('Listen Interfaces'),
			_('Limit listening to these interfaces, and loopback.'));
		o.optional = true;

		o = s.taboption('general', form.DynamicList, 'notinterface',
			_('Exclude interfaces'),
			_('Prevent listening on these interfaces.'));
		o.optional = true;

		o = s.taboption('leases', form.SectionValue, '__leases__', form.GridSection, 'host', null,
			_('Static leases are used to assign fixed IP addresses and symbolic hostnames to DHCP clients. They are also required for non-dynamic interface configurations where only hosts with a corresponding lease are served.') + '<br />' +
			_('Use the <em>Add</em> Button to add a new lease entry. The <em>MAC-Address</em> identifies the host, the <em>IPv4-Address</em> specifies the fixed address to use, and the <em>Hostname</em> is assigned as a symbolic name to the requesting host. The optional <em>Lease time</em> can be used to set non-standard host-specific lease time, e.g. 12h, 3d or infinite.'));

		ss = o.subsection;

		ss.addremove = true;
		ss.anonymous = true;

		so = ss.option(form.Value, 'name', _('Hostname'));
		so.datatype = 'hostname("strict")';
		so.rmempty  = true;
		so.write = function(section, value) {
			uci.set('dhcp', section, 'name', value);
			uci.set('dhcp', section, 'dns', '1');
		};
		so.remove = function(section) {
			uci.unset('dhcp', section, 'name');
			uci.unset('dhcp', section, 'dns');
		};

		so = ss.option(form.Value, 'mac', _('<abbr title="Media Access Control">MAC</abbr>-Address'));
		so.datatype = 'list(unique(macaddr))';
		so.rmempty  = true;
		so.cfgvalue = function(section) {
			var macs = uci.get('dhcp', section, 'mac'),
			    result = [];

			if (!Array.isArray(macs))
				macs = (macs != null && macs != '') ? macs.split(/\ss+/) : [];

			for (var i = 0, mac; (mac = macs[i]) != null; i++)
				if (/^([0-9a-fA-F]{1,2}):([0-9a-fA-F]{1,2}):([0-9a-fA-F]{1,2}):([0-9a-fA-F]{1,2}):([0-9a-fA-F]{1,2}):([0-9a-fA-F]{1,2})$/.test(mac))
					result.push('%02X:%02X:%02X:%02X:%02X:%02X'.format(
						parseInt(RegExp.$1, 16), parseInt(RegExp.$2, 16),
						parseInt(RegExp.$3, 16), parseInt(RegExp.$4, 16),
						parseInt(RegExp.$5, 16), parseInt(RegExp.$6, 16)));

			return result.length ? result.join(' ') : null;
		};
		so.renderWidget = function(section_id, option_index, cfgvalue) {
			var node = form.Value.prototype.renderWidget.apply(this, [section_id, option_index, cfgvalue]),
			    ipopt = this.section.children.filter(function(o) { return o.option == 'ip' })[0];

			node.addEventListener('cbi-dropdown-change', L.bind(function(ipopt, section_id, ev) {
				var mac = ev.detail.value.value;
				if (mac == null || mac == '' || !hosts[mac] || !hosts[mac].ipv4)
					return;

				var ip = ipopt.formvalue(section_id);
				if (ip != null && ip != '')
					return;

				var node = ipopt.map.findElement('id', ipopt.cbid(section_id));
				if (node)
					L.dom.callClassMethod(node, 'setValue', hosts[mac].ipv4);
			}, this, ipopt, section_id));

			return node;
		};
		Object.keys(hosts).forEach(function(mac) {
			var hint = hosts[mac].name || hosts[mac].ipv4;
			so.value(mac, hint ? '%s (%s)'.format(mac, hint) : mac);
		});

		so = ss.option(form.Value, 'ip', _('<abbr title="Internet Protocol Version 4">IPv4</abbr>-Address'));
		so.datatype = 'or(ip4addr,"ignore")';
		so.validate = function(section, value) {
			var mac = this.map.lookupOption('mac', section),
			    name = this.map.lookupOption('name', section),
			    m = mac ? mac[0].formvalue(section) : null,
			    n = name ? name[0].formvalue(section) : null;

			if ((m == null || m == '') && (n == null || n == ''))
				return _('One of hostname or mac address must be specified!');

			return true;
		};
		Object.keys(hosts).forEach(function(mac) {
			if (hosts[mac].ipv4) {
				var hint = hosts[mac].name;
				so.value(hosts[mac].ipv4, hint ? '%s (%s)'.format(hosts[mac].ipv4, hint) : hosts[mac].ipv4);
			}
		});

		so = ss.option(form.Value, 'leasetime', _('Lease time'));
		so.rmempty = true;

		so = ss.option(form.Value, 'duid', _('<abbr title="The DHCP Unique Identifier">DUID</abbr>'));
		so.datatype = 'and(rangelength(20,36),hexstring)';
		Object.keys(duids).forEach(function(duid) {
			so.value(duid, '%s (%s)'.format(duid, duids[duid].hostname || duids[duid].macaddr || duids[duid].ip6addr || '?'));
		});

		so = ss.option(form.Value, 'hostid', _('<abbr title="Internet Protocol Version 6">IPv6</abbr>-Suffix (hex)'));

		o = s.taboption('leases', CBILeaseStatus, '__status__');

		if (has_dhcpv6)
			o = s.taboption('leases', CBILease6Status, '__status6__');

		return m.render().then(function(mapEl) {
			L.Poll.add(function() {
				return callDHCPLeases().then(function(leaseinfo) {
					var leases = Array.isArray(leaseinfo.dhcp_leases) ? leaseinfo.dhcp_leases : [],
					    leases6 = Array.isArray(leaseinfo.dhcp6_leases) ? leaseinfo.dhcp6_leases : [];

					cbi_update_table(mapEl.querySelector('#lease_status_table'),
						leases.map(function(lease) {
							var exp;

							if (lease.expires === false)
								exp = E('em', _('unlimited'));
							else if (lease.expires <= 0)
								exp = E('em', _('expired'));
							else
								exp = '%t'.format(lease.expires);

							return [
								lease.hostname || '?',
								lease.ipaddr,
								lease.macaddr,
								exp
							];
						}),
						E('em', _('There are no active leases')));

					if (has_dhcpv6) {
						cbi_update_table(mapEl.querySelector('#lease6_status_table'),
							leases6.map(function(lease) {
								var exp;

								if (lease.expires === false)
									exp = E('em', _('unlimited'));
								else if (lease.expires <= 0)
									exp = E('em', _('expired'));
								else
									exp = '%t'.format(lease.expires);

								var hint = lease.macaddr ? hosts[lease.macaddr] : null,
								    name = hint ? (hint.name || hint.ipv4 || hint.ipv6) : null,
								    host = null;

								if (name && lease.hostname && lease.hostname != name && lease.ip6addr != name)
									host = '%s (%s)'.format(lease.hostname, name);
								else if (lease.hostname)
									host = lease.hostname;
								else if (name)
									host = name;

								return [
									host || '-',
									lease.ip6addrs ? lease.ip6addrs.join(' ') : lease.ip6addr,
									lease.duid,
									exp
								];
							}),
							E('em', _('There are no active leases')));
					}
				});
			});

			return mapEl;
		});
	}
});
