'use strict';

var Validator = L.Class.extend({
	__name__: 'Validation',

	__init__: function(field, type, optional, vfunc, validatorFactory) {
		this.field = field;
		this.optional = optional;
		this.vfunc = vfunc;
		this.vstack = validatorFactory.compile(type);
		this.factory = validatorFactory;
	},

	assert: function(condition, message) {
		if (!condition) {
			this.field.classList.add('cbi-input-invalid');
			this.error = message;
			return false;
		}

		this.field.classList.remove('cbi-input-invalid');
		this.error = null;
		return true;
	},

	apply: function(name, value, args) {
		var func;

		if (typeof(name) === 'function')
			func = name;
		else if (typeof(this.factory.types[name]) === 'function')
			func = this.factory.types[name];
		else
			return false;

		if (value != null)
			this.value = value;

		return func.apply(this, args);
	},

	validate: function() {
		/* element is detached */
		if (!findParent(this.field, 'body') && !findParent(this.field, '[data-field]'))
			return true;

		this.field.classList.remove('cbi-input-invalid');
		this.value = (this.field.value != null) ? this.field.value : '';
		this.error = null;

		var valid;

		if (this.value.length === 0)
			valid = this.assert(this.optional, _('non-empty value'));
		else
			valid = this.vstack[0].apply(this, this.vstack[1]);

		if (valid !== true) {
			this.field.setAttribute('data-tooltip', _('Expecting: %s').format(this.error));
			this.field.setAttribute('data-tooltip-style', 'error');
			this.field.dispatchEvent(new CustomEvent('validation-failure', { bubbles: true }));
			return false;
		}

		if (typeof(this.vfunc) == 'function')
			valid = this.vfunc(this.value);

		if (valid !== true) {
			this.assert(false, valid);
			this.field.setAttribute('data-tooltip', valid);
			this.field.setAttribute('data-tooltip-style', 'error');
			this.field.dispatchEvent(new CustomEvent('validation-failure', { bubbles: true }));
			return false;
		}

		this.field.removeAttribute('data-tooltip');
		this.field.removeAttribute('data-tooltip-style');
		this.field.dispatchEvent(new CustomEvent('validation-success', { bubbles: true }));
		return true;
	},

});

