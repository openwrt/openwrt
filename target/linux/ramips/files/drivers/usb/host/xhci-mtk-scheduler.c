#include "xhci-mtk-scheduler.h"
#include <linux/kernel.h>       /* printk() */

static struct sch_ep **ss_out_eps[MAX_EP_NUM];
static struct sch_ep **ss_in_eps[MAX_EP_NUM];
static struct sch_ep **hs_eps[MAX_EP_NUM];	//including tt isoc
static struct sch_ep **tt_intr_eps[MAX_EP_NUM];


int mtk_xhci_scheduler_init(void){
	int i;

	for(i=0; i<MAX_EP_NUM; i++){
		ss_out_eps[i] = NULL;
	}
	for(i=0; i<MAX_EP_NUM; i++){
		ss_in_eps[i] = NULL;
	}
	for(i=0; i<MAX_EP_NUM; i++){
		hs_eps[i] = NULL;
	}
	for(i=0; i<MAX_EP_NUM; i++){
		tt_intr_eps[i] = NULL;
	}
	return 0;
}

int add_sch_ep(int dev_speed, int is_in, int isTT, int ep_type, int maxp, int interval, int burst
	, int mult, int offset, int repeat, int pkts, int cs_count, int burst_mode
	, int bw_cost, mtk_u32 *ep, struct sch_ep *tmp_ep){

	struct sch_ep **ep_array;
	int i;

	if(is_in && dev_speed == USB_SPEED_SUPER ){
		ep_array = (struct sch_ep **)ss_in_eps;
	}
	else if(dev_speed == USB_SPEED_SUPER){
		ep_array = (struct sch_ep **)ss_out_eps;
	}
	else if(dev_speed == USB_SPEED_HIGH || (isTT && ep_type == USB_EP_ISOC)){
		ep_array = (struct sch_ep **)hs_eps;
	}
	else{
		ep_array = (struct sch_ep **)tt_intr_eps;
	}
	for(i=0; i<MAX_EP_NUM; i++){
		if(ep_array[i] == NULL){
			tmp_ep->dev_speed = dev_speed;
			tmp_ep->isTT = isTT;
			tmp_ep->is_in = is_in;
			tmp_ep->ep_type = ep_type;
			tmp_ep->maxp = maxp;
			tmp_ep->interval = interval;
			tmp_ep->burst = burst;
			tmp_ep->mult = mult;
			tmp_ep->offset = offset;
			tmp_ep->repeat = repeat;
			tmp_ep->pkts = pkts;
			tmp_ep->cs_count = cs_count;
			tmp_ep->burst_mode = burst_mode;
			tmp_ep->bw_cost = bw_cost;
			tmp_ep->ep = ep;
			ep_array[i] = tmp_ep;
			return SCH_SUCCESS;
		}
	}
	return SCH_FAIL;
}

