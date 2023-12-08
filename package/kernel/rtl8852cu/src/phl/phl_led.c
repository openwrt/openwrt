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
#define _PHL_LED_C_
#include "phl_headers.h"

#define PHL_LED_INTERVALS_ARR_LEN_MAX 4

struct phl_led_event_args_t {
	enum rtw_led_state state_condition;
	struct rtw_led_action_args_t *action_args_arr;
	u8 action_args_arr_len;
	u32 toggle_delay_unit;
	struct phl_led_event_args_t *next;
};

struct phl_led_timer_args_t {
	struct phl_info_t *phl_info;
	_os_timer timer;
	u32 delay_unit;
	bool timer_alive;
	bool is_avail;
	u32 led_manage_mask;
};

struct phl_led_info_t {
	enum rtw_led_ctrl_mode ctrl_mode;
	enum rtw_led_ctrl_mode reg_ctrl_mode;
	bool manual_mode_on;
	enum rtw_led_opt curr_opt;
	const struct rtw_led_toggle_args_t *toggle_args;
	struct phl_led_timer_args_t *toggle_timer_args;
	u32 toggle_interval_counter;
	u32 toggle_start_delay_counter;
	bool toggle_start_delay_over;
	u32 toggle_loop_counter;
	u8 toggle_curr_interval_idx;
};

struct phl_led_ctrl_t {
	struct phl_led_info_t led_info_arr[RTW_LED_ID_LENGTH];
	struct phl_led_event_args_t *event_args_list_arr[RTW_LED_EVENT_LENGTH];
	struct rtw_led_intervals_t intervals_arr[PHL_LED_INTERVALS_ARR_LEN_MAX];
	enum rtw_led_state state;
	struct phl_led_timer_args_t *toggle_timer_args[RTW_LED_TIMER_LENGTH];
};

static void _phl_led_timer_release(struct phl_led_timer_args_t *timer_args)
{
	void *drv_priv = phl_to_drvpriv(timer_args->phl_info);
	PHL_TRACE(COMP_PHL_LED, _PHL_INFO_, "%s: led_manage_mask == 0x%x\n",
		  __func__, timer_args->led_manage_mask);

	_os_cancel_timer(drv_priv, &(timer_args->timer));
	_os_release_timer(drv_priv, &(timer_args->timer));
	_os_mem_free(drv_priv, timer_args, sizeof(struct phl_led_timer_args_t));
}

static void _phl_led_remove_from_timer(struct phl_led_info_t *led_info,
				       enum rtw_led_id led_id)
{
	u32 *mask = NULL;

	if (led_info->toggle_timer_args != NULL) {
		mask = &(led_info->toggle_timer_args->led_manage_mask);
		*mask &= ~(BIT(led_id));

		if (*mask == 0)
			led_info->toggle_timer_args->timer_alive = false;

		led_info->toggle_timer_args = NULL;
		led_info->toggle_args = NULL;
	}
}

static void _phl_led_timer_cb_done(void* priv, struct phl_msg* msg)
{
	struct phl_led_timer_args_t *timer_args =
		 (struct phl_led_timer_args_t *)(msg->inbuf);

	if (!timer_args->timer_alive)
		timer_args->is_avail = true;
}

static void _phl_led_timer_cb(void *args)
{
	struct phl_led_timer_args_t *timer_args =
			(struct phl_led_timer_args_t *) args;
	enum rtw_phl_status phl_status = RTW_PHL_STATUS_SUCCESS;
	struct phl_info_t *phl_info = timer_args->phl_info;

	struct phl_msg msg = {0};
	struct phl_msg_attribute attr = {0};

	SET_MSG_MDL_ID_FIELD(msg.msg_id, PHL_MDL_LED);
	SET_MSG_EVT_ID_FIELD(msg.msg_id, MSG_EVT_LED_TICK);
	msg.band_idx = HW_BAND_0;
	msg.inbuf = (u8 *)(timer_args);
	msg.inlen = sizeof(struct phl_led_timer_args_t);

	attr.completion.completion = _phl_led_timer_cb_done;
	attr.completion.priv = phl_info;

	phl_status = phl_disp_eng_send_msg(timer_args->phl_info,
						&msg, &attr, NULL);
	if(phl_status != RTW_PHL_STATUS_SUCCESS){
		PHL_ERR("%s: phl_disp_eng_send_msg failed!\n", __func__);
		timer_args->timer_alive = false;
		_phl_led_timer_cb_done(phl_info, &msg);
	}
}

static enum rtw_phl_status _phl_led_ctrl_write_opt(void *hal,
						   enum rtw_led_id led_id,
						   enum rtw_led_opt *curr_opt,
						   enum rtw_led_opt opt)
{
	if (opt >= RTW_LED_OPT_UNKNOWN) {
		PHL_TRACE(COMP_PHL_LED, _PHL_INFO_, "%s: unknown opt (%d)\n",
			  __func__, opt);
		return RTW_PHL_STATUS_FAILURE;
	}

	if (RTW_HAL_STATUS_SUCCESS != rtw_hal_led_control(hal, led_id, opt))
		return RTW_PHL_STATUS_FAILURE;

	*curr_opt = opt;
	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status
_phl_led_ctrl_start_delay_hdlr(void *hal, struct phl_led_info_t *led_info,
			       enum rtw_led_id led_id)
{
	if (led_info->toggle_start_delay_counter >=
	    led_info->toggle_args->start_delay) {

		led_info->toggle_start_delay_over = true;

		PHL_TRACE(COMP_PHL_LED, _PHL_INFO_, "%s: start delay is over\n",
			  __func__);

		if (RTW_PHL_STATUS_SUCCESS !=
		    _phl_led_ctrl_write_opt(hal, led_id, &(led_info->curr_opt),
					    led_info->toggle_args->start_opt))
			return RTW_PHL_STATUS_FAILURE;

		return RTW_PHL_STATUS_SUCCESS;
	}

