
struct vmmc_coder {
	struct vmmc *vmmc;
	unsigned int id;

	struct vmmc_module module;

	unsigned int enabled;

	uint32_t cmd_cache[4];
	uint32_t jitter_buffer_cache[3];

	struct tapi_stream *stream;
};
