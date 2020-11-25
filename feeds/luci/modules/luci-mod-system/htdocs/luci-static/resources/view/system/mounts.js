'use strict';
'require fs';
'require ui';
'require uci';
'require rpc';
'require form';

var callBlockDevices, callMountPoints, callBlockDetect;

callBlockDevices = rpc.declare({
	object: 'luci',
	method: 'getBlockDevices',
	expect: { '': {} }
});

callMountPoints = rpc.declare({
	object: 'luci',
	method: 'getMountPoints',
	expect: { result: [] }
});

callBlockDetect = rpc.declare({
	object: 'luci',
	method: 'setBlockDetect',
	expect: { result: false }
});

function device_textvalue(devices, section_id) {
	var v = (uci.get('fstab', section_id, 'uuid') || '').toLowerCase(),
	    e = Object.keys(devices).filter(function(dev) { return (devices[dev].uuid || '-').toLowerCase() == v })[0];

	if (v) {
		this.section.devices[section_id] = devices[e];

		if (e && devices[e].size)
			return E('span', 'UUID: %h (%s, %1024.2mB)'.format(v, devices[e].dev, devices[e].size));
		else if (e)
			return E('span', 'UUID: %h (%s)'.format(v, devices[e].dev));
		else
			return E('span', 'UUID: %h (<em>%s</em>)'.format(v, _('not present')));
	}

	v = uci.get('fstab', section_id, 'label');
	e = Object.keys(devices).filter(function(dev) { return devices[dev].label == v })[0];

	if (v) {
		this.section.devices[section_id] = this.section.devices[section_id] || devices[e];

		if (e && devices[e].size)
			return E('span', 'Label: %h (%s, %1024.2mB)'.format(v, devices[e].dev, devices[e].size));
		else if (e)
			return E('span', 'Label: %h (%s)'.format(v, devices[e].dev));
		else
			return E('span', 'Label: %h (<em>%s</em>)'.format(v, _('not present')));
	}

	v = uci.get('fstab', section_id, 'device');
	e = Object.keys(devices).filter(function(dev) { return devices[dev].dev == v })[0];

	if (v) {
		this.section.devices[section_id] = this.section.devices[section_id] || devices[e];

		if (e && devices[e].size)
			return E('span', '%h (%1024.2mB)'.format(v, devices[e].size));
		else if (e)
			return E('span', '%h'.format(v));
		else
			return E('span', '%h (<em>%s</em>)'.format(v, _('not present')));
	}
}