	(led_info->toggle_start_delay_counter)++;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status
_phl_led_ctrl_interval_hdlr(void *hal, struct phl_led_info_t *led_info,
			    enum rtw_led_id led_id,
			    struct rtw_led_intervals_t *intervals)
{
	u32 interval = 0;
	enum rtw_led_opt opt = RTW_LED_OPT_UNKNOWN;

	if (intervals == NULL) {
		PHL_TRACE(COMP_PHL_LED, _PHL_INFO_, "%s: intervals == NULL\n",
			  __func__);
		return RTW_PHL_STATUS_FAILURE;
	}

	if (intervals->interval_arr == NULL) {
		PHL_TRACE(COMP_PHL_LED, _PHL_INFO_,
			  "%s: interval_arr == NULL\n", __func__);
		return RTW_PHL_STATUS_FAILURE;
	}

	if (led_info->toggle_curr_interval_idx >= intervals->len) {
		PHL_TRACE(
		    COMP_PHL_LED, _PHL_INFO_,
		    "%s: curr_interval_idx ( %d ) >= intervals' len ( %d )\n",
		    __func__, led_info->toggle_curr_interval_idx,
		    intervals->len);
		return RTW_PHL_STATUS_FAILURE;
	}

	interval = intervals->interval_arr[led_info->toggle_curr_interval_idx];

	PHL_TRACE(COMP_PHL_LED, _PHL_INFO_,
		  "%s: curr_interval_idx == %d, interval == %d, "
		  "interval_counter == %d\n",
		  __func__, led_info->toggle_curr_interval_idx, interval,
		  led_info->toggle_interval_counter);

	if (interval > ++(led_info->toggle_interval_counter))
		/* it is not time to toggle */
		return RTW_PHL_STATUS_SUCCESS;

	led_info->toggle_interval_counter = 0;

	/* set curr_interval_idx to next */
	if (++(led_info->toggle_curr_interval_idx) >= intervals->len) {
		led_info->toggle_curr_interval_idx = 0;
		if (led_info->toggle_args->loop > 0)
			(led_info->toggle_loop_counter)++;
	}

	PHL_TRACE(COMP_PHL_LED, _PHL_INFO_, "%s: toggle led_id: %d\n", __func__,
		  led_id);

	if (led_info->curr_opt == RTW_LED_OPT_LOW)
		opt = RTW_LED_OPT_HIGH;

	else if (led_info->curr_opt == RTW_LED_OPT_HIGH)
		opt = RTW_LED_OPT_LOW;

	else {
		PHL_TRACE(COMP_PHL_LED, _PHL_INFO_,
			  "%s: incorrect curr_opt ( %d ). led_id: %d\n",
			  __func__, led_info->curr_opt, led_id);
		return RTW_PHL_STATUS_FAILURE;
	}

	if (RTW_PHL_STATUS_SUCCESS !=
	    _phl_led_ctrl_write_opt(hal, led_id, &(led_info->curr_opt), opt))
		return RTW_PHL_STATUS_FAILURE;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status
_phl_led_ctrl_toggle_hdlr(struct phl_led_timer_args_t *timer_args)
{
	enum rtw_phl_status status = RTW_PHL_STATUS_SUCCESS;
	void *drv_priv = phl_to_drvpriv(timer_args->phl_info);

	enum rtw_led_id led_id = 0;
	struct phl_led_ctrl_t *led_ctrl =
	    (struct phl_led_ctrl_t *)(timer_args->phl_info->led_ctrl);
	struct phl_led_info_t *led_info = NULL;

	u8 intervals_idx = 0;

	for (led_id = 0; led_id < RTW_LED_ID_LENGTH; led_id++) {
		if ((timer_args->led_manage_mask & BIT(led_id)) == 0)
			continue;

		led_info = &(led_ctrl->led_info_arr[led_id]);

		/* start_delay handling */
		if (!led_info->toggle_start_delay_over) {
			if (RTW_PHL_STATUS_SUCCESS !=
			    _phl_led_ctrl_start_delay_hdlr(
				timer_args->phl_info->hal, led_info, led_id)) {

				status = RTW_PHL_STATUS_FAILURE;
			}
			continue;
		}

		/* start_delay is over, handle intervals */
		intervals_idx = led_info->toggle_args->intervals_idx;

		if (RTW_PHL_STATUS_SUCCESS !=
		    _phl_led_ctrl_interval_hdlr(
			timer_args->phl_info->hal, led_info, led_id,
			&(led_ctrl->intervals_arr[intervals_idx]))) {

			PHL_TRACE(COMP_PHL_LED, _PHL_INFO_,
				  "%s: intervals handling failed. led_id: %d\n",
				  __func__, led_id);
			status = RTW_PHL_STATUS_FAILURE;
		}

		if (led_info->toggle_args->loop > 0 &&
		    led_info->toggle_args->loop ==
			led_info->toggle_loop_counter) {

			_phl_led_remove_from_timer(led_info, led_id);
		}
	}

	if (timer_args->timer_alive)
		_os_set_timer(drv_priv, &(timer_args->timer),
			      timer_args->delay_unit);

	return status;
}

static enum rtw_phl_status
_phl_led_ctrl_action_hdlr(struct phl_info_t *phl_info, enum rtw_led_id led_id,
			  enum rtw_led_action action,
			  struct rtw_led_toggle_args_t *toggle_args,
			  struct phl_led_timer_args_t **timer_args_ptr)
{
	enum rtw_phl_status status = RTW_PHL_STATUS_SUCCESS;

