#include <linux/kernel.h>
#include <linux/slab.h>

#include <linux/tapi/tapi.h>

#include "vmmc.h"
#include "vmmc-port.h"
#include "vmmc-alm.h"
#include "vmmc-sig.h"

int vmmc_port_ring(struct tapi_device *tdev, struct tapi_port *port, bool ring)
{
	struct vmmc *vmmc = tdev_to_vmmc(tdev);

	return vmmc_alm_set_state(&vmmc->ports[port->id].alm,
		ring ? VMMC_ALM_STATE_RING : VMMC_ALM_STATE_ONHOOK);
}

int vmmc_port_send_dtmf_event(struct tapi_device *tdev,
	struct tapi_port *port, struct tapi_dtmf_event *event)
{
	return 0;
}

struct vmmc_port *vmmc_port_init(struct vmmc *vmmc, struct vmmc_port *port,
	struct tapi_port *tport, unsigned int id)
{
	vmmc_alm_init(&port->alm, vmmc, id);
	vmmc_sig_init(&port->sig, vmmc, id);

	port->sig_pin = vmmc_module_get_pin(&port->sig.module);
	vmmc_module_set_pin_input(&port->sig.module, port->sig_pin,
		&port->alm.module);

	vmmc_module_sync(&port->sig.module);

	vmmc_alm_set_state(&port->alm, VMMC_ALM_STATE_ONHOOK);

	tport->id = id;
	tapi_endpoint_set_data(&tport->ep, &port->alm.module);

	return port;
}

void vmmc_port_put(struct vmmc *vmmc, struct vmmc_port *port)
{
	vmmc_module_put_pin(&port->sig.module, port->sig_pin);
}
