'use strict';
'require rpc';

var object = 'omci.agent';

function declare(method, params, expect) {
	return rpc.declare({
		object: object,
		method: method,
		params: params || [],
		expect: expect || {}
	});
}

return {
	family: declare('family', [], { family: {} }),
	status: declare('status', [ 'device' ], { status: {} }),
	agentSet: declare('agent_set', [ 'device', 'enabled', 'permissive', 'fake_omci', 'dying_gasp' ], {}),
	configList: declare('config_list', [ 'device' ], { config: {} }),
	configDetails: declare('config_details', [ 'device' ], { config: {} }),
	configGet: declare('config_get', [ 'device', 'key' ], { config: {} }),
	configInspect: declare('config_inspect', [ 'device', 'key' ], { config: {} }),
	configSet: declare('config_set', [ 'device', 'key', 'value' ], {}),
	classList: declare('class_list', [ 'device' ], { classes: [] }),
	classGet: declare('class_get', [ 'device', 'class_id' ], { class: {} }),
	mibList: declare('mib_list', [ 'device' ], { objects: [] }),
	mibGet: declare('mib_get', [ 'device', 'class_id', 'entity_id' ], { object: {} }),
	mibSet: declare('mib_set', [ 'device', 'class_id', 'entity_id', 'attr_mask', 'data' ], {}),
	mibDelete: declare('mib_delete', [ 'device', 'class_id', 'entity_id' ], {}),
	mibReset: declare('mib_reset', [ 'device' ], {}),
	rawTx: declare('raw_tx', [ 'device', 'pdu' ], {}),
	apply: declare('apply', [ 'device' ], {}),
	profileList: declare('profile_list', [], { profiles: [] }),
	profileApply: declare('profile_apply', [ 'device', 'profile', 'serial', 'password' ], {})
};