	struct phl_led_ctrl_t *led_ctrl =
	    (struct phl_led_ctrl_t *)(phl_info->led_ctrl);
	struct phl_led_info_t *led_info = &(led_ctrl->led_info_arr[led_id]);
	enum rtw_led_ctrl_mode target_ctrl_mode;
	u8 i = 0;

	PHL_TRACE(COMP_PHL_LED, _PHL_INFO_,
		  "%s: led_id == %d, action == 0X%X\n", __func__, led_id,
		  action);

	if (led_info->reg_ctrl_mode != RTW_LED_CTRL_SW_PP_MODE &&
	    led_info->reg_ctrl_mode != RTW_LED_CTRL_SW_OD_MODE)
		return status;

	/* Set ctrl mode*/
	switch (action) {
	case RTW_LED_ACTION_LOW:
	case RTW_LED_ACTION_HIGH:
	case RTW_LED_ACTION_TOGGLE:
		target_ctrl_mode = led_info->reg_ctrl_mode;
		break;
	case RTW_LED_ACTION_HW_TRX:
		target_ctrl_mode = RTW_LED_CTRL_HW_TRX_MODE;
		break;
	default:
		target_ctrl_mode = led_info->ctrl_mode;
		break;
	}
	if(led_info->ctrl_mode != target_ctrl_mode){
		if (rtw_hal_led_set_ctrl_mode(phl_info->hal, led_id,
						target_ctrl_mode)){
			status = RTW_PHL_STATUS_FAILURE;
			return status;
		}
		led_info->ctrl_mode = target_ctrl_mode;
	}

	/* Sw action */
	switch (action) {
	case RTW_LED_ACTION_LOW:
		_phl_led_remove_from_timer(led_info, led_id);

		if (RTW_PHL_STATUS_SUCCESS !=
		    _phl_led_ctrl_write_opt(phl_info->hal, led_id,
					    &(led_info->curr_opt),
					    RTW_LED_OPT_LOW))
			status = RTW_PHL_STATUS_FAILURE;

		break;

	case RTW_LED_ACTION_HIGH:
		_phl_led_remove_from_timer(led_info, led_id);

		if (RTW_PHL_STATUS_SUCCESS !=
		    _phl_led_ctrl_write_opt(phl_info->hal, led_id,
					    &(led_info->curr_opt),
					    RTW_LED_OPT_HIGH))
			status = RTW_PHL_STATUS_FAILURE;

		break;

	case RTW_LED_ACTION_HW_TRX:
		_phl_led_remove_from_timer(led_info, led_id);
		break;

	case RTW_LED_ACTION_TOGGLE:
		_phl_led_remove_from_timer(led_info, led_id);

		led_info->toggle_args = toggle_args;

		led_info->toggle_interval_counter = 0;
		led_info->toggle_start_delay_counter = toggle_args->start_delay;
		led_info->toggle_start_delay_over = false;
		led_info->toggle_loop_counter = 0;
		led_info->toggle_curr_interval_idx = 0;

		if (*timer_args_ptr == NULL) {
			for (i = 0; i < RTW_LED_TIMER_LENGTH; i++) {
				if (led_ctrl->toggle_timer_args[i] == NULL)
					continue;
				if (led_ctrl->toggle_timer_args[i]->is_avail ==
									true) {
					*timer_args_ptr =
						led_ctrl->toggle_timer_args[i];
					(*timer_args_ptr)->is_avail = false;
					break;
				}
			}

			if (*timer_args_ptr == NULL) {
				PHL_ERR("%s: get available timer failed!\n", __func__);
				break;
			}

			(*timer_args_ptr)->phl_info = phl_info;
			(*timer_args_ptr)->led_manage_mask = 0;
			(*timer_args_ptr)->timer_alive = true;
			(*timer_args_ptr)->delay_unit = 0;
		}

		(*timer_args_ptr)->led_manage_mask |= BIT(led_id);
		led_info->toggle_timer_args = *timer_args_ptr;

		break;

	default:
		status = RTW_PHL_STATUS_FAILURE;
		break;
	}

	return status;
}

static enum rtw_phl_status _phl_led_ctrl_event_hdlr(struct phl_info_t *phl_info,
						    enum rtw_led_event event)
{
	enum rtw_phl_status status = RTW_PHL_STATUS_SUCCESS;
	struct phl_led_event_args_t *event_args = NULL;
	struct phl_led_ctrl_t *led_ctrl =
	    (struct phl_led_ctrl_t *)(phl_info->led_ctrl);

	u8 args_idx;
	enum rtw_led_id led_id;
	struct phl_led_info_t *led_info = NULL;
	struct rtw_led_action_args_t *action_args = NULL;
	struct phl_led_timer_args_t *timer_args = NULL;

	if(event >= RTW_LED_EVENT_LENGTH){
		PHL_TRACE(COMP_PHL_LED, _PHL_INFO_, "%s: invalid event(0X%X) >= RTW_LED_EVENT_LENGTH(0X%X).\n",
			  __func__, event, RTW_LED_EVENT_LENGTH);
		return RTW_PHL_STATUS_FAILURE;
	}

	PHL_TRACE(COMP_PHL_LED, _PHL_INFO_, "%s: event == 0X%X\n", __func__,
		  event);

