'use strict';
'require fs';
'require form';
'require tools.widgets as widgets';

return L.view.extend({
	load: function() {
		return Promise.all([
			L.resolveDefault(fs.stat('/sbin/block'), null),
			L.resolveDefault(fs.stat('/etc/config/fstab'), null),
			L.resolveDefault(fs.stat('/usr/sbin/nmbd'), {}),
			L.resolveDefault(fs.stat('/usr/sbin/samba'), {}),
			L.resolveDefault(fs.stat('/usr/sbin/winbindd'), {}),
			L.resolveDefault(fs.exec('/usr/sbin/smbd', ['-V']), null),
		]);
	},
	render: function(stats) {
		var m, s, o, v;
		v = '';
		
		m = new form.Map('samba4', _('Network Shares'));

		if (stats[5] && stats[5].code === 0) {
			v = stats[5].stdout.trim();
		}
		s = m.section(form.TypedSection, 'samba', 'Samba ' + v);
		s.anonymous = true;

		s.tab('general',  _('General Settings'));
		s.tab('template', _('Edit Template'));

		s.taboption('general', widgets.NetworkSelect, 'interface', _('Interface'),
			_('Listen only on the given interface or, if unspecified, on lan'));

		o = s.taboption('general', form.Value, 'workgroup', _('Workgroup'));
		o.placeholder = 'WORKGROUP';

		o = s.taboption('general', form.Value, 'description', _('Description'));
		o.placeholder = 'Samba4 on OpenWrt';
		
		s.taboption('general', form.Flag, 'disable_async_io', _('Force synchronous  I/O'),
			_('On lower-end devices may increase speeds, by forceing synchronous I/O instead of the default asynchronous.'));

		o = s.taboption('general', form.Flag, 'macos', _('Enable macOS compatible shares'),
			_('Enables Apple\'s AAPL extension globally and adds macOS compatibility options to all shares.'));
			
		o = s.taboption('general', form.Flag, 'allow_legacy_protocols', _('Allow legacy (insecure) protocols/authentication.'),
			_('Allow legacy smb(v1)/Lanman connections, needed for older devices without smb(v2.1/3) support.'));

		if (stats[2].type === 'file') {
			s.taboption('general', form.Flag, 'disable_netbios', _('Disable Netbios')) 
		}
		if (stats[3].type === 'file') {
			s.taboption('general', form.Flag, 'disable_ad_dc', _('Disable Active Directory Domain Controller')) 
		}
		if (stats[4].type === 'file') {
			s.taboption('general', form.Flag, 'disable_winbind', _('Disable Winbind')) 
		}
		
		o = s.taboption('template', form.TextValue, '_tmpl',
			_('Edit the template that is used for generating the samba configuration.'),
			_("This is the content of the file '/etc/samba/smb.conf.template' from which your samba configuration will be generated. \
			Values enclosed by pipe symbols ('|') should not be changed. They get their values from the 'General Settings' tab."));
		o.rows = 20;
		o.cfgvalue = function(section_id) {
			return fs.trimmed('/etc/samba/smb.conf.template');
		};
		o.write = function(section_id, formvalue) {
			return fs.write('/etc/samba/smb.conf.template', formvalue.trim().replace(/\r\n/g, '\n') + '\n');
		};


		s = m.section(form.TableSection, 'sambashare', _('Shared Directories'),
			_('Please add directories to share. Each directory refers to a folder on a mounted device.'));
		s.anonymous = true;
		s.addremove = true;

		s.option(form.Value, 'name', _('Name'));
		o = s.option(form.Value, 'path', _('Path'));
		if (stats[0] && stats[1]) {
			o.titleref = L.url('admin', 'system', 'mounts');
		}

		o = s.option(form.Flag, 'browseable', _('Browse-able'));
		o.enabled = 'yes';
		o.disabled = 'no';
		o.default = 'yes';

		o = s.option(form.Flag, 'read_only', _('Read-only'));
		o.enabled = 'yes';
		o.disabled = 'no';
		o.default = 'no'; // smb.conf default is 'yes'
		o.rmempty = false;

		s.option(form.Flag, 'force_root', _('Force Root'));

		o = s.option(form.Value, 'users', _('Allowed users'));
		o.rmempty = true;

		o = s.option(form.Flag, 'guest_ok', _('Allow guests'));
		o.enabled = 'yes';
		o.disabled = 'no';
		o.default = 'yes'; // smb.conf default is 'no'
		o.rmempty = false;

		o = s.option(form.Flag, 'guest_only', _('Guests only'));
		o.enabled = 'yes';
		o.disabled = 'no';
		o.default = 'no';
		
		o = s.option(form.Flag, 'inherit_owner', _('Inherit owner'));
		o.enabled = 'yes';
		o.disabled = 'no';
		o.default = 'no';

		o = s.option(form.Value, 'create_mask', _('Create mask'));
		o.maxlength = 4;
		o.default = '0666'; // smb.conf default is '0744'
		o.placeholder = '0666';
		o.rmempty = false;

		o = s.option(form.Value, 'dir_mask', _('Directory mask'));
		o.maxlength = 4;
		o.default = '0777'; // smb.conf default is '0755'
		o.placeholder = '0777';
		o.rmempty = false;
		
		o = s.option(form.Value, 'vfs_objects', _('Vfs objects'));
		o.rmempty = true;
		
		s.option(form.Flag, 'timemachine', _('Apple Time-machine share'));
		
		o = s.option(form.Value, 'timemachine_maxsize', _('Time-machine size in GB'));
		o.rmempty = true;
		o.maxlength = 5;

		return m.render();
	}
});
