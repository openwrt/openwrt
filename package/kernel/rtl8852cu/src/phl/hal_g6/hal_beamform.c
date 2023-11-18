/******************************************************************************
 *
 * Copyright(c) 2019 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 *****************************************************************************/

#include "hal_headers.h"

/**
 * rtw_hal_bf_dbg_dump_entry
 * @entry: hal_bf_entry for dump content
 **/
void rtw_hal_bf_dbg_dump_entry(void *entry)
{
	struct hal_bf_entry *bf_entry = (struct hal_bf_entry *)entry;
	if (bf_entry == NULL) {
		return;
	}

	PHL_INFO("-----DUMP BF ENTRY-----\n");
	PHL_INFO("bf_entry->bf_idx 0x%x \n", bf_entry->bf_idx);
	PHL_INFO("bf_entry->macid 0x%x \n", bf_entry->macid);
	PHL_INFO("bf_entry->aid12 0x%x \n", bf_entry->aid12);
	PHL_INFO("bf_entry->band 0x%x \n", bf_entry->band);
	PHL_INFO("bf_entry->en_swap 0x%x \n", bf_entry->en_swap ? 1 : 0);
	PHL_INFO("bf_entry->counter 0x%x \n", bf_entry->couter);
	PHL_INFO("bf_entry->csi_buf 0x%x \n", bf_entry->csi_buf);
	PHL_INFO("bf_entry->csi_buf_swap 0x%x \n", bf_entry->csi_buf_swap);
	if (bf_entry->bfee != NULL) {
		PHL_INFO("bf_entry->bfee->type 0x%x \n", bf_entry->bfee->type);
	}
}
/**
 * rtw_hal_bf_dbg_dump_entry_all
 * @hal: hal_info_t
 **/
void rtw_hal_bf_dbg_dump_entry_all(void *hal)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct rtw_hal_com_t *hal_com = hal_info->hal_com;
	struct hal_bf_obj *bf_obj = (struct hal_bf_obj *)hal_com->bf_obj;
	struct hal_bf_entry *bf_entry;
	_os_list *busy_list = &bf_obj->bf_busy_list;
	_os_list *idle_list = &bf_obj->bf_idle_list;

	PHL_INFO("-----DUMP BF BUSY LIST-----\n");
	if(list_empty(busy_list)) {
		PHL_INFO("BF Entry BUSY LIST is Empty\n");
	} else {
		phl_list_for_loop(bf_entry, struct hal_bf_entry, busy_list,
					list) {
			PHL_INFO("bf_entry->bf_idx 0x%x \n", bf_entry->bf_idx);
			PHL_INFO("bf_entry->macid 0x%x \n", bf_entry->macid);
			PHL_INFO("bf_entry->aid12 0x%x \n", bf_entry->aid12);
			PHL_INFO("bf_entry->band 0x%x \n", bf_entry->band);
			PHL_INFO("bf_entry->en_swap 0x%x \n", bf_entry->en_swap ? 1 : 0);
			PHL_INFO("bf_entry->counter 0x%x \n", bf_entry->couter);
			PHL_INFO("bf_entry->csi_buf 0x%x \n",
					bf_entry->csi_buf);
			PHL_INFO("bf_entry->csi_buf_swap 0x%x \n",
					bf_entry->csi_buf_swap);
			if (NULL!=bf_entry->bfee) {
				PHL_INFO("bfee->idx 0x%x \n",
					bf_entry->bfee->idx);
				PHL_INFO("bfee->type 0x%x \n",
					bf_entry->bfee->type);
			}
		}
	}
	PHL_INFO("-----DUMP BF IDLE LIST-----\n");
	if(list_empty(idle_list)) {
		PHL_INFO("BF Entry IDLE LIST is Empty\n");
	} else {
		phl_list_for_loop(bf_entry, struct hal_bf_entry, idle_list,
					list) {
			PHL_INFO("bf_entry->bf_idx 0x%x \n", bf_entry->bf_idx);
			PHL_INFO("bf_entry->macid 0x%x \n", bf_entry->macid);
			PHL_INFO("bf_entry->aid12 0x%x \n", bf_entry->aid12);
			PHL_INFO("bf_entry->band 0x%x \n", bf_entry->band);
			PHL_INFO("bf_entry->csi_buf 0x%x \n",
					bf_entry->csi_buf);
		}
	}
}

void _reset_bf_entry(struct hal_bf_entry *bf_entry)
{
	bf_entry->macid = 0;
	bf_entry->aid12 = 0;
	bf_entry->csi_buf = 0;
	bf_entry->bfee = NULL;
	bf_entry->csi_buf_swap = 0;
	bf_entry->couter = 0;
}

void _reset_sumu_entry(struct hal_sumu_entry *entry)
{
	entry->snd_sts = 0;
	return;
}

/* START of tx bf entry */
static struct hal_bf_entry *_query_idle_bf_entry(struct hal_info_t *hal_info,
				struct hal_bf_obj *bf_obj)
{
	void *drv_priv = hal_to_drvpriv(hal_info);
	_os_list *idle_list = &bf_obj->bf_idle_list;
	struct hal_bf_entry *bf_entry = NULL;

	_os_spinlock(drv_priv, &bf_obj->bf_lock, _ps, NULL);
	if (true == list_empty(idle_list)) {
		bf_entry = NULL;
	} else {
		bf_entry = list_first_entry(idle_list, struct hal_bf_entry,
					list);
		bf_obj->num_idle_bf_entry--;
		list_del(&bf_entry->list);
	}
	_os_spinunlock(drv_priv, &bf_obj->bf_lock, _ps, NULL);

	return bf_entry;
}