	/* set state */
	switch (event) {
	case RTW_LED_EVENT_SW_RF_ON:
		PHL_TRACE(COMP_PHL_LED, _PHL_INFO_, "%s: set state sw rf on\n",
			  __func__);
		led_ctrl->state |= RTW_LED_STATE_SW_RF_ON;
		break;

	case RTW_LED_EVENT_SW_RF_OFF:
		PHL_TRACE(COMP_PHL_LED, _PHL_INFO_, "%s: set state sw rf off\n",
			  __func__);
		led_ctrl->state &= ~RTW_LED_STATE_SW_RF_ON;
		break;

	default:
		break;
	}

	/* handle event */
	event_args =
	    led_ctrl->event_args_list_arr[event]; /* event_args = list head */
	for (; event_args != NULL; event_args = event_args->next) {
		if (!(event_args->state_condition &
		      (led_ctrl->state | RTW_LED_STATE_IGNORE)))
			continue;

		timer_args = NULL;

		for (args_idx = 0; args_idx < event_args->action_args_arr_len;
		     args_idx++) {

			action_args = &(event_args->action_args_arr[args_idx]);
			led_id = action_args->led_id;
			led_info = &(led_ctrl->led_info_arr[led_id]);

			if (led_info->manual_mode_on)
				continue;

			if (RTW_PHL_STATUS_SUCCESS !=
			    _phl_led_ctrl_action_hdlr(
				phl_info, led_id, action_args->led_action,
				&(action_args->toggle_args), &timer_args)) {

				status = RTW_PHL_STATUS_FAILURE;
			}
		}

		if (timer_args == NULL)
			continue;

		timer_args->delay_unit = event_args->toggle_delay_unit;

		if (RTW_PHL_STATUS_SUCCESS !=
		    _phl_led_ctrl_toggle_hdlr(timer_args))
			status = RTW_PHL_STATUS_FAILURE;
	}

	return status;
}

static enum rtw_phl_status
_phl_led_ctrl_manual_hdlr(struct phl_info_t *phl_info,
			  struct rtw_led_action_args_t *action_args)
{
	struct phl_led_ctrl_t *led_ctrl =
	    (struct phl_led_ctrl_t *)(phl_info->led_ctrl);
	struct phl_led_info_t *led_info =
	    &(led_ctrl->led_info_arr[action_args->led_id]);

	PHL_TRACE(COMP_PHL_LED, _PHL_INFO_,
		  "%s: led_id == %d, action == 0X%X, manual_mode_on == %d\n",
		  __func__, action_args->led_id, action_args->led_action,
		  led_info->manual_mode_on);

	if (action_args->led_action != RTW_LED_ACTION_LOW &&
	    action_args->led_action != RTW_LED_ACTION_HIGH) {

		PHL_ERR("%s: manual mode only supports to send the action of "
			"RTW_LED_ACTION_LOW or RTW_LED_ACTION_HIGH, "
			"led_id == %d, action == 0X%X\n",
			__func__, action_args->led_id, action_args->led_action);

		return RTW_PHL_STATUS_FAILURE;
	}

	if (!led_info->manual_mode_on) {

		PHL_ERR("%s: the manual ctrl event should not be sent if "
			"manual mode is not on, led_id == %d\n",
			__func__, action_args->led_id);

		return RTW_PHL_STATUS_FAILURE;
	}

	if (RTW_PHL_STATUS_SUCCESS !=
	    _phl_led_ctrl_action_hdlr(phl_info, action_args->led_id,
				      action_args->led_action, NULL, NULL)) {

		return RTW_PHL_STATUS_FAILURE;
	}

	return RTW_PHL_STATUS_SUCCESS;
}

static enum phl_mdl_ret_code _phl_led_module_init(void *phl, void *dispr,
						  void **priv)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	void *drv_priv = phl_to_drvpriv(phl_info);
	struct phl_led_ctrl_t *led_ctrl = NULL;

	enum rtw_led_id led_id = 0;
	enum rtw_led_event event_id = 0;

	struct phl_led_info_t *led_info = NULL;

	struct rtw_led_intervals_t *intervals = NULL;
	u8 intervals_idx = 0, i = 0;

	struct phl_led_timer_args_t *timer_args = NULL;

	PHL_TRACE(COMP_PHL_LED, _PHL_INFO_, "===> _phl_led_module_init()\n");

	if (NULL == (led_ctrl = _os_mem_alloc(drv_priv,
					      sizeof(struct phl_led_ctrl_t)))) {

		PHL_ERR("%s: alloc buffer failed!\n", __func__);
		phl_info->led_ctrl = NULL;
		return MDL_RET_FAIL;
	}

	*priv = phl;
	phl_info->led_ctrl = led_ctrl;

	/* set default value in led_ctrl */
	led_ctrl->state = 0;

	for (led_id = 0; led_id < RTW_LED_ID_LENGTH; led_id++) {
		led_info = &(led_ctrl->led_info_arr[led_id]);

		led_info->ctrl_mode = RTW_LED_CTRL_NOT_SUPPORT;
		led_info->reg_ctrl_mode = RTW_LED_CTRL_NOT_SUPPORT;
		led_info->manual_mode_on = false;
		led_info->curr_opt = RTW_LED_OPT_UNKNOWN;
		led_info->toggle_interval_counter = 0;
		led_info->toggle_start_delay_counter = 0;
		led_info->toggle_start_delay_over = false;
		led_info->toggle_loop_counter = 0;
		led_info->toggle_curr_interval_idx = 0;
		led_info->toggle_timer_args = NULL;
		led_info->toggle_args = NULL;
	}

