'use strict';
'require network';

return network.registerProtocol('batadv', {
	getI18n: function() {
		return _('Batman');
	}
});
