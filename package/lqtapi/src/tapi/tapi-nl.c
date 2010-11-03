
static struct tapi_attr default_port[] = {
	[PORTS] = {
		.type = TAPI_TYPE_PORTS,
		.name = "ports",
		.description = "foobar",
		.set = tapi_set_ports,
		.get = tapi_get_ports,
	},
};

static const struct nla_policy tapi_policy[] = {
	[TAPI_ATTR_ID] = { .type = NLA_U32 },
	[TAPI_ATTR_PORT] = { .type = NLA_U32 },
	[TAPI_ATTR_ENDPOINT] = { .type = NLA_U32 },
	[TAPI_ATTR_STREAM] = { .type = NLA_U32 }
};

static const struct nla_policy tapi_port_policy[] = {
	[TAPI_PORT_ID] = { .type = NLA_U32 },
};

static const struct nla_policy tapi_endpoint_policy[] = {
	[TAPI_ENDPOINT_ID] = { .type = NLA_U32 },
};

static const struct nla_policy tapi_stream_policy[] = {
	[TAPI_STREAM_ID] = { .type = NLA_U32 },
};

static struct genl_family tapi_nl_family = {
	.id = GENL_ID_GENERATE,
	.name = "tapi",
	.hdrsize = 0,
	.version = 1,
	.maxattr = ARRAY_SIZE(tapi_policy),
};



static struct genl_ops tapi_nl_ops[] = {
	TAPI_NL_OP(TAPI_CMD_LIST, list_attr),

};

static int __init tapi_nl_init(void)
{
	ret = genl_unregister_family(&tapi_nl_family);
	if (ret)
		return ret;

	genl_register_ops(&tapi_nl_family, tapi_nl_ops);

	return 0;
}
module_init(tapi_nl_init);

static void __exit tapi_nl_exit(void)
{
	genl_unregister_family(&tapi_nl_family);
}

