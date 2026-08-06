'use strict';
'require view';
'require rpc';
'require uci';

var callClassList = rpc.declare({
	object: 'omci.agent',
	method: 'class_list',
	params: [ 'device' ],
	expect: { classes: [] },
	reject: true
});

function loadClasses() {
	return uci.load('omci').then(function() {
		var device = Number(uci.get('omci', 'main', 'device')) || 0;

		return callClassList(device);
	});
}

var flagNames = [
	[ 1 << 0, _('Standard') ],
	[ 1 << 1, _('Deprecated') ],
	[ 1 << 2, _('Reserved') ],
	[ 1 << 3, _('Vendor-specific') ],
	[ 1 << 4, _('Performance monitoring') ],
	[ 1 << 5, _('Table') ],
	[ 1 << 6, _('Datapath') ]
];

function classFlags(value) {
	var flags = Number(value) || 0;
	var names = flagNames.filter(function(entry) {
		return (flags & entry[0]) !== 0;
	}).map(function(entry) {
		return entry[1];
	});

	return names.length ? names.join(', ') : '-';
}

function renderTable(classes) {
	var rows = (classes || []).map(function(info) {
		return E('tr', { 'class': 'tr' }, [
			E('td', { 'class': 'td' }, info.class_id),
			E('td', { 'class': 'td' }, info.name || '-'),
			E('td', { 'class': 'td' }, info.category_name || info.category),
			E('td', { 'class': 'td' }, info.support_name || info.support),
			E('td', { 'class': 'td' }, classFlags(info.flags))
		]);
	});

	return E('div', { 'style': 'overflow-x:auto' }, [
		E('table', { 'class': 'table' }, [
			E('tr', { 'class': 'tr table-titles' }, [
				E('th', { 'class': 'th' }, _('Class ID')),
				E('th', { 'class': 'th' }, _('Managed entity')),
				E('th', { 'class': 'th' }, _('Category')),
				E('th', { 'class': 'th' }, _('Local support')),
				E('th', { 'class': 'th' }, _('Flags'))
			])
		].concat(rows))
	]);
}

return view.extend({
	load: loadClasses,

	render: function(classes) {
		return E([], [
			E('h2', {}, _('OMCI managed entity class catalog')),
			E('div', { 'class': 'cbi-map-descr' },
				_('The catalog is resolved for the selected OMCI device and its effective OLT profile. It identifies standardized class IDs and separately reports the implementation level in this kernel. A recognized name does not imply that the managed entity is programmed into the datapath.')),
			renderTable(classes)
		]);
	},

	handleSaveApply: null,
	handleSave: null,
	handleReset: null
});