	for (event_id = 0; event_id < RTW_LED_EVENT_LENGTH; event_id++) {
		led_ctrl->event_args_list_arr[event_id] = NULL;
	}

	for (intervals_idx = 0; intervals_idx < PHL_LED_INTERVALS_ARR_LEN_MAX;
	     intervals_idx++) {

		intervals = &(led_ctrl->intervals_arr[intervals_idx]);

		intervals->interval_arr = NULL;
		intervals->len = 0;
	}

	for (i = 0; i < RTW_LED_TIMER_LENGTH; i++) {
		if (NULL == (timer_args = _os_mem_alloc( drv_priv,
				sizeof(struct phl_led_timer_args_t)))) {

			PHL_ERR("%s: alloc #%d timer buffer failed!\n", __func__, i);
			led_ctrl->toggle_timer_args[i] = NULL;
			continue;
		}

		timer_args->phl_info = phl_info;

		_os_init_timer(drv_priv, &(timer_args->timer),
				_phl_led_timer_cb, timer_args, "phl_led_timer");

		timer_args->delay_unit = 0;
		timer_args->timer_alive = false;
		timer_args->is_avail = true;
		timer_args->led_manage_mask = 0;

		led_ctrl->toggle_timer_args[i] = timer_args;
	}

	return MDL_RET_SUCCESS;
}

static void _phl_led_module_deinit(void *dispr, void *priv)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)priv;
	void *drv_priv = phl_to_drvpriv(phl_info);
	struct phl_led_ctrl_t *led_ctrl =
	    (struct phl_led_ctrl_t *)(phl_info->led_ctrl);

	enum rtw_led_event event = 0;
	struct phl_led_event_args_t *event_args = NULL;
	struct phl_led_event_args_t *event_args_next = NULL;

	struct rtw_led_intervals_t *intervals = NULL;
	u8 intervals_idx = 0, i = 0;

	enum rtw_led_id led_id = 0;
	struct phl_led_info_t *led_info = NULL;

	PHL_TRACE(COMP_PHL_LED, _PHL_INFO_, "===> _phl_led_module_deinit()\n");

	if (led_ctrl == NULL)
		return;

	/* free event_args_list_arr */
	for (event = 0; event < RTW_LED_EVENT_LENGTH; event++) {
		event_args = led_ctrl->event_args_list_arr[event];

		if (event_args == NULL)
			continue;

		while (event_args != NULL) {
			event_args_next = event_args->next;

			if (event_args->action_args_arr != NULL)
				_os_mem_free(
				    drv_priv, event_args->action_args_arr,
				    event_args->action_args_arr_len *
					sizeof(struct rtw_led_action_args_t));

			_os_mem_free(drv_priv, event_args,
				     sizeof(struct phl_led_event_args_t));

			event_args = event_args_next;
		}
	}

	/* free intervals_arr */
	for (intervals_idx = 0; intervals_idx < PHL_LED_INTERVALS_ARR_LEN_MAX;
	     intervals_idx++) {

		intervals = &(led_ctrl->intervals_arr[intervals_idx]);

		if (intervals->interval_arr == NULL)
			continue;

		_os_mem_free(drv_priv, intervals->interval_arr,
			     intervals->len * sizeof(u32));
	}

	/* free all timers */
	for (led_id = 0; led_id < RTW_LED_ID_LENGTH; led_id++) {
		led_info = &(led_ctrl->led_info_arr[led_id]);
		if (led_info->toggle_timer_args == NULL)
			continue;

		_phl_led_remove_from_timer(led_info, led_id);
	}
	for (i = 0; i < RTW_LED_TIMER_LENGTH; i++) {
		if (led_ctrl->toggle_timer_args[i] == NULL)
			continue;

		_phl_led_timer_release(led_ctrl->toggle_timer_args[i]);
	}

	_os_mem_free(drv_priv, led_ctrl, sizeof(struct phl_led_ctrl_t));

	phl_info->led_ctrl = NULL;
}

static enum phl_mdl_ret_code _phl_led_module_start(void *dispr, void *priv)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)priv;
	struct phl_led_ctrl_t *led_ctrl =
	    (struct phl_led_ctrl_t *)(phl_info->led_ctrl);

	enum phl_mdl_ret_code ret = MDL_RET_SUCCESS;
	enum rtw_led_id led_id = 0;

	PHL_TRACE(COMP_PHL_LED, _PHL_INFO_, "===> _phl_led_module_start()\n");

	if (led_ctrl == NULL) {
		PHL_TRACE(COMP_PHL_LED, _PHL_INFO_, "%s: led_ctrl == NULL\n",
			  __func__);
		return MDL_RET_FAIL;
	}

	for (led_id = 0; led_id < RTW_LED_ID_LENGTH; led_id++) {
		if (RTW_HAL_STATUS_SUCCESS !=
		    rtw_hal_led_set_ctrl_mode(
			phl_info->hal, led_id,
			led_ctrl->led_info_arr[led_id].reg_ctrl_mode))
			ret = MDL_RET_FAIL;

		led_ctrl->led_info_arr[led_id].ctrl_mode =
		    led_ctrl->led_info_arr[led_id].reg_ctrl_mode;
	}

	if (RTW_PHL_STATUS_SUCCESS !=
	    _phl_led_ctrl_event_hdlr(phl_info, RTW_LED_EVENT_PHL_START))
		ret = MDL_RET_FAIL;

	if (RTW_PHL_STATUS_SUCCESS !=
	    _phl_led_ctrl_event_hdlr(phl_info, RTW_LED_EVENT_SW_RF_ON))
		ret = MDL_RET_FAIL;

	return ret;
}

