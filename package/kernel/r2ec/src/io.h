#ifndef R2EC_IO_H
#define R2EC_IO_H

#define NO_OF_GPIOS 37

enum proto_version {
	PROTO_VERSION_1 = 0x01,
	PROTO_VERSION_2 = 0x02
};

enum cmd_type_id {
	CMD_GPIO = 0x06,
	CMD_PROTO = 0xFC,
	CMD_FW = 0xFD,
	CMD_BOOT = 0xFE
};

enum proto_id {
	PROTO_GET_SUPPORTED = 0x03
};

enum boot_id {
	BOOT_START_APP = 0x03,
	BOOT_STATE = 0xFD,
	BOOT_VERSION = 0xFE
};

enum state_id {
	NO_IMAGE_FOUND = 0x17,
	BOOT_STARTED = 0x18,
	WATCHDOG_RESET = 0x1B,
	APPLICATION_START_FAIL = 0x99,
	HARD_FAULT_ERROR = 0x9A,
	APP_STARTED = 0xFC,
	NO_DATA_AVAILABLE = 0xFF
};

enum ack_id {
	STATUS_ACK = 0x7D,
	STATUS_NACK = 0x7E
};

enum gpio_state {
	GPIO_STATE_HIGH = 0x1E,
	GPIO_STATE_LOW = 0x9F
};

enum gpio_mode {
	GPIO_VALUE_SET_LOW = 0x00,
	GPIO_VALUE_SET_HIGH = 0x01,
	GPIO_VALUE_GET = 0x02,
	GPIO_MODE_SET_OUTPUT = 0x04,
	GPIO_MODE_SET_INPUT = 0x05
};

enum fw_id {
	FW_VERSION = 0x01
};

#endif // R2EC_IO_H