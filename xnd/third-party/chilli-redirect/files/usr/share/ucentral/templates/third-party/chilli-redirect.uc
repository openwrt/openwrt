{%
let interfaces = services.lookup_interfaces("chilli-redirect");
let enable = length(interfaces);
services.set_enabled("chilli", enable);
if (!enable)
	return;
let name = ethernet.calculate_name(interfaces[0]);
let keys = {
	uamport: 3990,
	radiusauthport: 1812,
	radiusacctport: 1813,
	radiusserver1: false,
	radiusserver2: false,
	radiusnasid: false,
	uamallowed: false,
	uamdomain: false,
	defidletimeout: 0,
	definteriminterval: 300,
	acctupdate: 9,
	uamserver: false,
	radiussecret: false,
	nasmac: false,
	macauth: false,
	macpassword: false,
};

function get_value(key, value) {
	if (key in chilli_redirect)
		return chilli_redirect[key];

	return value ? value : '';
}

%}

set chilli.@chilli[0].dhcpif='{{ name }}'

{% if (interfaces[0].role == "upstream"): %}
set chilli.@chilli[0].net='198.18.0.0/255.255.254.0'
set chilli.@chilli[0].statip='198.18.0.0/255.255.254.0'
set chilli.@chilli[0].uamlisten='198.18.0.1'
set chilli.@chilli[0].uamanyip='1'
set chilli.@chilli[0].dns1='198.18.0.1'
set chilli.@chilli[0].nasip='198.18.0.1'
{% else %}
set chilli.@chilli[0].net='10.0.0.0/255.255.254.0'
set chilli.@chilli[0].statip='10.0.0.0/255.255.254.0'
set chilli.@chilli[0].uamlisten='10.0.0.1'
set chilli.@chilli[0].dns1='10.0.0.1'
set chilli.@chilli[0].nasip='10.0.0.1'
{% endif %}

{% for (let k, v in keys): %}
set chilli.@chilli[0].{{ k }}='{{ get_value(k, v) }}'
{% endfor %}
