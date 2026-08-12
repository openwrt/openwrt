'use strict';
'require omci.rpc as omci';

var groups = {
	inventory: function(c) { return c.category_name === 'equipment' || c.category_name === 'management'; },
	gpon: function(c) { return c.category_name === 'ani'; },
	bridge: function(c) { return c.category_name === 'uni' || c.category_name === 'layer2' || !!(c.flags & (1 << 6)); },
	multicast: function(c) { return c.category_name === 'multicast'; },
	other: function(c) {
		return c.category_name !== 'equipment' && c.category_name !== 'management' &&
		       c.category_name !== 'ani' && c.category_name !== 'uni' &&
		       c.category_name !== 'layer2' && c.category_name !== 'multicast' &&
		       !(c.flags & (1 << 6));
	}
};

function load() {
	return Promise.all([ omci.classList(0), omci.mibList(0) ]);
}

function render(group, data, title, description) {
	var classes = data[0] || [];
	var objects = data[1] || [];
	var byId = {};
	classes.forEach(function(c) { byId[c.class_id] = c; });
	var predicate = groups[group] || groups.other;
	var filtered = objects.filter(function(o) {
		return byId[o.class_id] && predicate(byId[o.class_id]);
	});

	var table = E('table', { 'class': 'table' }, [
		E('tr', { 'class': 'tr table-titles' }, [
			E('th', { 'class': 'th' }, _('Class')),
			E('th', { 'class': 'th' }, _('Entity')),
			E('th', { 'class': 'th' }, _('Managed entity')),
			E('th', { 'class': 'th' }, _('Origin')),
			E('th', { 'class': 'th' }, _('Mask')),
			E('th', { 'class': 'th' }, _('Data'))
		])
	]);

	filtered.forEach(function(o) {
		var c = byId[o.class_id] || {};
		table.appendChild(E('tr', { 'class': 'tr' }, [
			E('td', { 'class': 'td' }, String(o.class_id)),
			E('td', { 'class': 'td' }, '0x' + Number(o.entity_id || 0).toString(16).padStart(4, '0')),
			E('td', { 'class': 'td' }, o.name || c.name || '-'),
			E('td', { 'class': 'td' }, o.origin_name || '-'),
			E('td', { 'class': 'td' }, '0x' + Number(o.attr_mask || 0).toString(16).padStart(4, '0')),
			E('td', { 'class': 'td', 'style': 'font-family:monospace;overflow-wrap:anywhere' }, o.data || '')
		]));
	});

	return E([], [
		E('h2', {}, title),
		E('p', {}, description),
		filtered.length ? table : E('div', { 'class': 'alert-message notice' }, _('No MIB objects in this category.'))
	]);
}

return { load: load, render: render };
