'use strict';
'require form';
'require tools.widgets as widgets';

return L.view.extend({
	render: function() {
		var m, s, o;

		m = new form.Map('dropbear', _('SSH Access'), _('Dropbear offers <abbr title="Secure Shell">SSH</abbr> network shell access and an integrated <abbr title="Secure Copy">SCP</abbr> server'));

		s = m.section(form.TypedSection, 'dropbear', _('Dropbear Instance'));
		s.anonymous = true;
		s.addremove = true;
		s.addbtntitle = _('Add instance');

		o = s.option(widgets.NetworkSelect, 'Interface', _('Interface'), _('Listen only on the given interface or, if unspecified, on all'));
		o.nocreate    = true;
		o.unspecified = true;

		o = s.option(form.Value, 'Port', _('Port'));
		o.datatype    = 'port';
		o.placeholder = 22;

		o = s.option(form.Flag, 'PasswordAuth', _('Password authentication'), _('Allow <abbr title="Secure Shell">SSH</abbr> password authentication'));
		o.enabled  = 'on';
		o.disabled = 'off';
		o.default  = o.enabled;
		o.rmempty  = false;

		o = s.option(form.Flag, 'RootPasswordAuth', _('Allow root logins with password'), _('Allow the <em>root</em> user to login with password'));
		o.enabled  = 'on';
		o.disabled = 'off';
		o.default  = o.enabled;

		o = s.option(form.Flag, 'GatewayPorts', _('Gateway Ports'), _('Allow remote hosts to connect to local SSH forwarded ports'));
		o.enabled  = 'on';
		o.disabled = 'off';
		o.default  = o.disabled;

		return m.render();
	}
});
