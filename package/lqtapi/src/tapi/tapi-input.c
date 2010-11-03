
#include <linux/tapi/tapi.h>

#include <linux/input.h>

static unsigned short tapi_keycodes[] = {
	[0] = KEY_NUMERIC_0,
	[1] = KEY_NUMERIC_1,
	[2] = KEY_NUMERIC_2,
	[3] = KEY_NUMERIC_3,
	[4] = KEY_NUMERIC_4,
	[5] = KEY_NUMERIC_5,
	[6] = KEY_NUMERIC_6,
	[7] = KEY_NUMERIC_7,
	[8] = KEY_NUMERIC_8,
	[9] = KEY_NUMERIC_9,
	[10] = KEY_NUMERIC_STAR,
	[11] = KEY_NUMERIC_POUND,
	[12] = KEY_ENTER,
	[13] = KEY_ESC,
};

static int tapi_input_event(struct input_dev *input, unsigned int type,
	unsigned int code, int value)
{
	struct tapi_device *tdev = dev_to_tapi(input->dev.parent);
	struct tapi_port *port = input_get_drvdata(input);


	if (type != EV_SND || code != SND_BELL)
		return -EINVAL;

	tapi_port_set_ring(tdev, port, value);

	return 0;
}

void tapi_alloc_input(struct tapi_device *tdev, struct tapi_port *port)
{
	struct input_dev *input;
	int i;
	char *phys;

	input = input_allocate_device();

	phys = kzalloc(sizeof("tapi/input000"), GFP_KERNEL);
	sprintf(phys, "tapi/input%d", port->id);

	input->name = "tapi";
	input->phys = phys;
	input->id.bustype = BUS_HOST;
	input->dev.parent = &tdev->dev;
	input->evbit[0] = BIT(EV_KEY) | BIT(EV_SND);
	input->sndbit[0] = BIT(SND_BELL);

	input->event = tapi_input_event;

	input->keycodesize = sizeof(unsigned short);
	input->keycodemax = ARRAY_SIZE(tapi_keycodes);
	input->keycode = tapi_keycodes;

	port->input = input;

	for (i = 0; i < ARRAY_SIZE(tapi_keycodes); ++i)
		__set_bit(tapi_keycodes[i], input->keybit);

	input_set_drvdata(input, port);
	input_register_device(input);
}

void tapi_report_event(struct tapi_device *tdev,
	struct tapi_event *event)
{
	unsigned short key_code;
	struct input_dev *input;

	if (!tdev || !tdev->ports)
		return;

	switch (event->type) {
		case TAPI_EVENT_TYPE_HOOK:
			if (event->hook.on)
				key_code = KEY_ENTER;
			else
				key_code = KEY_ESC;
			break;
		case TAPI_EVENT_TYPE_DTMF:
			key_code = tapi_keycodes[event->dtmf.code];
			break;
		default:
			return;
	}

	input = tdev->ports[event->port].input;
	input_report_key(input, key_code, 1);
	input_sync(input);
	input_report_key(input, key_code, 0);
	input_sync(input);
}