static enum rtw_hal_status _enqueue_idle_bf_entry(
				struct hal_info_t *hal_info,
				struct hal_bf_obj *bf_obj,
				struct hal_bf_entry *bf_entry)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_FAILURE;
	void *drv_priv = hal_to_drvpriv(hal_info);
	_os_list *list = &bf_obj->bf_idle_list;

	if (bf_entry != NULL) {
		_os_spinlock(drv_priv, &bf_obj->bf_lock, _ps, NULL);
		list_add(&bf_entry->list, list);
		bf_obj->num_idle_bf_entry++;
		_os_spinunlock(drv_priv, &bf_obj->bf_lock, _ps, NULL);
		status = RTW_HAL_STATUS_SUCCESS;
	}

	return status;
}

static enum rtw_hal_status _enqueue_busy_bf_entry(
				struct hal_info_t *hal_info,
				struct hal_bf_obj *bf_obj,
				struct hal_bf_entry *bf_entry)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_FAILURE;
	void *drv_priv = hal_to_drvpriv(hal_info);
	_os_list *list = &bf_obj->bf_busy_list;

	if (bf_entry != NULL) {
		_os_spinlock(drv_priv, &bf_obj->bf_lock, _ps, NULL);
		list_add_tail(&bf_entry->list, list);
		_os_spinunlock(drv_priv, &bf_obj->bf_lock, _ps, NULL);
		status = RTW_HAL_STATUS_SUCCESS;
	}

	return status;
}
/*su entry*/
static struct hal_sumu_entry *_query_idle_su_entry(struct hal_info_t *hal_info,
				struct hal_bf_obj *bf_obj)
{
	void *drv_priv = hal_to_drvpriv(hal_info);
	_os_list *idle_list = &bf_obj->su_idle_list;
	struct hal_sumu_entry *su_entry = NULL;

	_os_spinlock(drv_priv, &bf_obj->bf_lock, _ps, NULL);
	if (true == list_empty(idle_list)) {
		su_entry = NULL;
	} else {
		su_entry = list_first_entry(idle_list, struct hal_sumu_entry,
					list);
		bf_obj->num_idle_su_entry--;
		list_del(&su_entry->list);
	}
	_os_spinunlock(drv_priv, &bf_obj->bf_lock, _ps, NULL);

	return su_entry;
}

static enum rtw_hal_status _enqueue_idle_su_entry(
				struct hal_info_t *hal_info,
				struct hal_bf_obj *bf_obj,
				struct hal_sumu_entry *su_entry)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_FAILURE;
	void *drv_priv = hal_to_drvpriv(hal_info);
	_os_list *list = &bf_obj->su_idle_list;

	if (su_entry != NULL) {
		_os_spinlock(drv_priv, &bf_obj->bf_lock, _ps, NULL);
		list_add(&su_entry->list, list);
		bf_obj->num_idle_su_entry++;
		_os_spinunlock(drv_priv, &bf_obj->bf_lock, _ps, NULL);
		status = RTW_HAL_STATUS_SUCCESS;
	}

	return status;
}

static enum rtw_hal_status _enqueue_busy_su_entry(
				struct hal_info_t *hal_info,
				struct hal_bf_obj *bf_obj,
				struct hal_sumu_entry *su_entry)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_FAILURE;
	void *drv_priv = hal_to_drvpriv(hal_info);
	_os_list *list = &bf_obj->su_busy_list;

	if (su_entry != NULL) {
		_os_spinlock(drv_priv, &bf_obj->bf_lock, _ps, NULL);
		list_add_tail(&su_entry->list, list);
		_os_spinunlock(drv_priv, &bf_obj->bf_lock, _ps, NULL);
		status = RTW_HAL_STATUS_SUCCESS;
	}

	return status;
}


/*mu entry*/
static struct hal_sumu_entry *_query_idle_mu_entry(struct hal_info_t *hal_info,
				struct hal_bf_obj *bf_obj)
{
	void *drv_priv = hal_to_drvpriv(hal_info);
	_os_list *idle_list = &bf_obj->mu_idle_list;
	struct hal_sumu_entry *mu_entry = NULL;

	_os_spinlock(drv_priv, &bf_obj->bf_lock, _ps, NULL);
	if (true == list_empty(idle_list)) {
		mu_entry = NULL;
	} else {
		mu_entry = list_first_entry(idle_list, struct hal_sumu_entry,
					list);
		bf_obj->num_idle_mu_entry--;
		list_del(&mu_entry->list);
	}
	_os_spinunlock(drv_priv, &bf_obj->bf_lock, _ps, NULL);

	return mu_entry;
}

static enum rtw_hal_status _enqueue_idle_mu_entry(
				struct hal_info_t *hal_info,
				struct hal_bf_obj *bf_obj,
				struct hal_sumu_entry *mu_entry)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_FAILURE;
	void *drv_priv = hal_to_drvpriv(hal_info);
	_os_list *list = &bf_obj->mu_idle_list;

	if (mu_entry != NULL) {
		_os_spinlock(drv_priv, &bf_obj->bf_lock, _ps, NULL);
		list_add(&mu_entry->list, list);
		bf_obj->num_idle_mu_entry++;
		_os_spinunlock(drv_priv, &bf_obj->bf_lock, _ps, NULL);
		status = RTW_HAL_STATUS_SUCCESS;
	}

	return status;
}

static enum rtw_hal_status _enqueue_busy_mu_entry(
				struct hal_info_t *hal_info,
				struct hal_bf_obj *bf_obj,
				struct hal_sumu_entry *mu_entry)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_FAILURE;
	void *drv_priv = hal_to_drvpriv(hal_info);
	_os_list *list = &bf_obj->mu_busy_list;

	if (mu_entry != NULL) {
		_os_spinlock(drv_priv, &bf_obj->bf_lock, _ps, NULL);
		list_add_tail(&mu_entry->list, list);
		_os_spinunlock(drv_priv, &bf_obj->bf_lock, _ps, NULL);
		status = RTW_HAL_STATUS_SUCCESS;
	}

	return status;
}

