'use strict';
'require uci';
'require rpc';
'require tools.prng as random';


function initFirewallState() {
	return uci.load('firewall');
}

function parseEnum(s, values) {
	if (s == null)
		return null;

	s = String(s).toUpperCase();

	if (s == '')
		return null;

	for (var i = 0; i < values.length; i++)
		if (values[i].toUpperCase().indexOf(s) == 0)
			return values[i];

	return null;
}

function parsePolicy(s, defaultValue) {
	return parseEnum(s, ['DROP', 'REJECT', 'ACCEPT']) || (arguments.length < 2 ? null : defaultValue);
}


var Firewall, AbstractFirewallItem, Defaults, Zone, Forwarding, Redirect, Rule;

function lookupZone(name) {
	var z = uci.get('firewall', name);

	if (z != null && z['.type'] == 'zone')
		return new Zone(z['.name']);

	var sections = uci.sections('firewall', 'zone');

	for (var i = 0; i < sections.length; i++) {
		if (sections[i].name != name)
			continue;

		return new Zone(sections[i]['.name']);
	}

	return null;
}

function getColorForName(forName) {
	if (forName == null)
		return '#eeeeee';
	else if (forName == 'lan')
		return '#90f090';
	else if (forName == 'wan')
		return '#f09090';

	random.seed(parseInt(sfh(forName), 16));

	var r = random.get(128),
	    g = random.get(128),
	    min = 0,
	    max = 128;

	if ((r + g) < 128)
		min = 128 - r - g;
	else
		max = 255 - r - g;

	var b = min + Math.floor(random.get() * (max - min));

	return '#%02x%02x%02x'.format(0xff - r, 0xff - g, 0xff - b);
}


Firewall = L.Class.extend({
	getDefaults: function() {
		return initFirewallState().then(function() {
			return new Defaults();
		});
	},

	newZone: function() {
		return initFirewallState().then(L.bind(function() {
			var name = 'newzone',
			    count = 1;

			while (this.getZone(name) != null)
				name = 'newzone%d'.format(++count);

			return this.addZone(name);
		}, this));
	},

	addZone: function(name) {
		return initFirewallState().then(L.bind(function() {
			if (name == null || !/^[a-zA-Z0-9_]+$/.test(name))
				return null;

			if (lookupZone(name) != null)
				return null;

			var d = new Defaults(),
			    z = uci.add('firewall', 'zone');

			uci.set('firewall', z, 'name',    name);
			uci.set('firewall', z, 'network', ' ');
			uci.set('firewall', z, 'input',   d.getInput()   || 'DROP');
			uci.set('firewall', z, 'output',  d.getOutput()  || 'DROP');
			uci.set('firewall', z, 'forward', d.getForward() || 'DROP');

			return new Zone(z);
		}, this));
	},

	getZone: function(name) {
		return initFirewallState().then(function() {
			return lookupZone(name);
		});
	},

	getZones: function() {
		return initFirewallState().then(function() {
			var sections = uci.sections('firewall', 'zone'),
			    zones = [];

			for (var i = 0; i < sections.length; i++)
				zones.push(new Zone(sections[i]['.name']));

			zones.sort(function(a, b) { return a.getName() > b.getName() });

			return zones;
		});
	},

	getZoneByNetwork: function(network) {
		return initFirewallState().then(function() {
			var sections = uci.sections('firewall', 'zone');

			for (var i = 0; i < sections.length; i++)
				if (L.toArray(sections[i].network || sections[i].name).indexOf(network) != -1)
					return new Zone(sections[i]['.name']);

			return null;
		});
	},

	deleteZone: function(name) {
		return initFirewallState().then(function() {
			var section = uci.get('firewall', name),
			    found = false;

			if (section != null && section['.type'] == 'zone') {
				found = true;
				name = zone.name;
				uci.remove('firewall', zone['.name']);
			}
			else if (name != null) {
				var sections = uci.sections('firewall', 'zone');

				for (var i = 0; i < sections.length; i++) {
					if (sections[i].name != name)
						continue;

					found = true;
					uci.remove('firewall', sections[i]['.name']);
				}
			}

			if (found == true) {
				sections = uci.sections('firewall');

				for (var i = 0; i < sections.length; i++) {
					if (sections[i]['.type'] != 'rule' &&
					    sections[i]['.type'] != 'redirect' &&
					    sections[i]['.type'] != 'forwarding')
					    continue;

					if (sections[i].src == name || sections[i].dest == name)
						uci.remove('firewall', sections[i]['.name']);
				}
			}

			return found;
		});
	},

	renameZone: function(oldName, newName) {
		return initFirewallState().then(L.bind(function() {
			if (oldName == null || newName == null || !/^[a-zA-Z0-9_]+$/.test(newName))
				return false;

			if (lookupZone(newName) != null)
				return false;

			var sections = uci.sections('firewall', 'zone'),
			    found = false;

			for (var i = 0; i < sections.length; i++) {
				if (sections[i].name != oldName)
					continue;

				if (L.toArray(sections[i].network).length == 0)
					uci.set('firewall', sections[i]['.name'], 'network', oldName);

				uci.set('firewall', sections[i]['.name'], 'name', newName);
				found = true;
			}

			if (found == true) {
				sections = uci.sections('firewall');

				for (var i = 0; i < sections.length; i++) {
					if (sections[i]['.type'] != 'rule' &&
					    sections[i]['.type'] != 'redirect' &&
					    sections[i]['.type'] != 'forwarding')
					    continue;

					if (sections[i].src == oldName)
						uci.set('firewall', sections[i]['.name'], 'src', newName);

					if (sections[i].dest == oldName)
						uci.set('firewall', sections[i]['.name'], 'dest', newName);
				}
			}

			return found;
		}, this));
	},

	deleteNetwork: function(network) {
		return this.getZones().then(L.bind(function(zones) {
			var rv = false;

			for (var i = 0; i < zones.length; i++)
				if (zones[i].deleteNetwork(network))
					rv = true;

			return rv;
		}, this));
	},

	getColorForName: getColorForName
});