static enum phl_mdl_ret_code _phl_led_module_stop(void *dispr, void *priv)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)priv;
	void *drv_priv = phl_to_drvpriv(phl_info);
	struct phl_led_ctrl_t *led_ctrl =
	    (struct phl_led_ctrl_t *)(phl_info->led_ctrl);

	enum phl_mdl_ret_code ret = MDL_RET_SUCCESS;
	u8 i = 0;

	PHL_TRACE(COMP_PHL_LED, _PHL_INFO_, "===> _phl_led_module_stop()\n");

	if (led_ctrl == NULL) {
		PHL_TRACE(COMP_PHL_LED, _PHL_INFO_, "%s: led_ctrl == NULL\n",
			  __func__);
		return MDL_RET_FAIL;
	}

	if (RTW_PHL_STATUS_SUCCESS !=
	    _phl_led_ctrl_event_hdlr(phl_info, RTW_LED_EVENT_SW_RF_OFF))
		ret = MDL_RET_FAIL;

	if (RTW_PHL_STATUS_SUCCESS !=
	    _phl_led_ctrl_event_hdlr(phl_info, RTW_LED_EVENT_PHL_STOP))
		ret = MDL_RET_FAIL;

	for (i = 0; i < RTW_LED_TIMER_LENGTH; i++) {
		_os_cancel_timer(drv_priv, &(led_ctrl->toggle_timer_args[i]->timer));
		led_ctrl->toggle_timer_args[i]->timer_alive = false;
		led_ctrl->toggle_timer_args[i]->is_avail = true;
	}

	return ret;
}

static enum phl_mdl_ret_code _phl_led_module_msg_hdlr(void *dispr, void *priv,
						      struct phl_msg *msg)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)priv;
	struct phl_led_ctrl_t *led_ctrl =
	    (struct phl_led_ctrl_t *)(phl_info->led_ctrl);
	enum phl_msg_evt_id msg_evt_id = MSG_EVT_ID_FIELD(msg->msg_id);
	struct phl_led_timer_args_t *timer_args = NULL;

	PHL_TRACE(COMP_PHL_LED, _PHL_INFO_,
		  "===> _phl_led_module_msg_hdlr()\n");

	if (IS_MSG_IN_PRE_PHASE(msg->msg_id))
		return MDL_RET_SUCCESS;

	if (MSG_MDL_ID_FIELD(msg->msg_id) != PHL_MDL_LED)
		return MDL_RET_IGNORE;

	if(IS_MSG_CANNOT_IO(msg->msg_id))
		return MDL_RET_FAIL;

	if (led_ctrl == NULL) {
		PHL_TRACE(COMP_PHL_LED, _PHL_INFO_, "%s: led_ctrl == NULL\n",
			  __func__);
		return MDL_RET_FAIL;
	}

	if (msg_evt_id == MSG_EVT_LED_TICK) {
		PHL_TRACE(COMP_PHL_LED, _PHL_INFO_, "%s: MSG_EVT_LED_TICK\n",
			  __func__);
		timer_args = (struct phl_led_timer_args_t *)(msg->inbuf);

		if (!timer_args->timer_alive) {
			return MDL_RET_SUCCESS;
		}

		if (RTW_PHL_STATUS_SUCCESS !=
		    _phl_led_ctrl_toggle_hdlr(timer_args))
			return MDL_RET_FAIL;

		return MDL_RET_SUCCESS;
	}

	if (msg_evt_id == MSG_EVT_LED_MANUAL_CTRL) {

		if (RTW_PHL_STATUS_SUCCESS !=
		    _phl_led_ctrl_manual_hdlr(
			phl_info, (struct rtw_led_action_args_t *)(msg->inbuf)))
			return MDL_RET_FAIL;

		return MDL_RET_SUCCESS;
	}

	if (msg_evt_id >= MSG_EVT_LED_EVT_START &&
	    msg_evt_id <= MSG_EVT_LED_EVT_END) {

		if (RTW_PHL_STATUS_SUCCESS !=
		    _phl_led_ctrl_event_hdlr(
			phl_info, msg_evt_id - MSG_EVT_LED_EVT_START))
			return MDL_RET_FAIL;
	}

	return MDL_RET_SUCCESS;
}

static enum phl_mdl_ret_code
_phl_led_module_set_info(void *dispr, void *priv,
			 struct phl_module_op_info *info)
{
	return MDL_RET_SUCCESS;
}

static enum phl_mdl_ret_code
_phl_led_module_query_info(void *dispr, void *priv,
			   struct phl_module_op_info *info)
{
	return MDL_RET_SUCCESS;
}

enum rtw_phl_status phl_register_led_module(struct phl_info_t *phl_info)
{
#ifdef CONFIG_CMD_DISP
	enum rtw_phl_status phl_status = RTW_PHL_STATUS_FAILURE;

	struct phl_bk_module_ops bk_ops;
	bk_ops.init = _phl_led_module_init;
	bk_ops.deinit = _phl_led_module_deinit;
	bk_ops.start = _phl_led_module_start;
	bk_ops.stop = _phl_led_module_stop;
	bk_ops.msg_hdlr = _phl_led_module_msg_hdlr;
	bk_ops.set_info = _phl_led_module_set_info;
	bk_ops.query_info = _phl_led_module_query_info;

	phl_status = phl_disp_eng_register_module(phl_info, HW_BAND_0,
						  PHL_MDL_LED, &bk_ops);