/* hal bf init */
enum rtw_hal_status _hal_bf_init_su_entry(
				struct hal_info_t *hal_info,
				u8 num)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_SUCCESS;
	void *drv_priv = hal_to_drvpriv(hal_info);
	struct rtw_hal_com_t *hal_com = hal_info->hal_com;
	struct hal_bf_obj *bf_obj = (struct hal_bf_obj *)hal_com->bf_obj;
	struct hal_sumu_entry *su_entry = NULL;
	u8 i;

	do {
		bf_obj->su_entry = _os_mem_alloc(drv_priv,
					sizeof(*su_entry) * num);

		if (NULL == bf_obj->su_entry) {
			status = RTW_HAL_STATUS_RESOURCE;
			break;
		}
		_os_mem_set(drv_priv, bf_obj->su_entry, 0,
					sizeof(*su_entry) * num);

		su_entry = bf_obj->su_entry;

		for ( i = 0 ; i < num; i++) {
			su_entry[i].idx = i;
			su_entry[i].type = HAL_BFEE_SU;
			_reset_sumu_entry(&su_entry[i]);
			INIT_LIST_HEAD(&su_entry[i].list);
			list_add_tail(&su_entry[i].list, &bf_obj->su_idle_list);
			bf_obj->num_idle_su_entry++;
		}

	} while (0);
	return status;
}


enum rtw_hal_status _hal_bf_init_mu_entry(
	struct hal_info_t *hal_info,
	u8 num)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_SUCCESS;
	void *drv_priv = hal_to_drvpriv(hal_info);
	struct rtw_hal_com_t *hal_com = hal_info->hal_com;
	struct hal_bf_obj *bf_obj = (struct hal_bf_obj *)hal_com->bf_obj;
	struct hal_sumu_entry *mu_entry = NULL;
	u8 i;

	do {
		bf_obj->mu_entry = _os_mem_alloc(drv_priv,
					sizeof(*mu_entry) * num);

		if (NULL == bf_obj->mu_entry) {
			status = RTW_HAL_STATUS_RESOURCE;
			break;
		}
		_os_mem_set(drv_priv, bf_obj->mu_entry, 0,
					sizeof(*mu_entry) * num);

		mu_entry = bf_obj->mu_entry;

		for ( i = 0 ; i < num; i++) {
			mu_entry[i].idx = i;
			mu_entry[i].type = HAL_BFEE_MU;
			_reset_sumu_entry(&mu_entry[i]);
			INIT_LIST_HEAD(&mu_entry[i].list);
			list_add_tail(&mu_entry[i].list, &bf_obj->mu_idle_list);
			bf_obj->num_idle_mu_entry++;
		}

	} while (0);
	return status;
}


enum rtw_hal_status _hal_bf_init_bf_entry(
	struct hal_info_t *hal_info,
	u8 num)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_SUCCESS;
	void *drv_priv = hal_to_drvpriv(hal_info);
	struct rtw_hal_com_t *hal_com = hal_info->hal_com;
	struct hal_bf_obj *bf_obj = (struct hal_bf_obj *)hal_com->bf_obj;
	struct hal_bf_entry *bf_entry = NULL;
	u8 i;

	do {
		bf_obj->bf_entry = _os_mem_alloc(drv_priv,
						sizeof(*bf_entry) * num);
		if (NULL == bf_obj->bf_entry) {
			status = RTW_HAL_STATUS_RESOURCE;
			break;
		}
		_os_mem_set(drv_priv, bf_obj->bf_entry, 0,
					sizeof(*bf_entry) * num);

		bf_entry = bf_obj->bf_entry;

		for ( i = 0 ; i < num; i++) {
			bf_entry[i].bf_idx = i;
			_reset_bf_entry(&bf_entry[i]);
			INIT_LIST_HEAD(&bf_entry[i].list);
			list_add_tail(&bf_entry[i].list,
					&bf_obj->bf_idle_list);
			bf_obj->num_idle_bf_entry++;
		}

	} while (0);
	return status;
}


/* Start of HAL API for other HAL modules */
/**
 * hal_bf_init:
 * 	initialize of beamform resource mgnt module
 * input :
 * @hal_info: (struct hal_info_t *)
 * @bf_entry_nr:  Number of HW support TxBF Entry
 * @su_entry_nr: Number of HW support BFee-SU Entry
 * @mu_entry_nr: Number of HW support BFee-MU Entry
 **/
enum rtw_hal_status hal_bf_init(
	struct hal_info_t *hal_info,
	u8 bf_entry_nr,
	u8 su_entry_nr,
	u8 mu_entry_nr)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_SUCCESS;
	struct rtw_hal_com_t *hal_com = hal_info->hal_com;
	struct hal_bf_obj *bf_obj = NULL;
	void *drv_priv = hal_to_drvpriv(hal_info);

	FUNCIN();
	do {
		bf_obj = _os_mem_alloc(drv_priv, sizeof(*bf_obj));

		if (bf_obj == NULL) {
			status = RTW_HAL_STATUS_RESOURCE;
			break;
		}
		hal_com->bf_obj = bf_obj;

		_os_spinlock_init(drv_priv, &bf_obj->bf_lock);

		INIT_LIST_HEAD(&bf_obj->bf_idle_list);
		INIT_LIST_HEAD(&bf_obj->bf_busy_list);

		INIT_LIST_HEAD(&bf_obj->su_idle_list);
		INIT_LIST_HEAD(&bf_obj->su_busy_list);

		INIT_LIST_HEAD(&bf_obj->mu_idle_list);
		INIT_LIST_HEAD(&bf_obj->mu_busy_list);

		if (RTW_HAL_STATUS_SUCCESS !=
			_hal_bf_init_bf_entry(hal_info, bf_entry_nr)) {
			status = RTW_HAL_STATUS_RESOURCE;
			break;
		}
		bf_obj->max_bf_entry_nr =  bf_entry_nr;

		if (RTW_HAL_STATUS_SUCCESS !=
			_hal_bf_init_su_entry(hal_info, su_entry_nr)) {
			status = RTW_HAL_STATUS_RESOURCE;
			break;
		}
		bf_obj->max_su_bfee_nr = su_entry_nr;

		if (RTW_HAL_STATUS_SUCCESS !=
			_hal_bf_init_mu_entry(hal_info, mu_entry_nr)) {
			status = RTW_HAL_STATUS_RESOURCE;
			break;
		}
		bf_obj->max_mu_bfee_nr =  mu_entry_nr;

		bf_obj->self_bf_cap[0] = 0;
		bf_obj->self_bf_cap[1] = 0;

	} while (0);

	if (RTW_HAL_STATUS_SUCCESS != status) {
		hal_bf_deinit(hal_info);
	}
	FUNCOUT();

	return status;
}