AbstractFirewallItem = L.Class.extend({
	get: function(option) {
		return uci.get('firewall', this.sid, option);
	},

	set: function(option, value) {
		return uci.set('firewall', this.sid, option, value);
	}
});


Defaults = AbstractFirewallItem.extend({
	__init__: function() {
		var sections = uci.sections('firewall', 'defaults');

		for (var i = 0; i < sections.length; i++) {
			this.sid = sections[i]['.name'];
			break;
		}

		if (this.sid == null)
			this.sid = uci.add('firewall', 'defaults');
	},

	isSynFlood: function() {
		return (this.get('syn_flood') == '1');
	},

	isDropInvalid: function() {
		return (this.get('drop_invalid') == '1');
	},

	getInput: function() {
		return parsePolicy(this.get('input'), 'DROP');
	},

	getOutput: function() {
		return parsePolicy(this.get('output'), 'DROP');
	},

	getForward: function() {
		return parsePolicy(this.get('forward'), 'DROP');
	}
});


Zone = AbstractFirewallItem.extend({
	__init__: function(name) {
		var section = uci.get('firewall', name);

		if (section != null && section['.type'] == 'zone') {
			this.sid  = name;
			this.data = section;
		}
		else if (name != null) {
			var sections = uci.get('firewall', 'zone');

			for (var i = 0; i < sections.length; i++) {
				if (sections[i].name != name)
					continue;

				this.sid  = sections[i]['.name'];
				this.data = sections[i];
				break;
			}
		}
	},

	isMasquerade: function() {
		return (this.get('masq') == '1');
	},

	getName: function() {
		return this.get('name');
	},

	getNetwork: function() {
		return this.get('network');
	},

	getInput: function() {
		return parsePolicy(this.get('input'), (new Defaults()).getInput());
	},

	getOutput: function() {
		return parsePolicy(this.get('output'), (new Defaults()).getOutput());
	},

	getForward: function() {
		return parsePolicy(this.get('forward'), (new Defaults()).getForward());
	},

	addNetwork: function(network) {
		var section = uci.get('network', network);

		if (section == null || section['.type'] != 'interface')
			return false;

		var newNetworks = this.getNetworks();

		if (newNetworks.filter(function(net) { return net == network }).length)
			return false;

		newNetworks.push(network);
		this.set('network', newNetworks.join(' '));

		return true;
	},

	deleteNetwork: function(network) {
		var oldNetworks = this.getNetworks(),
            newNetworks = oldNetworks.filter(function(net) { return net != network });

		if (newNetworks.length > 0)
			this.set('network', newNetworks.join(' '));
		else
			this.set('network', ' ');

		return (newNetworks.length < oldNetworks.length);
	},

	getNetworks: function() {
		return L.toArray(this.get('network') || this.get('name'));
	},

	clearNetworks: function() {
		this.set('network', ' ');
	},

	getDevices: function() {
		return L.toArray(this.get('device'));
	},

	getSubnets: function() {
		return L.toArray(this.get('subnet'));
	},

	getForwardingsBy: function(what) {
		var sections = uci.sections('firewall', 'forwarding'),
		    forwards = [];

		for (var i = 0; i < sections.length; i++) {
			if (sections[i].src == null || sections[i].dest == null)
				continue;

			if (sections[i][what] != this.getName())
				continue;

			forwards.push(new Forwarding(sections[i]['.name']));
		}

		return forwards;
	},

	addForwardingTo: function(dest) {
		var forwards = this.getForwardingsBy('src'),
		    zone = lookupZone(dest);

		if (zone == null || zone.getName() == this.getName())
			return null;

		for (var i = 0; i < forwards.length; i++)
			if (forwards[i].getDestination() == zone.getName())
				return null;

		var sid = uci.add('firewall', 'forwarding');

		uci.set('firewall', sid, 'src', this.getName());
		uci.set('firewall', sid, 'dest', zone.getName());

		return new Forwarding(sid);
	},

	addForwardingFrom: function(src) {
		var forwards = this.getForwardingsBy('dest'),
		    zone = lookupZone(src);

		if (zone == null || zone.getName() == this.getName())
			return null;

		for (var i = 0; i < forwards.length; i++)
			if (forwards[i].getSource() == zone.getName())
				return null;

		var sid = uci.add('firewall', 'forwarding');

		uci.set('firewall', sid, 'src', zone.getName());
		uci.set('firewall', sid, 'dest', this.getName());

		return new Forwarding(sid);
	},

	deleteForwardingsBy: function(what) {
		var sections = uci.sections('firewall', 'forwarding'),
		    found = false;

		for (var i = 0; i < sections.length; i++) {
			if (sections[i].src == null || sections[i].dest == null)
				continue;

			if (sections[i][what] != this.getName())
				continue;

			uci.remove('firewall', sections[i]['.name']);
			found = true;
		}

		return found;
	},

	deleteForwarding: function(forwarding) {
		if (!(forwarding instanceof Forwarding))
			return false;

		var section = uci.get('firewall', forwarding.sid);

		if (!section || section['.type'] != 'forwarding')
			return false;

		uci.remove('firewall', section['.name']);

		return true;
	},

	addRedirect: function(options) {
		var sid = uci.add('firewall', 'redirect');

		if (options != null && typeof(options) == 'object')
			for (var key in options)
				if (options.hasOwnProperty(key))
					uci.set('firewall', sid, key, options[key]);

		uci.set('firewall', sid, 'src', this.getName());

		return new Redirect(sid);
	},

	addRule: function(options) {
		var sid = uci.add('firewall', 'rule');

		if (options != null && typeof(options) == 'object')
			for (var key in options)
				if (options.hasOwnProperty(key))
					uci.set('firewall', sid, key, options[key]);

		uci.set('firewall', sid, 'src', this.getName());

		return new Redirect(sid);
	},

	getColor: function(forName) {
		var name = (arguments.length > 0 ? forName : this.getName());

		return getColorForName(name);
	}
});


Forwarding = AbstractFirewallItem.extend({
	__init__: function(sid) {
		this.sid = sid;
	},

	getSource: function() {
		return this.get('src');
	},

	getDestination: function() {
		return this.get('dest');
	},

	getSourceZone: function() {
		return lookupZone(this.getSource());
	},

	getDestinationZone: function() {
		return lookupZone(this.getDestination());
	}
});


Rule = AbstractFirewallItem.extend({
	getSource: function() {
		return this.get('src');
	},

	getDestination: function() {
		return this.get('dest');
	},

	getSourceZone: function() {
		return lookupZone(this.getSource());
	},

	getDestinationZone: function() {
		return lookupZone(this.getDestination());
	}
});


Redirect = AbstractFirewallItem.extend({
	getSource: function() {
		return this.get('src');
	},

	getDestination: function() {
		return this.get('dest');
	},

	getSourceZone: function() {
		return lookupZone(this.getSource());
	},

	getDestinationZone: function() {
		return lookupZone(this.getDestination());
	}
});


return Firewall;