int count_ss_bw(int is_in, int ep_type, int maxp, int interval, int burst, int mult, int offset, int repeat
	, int td_size){
	int i, j, k;
	int bw_required[3];
	int final_bw_required;
	int bw_required_per_repeat;
	int tmp_bw_required;
	struct sch_ep *cur_sch_ep;
	struct sch_ep **ep_array;
	int cur_offset;
	int cur_ep_offset;
	int tmp_offset;
	int tmp_interval;
	int ep_offset;
	int ep_interval;
	int ep_repeat;
	int ep_mult;
	
	if(is_in){
		ep_array = (struct sch_ep **)ss_in_eps;
	}
	else{
		ep_array = (struct sch_ep **)ss_out_eps;
	}
	
	bw_required[0] = 0;
	bw_required[1] = 0;
	bw_required[2] = 0;
	
	if(repeat == 0){
		final_bw_required = 0;
		for(i=0; i<MAX_EP_NUM; i++){
			cur_sch_ep = ep_array[i];
			if(cur_sch_ep == NULL){
				continue;
			}
			ep_interval = cur_sch_ep->interval;
			ep_offset = cur_sch_ep->offset;
			if(cur_sch_ep->repeat == 0){
				if(ep_interval >= interval){
					tmp_offset = ep_offset + ep_interval - offset;
					tmp_interval = interval;
				}
				else{
					tmp_offset = offset + interval - ep_offset;
					tmp_interval = ep_interval;
				}
				if(tmp_offset % tmp_interval == 0){
					final_bw_required += cur_sch_ep->bw_cost;
				}
			}
			else{
				ep_repeat = cur_sch_ep->repeat;
				ep_mult = cur_sch_ep->mult;
				for(k=0; k<=ep_mult; k++){
					cur_ep_offset = ep_offset+(k*ep_mult);
					if(ep_interval >= interval){
						tmp_offset = cur_ep_offset + ep_interval - offset;
						tmp_interval = interval;
					}
					else{
						tmp_offset = offset + interval - cur_ep_offset;
						tmp_interval = ep_interval;
					}
					if(tmp_offset % tmp_interval == 0){
						final_bw_required += cur_sch_ep->bw_cost;
						break;
					}
				}
			}
		}
		final_bw_required += td_size;
	}
	else{
		bw_required_per_repeat = maxp * (burst+1);
		for(j=0; j<=mult; j++){
			tmp_bw_required = 0;
			cur_offset = offset+(j*repeat);
			for(i=0; i<MAX_EP_NUM; i++){
				cur_sch_ep = ep_array[i];
				if(cur_sch_ep == NULL){
					continue;
				}
				ep_interval = cur_sch_ep->interval;
				ep_offset = cur_sch_ep->offset;
				if(cur_sch_ep->repeat == 0){
					if(ep_interval >= interval){
						tmp_offset = ep_offset + ep_interval - cur_offset;
						tmp_interval = interval;
					}
					else{
						tmp_offset = cur_offset + interval - ep_offset;
						tmp_interval = ep_interval;
					}
					if(tmp_offset % tmp_interval == 0){
						tmp_bw_required += cur_sch_ep->bw_cost;
					}
				}
				else{
					ep_repeat = cur_sch_ep->repeat;
					ep_mult = cur_sch_ep->mult;
					for(k=0; k<=ep_mult; k++){
						cur_ep_offset = ep_offset+(k*ep_repeat);
						if(ep_interval >= interval){
							tmp_offset = cur_ep_offset + ep_interval - cur_offset;
							tmp_interval = interval;
						}
						else{
							tmp_offset = cur_offset + interval - cur_ep_offset;
							tmp_interval = ep_interval;
						}
						if(tmp_offset % tmp_interval == 0){
							tmp_bw_required += cur_sch_ep->bw_cost;
							break;
						}
					}
				}
			}
			bw_required[j] = tmp_bw_required;
		}
		final_bw_required = SS_BW_BOUND;
		for(j=0; j<=mult; j++){
			if(bw_required[j] < final_bw_required){
				final_bw_required = bw_required[j];
			}
		}
		final_bw_required += bw_required_per_repeat;
	}
	return final_bw_required;
}

int count_hs_bw(int ep_type, int maxp, int interval, int offset, int td_size){
	int i;
	int bw_required;
	struct sch_ep *cur_sch_ep;
	int tmp_offset;
	int tmp_interval;
	int ep_offset;
	int ep_interval;
	int cur_tt_isoc_interval;	//for isoc tt check
	
	bw_required = 0;
	for(i=0; i<MAX_EP_NUM; i++){
		
		cur_sch_ep = (struct sch_ep *)hs_eps[i];
		if(cur_sch_ep == NULL){
				continue;
		}
		ep_offset = cur_sch_ep->offset;
		ep_interval = cur_sch_ep->interval;
		
		if(cur_sch_ep->isTT && cur_sch_ep->ep_type == USB_EP_ISOC){
			cur_tt_isoc_interval = ep_interval<<3;
			if(ep_interval >= interval){
				tmp_offset = ep_offset + cur_tt_isoc_interval - offset;
				tmp_interval = interval;
			}
			else{
				tmp_offset = offset + interval - ep_offset;
				tmp_interval = cur_tt_isoc_interval;
			}
			if(cur_sch_ep->is_in){
				if((tmp_offset%tmp_interval >=2) && (tmp_offset%tmp_interval <= cur_sch_ep->cs_count)){
					bw_required += 188;
				}
			}
			else{
				if(tmp_offset%tmp_interval <= cur_sch_ep->cs_count){
					bw_required += 188;
				}
			}
		}
		else{
			if(ep_interval >= interval){
				tmp_offset = ep_offset + ep_interval - offset;
				tmp_interval = interval;
			}
			else{
				tmp_offset = offset + interval - ep_offset;
				tmp_interval = ep_interval;
			}
			if(tmp_offset%tmp_interval == 0){
				bw_required += cur_sch_ep->bw_cost;
			}
		}
	}
	bw_required += td_size;
	return bw_required;
}

