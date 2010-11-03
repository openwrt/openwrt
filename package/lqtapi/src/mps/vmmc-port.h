#ifndef __VMMC_PORT_H__
#define __VMMC_PORT_H__

#include "vmmc-alm.h"
#include "vmmc-sig.h"

void vmmc_port_free(struct vmmc *vmmc, struct vmmc_port *port);

int vmmc_port_ring(struct tapi_device *tdev, struct tapi_port *port, bool ring);
int vmmc_port_send_dtmf_event(struct tapi_device *tdev,
	struct tapi_port *port, struct tapi_dtmf_event *event);
struct vmmc_port *vmmc_port_init(struct vmmc *vmmc, struct vmmc_port *port,
struct tapi_port *tport, unsigned int id);

struct vmmc_port
{
	struct vmmc_alm alm;
	struct vmmc_sig sig;
	unsigned int sig_pin;
};

#endif