/**
 * hal_bf_deinit
 * 	deinitialize of beamform resource mgnt module
 * input :
 * @hal_info: (struct hal_info_t *)
 **/
void hal_bf_deinit(struct hal_info_t *hal_info)
{
	struct rtw_hal_com_t *hal_com = hal_info->hal_com;
	struct hal_bf_obj *bf_obj = (struct hal_bf_obj *)hal_com->bf_obj;
	void *drv_priv = hal_to_drvpriv(hal_info);
	struct hal_bf_entry *bf_entry = bf_obj->bf_entry;
	struct hal_sumu_entry *mu_entry = bf_obj->mu_entry;
	struct hal_sumu_entry *su_entry = bf_obj->su_entry;

	if (bf_obj != NULL) {
		if (bf_entry != NULL) {
			_os_mem_free(hal_to_drvpriv(hal_info), bf_entry,
				sizeof(*bf_entry) * bf_obj->max_bf_entry_nr);
			bf_obj->bf_entry = NULL;
		}
		if (su_entry != NULL) {
			_os_mem_free(hal_to_drvpriv(hal_info), su_entry,
				sizeof(*su_entry) * bf_obj->max_su_bfee_nr);
			bf_obj->su_entry = NULL;
		}

		if (mu_entry != NULL) {
			_os_mem_free(hal_to_drvpriv(hal_info), mu_entry,
				sizeof(*mu_entry) * bf_obj->max_mu_bfee_nr);
			bf_obj->mu_entry = NULL;
		}

		_os_spinlock_free(drv_priv, &bf_obj->bf_lock);

		/* bf obj need free as last */
		_os_mem_free(hal_to_drvpriv(hal_info), bf_obj,
					sizeof(struct hal_bf_obj));
		hal_com->bf_obj = NULL;
	}
}
/**
 * hal_bf_release_target_bf_entry
 * 	Release the bf entry resource
 * input
 * @hal_info: (struct hal_info_t *)
 * @entry:  hal_bf_entry to be released
 **/
enum rtw_hal_status hal_bf_release_target_bf_entry(
				struct hal_info_t *hal_info,
				void *entry)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_FAILURE;
	struct rtw_hal_com_t *hal_com = hal_info->hal_com;
	struct hal_bf_obj *bf_obj = (struct hal_bf_obj *)hal_com->bf_obj;
	struct hal_bf_entry *bf_entry = (struct hal_bf_entry *)entry;
	void *drv_priv = hal_to_drvpriv(hal_info);
	u8 bfee_idx = 0;
	u16 macid_rsvd = 0;
	FUNCIN();
	if (bf_obj != NULL && bf_entry != NULL) {
		if (bf_entry->bfee != NULL) {

			bfee_idx = bf_entry->bfee->idx +
				((bf_entry->bfee->type == HAL_BFEE_MU) ?
				  bf_obj->max_su_bfee_nr : 0);
			/* Clear HW CR to avoid TxBF when sounding is abort */
			rtw_hal_mac_ax_set_bf_entry(hal_info->mac,
					bf_entry->band, (u8)macid_rsvd,
					bfee_idx, bf_entry->bf_idx, 0);

			_reset_sumu_entry(bf_entry->bfee);
			_os_spinlock(drv_priv, &bf_obj->bf_lock, _ps, NULL);
			list_del(&bf_entry->bfee->list);
			_os_spinunlock(drv_priv, &bf_obj->bf_lock, _ps, NULL);
			if (bf_entry->bfee->type == HAL_BFEE_MU) {
				_enqueue_idle_mu_entry(
					hal_info, bf_obj, bf_entry->bfee);
			} else {
				_enqueue_idle_su_entry(
					hal_info, bf_obj, bf_entry->bfee);
			}
		}
		_reset_bf_entry(bf_entry);
		_os_spinlock(drv_priv, &bf_obj->bf_lock, _ps, NULL);
		list_del(&bf_entry->list);
		_os_spinunlock(drv_priv, &bf_obj->bf_lock, _ps, NULL);
		_enqueue_idle_bf_entry(hal_info, bf_obj, bf_entry);
		status = RTW_HAL_STATUS_SUCCESS;
	}
	FUNCOUT();
	return status;
}

/**
 * hal_bf_query_idle_bf_entry
 * 	Get available beamformee entry
 * @hal_info: (struct hal_info_t *)
 * @mu:  Is MU BFee ? 1 = MU / 0 = SU
 * return :
 * @hal_bf_entry:  hal_bf_entry pointer
 **/
