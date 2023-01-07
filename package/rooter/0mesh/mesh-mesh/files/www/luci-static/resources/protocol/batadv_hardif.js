'use strict';
'require network';

return network.registerProtocol('batadv_hardif', {
	getI18n: function() {
		return _('Mesh');
	}
});
