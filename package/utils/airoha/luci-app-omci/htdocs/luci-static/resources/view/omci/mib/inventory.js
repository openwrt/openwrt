'use strict';
'require omci.mib as mib';

return mib.createListView({
	title: _('ONU inventory and identity MIBs'),
	description: _('ONU-G, ONU2-G, ONU data, software, equipment and related identity objects.'),
	category: 'inventory'
});
