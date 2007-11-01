/*
 * OHCI HCD (Host Controller Driver) for USB.
 *
 * (C) Copyright 1999 Roman Weissgaerber <weissg@vienna.at>
 * (C) Copyright 2000-2002 David Brownell <dbrownell@users.sourceforge.net>
 *
 * This file is licenced under the GPL.
 */

/*-------------------------------------------------------------------------*/

/*
 * OHCI deals with three types of memory:
 *	- data used only by the HCD ... kmalloc is fine
 *	- async and periodic schedules, shared by HC and HCD ... these
 *	  need to use dma_pool or dma_alloc_coherent
 *	- driver buffers, read/written by HC ... the hcd glue or the
 *	  device driver provides us with dma addresses
 *
 * There's also "register" data, which is memory mapped.
 * No memory seen by this driver (or any HCD) may be paged out.
 */

/*-------------------------------------------------------------------------*/

static void admhc_hcd_init(struct admhcd *ahcd)
{
	ahcd->next_statechange = jiffies;
	spin_lock_init(&ahcd->lock);
	spin_lock_init(&ahcd->dma_lock);
}

/*-------------------------------------------------------------------------*/

static int admhc_mem_init(struct admhcd *ahcd)
{
	ahcd->td_cache = dma_pool_create("admhc_td",
		admhcd_to_hcd(ahcd)->self.controller,
		sizeof(struct td),
		TD_ALIGN, /* byte alignment */
		0 /* no page-crossing issues */
		);
	if (!ahcd->td_cache)
		goto err;

	ahcd->ed_cache = dma_pool_create("admhc_ed",
		admhcd_to_hcd(ahcd)->self.controller,
		sizeof(struct ed),
		ED_ALIGN, /* byte alignment */
		0 /* no page-crossing issues */
		);
	if (!ahcd->ed_cache)
		goto err_td_cache;

	return 0;

err_td_cache:
	dma_pool_destroy(ahcd->td_cache);
	ahcd->td_cache = NULL;
err:
	return -ENOMEM;
}

static void admhc_mem_cleanup(struct admhcd *ahcd)
{
	if (ahcd->td_cache) {
		dma_pool_destroy(ahcd->td_cache);
		ahcd->td_cache = NULL;
	}

	if (ahcd->ed_cache) {
		dma_pool_destroy(ahcd->ed_cache);
		ahcd->ed_cache = NULL;
	}
}

/*-------------------------------------------------------------------------*/

/* TDs ... */
static struct td *td_alloc(struct admhcd *ahcd, gfp_t mem_flags)
{
	dma_addr_t	dma;
	struct td	*td;

	td = dma_pool_alloc(ahcd->td_cache, mem_flags, &dma);
	if (!td)
		return NULL;

	/* in case ahcd fetches it, make it look dead */
	memset(td, 0, sizeof *td);
	td->td_dma = dma;

	return td;
}

static void td_free(struct admhcd *ahcd, struct td *td)
{
	dma_pool_free(ahcd->td_cache, td, td->td_dma);
}

/*-------------------------------------------------------------------------*/

/* EDs ... */
static struct ed *ed_alloc(struct admhcd *ahcd, gfp_t mem_flags)
{
	dma_addr_t	dma;
	struct ed	*ed;

	ed = dma_pool_alloc(ahcd->ed_cache, mem_flags, &dma);
	if (!ed)
		return NULL;

	memset(ed, 0, sizeof(*ed));
	ed->dma = dma;

	INIT_LIST_HEAD(&ed->urb_pending);

	return ed;
}

static void ed_free(struct admhcd *ahcd, struct ed *ed)
{
	dma_pool_free(ahcd->ed_cache, ed, ed->dma);
}

/*-------------------------------------------------------------------------*/

/* URB priv ... */
static void urb_priv_free(struct admhcd *ahcd, struct urb_priv *urb_priv)
{
	int i;

	for (i = 0; i < urb_priv->td_cnt; i++)
		if (urb_priv->td[i])
			td_free(ahcd, urb_priv->td[i]);

	kfree(urb_priv);
}

static struct urb_priv *urb_priv_alloc(struct admhcd *ahcd, int num_tds,
		gfp_t mem_flags)
{
	struct urb_priv	*priv;
	int i;

	/* allocate the private part of the URB */
	priv = kzalloc(sizeof(*priv) + sizeof(struct td) * num_tds, mem_flags);
	if (!priv)
		goto err;

	/* allocate the TDs (deferring hash chain updates) */
	for (i = 0; i < num_tds; i++) {
		priv->td[i] = td_alloc(ahcd, mem_flags);
		if (priv->td[i] == NULL)
			goto err_free;
		priv->td[i]->index = i;
	}

	INIT_LIST_HEAD(&priv->pending);
	priv->td_cnt = num_tds;

	return priv;

err_free:
	urb_priv_free(ahcd, priv);
err:
	return NULL;
}
