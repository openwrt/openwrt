'use strict';
'require omci.mib as mib';

return mib.createListView({
	title: _('Multicast MIBs'),
	description: _('Multicast GEM, operations profile and subscriber configuration objects.'),
	category: 'multicast'
});
