'use strict';
'require view';
'require omci.mib as mib';

return view.extend({
	load: mib.load,
	render: function(data) {
		return mib.render('other', data, _('Other MIB objects'), _('Managed entities that are not part of the primary inventory, GPON, bridge or multicast groups.'));
	},
	handleSaveApply: null,
	handleSave: null,
	handleReset: null
});
