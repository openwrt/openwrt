



static int __init glamo_mmc_probe(struct platform_device *pdev) {

	struct mmc_host *mmc;
	struct glamo_host *host;
	struct resource *res;
	int irq;

	res = platform_get_resource(pdev, IO_RESOURCE_MEM, 0);
	irq = platform_get_irq(pdev, 0);


	res = request_mem_region(res->start, res->end - res->start + 1, pdev->name);

	mmc = mmc_alloc_host(sizeof(struct glamo_host), &pdev->dev);

	mmc->ops = &glamo_mmc_ops;

	mmc->f_min = 0;
	mmc->f_max = 100;
	mmc->ocr_avail;

	mmc->caps = MMC_CAP_4_BIT_DATA;

	mmc->max_hw_segs;
	mmc->max_phys_segs;
	mmc->max_req_size;
	mmc->max_blk_size;
	mmc->max_blk_count;


	mmc_add_host(mmc);

	return 0;
}