int count_tt_isoc_bw(int is_in, int maxp, int interval, int offset, int td_size){
	char is_cs;
	int mframe_idx, frame_idx, s_frame, s_mframe, cur_mframe;
	int bw_required, max_bw;
	int ss_cs_count;
	int cs_mframe;
	int max_frame;
	int i,j;
	struct sch_ep *cur_sch_ep;
	int ep_offset;
	int ep_interval;
	int ep_cs_count;
	int tt_isoc_interval;	//for isoc tt check
	int cur_tt_isoc_interval;	//for isoc tt check
	int tmp_offset;
	int tmp_interval;
	
	is_cs = 0;
	
	tt_isoc_interval = interval<<3;	//frame to mframe
	if(is_in){
		is_cs = 1;
	}
	s_frame = offset/8;
	s_mframe = offset%8;
	ss_cs_count = (maxp + (188 - 1))/188;
	if(is_cs){
		cs_mframe = offset%8 + 2 + ss_cs_count;
		if (cs_mframe <= 6)
			ss_cs_count += 2;
		else if (cs_mframe == 7)
			ss_cs_count++;
		else if (cs_mframe > 8)
			return -1;
	}
	max_bw = 0;
	if(is_in){
		i=2;
	}
	for(cur_mframe = offset+i; i<ss_cs_count; cur_mframe++, i++){
		bw_required = 0;
		for(j=0; j<MAX_EP_NUM; j++){
			cur_sch_ep = (struct sch_ep *)hs_eps[j];
			if(cur_sch_ep == NULL){
				continue;
			}
			ep_offset = cur_sch_ep->offset;
			ep_interval = cur_sch_ep->interval;
			if(cur_sch_ep->isTT && cur_sch_ep->ep_type == USB_EP_ISOC){
				//isoc tt
				//check if mframe offset overlap
				//if overlap, add 188 to the bw
				cur_tt_isoc_interval = ep_interval<<3;
				if(cur_tt_isoc_interval >= tt_isoc_interval){
					tmp_offset = (ep_offset+cur_tt_isoc_interval)  - cur_mframe;
					tmp_interval = tt_isoc_interval;
				}
				else{
					tmp_offset = (cur_mframe+tt_isoc_interval) - ep_offset;
					tmp_interval = cur_tt_isoc_interval;
				}
				if(cur_sch_ep->is_in){
					if((tmp_offset%tmp_interval >=2) && (tmp_offset%tmp_interval <= cur_sch_ep->cs_count)){
						bw_required += 188;
					}
				}
				else{
					if(tmp_offset%tmp_interval <= cur_sch_ep->cs_count){
						bw_required += 188;
					}
				}
				
			}
			else if(cur_sch_ep->ep_type == USB_EP_INT || cur_sch_ep->ep_type == USB_EP_ISOC){
				//check if mframe
				if(ep_interval >= tt_isoc_interval){
					tmp_offset = (ep_offset+ep_interval) - cur_mframe;
					tmp_interval = tt_isoc_interval;
				}
				else{
					tmp_offset = (cur_mframe+tt_isoc_interval) - ep_offset;
					tmp_interval = ep_interval;
				}
				if(tmp_offset%tmp_interval == 0){
					bw_required += cur_sch_ep->bw_cost;
				}
			}
		}
		bw_required += 188;
		if(bw_required > max_bw){
			max_bw = bw_required;
		}
	}
	return max_bw;
}

int count_tt_intr_bw(int interval, int frame_offset){
	//check all eps in tt_intr_eps
	int ret;
	int i,j;
	int ep_offset;
	int ep_interval;
	int tmp_offset;
	int tmp_interval;
	ret = SCH_SUCCESS;
	struct sch_ep *cur_sch_ep;
	
	for(i=0; i<MAX_EP_NUM; i++){
		cur_sch_ep = (struct sch_ep *)tt_intr_eps[i];
		if(cur_sch_ep == NULL){
			continue;
		}
		ep_offset = cur_sch_ep->offset;
		ep_interval = cur_sch_ep->interval;
		if(ep_interval  >= interval){
			tmp_offset = ep_offset + ep_interval - frame_offset;
			tmp_interval = interval;
		}
		else{
			tmp_offset = frame_offset + interval - ep_offset;
			tmp_interval = ep_interval;
		}
		
		if(tmp_offset%tmp_interval==0){
			return SCH_FAIL;
		}
	}
	return SCH_SUCCESS;
}

struct sch_ep * mtk_xhci_scheduler_remove_ep(int dev_speed, int is_in, int isTT, int ep_type, mtk_u32 *ep){
	int i;
	struct sch_ep **ep_array;
	struct sch_ep *cur_ep;

