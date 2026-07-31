'use strict';
'require view';
'require rpc';
'require ui';

var callProfileList = rpc.declare({
	object: 'omci.agent',
	method: 'profile_list',
	expect: { profiles: [] },
	reject: true
});

var callProfileApply = rpc.declare({
	object: 'omci.agent',
	method: 'profile_apply',
	params: [ 'profile', 'inputs' ],
	reject: true
});

var callApply = rpc.declare({
	object: 'omci.agent',
	method: 'apply',
	reject: true
});

function notifyError(err) {
	ui.hideModal();
	ui.addNotification(null, E('p', {}, err && err.message || String(err)));
}

function profileById(profiles, id) {
	for (var i = 0; i < profiles.length; i++)
		if (profiles[i].id === id)
			return profiles[i];
	return null;
}

function validateInput(input, spec) {
	var value = input.value || '';
	var valid = true;
	var message = '';

	if (spec.required && value === '') {
		valid = false;
		message = _('This field is required.');
	}
	else if (spec.max_length != null && value.length > Number(spec.max_length)) {
		valid = false;
		message = _('The value is too long.');
	}
	else if (value !== '' && spec.pattern) {
		try {
			valid = new RegExp(spec.pattern).test(value);
		}
		catch (e) {
			valid = false;
		}

		if (!valid)
			message = spec.name === 'gpon_sn' ?
				_('Use VENDXXXXXXXX or sixteen hexadecimal digits.') :
				_('Use 1 to 32 printable ASCII characters.');
	}

	input.setCustomValidity(message);
	if (!valid)
		input.reportValidity();
	return valid;
}

function renderProfileFields(profile, container, inputNodes) {
	container.replaceChildren();
	for (var key in inputNodes)
		delete inputNodes[key];

	container.appendChild(E('h3', {}, profile.name));
	container.appendChild(E('p', {}, profile.description || ''));

	(profile.inputs || []).forEach(function(spec) {
		var input = E('input', {
			'class': 'cbi-input-text',
			'type': spec.type === 'password' ? 'password' : 'text',
			'placeholder': spec.placeholder || '',
			'maxlength': spec.max_length || null,
			'autocomplete': spec.type === 'password' ? 'new-password' : 'off'
		});

		inputNodes[spec.name] = { node: input, spec: spec };
		container.appendChild(E('div', { 'class': 'cbi-value' }, [
			E('label', { 'class': 'cbi-value-title' }, spec.label),
			E('div', { 'class': 'cbi-value-field' }, [
				input,
				spec.required ? E('div', { 'class': 'cbi-value-description' }, _('Required')) : ''
			])
		]));
	});

	if ((profile.settings || []).length) {
		container.appendChild(E('h4', {}, _('Profile settings')));
		container.appendChild(E('ul', {}, profile.settings.map(function(setting) {
			return E('li', {}, setting);
		})));
	}
}

function collectInputs(inputNodes) {
	var values = {};
	var valid = true;

	for (var name in inputNodes) {
		var field = inputNodes[name];
		if (!validateInput(field.node, field.spec)) {
			valid = false;
			break;
		}
		values[name] = field.node.value;
	}

	return valid ? values : null;
}

function confirmApply(profile) {
	return new Promise(function(resolve) {
		ui.showModal(_('Apply OMCI profile'), [
			E('p', {}, _('Apply “%s”? This replaces the current OMCI identity and interoperability settings.').format(profile.name)),
			E('div', { 'class': 'right' }, [
				E('button', {
					'class': 'btn',
					'click': function() {
						ui.hideModal();
						resolve(false);
					}
				}, _('Cancel')),
				' ',
				E('button', {
					'class': 'btn cbi-button-positive important',
					'click': function() {
						ui.showModal(_('Applying OMCI profile'), [
							E('p', { 'class': 'spinning' }, _('Writing the UCI configuration and applying it to the kernel agent…'))
						]);
						resolve(true);
					}
				}, _('Apply profile'))
			])
		]);
	});
}

return view.extend({
	load: function() {
		return callProfileList();
	},

	render: function(profiles) {
		profiles = (profiles || []).slice().sort(function(a, b) {
			return Number(a.order || 0) - Number(b.order || 0);
		});

		if (!profiles.length)
			return E([], [
				E('h2', {}, _('OMCI profiles')),
				E('div', { 'class': 'alert-message warning' }, _('No preconfigured profiles are installed.'))
			]);

		var inputNodes = {};
		var profileSelect = E('select', { 'class': 'cbi-input-select' }, profiles.map(function(profile) {
			return E('option', { 'value': profile.id }, profile.name);
		}));
		var fields = E('div', { 'class': 'cbi-section' });
		var applyButton = E('button', {
			'class': 'btn cbi-button-positive important',
			'click': function(ev) {
				ev.preventDefault();
				var profile = profileById(profiles, profileSelect.value);
				var inputs = collectInputs(inputNodes);

				if (!profile || inputs == null)
					return;

				confirmApply(profile).then(function(confirmed) {
					if (!confirmed)
						return null;

					return callProfileApply(profile.id, inputs)
						.then(function() { return callApply(); })
						.then(function() {
							ui.hideModal();
							ui.addNotification(null, E('p', {},
								_('Profile “%s” was saved and applied successfully.').format(profile.name)));
						});
				}).catch(notifyError);
			}
		}, _('Apply profile'));

		profileSelect.addEventListener('change', function() {
			var profile = profileById(profiles, profileSelect.value);
			if (profile)
				renderProfileFields(profile, fields, inputNodes);
		});

		renderProfileFields(profiles[0], fields, inputNodes);

		return E([], [
			E('h2', {}, _('OMCI profiles')),
			E('p', {}, _('Select a preconfigured ONU identity. Profile scripts receive the entered values as OMCI_INPUT_* environment variables, write /etc/config/omci, and the resulting configuration is then applied to the kernel agent.')),
			E('div', { 'class': 'cbi-section' }, [
				E('div', { 'class': 'cbi-value' }, [
					E('label', { 'class': 'cbi-value-title' }, _('Profile')),
					E('div', { 'class': 'cbi-value-field' }, profileSelect)
				])
			]),
			fields,
			E('div', { 'class': 'cbi-page-actions' }, applyButton)
		]);
	},

	handleSaveApply: null,
	handleSave: null,
	handleReset: null
});
