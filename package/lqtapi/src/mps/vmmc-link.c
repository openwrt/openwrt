#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/tapi/tapi.h>

#include "vmmc-link.h"
#include "vmmc-module.h"

struct vmmc_tapi_link {
	struct tapi_link tapi_link;
	struct vmmc_link vmmc_link;
};

struct tapi_link *vmmc_tapi_link_alloc(struct tapi_device *tdev,
	struct tapi_endpoint *ep1, struct tapi_endpoint *ep2)
{
	struct vmmc_tapi_link *link = kzalloc(sizeof(*link), GFP_KERNEL);
	struct vmmc_module *module1 = tapi_endpoint_to_vmmc_module(ep1);
	struct vmmc_module *module2 = tapi_endpoint_to_vmmc_module(ep2);

	vmmc_link_init(&link->vmmc_link, module1, module2);

	return &link->tapi_link;
}

void vmmc_tapi_link_free(struct tapi_device *tdev, struct tapi_link *tapi_link)
{
	struct vmmc_tapi_link *link = container_of(tapi_link, struct vmmc_tapi_link,
		tapi_link);

	vmmc_link_put(&link->vmmc_link);
	kfree(link);
}

int vmmc_tapi_link_enable(struct tapi_device *tdev,
	struct tapi_link *tapi_link)
{
	struct vmmc_tapi_link *link = container_of(tapi_link, struct vmmc_tapi_link,
		tapi_link);

	vmmc_link_enable(&link->vmmc_link);
	return 0;
}

int vmmc_tapi_link_disable(struct tapi_device *tdev,
	struct tapi_link *tapi_link)
{
	struct vmmc_tapi_link *link = container_of(tapi_link, struct vmmc_tapi_link,
		tapi_link);

	vmmc_link_disable(&link->vmmc_link);
	return 0;
}