return L.view.extend({
	handleDetect: function(m, ev) {
		return callBlockDetect()
			.then(L.bind(uci.unload, uci, 'fstab'))
			.then(L.bind(m.render, m));
	},

	handleMountAll: function(m, ev) {
		return fs.exec('/sbin/block', ['mount'])
			.then(function(res) {
				if (res.code != 0)
					ui.addNotification(null, E('p', _('The <em>block mount</em> command failed with code %d').format(res.code)));
			})
			.then(L.bind(uci.unload, uci, 'fstab'))
			.then(L.bind(m.render, m));
	},

	handleUmount: function(m, path, ev) {
		return fs.exec('/bin/umount', [path])
			.then(L.bind(uci.unload, uci, 'fstab'))
			.then(L.bind(m.render, m))
			.catch(function(e) { ui.addNotification(null, E('p', e.message)) });
	},

	load: function() {
		return Promise.all([
			callBlockDevices(),
			fs.lines('/proc/filesystems'),
			fs.lines('/etc/filesystems'),
			L.resolveDefault(fs.stat('/usr/sbin/e2fsck'), null),
			L.resolveDefault(fs.stat('/usr/sbin/fsck.f2fs'), null),
			L.resolveDefault(fs.stat('/usr/sbin/fsck.fat'), null),
			L.resolveDefault(fs.stat('/usr/bin/btrfsck'), null),
			L.resolveDefault(fs.stat('/usr/bin/ntfsfix'), null),
			uci.load('fstab')
		]);
	},

	render: function(results) {
		var devices = results[0],
		    procfs = results[1],
		    etcfs = results[2],
		    triggers = {},
		    trigger, m, s, o;

		var fsck = {
			ext2: results[3],
			ext3: results[3],
			ext4: results[3],
			f2fs: results[4],
			vfat: results[5],
			btrfs: results[6],
			ntfs: results[7]
		};

		var filesystems = {};

		for (var i = 0; i < procfs.length; i++)
			if (procfs[i].match(/\S/) && !procfs[i].match(/^nodev\t/))
				filesystems[procfs[i].trim()] = true;

		for (var i = 0; i < etcfs.length; i++)
			if (etcfs[i].match(/\S/))
				filesystems[etcfs[i].trim()] = true;

		filesystems = Object.keys(filesystems).sort();


		if (!uci.sections('fstab', 'global').length)
			uci.add('fstab', 'global');

		m = new form.Map('fstab', _('Mount Points'));

		s = m.section(form.TypedSection, 'global', _('Global Settings'));
		s.addremove = false;
		s.anonymous = true;

		o = s.option(form.Button, '_detect', _('Generate Config'), _('Find all currently attached filesystems and swap and replace configuration with defaults based on what was detected'));
		o.onclick = this.handleDetect.bind(this, m);
		o.inputstyle = 'reload';

		o = s.option(form.Button, '_mountall', _('Mount attached devices'), _('Attempt to enable configured mount points for attached devices'));
		o.onclick = this.handleMountAll.bind(this, m);
		o.inputstyle = 'reload';

		o = s.option(form.Flag, 'anon_swap', _('Anonymous Swap'), _('Mount swap not specifically configured'));
		o.default = o.disabled;
		o.rmempty = false;

		o = s.option(form.Flag, 'anon_mount', _('Anonymous Mount'), _('Mount filesystems not specifically configured'));
		o.default = o.disabled;
		o.rmempty = false;

		o = s.option(form.Flag, 'auto_swap', _('Automount Swap'), _('Automatically mount swap on hotplug'));
		o.default = o.enabled;
		o.rmempty = false;

		o = s.option(form.Flag, 'auto_mount', _('Automount Filesystem'), _('Automatically mount filesystems on hotplug'));
		o.default = o.enabled;
		o.rmempty = false;

		o = s.option(form.Flag, 'check_fs', _('Check filesystems before mount'), _('Automatically check filesystem for errors before mounting'));
		o.default = o.disabled;
		o.rmempty = false;


		// Mount status table
		o = s.option(form.DummyValue, '_mtab');

		o.load = function(section_id) {
			return callMountPoints().then(L.bind(function(mounts) {
				this.mounts = mounts;
			}, this));
		};

		o.render = L.bind(function(view, section_id) {
			var table = E('div', { 'class': 'table' }, [
				E('div', { 'class': 'tr table-titles' }, [
					E('div', { 'class': 'th' }, _('Filesystem')),
					E('div', { 'class': 'th' }, _('Mount Point')),
					E('div', { 'class': 'th center' }, _('Available')),
					E('div', { 'class': 'th center' }, _('Used')),
					E('div', { 'class': 'th' }, _('Unmount'))
				])
			]);

			var rows = [];

			for (var i = 0; i < this.mounts.length; i++) {
				var used = this.mounts[i].size - this.mounts[i].free,
				    umount = true;

				if (/^\/(overlay|rom|tmp(?:\/.+)?|dev(?:\/.+)?|)$/.test(this.mounts[i].mount))
					umount = false;

				rows.push([
					this.mounts[i].device,
					this.mounts[i].mount,
					'%1024.2mB / %1024.2mB'.format(this.mounts[i].avail, this.mounts[i].size),
					'%.2f%% (%1024.2mB)'.format(100 / this.mounts[i].size * used, used),
					umount ? E('button', {
						'class': 'btn cbi-button-remove',
						'click': ui.createHandlerFn(view, 'handleUmount', m, this.mounts[i].mount)
					}, [ _('Unmount') ]) : '-'
				]);
			}

			cbi_update_table(table, rows, E('em', _('Unable to obtain mount information')));

			return E([], [ E('h3', _('Mounted file systems')), table ]);
		}, o, this);


		// Mounts
		s = m.section(form.GridSection, 'mount', _('Mount Points'), _('Mount Points define at which point a memory device will be attached to the filesystem'));
		s.modaltitle = _('Mount Points - Mount Entry');
		s.anonymous = true;
		s.addremove = true;
		s.sortable  = true;
		s.devices   = {};

		s.renderHeaderRows = function(/* ... */) {
			var trEls = form.GridSection.prototype.renderHeaderRows.apply(this, arguments);
			return trEls.childNodes[0];
		}

		s.tab('general', _('General Settings'));
		s.tab('advanced', _('Advanced Settings'));

		o = s.taboption('general', form.Flag, 'enabled', _('Enabled'));
		o.rmempty  = false;
		o.editable = true;

		o = s.taboption('general', form.DummyValue, '_device', _('Device'));
		o.rawhtml   = true;
		o.modalonly = false;
		o.write = function() {};
		o.remove = function() {};
		o.textvalue = device_textvalue.bind(o, devices);

		o = s.taboption('general', form.Value, 'uuid', _('UUID'), _('If specified, mount the device by its UUID instead of a fixed device node'));
		o.modalonly = true;
		o.value('', _('-- match by uuid --'));

		var devs = Object.keys(devices).sort();
		for (var i = 0; i < devs.length; i++) {
			var dev = devices[devs[i]];
			if (dev.uuid && dev.size)
				o.value(dev.uuid, '%s (%s, %1024.2mB)'.format(dev.uuid, dev.dev, dev.size));
			else if (dev.uuid)
				o.value(dev.uuid, '%s (%s)'.format(dev.uuid, dev.dev));
		}

		o = s.taboption('general', form.Value, 'label', _('Label'), _('If specified, mount the device by the partition label instead of a fixed device node'));
		o.modalonly = true;
		o.depends('uuid', '');
		o.value('', _('-- match by label --'));

		for (var i = 0; i < devs.length; i++) {
			var dev = devices[devs[i]];
			if (dev.label && dev.size)
				o.value(dev.label, '%s (%s, %1024.2mB)'.format(dev.label, dev.dev, dev.size));
			else if (dev.label)
				o.value(dev.label, '%s (%s)'.format(dev.label, dev.dev));
		}

		o = s.taboption('general', form.Value, 'device', _('Device'), _('The device file of the memory or partition (<abbr title="for example">e.g.</abbr> <code>/dev/sda1</code>)'));
		o.modalonly = true;
		o.depends({ uuid: '', label: '' });

		for (var i = 0; i < devs.length; i++) {
			var dev = devices[devs[i]];
			if (dev.size)
				o.value(dev.dev, '%s (%1024.2mB)'.format(dev.dev, dev.size));
			else
				o.value(dev.dev);
		}

		o = s.taboption('general', form.Value, 'target', _('Mount point'), _('Specifies the directory the device is attached to'));
		o.value('/', _('Use as root filesystem (/)'));
		o.value('/overlay', _('Use as external overlay (/overlay)'));
		o.rmempty = false;

		o = s.taboption('general', form.DummyValue, '__notice', _('Root preparation'));
		o.depends('target', '/');
		o.modalonly = true;
		o.rawhtml = true;
		o.default = '' +
			'<p>%s</p>'.format(_('Make sure to clone the root filesystem using something like the commands below:')) +
			'<pre>' +
				'mkdir -p /tmp/introot\n' +
				'mkdir -p /tmp/extroot\n' +
				'mount --bind / /tmp/introot\n' +
				'mount /dev/sda1 /tmp/extroot\n' +
				'tar -C /tmp/introot -cvf - . | tar -C /tmp/extroot -xf -\n' +
				'umount /tmp/introot\n' +
				'umount /tmp/extroot\n' +
			'</pre>'
		;

		o = s.taboption('advanced', form.ListValue, 'fstype', _('Filesystem'));

		o.textvalue = function(section_id) {
			var dev = this.section.devices[section_id],
			    text = this.cfgvalue(section_id) || 'auto';

			if (dev && dev.type && dev.type != text)
				text += ' (%s)'.format(dev.type);

			return text;
		};

		o.value('', 'auto');

		for (var i = 0; i < filesystems.length; i++)
			o.value(filesystems[i]);

		o = s.taboption('advanced', form.Value, 'options', _('Mount options'), _('See "mount" manpage for details'));
		o.textvalue = function(section_id) { return this.cfgvalue(section_id) || 'defaults' };
		o.placeholder = 'defaults';

		s.taboption('advanced', form.Flag, 'enabled_fsck', _('Run filesystem check'), _('Run a filesystem check before mounting the device'));


		// Swaps
		s = m.section(form.GridSection, 'swap', _('SWAP'), _('If your physical memory is insufficient unused data can be temporarily swapped to a swap-device resulting in a higher amount of usable <abbr title="Random Access Memory">RAM</abbr>. Be aware that swapping data is a very slow process as the swap-device cannot be accessed with the high datarates of the <abbr title="Random Access Memory">RAM</abbr>.'));
		s.modaltitle = _('Mount Points - Swap Entry');
		s.anonymous = true;
		s.addremove = true;
		s.sortable  = true;
		s.devices   = {};

		s.renderHeaderRows = function(/* ... */) {
			var trEls = form.GridSection.prototype.renderHeaderRows.apply(this, arguments);
			trEls.childNodes[0].childNodes[1].style.width = '90%';
			return trEls.childNodes[0];
		}

		o = s.option(form.Flag, 'enabled', _('Enabled'));
		o.rmempty  = false;
		o.editable = true;

		o = s.option(form.DummyValue, '_device', _('Device'));
		o.modalonly = false;
		o.textvalue = device_textvalue.bind(o, devices);

		o = s.option(form.Value, 'uuid', _('UUID'), _('If specified, mount the device by its UUID instead of a fixed device node'));
		o.modalonly = true;
		o.value('', _('-- match by uuid --'));

		var devs = Object.keys(devices).sort();
		for (var i = 0; i < devs.length; i++) {
			var dev = devices[devs[i]];
			if (dev.dev.match(/^\/dev\/(mtdblock|ubi|ubiblock)\d/))
				continue;

			if (dev.uuid && dev.size)
				o.value(dev.uuid, '%s (%s, %1024.2mB)'.format(dev.uuid, dev.dev, dev.size));
			else if (dev.uuid)
				o.value(dev.uuid, '%s (%s)'.format(dev.uuid, dev.dev));
		}

		o = s.option(form.Value, 'label', _('Label'), _('If specified, mount the device by the partition label instead of a fixed device node'));
		o.modalonly = true;
		o.depends('uuid', '');
		o.value('', _('-- match by label --'));

		for (var i = 0; i < devs.length; i++) {
			var dev = devices[devs[i]];
			if (dev.dev.match(/^\/dev\/(mtdblock|ubi|ubiblock)\d/))
				continue;

			if (dev.label && dev.size)
				o.value(dev.label, '%s (%s, %1024.2mB)'.format(dev.label, dev.dev, dev.size));
			else if (dev.label)
				o.value(dev.label, '%s (%s)'.format(dev.label, dev.dev));
		}

		o = s.option(form.Value, 'device', _('Device'), _('The device file of the memory or partition (<abbr title="for example">e.g.</abbr> <code>/dev/sda1</code>)'));
		o.modalonly = true;
		o.depends({ uuid: '', label: '' });

		for (var i = 0; i < devs.length; i++) {
			var dev = devices[devs[i]];
			if (dev.dev.match(/^\/dev\/(mtdblock|ubi|ubiblock)\d/))
				continue;

			if (dev.size)
				o.value(dev.dev, '%s (%1024.2mB)'.format(dev.dev, dev.size));
			else
				o.value(dev.dev);
		}

		return m.render();
	}
});
