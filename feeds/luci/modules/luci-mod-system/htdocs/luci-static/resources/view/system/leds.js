'use strict';
'require uci';
'require rpc';
'require form';
'require tools.widgets as widgets';

var callLeds, callUSB;

callLeds = rpc.declare({
	object: 'luci',
	method: 'getLEDs',
	expect: { '': {} }
});

callUSB = rpc.declare({
	object: 'luci',
	method: 'getUSBDevices',
	expect: { '': {} }
});

return L.view.extend({
	load: function() {
		return Promise.all([
			callLeds(),
			callUSB()
		]);
	},

	render: function(results) {
		var leds = results[0],
		    usb = results[1],
		    triggers = {},
		    m, s, o;

		for (var k in leds)
			for (var i = 0; i < leds[k].triggers.length; i++)
				triggers[leds[k].triggers[i]] = true;

		m = new form.Map('system',
			_('<abbr title="Light Emitting Diode">LED</abbr> Configuration'),
			_('Customizes the behaviour of the device <abbr title="Light Emitting Diode">LED</abbr>s if possible.'));

		s = m.section(form.GridSection, 'led', '');
		s.anonymous = true;
		s.addremove = true;
		s.sortable = true;
		s.addbtntitle = _('Add LED action');

		s.option(form.Value, 'name', _('Name'));

		o = s.option(form.ListValue, 'sysfs', _('<abbr title="Light Emitting Diode">LED</abbr> Name'));
		Object.keys(leds).sort().forEach(function(name) { o.value(name) });

		o = s.option(form.Flag, 'default', _('Default state'));
		o.rmempty = false;
		o.textvalue = function(section_id) {
			var cval = this.cfgvalue(section_id);

			if (cval == null)
				cval = this.default;

			return (cval == this.enabled) ? _('On') : _('Off');
		};

		o = s.option(form.ListValue, 'trigger', _('Trigger'));
		if (usb.devices && usb.devices.length)
			triggers['usbdev'] = true;
		if (usb.ports && usb.ports.length)
			triggers['usbport'] = true;
		Object.keys(triggers).sort().forEach(function(t) { o.value(t, t.replace(/-/g, '')) });

		o = s.option(form.Value, 'delayon', _('On-State Delay'));
		o.modalonly = true;
		o.depends('trigger', 'timer');

		o = s.option(form.Value, 'delayoff', _('Off-State Delay'));
		o.modalonly = true;
		o.depends('trigger', 'timer');

		o = s.option(widgets.DeviceSelect, '_net_dev', _('Device'));
		o.rmempty = true;
		o.ucioption = 'dev';
		o.modalonly = true;
		o.noaliases = true;
		o.depends('trigger', 'netdev');
		o.remove = function(section_id) {
			var topt = this.map.lookupOption('trigger', section_id),
			    tval = topt ? topt[0].formvalue(section_id) : null;

			if (tval != 'netdev' && tval != 'usbdev')
				uci.unset('system', section_id, 'dev');
		};

		o = s.option(form.MultiValue, 'mode', _('Trigger Mode'));
		o.rmempty = true;
		o.modalonly = true;
		o.depends('trigger', 'netdev');
		o.value('link', _('Link On'));
		o.value('tx', _('Transmit'));
		o.value('rx', _('Receive'));

		if (usb.devices && usb.devices.length) {
			o = s.option(form.ListValue, '_usb_dev', _('USB Device'));
			o.depends('trigger', 'usbdev');
			o.rmempty = true;
			o.ucioption = 'dev';
			o.modalonly = true;
			o.remove = function(section_id) {
				var topt = this.map.lookupOption('trigger', section_id),
				    tval = topt ? topt[0].formvalue(section_id) : null;

				if (tval != 'netdev' && tval != 'usbdev')
					uci.unset('system', section_id, 'dev');
			}
			o.value('');
			usb.devices.forEach(function(usbdev) {
				o.value(usbdev.id, '%s (%s - %s)'.format(usbdev.id, usbdev.vendor || '?', usbdev.product || '?'));
			});
		}

		if (usb.ports && usb.ports.length) {
			o = s.option(form.MultiValue, 'port', _('USB Ports'));
			o.depends('trigger', 'usbport');
			o.rmempty = true;
			o.modalonly = true;
			o.cfgvalue = function(section_id) {
				var ports = [],
				    value = uci.get('system', section_id, 'port');

				if (!Array.isArray(value))
					value = String(value || '').split(/\s+/);

				for (var i = 0; i < value.length; i++)
					if (value[i].match(/^(\d+)-(\d+)$/))
						ports.push('usb%d-port%d'.format(Regexp.$1, Regexp.$2));
					else
						ports.push(value[i]);

				return ports;
			};
			usb.ports.forEach(function(usbport) {
				var dev = (usbport.device && Array.isArray(usb.devices))
					? usb.devices.filter(function(d) { return d.id == usbport.device })[0] : null;

				var label = _('Port %s').format(usbport.port);

				if (dev)
					label += ' (%s - %s)'.format(dev.vendor || '?', dev.product || '?');

				o.value(usbport.port, label);
			});
		}

		o = s.option(form.Value, 'port_mask', _('Switch Port Mask'));
		o.modalonly = true;
		o.depends('trigger', 'switch0');
		o.depends('trigger', 'switch1');

		o = s.option(form.Value, 'speed_mask', _('Switch Speed Mask'));
		o.modalonly = true;
		o.depends('trigger', 'switch0');
		o.depends('trigger', 'switch1');

		return m.render();
	}
});