struct hal_bf_entry *
hal_bf_query_idle_bf_entry(
	struct hal_info_t *hal_info,
	bool mu)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_FAILURE;
	struct rtw_hal_com_t *hal_com = hal_info->hal_com;
	struct hal_bf_obj *bf_obj = (struct hal_bf_obj *)hal_com->bf_obj;
	struct hal_bf_entry *bf_entry = NULL;
	struct hal_sumu_entry *sumu_entry;

	FUNCIN();

	do {
		if (bf_obj == NULL)
			break;

		bf_entry = _query_idle_bf_entry(hal_info, bf_obj);

		PHL_TRACE(COMP_PHL_SOUND, _PHL_INFO_,
			  "_query_idle_bf_entry bf_entry %p \n", bf_entry);

		if (NULL == bf_entry)
			break;

		if (true == mu)
			bf_entry->bfee = _query_idle_mu_entry(hal_info, bf_obj);
		else
			bf_entry->bfee = _query_idle_su_entry(hal_info, bf_obj);

		if (NULL == bf_entry->bfee)
			break;

		PHL_TRACE(COMP_PHL_SOUND, _PHL_INFO_,
			  "_query_idle_bf_entry sumu_entry %p \n", bf_entry->bfee);
		sumu_entry = bf_entry->bfee;

		status = _enqueue_busy_bf_entry(hal_info, bf_obj, bf_entry);
		if (RTW_HAL_STATUS_SUCCESS != status) {
			PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, "warring :hal_bf_entry add busy queue fail !!!\n");
			break;
		}

		if (true == mu)
			status = _enqueue_busy_mu_entry(
					hal_info, bf_obj, sumu_entry);
		else
			status = _enqueue_busy_su_entry(
					hal_info, bf_obj, sumu_entry);
		if (RTW_HAL_STATUS_SUCCESS != status) {
			PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, "warring :sumu_entry add busy queue fail !!!\n");
			break;
		}
	} while (0);

	if((status != RTW_HAL_STATUS_SUCCESS) && (bf_entry != NULL)) {
		PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, "Get BF Entry Fail !!!\n");
		hal_bf_release_target_bf_entry(hal_info, (void *)bf_entry);
	}
	FUNCOUT();
	return bf_entry;
}
/**
 * hal_bf_cfg_swbf_entry
 * 	Fill BF Entry SW Content
 * input:
 * @sta: rtw_phl_stainfo_t
 **/
enum rtw_hal_status
hal_bf_cfg_swbf_entry(struct rtw_phl_stainfo_t *sta, bool swap)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_SUCCESS;
	struct hal_bf_entry *bf_entry =
		(struct hal_bf_entry *)sta->hal_sta->bf_entry;
	PHL_TRACE(COMP_PHL_SOUND, _PHL_INFO_, "hal_bf_cfg_swbf_entry bf_entry->idx = 0x%x\n",
		bf_entry->bf_idx);

	bf_entry->macid = sta->macid;
	bf_entry->aid12 = sta->aid;
	bf_entry->band = sta->rlink->hw_band;
	bf_entry->csi_buf = sta->hal_sta->bf_csi_buf;
	if (swap) {
		bf_entry->en_swap = true;
		bf_entry->csi_buf_swap = sta->hal_sta->bf_csi_buf_swap;
	}

	return status;
}

/**
 * hal_bf_set_entry_hwcfg
 * 	Configure BF Entry HW Setting
 * input
 * @hal_info: (struct hal_info_t *)
 * @entry: struct hal_bf_entry
 **/
enum rtw_hal_status hal_bf_set_entry_hwcfg(
	struct hal_info_t *hal_info,
	void *entry)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_FAILURE;
	struct rtw_hal_com_t *hal_com = hal_info->hal_com;
	struct hal_bf_obj *bf_obj = (struct hal_bf_obj *)hal_com->bf_obj;
	struct hal_bf_entry *bf_entry = (struct hal_bf_entry *)entry;
	u8 bfee_idx = 0;
	u16 csi_buf = 0;

	/*TODO: 8852A : SU : 0 ~ N-1;  MU : N ~ M*/
	bfee_idx = bf_entry->bfee->idx +
		((bf_entry->bfee->type == HAL_BFEE_MU) ?
			bf_obj->max_su_bfee_nr : 0);
	PHL_TRACE(COMP_PHL_SOUND, _PHL_INFO_,
		  "hal_bf_set_entry_hwcfg set bf sts idx = 0x%x \n", bfee_idx);

	/*1. HALMAC Settings*/
	if ((bf_entry->bfee->type == HAL_BFEE_MU) && bf_entry->en_swap) {
		/*swap mode*/
		if(0 == (bf_entry->couter % 2))
			csi_buf = bf_entry->csi_buf&CSI_BUF_IDX_HW_MSK;
		else
			csi_buf = bf_entry->csi_buf_swap&CSI_BUF_IDX_HW_MSK;

		status = rtw_hal_mac_ax_set_bf_entry(
				hal_info->mac, bf_entry->band,
				(u8)(bf_entry->macid&0xFF), bfee_idx,
				bf_entry->bf_idx, csi_buf);

		bf_entry->couter++;
	} else {
		status = rtw_hal_mac_ax_set_bf_entry(
				hal_info->mac, bf_entry->band,
				(u8)(bf_entry->macid&0xFF), bfee_idx,
				bf_entry->bf_idx,
				bf_entry->csi_buf&CSI_BUF_IDX_HW_MSK);
	}
	/*2.TODO: HALBB Settings if needed*/

	return status;
}

/**
 * hal_bf_update_entry_snd_sts
 * 	Update BF Entry Sounding Status
 * input
 * @hal_info: (struct hal_info_t *)
 * @entry: struct hal_bf_entry
 **/