var ValidatorFactory = L.Class.extend({
	__name__: 'ValidatorFactory',

	create: function(field, type, optional, vfunc) {
		return new Validator(field, type, optional, vfunc, this);
	},

	compile: function(code) {
		var pos = 0;
		var esc = false;
		var depth = 0;
		var stack = [ ];

		code += ',';

		for (var i = 0; i < code.length; i++) {
			if (esc) {
				esc = false;
				continue;
			}

			switch (code.charCodeAt(i))
			{
			case 92:
				esc = true;
				break;

			case 40:
			case 44:
				if (depth <= 0) {
					if (pos < i) {
						var label = code.substring(pos, i);
							label = label.replace(/\\(.)/g, '$1');
							label = label.replace(/^[ \t]+/g, '');
							label = label.replace(/[ \t]+$/g, '');

						if (label && !isNaN(label)) {
							stack.push(parseFloat(label));
						}
						else if (label.match(/^(['"]).*\1$/)) {
							stack.push(label.replace(/^(['"])(.*)\1$/, '$2'));
						}
						else if (typeof this.types[label] == 'function') {
							stack.push(this.types[label]);
							stack.push(null);
						}
						else {
							L.raise('SyntaxError', 'Unhandled token "%s"', label);
						}
					}

					pos = i+1;
				}

				depth += (code.charCodeAt(i) == 40);
				break;

			case 41:
				if (--depth <= 0) {
					if (typeof stack[stack.length-2] != 'function')
						L.raise('SyntaxError', 'Argument list follows non-function');

					stack[stack.length-1] = this.compile(code.substring(pos, i));
					pos = i+1;
				}

				break;
			}
		}

		return stack;
	},

	parseInteger: function(x) {
		return (/^-?\d+$/.test(x) ? +x : NaN);
	},

	parseDecimal: function(x) {
		return (/^-?\d+(?:\.\d+)?$/.test(x) ? +x : NaN);
	},

	parseIPv4: function(x) {
		if (!x.match(/^(\d{1,3})\.(\d{1,3})\.(\d{1,3})\.(\d{1,3})$/))
			return null;

		if (RegExp.$1 > 255 || RegExp.$2 > 255 || RegExp.$3 > 255 || RegExp.$4 > 255)
			return null;

		return [ +RegExp.$1, +RegExp.$2, +RegExp.$3, +RegExp.$4 ];
	},

	parseIPv6: function(x) {
		if (x.match(/^([a-fA-F0-9:]+):(\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3})$/)) {
			var v6 = RegExp.$1, v4 = this.parseIPv4(RegExp.$2);

			if (!v4)
				return null;

			x = v6 + ':' + (v4[0] * 256 + v4[1]).toString(16)
			       + ':' + (v4[2] * 256 + v4[3]).toString(16);
		}

		if (!x.match(/^[a-fA-F0-9:]+$/))
			return null;

		var prefix_suffix = x.split(/::/);

		if (prefix_suffix.length > 2)
			return null;

		var prefix = (prefix_suffix[0] || '0').split(/:/);
		var suffix = prefix_suffix.length > 1 ? (prefix_suffix[1] || '0').split(/:/) : [];

		if (suffix.length ? (prefix.length + suffix.length > 7)
		                  : ((prefix_suffix.length < 2 && prefix.length < 8) || prefix.length > 8))
			return null;

		var i, word;
		var words = [];

		for (i = 0, word = parseInt(prefix[0], 16); i < prefix.length; word = parseInt(prefix[++i], 16))
			if (prefix[i].length <= 4 && !isNaN(word) && word <= 0xFFFF)
				words.push(word);
			else
				return null;

		for (i = 0; i < (8 - prefix.length - suffix.length); i++)
			words.push(0);

		for (i = 0, word = parseInt(suffix[0], 16); i < suffix.length; word = parseInt(suffix[++i], 16))
			if (suffix[i].length <= 4 && !isNaN(word) && word <= 0xFFFF)
				words.push(word);
			else
				return null;

		return words;
	},

	types: {
		integer: function() {
			return this.assert(!isNaN(this.factory.parseInteger(this.value)), _('valid integer value'));
		},

		uinteger: function() {
			return this.assert(this.factory.parseInteger(this.value) >= 0, _('positive integer value'));
		},

		float: function() {
			return this.assert(!isNaN(this.factory.parseDecimal(this.value)), _('valid decimal value'));
		},

		ufloat: function() {
			return this.assert(this.factory.parseDecimal(this.value) >= 0, _('positive decimal value'));
		},

		ipaddr: function(nomask) {
			return this.assert(this.apply('ip4addr', null, [nomask]) || this.apply('ip6addr', null, [nomask]),
				nomask ? _('valid IP address') : _('valid IP address or prefix'));
		},

		ip4addr: function(nomask) {
			var re = nomask ? /^(\d+\.\d+\.\d+\.\d+)$/ : /^(\d+\.\d+\.\d+\.\d+)(?:\/(\d+\.\d+\.\d+\.\d+)|\/(\d{1,2}))?$/,
			    m = this.value.match(re);

			return this.assert(m && this.factory.parseIPv4(m[1]) && (m[2] ? this.factory.parseIPv4(m[2]) : (m[3] ? this.apply('ip4prefix', m[3]) : true)),
				nomask ? _('valid IPv4 address') : _('valid IPv4 address or network'));
		},

		ip6addr: function(nomask) {
			var re = nomask ? /^([0-9a-fA-F:.]+)$/ : /^([0-9a-fA-F:.]+)(?:\/(\d{1,3}))?$/,
			    m = this.value.match(re);

			return this.assert(m && this.factory.parseIPv6(m[1]) && (m[2] ? this.apply('ip6prefix', m[2]) : true),
				nomask ? _('valid IPv6 address') : _('valid IPv6 address or prefix'));
		},

		ip4prefix: function() {
			return this.assert(!isNaN(this.value) && this.value >= 0 && this.value <= 32,
				_('valid IPv4 prefix value (0-32)'));
		},

		ip6prefix: function() {
			return this.assert(!isNaN(this.value) && this.value >= 0 && this.value <= 128,
				_('valid IPv6 prefix value (0-128)'));
		},

		cidr: function() {
			return this.assert(this.apply('cidr4') || this.apply('cidr6'), _('valid IPv4 or IPv6 CIDR'));
		},

		cidr4: function() {
			var m = this.value.match(/^(\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3})\/(\d{1,2})$/);
			return this.assert(m && this.factory.parseIPv4(m[1]) && this.apply('ip4prefix', m[2]), _('valid IPv4 CIDR'));
		},

		cidr6: function() {
			var m = this.value.match(/^([0-9a-fA-F:.]+)\/(\d{1,3})$/);
			return this.assert(m && this.factory.parseIPv6(m[1]) && this.apply('ip6prefix', m[2]), _('valid IPv6 CIDR'));
		},

		ipnet4: function() {
			var m = this.value.match(/^(\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3})\/(\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3})$/);
			return this.assert(m && this.factory.parseIPv4(m[1]) && this.factory.parseIPv4(m[2]), _('IPv4 network in address/netmask notation'));
		},

		ipnet6: function() {
			var m = this.value.match(/^([0-9a-fA-F:.]+)\/([0-9a-fA-F:.]+)$/);
			return this.assert(m && this.factory.parseIPv6(m[1]) && this.factory.parseIPv6(m[2]), _('IPv6 network in address/netmask notation'));
		},

		ip6hostid: function() {
			if (this.value == "eui64" || this.value == "random")
				return true;

			var v6 = this.factory.parseIPv6(this.value);
			return this.assert(!(!v6 || v6[0] || v6[1] || v6[2] || v6[3]), _('valid IPv6 host id'));
		},

		ipmask: function() {
			return this.assert(this.apply('ipmask4') || this.apply('ipmask6'),
				_('valid network in address/netmask notation'));
		},

		ipmask4: function() {
			return this.assert(this.apply('cidr4') || this.apply('ipnet4') || this.apply('ip4addr'),
				_('valid IPv4 network'));
		},

		ipmask6: function() {
			return this.assert(this.apply('cidr6') || this.apply('ipnet6') || this.apply('ip6addr'),
				_('valid IPv6 network'));
		},

		port: function() {
			var p = this.factory.parseInteger(this.value);
			return this.assert(p >= 0 && p <= 65535, _('valid port value'));
		},

		portrange: function() {
			if (this.value.match(/^(\d+)-(\d+)$/)) {
				var p1 = +RegExp.$1;
				var p2 = +RegExp.$2;
				return this.assert(p1 <= p2 && p2 <= 65535,
					_('valid port or port range (port1-port2)'));
			}

			return this.assert(this.apply('port'), _('valid port or port range (port1-port2)'));
		},

		macaddr: function() {
			return this.assert(this.value.match(/^([a-fA-F0-9]{2}:){5}[a-fA-F0-9]{2}$/) != null,
				_('valid MAC address'));
		},

		host: function(ipv4only) {
			return this.assert(this.apply('hostname') || this.apply(ipv4only == 1 ? 'ip4addr' : 'ipaddr'),
				_('valid hostname or IP address'));
		},

		hostname: function(strict) {
			if (this.value.length <= 253)
				return this.assert(
					(this.value.match(/^[a-zA-Z0-9_]+$/) != null ||
						(this.value.match(/^[a-zA-Z0-9_][a-zA-Z0-9_\-.]*[a-zA-Z0-9]$/) &&
						 this.value.match(/[^0-9.]/))) &&
					(!strict || !this.value.match(/^_/)),
					_('valid hostname'));

			return this.assert(false, _('valid hostname'));
		},

		network: function() {
			return this.assert(this.apply('uciname') || this.apply('host'),
				_('valid UCI identifier, hostname or IP address'));
		},

		hostport: function(ipv4only) {
			var hp = this.value.split(/:/);
			return this.assert(hp.length == 2 && this.apply('host', hp[0], [ipv4only]) && this.apply('port', hp[1]),
				_('valid host:port'));
		},

		ip4addrport: function() {
			var hp = this.value.split(/:/);
			return this.assert(hp.length == 2 && this.apply('ip4addr', hp[0], [true]) && this.apply('port', hp[1]),
				_('valid IPv4 address:port'));
		},

		ipaddrport: function(bracket) {
			var m4 = this.value.match(/^([^\[\]:]+):(\d+)$/),
			    m6 = this.value.match((bracket == 1) ? /^\[(.+)\]:(\d+)$/ : /^([^\[\]]+):(\d+)$/);

			if (m4)
				return this.assert(this.apply('ip4addr', m4[1], [true]) && this.apply('port', m4[2]),
					_('valid address:port'));

			return this.assert(m6 && this.apply('ip6addr', m6[1], [true]) && this.apply('port', m6[2]),
				_('valid address:port'));
		},

		wpakey: function() {
			var v = this.value;

			if (v.length == 64)
				return this.assert(v.match(/^[a-fA-F0-9]{64}$/), _('valid hexadecimal WPA key'));

			return this.assert((v.length >= 8) && (v.length <= 63), _('key between 8 and 63 characters'));
		},

		wepkey: function() {
			var v = this.value;

			if (v.substr(0, 2) === 's:')
				v = v.substr(2);

			if ((v.length == 10) || (v.length == 26))
				return this.assert(v.match(/^[a-fA-F0-9]{10,26}$/), _('valid hexadecimal WEP key'));

			return this.assert((v.length === 5) || (v.length === 13), _('key with either 5 or 13 characters'));
		},

		uciname: function() {
			return this.assert(this.value.match(/^[a-zA-Z0-9_]+$/), _('valid UCI identifier'));
		},

		range: function(min, max) {
			var val = this.factory.parseDecimal(this.value);
			return this.assert(val >= +min && val <= +max, _('value between %f and %f').format(min, max));
		},

		min: function(min) {
			return this.assert(this.factory.parseDecimal(this.value) >= +min, _('value greater or equal to %f').format(min));
		},

		max: function(max) {
			return this.assert(this.factory.parseDecimal(this.value) <= +max, _('value smaller or equal to %f').format(max));
		},

		length: function(len) {
			var val = '' + this.value;
			return this.assert(val.length == +len,
				_('value with %d characters').format(len));
		},

		rangelength: function(min, max) {
			var val = '' + this.value;
			return this.assert((val.length >= +min) && (val.length <= +max),
				_('value between %d and %d characters').format(min, max));
		},

		minlength: function(min) {
			return this.assert((''+this.value).length >= +min,
				_('value with at least %d characters').format(min));
		},

		maxlength: function(max) {
			return this.assert((''+this.value).length <= +max,
				_('value with at most %d characters').format(max));
		},

		or: function() {
			var errors = [];

			for (var i = 0; i < arguments.length; i += 2) {
				if (typeof arguments[i] != 'function') {
					if (arguments[i] == this.value)
						return this.assert(true);
					errors.push('"%s"'.format(arguments[i]));
					i--;
				}
				else if (arguments[i].apply(this, arguments[i+1])) {
					return this.assert(true);
				}
				else {
					errors.push(this.error);
				}
			}

			var t = _('One of the following: %s');

			return this.assert(false, t.format('\n - ' + errors.join('\n - ')));
		},

		and: function() {
			for (var i = 0; i < arguments.length; i += 2) {
				if (typeof arguments[i] != 'function') {
					if (arguments[i] != this.value)
						return this.assert(false, '"%s"'.format(arguments[i]));
					i--;
				}
				else if (!arguments[i].apply(this, arguments[i+1])) {
					return this.assert(false, this.error);
				}
			}

			return this.assert(true);
		},

		neg: function() {
			this.value = this.value.replace(/^[ \t]*![ \t]*/, '');

			if (arguments[0].apply(this, arguments[1]))
				return this.assert(true);

			return this.assert(false, _('Potential negation of: %s').format(this.error));
		},

		list: function(subvalidator, subargs) {
			this.field.setAttribute('data-is-list', 'true');

			var tokens = this.value.match(/[^ \t]+/g);
			for (var i = 0; i < tokens.length; i++)
				if (!this.apply(subvalidator, tokens[i], subargs))
					return this.assert(false, this.error);

			return this.assert(true);
		},

		phonedigit: function() {
			return this.assert(this.value.match(/^[0-9\*#!\.]+$/),
				_('valid phone digit (0-9, "*", "#", "!" or ".")'));
		},

		timehhmmss: function() {
			return this.assert(this.value.match(/^[0-6][0-9]:[0-6][0-9]:[0-6][0-9]$/),
				_('valid time (HH:MM:SS)'));
		},

		dateyyyymmdd: function() {
			if (this.value.match(/^(\d\d\d\d)-(\d\d)-(\d\d)/)) {
				var year  = +RegExp.$1,
				    month = +RegExp.$2,
				    day   = +RegExp.$3,
				    days_in_month = [ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 ];

				var is_leap_year = function(year) {
					return ((!(year % 4) && (year % 100)) || !(year % 400));
				}

				var get_days_in_month = function(month, year) {
					return (month === 2 && is_leap_year(year)) ? 29 : days_in_month[month - 1];
				}

				/* Firewall rules in the past don't make sense */
				return this.assert(year >= 2015 && month && month <= 12 && day && day <= get_days_in_month(month, year),
					_('valid date (YYYY-MM-DD)'));

			}

			return this.assert(false, _('valid date (YYYY-MM-DD)'));
		},

		unique: function(subvalidator, subargs) {
			var ctx = this,
			    option = findParent(ctx.field, '[data-widget][data-name]'),
			    section = findParent(option, '.cbi-section'),
			    query = '[data-widget="%s"][data-name="%s"]'.format(option.getAttribute('data-widget'), option.getAttribute('data-name')),
			    unique = true;

			section.querySelectorAll(query).forEach(function(sibling) {
				if (sibling === option)
					return;

				var input = sibling.querySelector('[data-type]'),
				    values = input ? (input.getAttribute('data-is-list') ? input.value.match(/[^ \t]+/g) : [ input.value ]) : null;

				if (values !== null && values.indexOf(ctx.value) !== -1)
					unique = false;
			});

			if (!unique)
				return this.assert(false, _('unique value'));

			if (typeof(subvalidator) === 'function')
				return this.apply(subvalidator, null, subargs);

			return this.assert(true);
		},

		hexstring: function() {
			return this.assert(this.value.match(/^([a-f0-9][a-f0-9]|[A-F0-9][A-F0-9])+$/),
				_('hexadecimal encoded value'));
		},

		string: function() {
			return true;
		}
	}
});

return ValidatorFactory;