	if (phl_status != RTW_PHL_STATUS_SUCCESS) {
		PHL_ERR("%s register LED module in cmd disp failed\n",
			__func__);
		phl_status = RTW_PHL_STATUS_FAILURE;
	}

	return phl_status;
#else
	return RTW_PHL_STATUS_FAILURE;
#endif
}

void rtw_phl_led_set_ctrl_mode(void *phl, enum rtw_led_id led_id,
			       enum rtw_led_ctrl_mode ctrl_mode)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct phl_led_ctrl_t *led_ctrl =
	    (struct phl_led_ctrl_t *)phl_info->led_ctrl;

	PHL_TRACE(COMP_PHL_LED, _PHL_INFO_,
		  "===> %s()\n", __func__);

	if (led_ctrl == NULL) {
		PHL_TRACE(COMP_PHL_LED, _PHL_INFO_, "%s: led_ctrl == NULL\n",
			  __func__);
		return;
	}

	PHL_TRACE(COMP_PHL_LED, _PHL_INFO_,
		  "%s: led_id == %d, ctrl_mode == %d\n", __func__, led_id,
		  ctrl_mode);

	led_ctrl->led_info_arr[led_id].reg_ctrl_mode = ctrl_mode;
}

void rtw_phl_led_set_toggle_intervals(void *phl, u8 intervals_idx,
				      u32 *interval_arr, u8 intervals_len)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct phl_led_ctrl_t *led_ctrl =
	    (struct phl_led_ctrl_t *)phl_info->led_ctrl;
	void *drv_priv = phl_to_drvpriv(phl_info);

	struct rtw_led_intervals_t *intervals = NULL;

	PHL_TRACE(COMP_PHL_LED, _PHL_INFO_,
		  "===> rtw_phl_led_set_toggle_intervals()\n");

	if (led_ctrl == NULL) {
		PHL_TRACE(COMP_PHL_LED, _PHL_INFO_, "%s: led_ctrl == NULL\n",
			  __func__);
		return;
	}

	if (intervals_idx >= PHL_LED_INTERVALS_ARR_LEN_MAX) {
		PHL_TRACE(
		    COMP_PHL_LED, _PHL_INFO_,
		    "%s: intervals_idx >= PHL_LED_INTERVALS_ARR_LEN_MAX\n",
		    __func__);
		return;
	}

	intervals = &(led_ctrl->intervals_arr[intervals_idx]);

	/* check if the target intervals_arr has already been set */
	if (intervals->interval_arr != NULL) {
		PHL_TRACE(COMP_PHL_LED, _PHL_INFO_,
			  "%s: intervals_arr[%d] has already been set. "
			  "The new one is going to replace the old one!\n",
			  __func__, intervals_idx);

		_os_mem_free(drv_priv, intervals->interval_arr,
			     intervals->len * sizeof(u32));

		intervals->interval_arr = NULL;
		intervals->len = 0;
	}

	if (NULL == (intervals->interval_arr = _os_mem_alloc(
			 drv_priv, intervals_len * sizeof(u32)))) {

		PHL_ERR("%s: alloc buffer failed!\n", __func__);
		return;
	}

	_os_mem_cpy(drv_priv, intervals->interval_arr, interval_arr,
		    intervals_len * sizeof(u32));
	intervals->len = intervals_len;

	return;
}

void rtw_phl_led_set_action(void *phl, enum rtw_led_event event,
			    enum rtw_led_state state_condition,
			    struct rtw_led_action_args_t *action_args_arr,
			    u8 action_args_arr_len, u32 toggle_delay_unit)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct phl_led_ctrl_t *led_ctrl =
	    (struct phl_led_ctrl_t *)phl_info->led_ctrl;
	void *drv_priv = phl_to_drvpriv(phl_info);

	struct phl_led_event_args_t *event_args = NULL;
	struct phl_led_event_args_t *event_args_prev = NULL;

	PHL_TRACE(COMP_PHL_LED, _PHL_INFO_, "===> %s()\n", __func__);

	if (led_ctrl == NULL) {
		PHL_TRACE(COMP_PHL_LED, _PHL_INFO_, "%s: led_ctrl == NULL\n",
			  __func__);
		return;
	}

	if (action_args_arr == NULL) {
		PHL_TRACE(COMP_PHL_LED, _PHL_INFO_,
			  "%s: input -- action_args_arr == NULL\n", __func__);
		return;
	}

	PHL_TRACE(COMP_PHL_LED, _PHL_INFO_,
		  "%s: event == %d, state_condition == %d\n", __func__, event,
		  state_condition);

	event_args =
	    led_ctrl->event_args_list_arr[event]; /* event_args = list head */

	while (event_args != NULL) {
		if (event_args->state_condition == state_condition) {
			PHL_TRACE(
			    COMP_PHL_LED, _PHL_INFO_,
			    "%s: the event_args_list of event 0x%x already has "
			    "a node with state_condition == 0x%x\n",
			    __func__, event, state_condition);
			return;
		}

		event_args_prev = event_args;
		event_args = event_args->next;
	}

	if (NULL == (event_args = _os_mem_alloc(
			 drv_priv, sizeof(struct phl_led_event_args_t)))) {

		PHL_ERR("%s: alloc buffer failed!\n", __func__);
		return;
	}

	if (NULL == (event_args->action_args_arr = _os_mem_alloc(
			 drv_priv, action_args_arr_len *
				       sizeof(struct rtw_led_action_args_t)))) {

		PHL_ERR("%s: alloc buffer failed!\n", __func__);
		_os_mem_free(drv_priv, event_args,
			     sizeof(struct phl_led_event_args_t));
		return;
	}

	event_args->action_args_arr_len = action_args_arr_len;
	event_args->state_condition = state_condition;
	event_args->toggle_delay_unit = toggle_delay_unit;

	_os_mem_cpy(drv_priv, event_args->action_args_arr, action_args_arr,
		    action_args_arr_len * sizeof(struct rtw_led_action_args_t));

	event_args->next = NULL;

	if (event_args_prev == NULL)
		/* the event_args_list was empty */
		led_ctrl->event_args_list_arr[event] = event_args;
	else
		event_args_prev->next = event_args;
}