void hal_bf_update_entry_snd_sts(struct hal_info_t *hal_info, void *entry)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_FAILURE;
	struct rtw_hal_com_t *hal_com = hal_info->hal_com;
	struct hal_bf_obj *bf_obj = (struct hal_bf_obj *)hal_com->bf_obj;
	struct hal_bf_entry *bf_entry = (struct hal_bf_entry *)entry;
	u8 bfee_idx = 0;
	/*TODO: 8852A : SU : 0 ~ N-1;  MU : N ~ M*/
	if (NULL == bf_entry) {
		PHL_TRACE(COMP_PHL_SOUND, _PHL_INFO_, "[ERROR] hal_bf_update_entry_snd_sts BF_Entry = NULL\n");
		return;
	}
	if (NULL == bf_entry->bfee) {
		PHL_TRACE(COMP_PHL_SOUND, _PHL_INFO_,
			  "[ERROR] rtw_hal_bf_get_entry_snd_sts bf_entry->macid = 0x%x\n", bf_entry->macid);
		PHL_TRACE(COMP_PHL_SOUND, _PHL_INFO_,
			  "[ERROR] rtw_hal_bf_get_entry_snd_sts bf_entry->bfee = NULL\n");
		return;
	}
	bfee_idx = bf_entry->bfee->idx +
			((bf_entry->bfee->type == HAL_BFEE_MU) ?
			bf_obj->max_su_bfee_nr : 0);
	PHL_TRACE(COMP_PHL_SOUND, _PHL_INFO_, "hal_bf_update_entry_snd_sts idx = 0x%x \n", bfee_idx);

	status = rtw_hal_mac_ax_get_snd_sts(hal_info->mac,
							bf_entry->band, bfee_idx);

	bf_entry->bfee->snd_sts = (status == RTW_HAL_STATUS_SUCCESS) ? 1 : 0;
	PHL_TRACE(COMP_PHL_SOUND, _PHL_INFO_, "hal_bf_update_entry_snd_sts bf_entry->bfee->snd_sts = 0x%x \n",
		  bf_entry->bfee->snd_sts);

}

enum rtw_hal_status
hal_bf_hw_mac_deinit_bfee(struct hal_info_t *hal_info, u8 band)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_SUCCESS;
	struct rtw_hal_com_t *hal_com = hal_info->hal_com;
	struct hal_bf_obj *bf_obj = (struct hal_bf_obj *)hal_com->bf_obj;

	do {
		if (band >= 2) {
			status = RTW_HAL_STATUS_FAILURE;
			break;
		}
		if ((0 == (bf_obj->self_bf_cap[band]&BF_CAP_HE_BFEE)) &&
		    (0 == (bf_obj->self_bf_cap[band]&BF_CAP_VHT_BFEE))) {
			/* already disable */
			break;
		}
		status = rtw_hal_mac_ax_deinit_bfee(hal_info->hal_com, band);

		if (status != RTW_HAL_STATUS_SUCCESS)
			break;

		bf_obj->self_bf_cap[band] &= ~(BF_CAP_HE_BFEE|BF_CAP_VHT_BFEE);
	} while (0);

	return status;
}

/**
 * rtw_hal_bf_get_entry_snd_sts
 * 	Get BF Entry Sounding Status
 * input
 * @hal_info: (struct hal_info_t *)
 * @entry: struct hal_bf_entry
 * return
 * 	rtw_hal_status : RTW_HAL_STATUS_FAILURE or RTW_HAL_STATUS_SUCCESS
 **/
enum rtw_hal_status
rtw_hal_bf_get_entry_snd_sts(void *entry)
{
	enum rtw_hal_status hstatus = RTW_HAL_STATUS_FAILURE;
	struct hal_bf_entry *bf_entry = (struct hal_bf_entry *)entry;
	if (NULL == bf_entry) {
		PHL_TRACE(COMP_PHL_SOUND, _PHL_INFO_, "[ERROR] rtw_hal_bf_get_entry_snd_sts BF_Entry = NULL\n");
		return hstatus;
	}
	if (NULL == bf_entry->bfee) {
		PHL_TRACE(COMP_PHL_SOUND, _PHL_INFO_,
			  "[ERROR] rtw_hal_bf_get_entry_snd_sts bf_entry->macid = 0x%x\n", bf_entry->macid);
		PHL_TRACE(COMP_PHL_SOUND, _PHL_INFO_,
			  "[ERROR] rtw_hal_bf_get_entry_snd_sts bf_entry->bfee = NULL\n");
		return hstatus;
	}

	if (1 == bf_entry->bfee->snd_sts)
		hstatus = RTW_HAL_STATUS_SUCCESS;

	return hstatus;
}

/**
 * hal_bf_hw_mac_init_bfee
 * 	Initialize BFee HW Settings
 * input
 * @hal_info: struct hal_info_t
 * @band: Band 0 / Band 1
 **/
enum rtw_hal_status hal_bf_hw_mac_init_bfee(
	struct hal_info_t *hal_info,
	u8 band)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_SUCCESS;
	struct rtw_hal_com_t *hal_com = hal_info->hal_com;
	struct hal_bf_obj *bf_obj = (struct hal_bf_obj *)hal_com->bf_obj;

	do {
		if (band >= 2) {
			status = RTW_HAL_STATUS_FAILURE;
			break;
		}
		status = rtw_hal_mac_ax_init_bf_role(
				hal_info->hal_com, HAL_BF_ROLE_BFEE, band);

		if(status != RTW_HAL_STATUS_SUCCESS)
			break;

		/*TODO: Check Wireless Mode*/
		bf_obj->self_bf_cap[band] |= BF_CAP_HE_BFEE|BF_CAP_VHT_BFEE;

	} while (0);

	return status;
}
#ifdef RTW_WKARD_DYNAMIC_BFEE_CAP
enum rtw_hal_status
rtw_hal_bf_bfee_ctrl(void *hal, u8 band, bool ctrl)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct rtw_hal_com_t *hal_com = hal_info->hal_com;
	struct hal_bf_obj *bf_obj = (struct hal_bf_obj *)hal_com->bf_obj;
	enum rtw_hal_status status = RTW_HAL_STATUS_SUCCESS;
	bool cur_bfee_cap = false;

	if ((bf_obj->self_bf_cap[band] & BF_CAP_HE_BFEE) ||
	    (bf_obj->self_bf_cap[band] & BF_CAP_VHT_BFEE)) {
		    cur_bfee_cap = true;
	}

	/* BB Workaround */
	if ((ctrl != cur_bfee_cap) && (true == ctrl)) {
		/* From Tx to Rx (need Enable BFee) */
		rtw_hal_bb_dcr_en(hal_info, true);
	} else if ((ctrl != cur_bfee_cap) && (false == ctrl)) {
		rtw_hal_bb_dcr_en(hal_info, false);
	}

	if ((ctrl == true) &&
	    (false == rtw_hal_bb_csi_rsp(hal_info))) {
		ctrl = false;
	}

	if (ctrl != cur_bfee_cap) {
		status = ((true == ctrl) ?
			   hal_bf_hw_mac_init_bfee(hal_info, band) :
			   hal_bf_hw_mac_deinit_bfee(hal_info, band));
	}

	return status;
}
#endif

