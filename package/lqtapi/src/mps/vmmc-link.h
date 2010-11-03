#ifndef __VMMC_LINK_H__
#define __VMMC_LINK_H__

struct tapi_link *vmmc_tapi_link_alloc(struct tapi_device *tdev,
	struct tapi_endpoint *ep1, struct tapi_endpoint *ep2);
void vmmc_tapi_link_free(struct tapi_device *tdev, struct tapi_link *link);
int vmmc_tapi_link_enable(struct tapi_device *tdev, struct tapi_link *link);
int vmmc_tapi_link_disable(struct tapi_device *tdev, struct tapi_link *link);

#endif