	if (is_in && dev_speed == USB_SPEED_SUPER) {
		ep_array = (struct sch_ep **)ss_in_eps;
	}
	else if (dev_speed == USB_SPEED_SUPER) {
		ep_array = (struct sch_ep **)ss_out_eps;
	}
	else if (dev_speed == USB_SPEED_HIGH || (isTT && ep_type == USB_EP_ISOC)) {
		ep_array = (struct sch_ep **)hs_eps;
	}
	else {
		ep_array = (struct sch_ep **)tt_intr_eps;
	}
	for (i = 0; i < MAX_EP_NUM; i++) {
		cur_ep = (struct sch_ep *)ep_array[i];
		if(cur_ep != NULL && cur_ep->ep == ep){
			ep_array[i] = NULL;
			return cur_ep;
		}
	}
	return NULL;
}

int mtk_xhci_scheduler_add_ep(int dev_speed, int is_in, int isTT, int ep_type, int maxp, int interval, int burst
	, int mult, mtk_u32 *ep, mtk_u32 *ep_ctx, struct sch_ep *sch_ep){
	mtk_u32 bPkts = 0;
	mtk_u32 bCsCount = 0;
	mtk_u32 bBm = 1;
	mtk_u32 bOffset = 0;
	mtk_u32 bRepeat = 0;
	int ret;
	struct mtk_xhci_ep_ctx *temp_ep_ctx;
	int td_size;
	int mframe_idx, frame_idx;
	int bw_cost;
	int cur_bw, best_bw, best_bw_idx,repeat, max_repeat, best_bw_repeat;
	int cur_offset, cs_mframe;
	int break_out;
	int frame_interval;

	printk(KERN_ERR "add_ep parameters, dev_speed %d, is_in %d, isTT %d, ep_type %d, maxp %d, interval %d, burst %d, mult %d, ep 0x%x, ep_ctx 0x%x, sch_ep 0x%x\n", dev_speed, is_in, isTT, ep_type, maxp
		, interval, burst, mult, ep, ep_ctx, sch_ep);
	if(isTT && ep_type == USB_EP_INT && ((dev_speed == USB_SPEED_LOW) || (dev_speed == USB_SPEED_FULL))){
		frame_interval = interval >> 3;
		for(frame_idx=0; frame_idx<frame_interval; frame_idx++){
			printk(KERN_ERR "check tt_intr_bw interval %d, frame_idx %d\n", frame_interval, frame_idx);
			if(count_tt_intr_bw(frame_interval, frame_idx) == SCH_SUCCESS){
				printk(KERN_ERR "check OK............\n");
				bOffset = frame_idx<<3;
				bPkts = 1;
				bCsCount = 3;
				bw_cost = maxp;
				bRepeat = 0;
				if(add_sch_ep(dev_speed, is_in, isTT, ep_type, maxp, frame_interval, burst, mult
					, bOffset, bRepeat, bPkts, bCsCount, bBm, maxp, ep, sch_ep) == SCH_FAIL){
					return SCH_FAIL;
				}
				ret = SCH_SUCCESS;
				break;
			}
		}
	}
	else if(isTT && ep_type == USB_EP_ISOC){
		best_bw = HS_BW_BOUND;
		best_bw_idx = -1;
		cur_bw = 0;
		td_size = maxp;
		break_out = 0;
		frame_interval = interval>>3;
		for(frame_idx=0; frame_idx<frame_interval && !break_out; frame_idx++){
			for(mframe_idx=0; mframe_idx<8; mframe_idx++){
				cur_offset = (frame_idx*8) + mframe_idx;
				cur_bw = count_tt_isoc_bw(is_in, maxp, frame_interval, cur_offset, td_size);
				if(cur_bw > 0 && cur_bw < best_bw){
					best_bw_idx = cur_offset;
					best_bw = cur_bw;
					if(cur_bw == td_size || cur_bw < (HS_BW_BOUND>>1)){
						break_out = 1;
						break;
					}
				}
			}
		}
		if(best_bw_idx == -1){
			return SCH_FAIL;
		}
		else{
			bOffset = best_bw_idx;
			bPkts = 1;
			bCsCount = (maxp + (188 - 1)) / 188;
			if(is_in){
				cs_mframe = bOffset%8 + 2 + bCsCount;
				if (cs_mframe <= 6)
					bCsCount += 2;
				else if (cs_mframe == 7)
					bCsCount++;
			}
			bw_cost = 188;
			bRepeat = 0;
			if(add_sch_ep( dev_speed, is_in, isTT, ep_type, maxp, interval, burst, mult
				, bOffset, bRepeat, bPkts, bCsCount, bBm, bw_cost, ep, sch_ep) == SCH_FAIL){
				return SCH_FAIL;
			}
			ret = SCH_SUCCESS;
		}
	}
	else if((dev_speed == USB_SPEED_FULL || dev_speed == USB_SPEED_LOW) && ep_type == USB_EP_INT){
		bPkts = 1;
		ret = SCH_SUCCESS;
	}
	else if(dev_speed == USB_SPEED_FULL && ep_type == USB_EP_ISOC){
		bPkts = 1;
		ret = SCH_SUCCESS;
	}
	else if(dev_speed == USB_SPEED_HIGH && (ep_type == USB_EP_INT || ep_type == USB_EP_ISOC)){
		best_bw = HS_BW_BOUND;
		best_bw_idx = -1;
		cur_bw = 0;
		td_size = maxp*(burst+1);
		for(cur_offset = 0; cur_offset<interval; cur_offset++){
			cur_bw = count_hs_bw(ep_type, maxp, interval, cur_offset, td_size);
			if(cur_bw > 0 && cur_bw < best_bw){
				best_bw_idx = cur_offset;
				best_bw = cur_bw;
				if(cur_bw == td_size || cur_bw < (HS_BW_BOUND>>1)){
					break;
				}
			}
		}
		if(best_bw_idx == -1){
			return SCH_FAIL;
		}
		else{
			bOffset = best_bw_idx;
			bPkts = burst + 1;
			bCsCount = 0;
			bw_cost = td_size;
			bRepeat = 0;
			if(add_sch_ep(dev_speed, is_in, isTT, ep_type, maxp, interval, burst, mult
				, bOffset, bRepeat, bPkts, bCsCount, bBm, bw_cost, ep, sch_ep) == SCH_FAIL){
				return SCH_FAIL;
			}
			ret = SCH_SUCCESS;
		}
	}
	else if(dev_speed == USB_SPEED_SUPER && (ep_type == USB_EP_INT || ep_type == USB_EP_ISOC)){
		best_bw = SS_BW_BOUND;
		best_bw_idx = -1;
		cur_bw = 0;
		td_size = maxp * (mult+1) * (burst+1);
		if(mult == 0){
			max_repeat = 0;
		}
		else{
			max_repeat = (interval-1)/(mult+1);
		}
		break_out = 0;
		for(frame_idx = 0; (frame_idx < interval) && !break_out; frame_idx++){
			for(repeat = max_repeat; repeat >= 0; repeat--){
				cur_bw = count_ss_bw(is_in, ep_type, maxp, interval, burst, mult, frame_idx
					, repeat, td_size);
				printk(KERN_ERR "count_ss_bw, frame_idx %d, repeat %d, td_size %d, result bw %d\n"
					, frame_idx, repeat, td_size, cur_bw);
				if(cur_bw > 0 && cur_bw < best_bw){
					best_bw_idx = frame_idx;
					best_bw_repeat = repeat;
					best_bw = cur_bw;
					if(cur_bw <= td_size || cur_bw < (HS_BW_BOUND>>1)){
						break_out = 1;
						break;
					}
				}
			}
		}
		printk(KERN_ERR "final best idx %d, best repeat %d\n", best_bw_idx, best_bw_repeat);
		if(best_bw_idx == -1){
			return SCH_FAIL;
		}
		else{
			bOffset = best_bw_idx;
			bCsCount = 0;
			bRepeat = best_bw_repeat;
			if(bRepeat == 0){
				bw_cost = (burst+1)*(mult+1)*maxp;
				bPkts = (burst+1)*(mult+1);
			}
			else{
				bw_cost = (burst+1)*maxp;
				bPkts = (burst+1);
			}
			if(add_sch_ep(dev_speed, is_in, isTT, ep_type, maxp, interval, burst, mult
				, bOffset, bRepeat, bPkts, bCsCount, bBm, bw_cost, ep, sch_ep) == SCH_FAIL){
				return SCH_FAIL;
			}
			ret = SCH_SUCCESS;
		}
	}
	else{
		bPkts = 1;
		ret = SCH_SUCCESS;
	}
	if(ret == SCH_SUCCESS){
		temp_ep_ctx = (struct mtk_xhci_ep_ctx *)ep_ctx;
		temp_ep_ctx->reserved[0] |= (BPKTS(bPkts) | BCSCOUNT(bCsCount) | BBM(bBm));
		temp_ep_ctx->reserved[1] |= (BOFFSET(bOffset) | BREPEAT(bRepeat));

		printk(KERN_DEBUG "[DBG] BPKTS: %x, BCSCOUNT: %x, BBM: %x\n", bPkts, bCsCount, bBm);
		printk(KERN_DEBUG "[DBG] BOFFSET: %x, BREPEAT: %x\n", bOffset, bRepeat);
		return SCH_SUCCESS;
	}
	else{
		return SCH_FAIL;
	}
}