/**
 * hal_bf_set_bfee_csi_para
 * 	Set self bfee hw capability.
 * input:
 * @hal_info: struct hal_info_t *
 * @cr_cctl: set bfee capabiliy method
 * 		true = by register  (only support 2 sets, Port 0 / Port 1-4 )
 * 		false = by cmac control table
 * @sta: struct rtw_phl_stainfo_t * (self)
 */
enum rtw_hal_status hal_bf_set_bfee_csi_para(struct hal_info_t *hal_info,
	bool cr_cctl, struct rtw_phl_stainfo_t *sta)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_SUCCESS;
	u32 rrsc = BIT(HAL_BF_RRSC_6M) | BIT(HAL_BF_RRSC_24M);

	if (false == cr_cctl) {
		status = rtw_hal_mac_ax_bfee_para_reg(hal_info->mac, sta);
	} else {
		status = rtw_hal_mac_ax_bfee_para_cctl(hal_info->mac, sta);
	}
	/* mac/bb csi rate settings initialize */
	if (RTW_HAL_STATUS_SUCCESS == status) {
		/* Initialize CSI rate RA parameters */
		sta->hal_sta->ra_info.fixed_csi_rate_en = false;
		sta->hal_sta->ra_info.ra_csi_rate_en = true;
		sta->hal_sta->ra_info.band_num = sta->rlink->hw_band;
		if (sta->chandef.bw >= CHANNEL_WIDTH_80)
			sta->hal_sta->ra_info.csi_rate.bw = HAL_RATE_BW_80;
		else if (sta->chandef.bw == CHANNEL_WIDTH_40)
			sta->hal_sta->ra_info.csi_rate.bw = HAL_RATE_BW_40;
		else
			sta->hal_sta->ra_info.csi_rate.bw = HAL_RATE_BW_20;
		sta->hal_sta->ra_info.csi_rate.gi_ltf = RTW_GILTF_LGI_4XHE32;
		sta->hal_sta->ra_info.csi_rate.mcs_ss_idx = 5;
		if (sta->wmode & WLAN_MD_11N) {
			rrsc |= (BIT(HAL_BF_RRSC_HT_MSC0) |
				 BIT(HAL_BF_RRSC_HT_MSC3) |
				 BIT(HAL_BF_RRSC_HT_MSC5));
			sta->hal_sta->ra_info.csi_rate.mode = HAL_HT_MODE;
		}
		if (sta->wmode & WLAN_MD_11AC) {
			rrsc |= (BIT(HAL_BF_RRSC_VHT_MSC0) |
				 BIT(HAL_BF_RRSC_VHT_MSC3) |
				 BIT(HAL_BF_RRSC_VHT_MSC5));
			sta->hal_sta->ra_info.csi_rate.mode = HAL_VHT_MODE;
		}
		if (sta->wmode & WLAN_MD_11AX) {
			rrsc |= (BIT(HAL_BF_RRSC_HE_MSC0) |
				 BIT(HAL_BF_RRSC_HE_MSC3) |
				 BIT(HAL_BF_RRSC_HE_MSC5));
			sta->hal_sta->ra_info.csi_rate.mode = HAL_HE_MODE;
		}
		/* Initialize mac rrsc function */
		rtw_hal_mac_ax_bfee_set_csi_rrsc(hal_info->mac,
		                                 sta->rlink->hw_band,
		                                 rrsc);

		PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, " set bfee csi rrsc =  0x%x\n", rrsc);
	}

	return status;
}



/**
 * hal_bf_hw_mac_init_bfer
 * 	Initialize BFer HW Settings
 * @hal_info: struct hal_info_t
 * @band: Band 0 / Band 1
 **/
enum rtw_hal_status hal_bf_hw_mac_init_bfer(
	struct hal_info_t *hal_info,
	u8 band)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_SUCCESS;
	struct rtw_hal_com_t *hal_com = hal_info->hal_com;
	struct hal_bf_obj *bf_obj = (struct hal_bf_obj *)hal_com->bf_obj;

	do {
		if (band >= 2) {
			status = RTW_HAL_STATUS_FAILURE;
			break;
		}
		status = rtw_hal_mac_ax_init_bf_role(hal_info->hal_com,
						     HAL_BF_ROLE_BFER, band);
		if(status != RTW_HAL_STATUS_SUCCESS)
			break;
		/*TODO: Check Wireless Mode*/
		bf_obj->self_bf_cap[band] |= BF_CAP_HE_BFER|BF_CAP_VHT_BFER;

	} while (0);

	return status;
}

/**
 * rtw_hal_bf_chk_bf_type
 * 	Check the STA's BF Entry Type
 * @hal_info: struct hal_info_t
 * @sta: rtw_phl_stainfo_t *
 * @mu: check condition MU or SU?
 * return :
 * @ret: true : bf entry type is same to flag(mu)
 *		false : bf entry is not same to flag(mu) or bf entry is NULL
 **/
