'use strict';
'require omci.mib as mib';

return mib.createListView({
	title: _('Ethernet and bridge MIBs'),
	description: _('Ethernet UNI, VEIP, MAC bridge, bridge port and mapper objects.'),
	category: 'bridge'
});