void phl_led_control(struct phl_info_t *phl_info, enum rtw_led_event led_event)
{
#ifdef CONFIG_CMD_DISP
	struct phl_msg msg = {0};
	struct phl_msg_attribute attr = {0};

	PHL_TRACE(COMP_PHL_LED, _PHL_INFO_, "===> %s()\n", __func__);

	SET_MSG_MDL_ID_FIELD(msg.msg_id, PHL_MDL_LED);
	SET_MSG_EVT_ID_FIELD(msg.msg_id, led_event + MSG_EVT_LED_EVT_START);
	msg.band_idx = HW_BAND_0;
	phl_disp_eng_send_msg(phl_info, &msg, &attr, NULL);
#else
	PHL_ERR("phl_fsm not support %s\n", __func__);
#endif
}

static void _phl_led_manual_control_completion(void *priv, struct phl_msg *msg)
{
	_os_mem_free(priv, msg->inbuf, sizeof(struct rtw_led_action_args_t));
}

void rtw_phl_led_manual_control(void *phl, enum rtw_led_id led_id,
				enum rtw_led_opt opt)
{
#ifdef CONFIG_CMD_DISP
	enum rtw_phl_status status = RTW_PHL_STATUS_FAILURE;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	void *drv_priv = phl_to_drvpriv(phl_info);

	struct phl_led_ctrl_t *led_ctrl =
	    (struct phl_led_ctrl_t *)phl_info->led_ctrl;
	struct phl_led_info_t *led_info = NULL;
	struct rtw_led_action_args_t *action_args = NULL;
	enum rtw_led_action led_action;

	struct phl_msg msg = {0};
	struct phl_msg_attribute attr = {0};

	PHL_TRACE(COMP_PHL_LED, _PHL_INFO_, "===> %s()\n", __func__);

	if (led_ctrl == NULL) {
		PHL_TRACE(COMP_PHL_LED, _PHL_INFO_, "%s: led_ctrl == NULL\n",
			  __func__);
		return;
	}

	led_info = &(led_ctrl->led_info_arr[led_id]);

	if (!led_info->manual_mode_on)
		return;

	if (opt == RTW_LED_OPT_LOW)
		led_action = RTW_LED_ACTION_LOW;
	else if (opt == RTW_LED_OPT_HIGH)
		led_action = RTW_LED_ACTION_HIGH;
	else {
		PHL_ERR("%s: unknown opt: %d\n", __func__, opt);
		return;
	}

	if (NULL == (action_args = _os_mem_alloc(
			 drv_priv, sizeof(struct rtw_led_action_args_t)))) {

		PHL_ERR("%s: alloc buffer failed!\n", __func__);
		return;
	}

	action_args->led_id = led_id;
	action_args->led_action = led_action;
	_os_mem_set(drv_priv, &(action_args->toggle_args), 0,
		    +sizeof(struct rtw_led_toggle_args_t));

	SET_MSG_MDL_ID_FIELD(msg.msg_id, PHL_MDL_LED);
	SET_MSG_EVT_ID_FIELD(msg.msg_id, MSG_EVT_LED_MANUAL_CTRL);
	msg.band_idx = HW_BAND_0;
	msg.inbuf = (u8 *)action_args;
	msg.inlen = sizeof(struct rtw_led_action_args_t);

	attr.completion.completion = _phl_led_manual_control_completion;
	attr.completion.priv = drv_priv;

	if (RTW_PHL_STATUS_SUCCESS ==
	    (status = phl_disp_eng_send_msg(phl_info, &msg, &attr, NULL)))
		return;

	_phl_led_manual_control_completion(attr.completion.priv, &msg);

	if (status == RTW_PHL_STATUS_UNEXPECTED_ERROR)
		/* cmd dispatcher is not started */
		_phl_led_ctrl_write_opt(phl_info->hal, led_id,
					&(led_info->curr_opt), opt);

#else
	PHL_ERR("phl_fsm not support %s\n", __func__);
#endif
}

void rtw_phl_led_control(void *phl, enum rtw_led_event led_event)
{
	phl_led_control((struct phl_info_t *)phl, led_event);
}

void rtw_phl_led_manual_mode_switch(void *phl, enum rtw_led_id led_id,
				    bool manual_mode_on)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct phl_led_ctrl_t *led_ctrl =
	    (struct phl_led_ctrl_t *)phl_info->led_ctrl;
	struct phl_led_info_t *led_info = NULL;

	PHL_TRACE(COMP_PHL_LED, _PHL_INFO_, "===> %s()\n", __func__);

	if (led_ctrl == NULL) {
		PHL_TRACE(COMP_PHL_LED, _PHL_INFO_, "%s: led_ctrl == NULL\n",
			  __func__);
		return;
	}

	led_info = &(led_ctrl->led_info_arr[led_id]);

	led_info->manual_mode_on = manual_mode_on;
}