bool rtw_hal_bf_chk_bf_type(
	void *hal_info,
	struct rtw_phl_stainfo_t *sta,
	bool mu)
{
	struct hal_bf_entry *bf_entry =
		(struct hal_bf_entry *)sta->hal_sta->bf_entry;
	bool ret = false;
	do {
		if (NULL == bf_entry)
			break;

		if (bf_entry->bfee == NULL)
			break;

		if (true == mu) {
			if (HAL_BFEE_MU != bf_entry->bfee->type)
				break;
		} else {
			if (HAL_BFEE_SU != bf_entry->bfee->type)
				break;
		}

		ret = true;
	} while(0);

	return ret;
}

u8 rtw_hal_bf_get_sumu_idx(void *hal, void *entry)
{
	struct hal_bf_entry *bf_entry = (struct hal_bf_entry *)entry;
	u8 ret = 0xFF;
	if (bf_entry->bfee != NULL)
		ret = bf_entry->bfee->idx;

	return ret;
}


void
rtw_hal_bf_preset_mu_ba_info(
	void *hal,
	struct rtw_phl_stainfo_t *psta,
	void *hal_ba_info)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct rtw_hal_com_t *hal_com = hal_info->hal_com;
	struct hal_bf_obj *bf_obj = (struct hal_bf_obj *)hal_com->bf_obj;
	void *drv_priv = hal_to_drvpriv(hal_info);
	struct rtw_hal_muba_info *ba_info =
		(struct rtw_hal_muba_info *)hal_ba_info;

	if (1 == bf_obj->fixed_para.fix_ba_info.fix_ba) {
		/*Fixed BA info*/
		_os_mem_cpy(drv_priv, ba_info, &bf_obj->fixed_para.fix_ba_info, sizeof(*ba_info));
	} else {
		/* TODO:  Config BA by STA Capabiliy or default value*/
		ba_info->fix_ba = 0;
		ba_info->ru_psd = 0;
		ba_info->tf_rate = RTW_DATA_RATE_OFDM24;
		ba_info->rf_gain_fix = 0;
		ba_info->rf_gain_idx = 0;
		ba_info->tb_ppdu_bw = (psta->chandef.bw == CHANNEL_WIDTH_80) ? 2 : 0;
		ba_info->dcm = 0;
		ba_info->ss = 0;
		ba_info->mcs = 3;
		ba_info->gi_ltf = 2;
		ba_info->doppler = 0;
		ba_info->stbc = 0;
		ba_info->sta_coding = 0;
		ba_info->tb_t_pe_nom = 0;
		ba_info->pr20_bw_en = 0;
		ba_info->ma_type = 0;
	}
}

void
rtw_hal_bf_set_txmu_para(void *hal, u8 gid , u8 en,
	enum rtw_hal_protection_type rts_type,
	enum rtw_hal_ack_resp_type ack_type)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct rtw_hal_com_t *hal_com = hal_info->hal_com;
	struct hal_bf_obj *bf_obj = (struct hal_bf_obj *)hal_com->bf_obj;
	struct hal_bf_fixed_m_para *para = &bf_obj->fixed_para;

	para->fix_m = (1 == en) ? HAL_BF_FIX_M_MU : HAL_BF_FIX_M_DISABLE;
	para->gid = gid;
	para->fix_resp = en;
	para->fix_prot = en;
	if (en) {
		para->prot_type = rts_type;
		para->resp_type = ack_type;
	}
}

enum rtw_hal_status
rtw_hal_bf_set_fix_mode(void *hal, bool mu, bool he)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_SUCCESS;
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct rtw_hal_com_t *hal_com = hal_info->hal_com;
	struct hal_bf_obj *bf_obj = (struct hal_bf_obj *)hal_com->bf_obj;
	struct hal_bf_fixed_m_para *para = &bf_obj->fixed_para;


	if (mu) {
		status = rtw_hal_mac_ax_set_mu_fix_mode(
			hal_info->mac,
			para->gid,
			para->prot_type,
			para->resp_type,
			(para->fix_m == HAL_BF_FIX_M_MU) ? true : false,
			he,
			para->fix_resp,
			para->fix_prot);
	} else {
		/*TODO: Force SU*/
	}
	return status;
}

enum rtw_hal_status
hal_bf_set_mu_sta_fw(void *hal, struct rtw_phl_stainfo_t *sta)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_SUCCESS;
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct hal_bf_entry *bf_entry =
			(struct hal_bf_entry *)sta->hal_sta->bf_entry;
	struct hal_sumu_entry *mu_entry = NULL;

	do {
		if (NULL == bf_entry)
			break;

		if (NULL == bf_entry->bfee)
			break;

		mu_entry = bf_entry->bfee;

		if (HAL_BFEE_MU != mu_entry->type)
			break;

		status = rtw_hal_mac_ax_mu_sta_upd(
				hal_info->mac,
				(u8)(sta->macid&0xFF),
				mu_entry->idx,
				sta->hal_sta->prot_type,
				sta->hal_sta->resp_type,
				sta->hal_sta->mugrp_bmp);

	} while (0);

	return status;
}

void rtw_hal_beamform_set_vht_gid(void *hal, u8 band,
				  struct rtw_phl_gid_pos_tbl *tbl)
{
	PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, "--> %s()\n", __func__);
	if (RTW_HAL_STATUS_SUCCESS !=
		rtw_hal_mac_bfee_set_vht_gid(hal, band, tbl)) {
		PHL_INFO("%s : Error to set VHT GID Position to MAC !!! \n",
			 __func__);
	}
	PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, "<-- %s()\n", __func__);
}

enum rtw_hal_status rtw_hal_beamform_set_aid(void *hal, struct rtw_phl_stainfo_t *sta, u16 aid)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_SUCCESS;
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;

	PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, "--> %s() : set aid = %d\n", __func__, aid);

	/*TODO: halmac need provide api that only change AID */
	status = rtw_hal_mac_addr_cam_set_aid(hal_info, sta, aid);

	if (status != RTW_HAL_STATUS_SUCCESS) {
		PHL_ERR("rtw_hal_mac_addr_cam_change_entry failed\n");
	}

	PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, "<-- %s()\n", __func__);

	return status;
}