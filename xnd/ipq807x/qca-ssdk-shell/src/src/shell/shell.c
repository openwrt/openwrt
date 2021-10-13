/*
 * Copyright (c) 2014, 2017-2018, The Linux Foundation. All rights reserved.
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all copies.
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
/*qca808x_start*/
#include <stdio.h>
#include <stdarg.h>
#include <pthread.h>
#include "shell.h"
#include "shell_io.h"
#include "shell_sw.h"
#include "shell_lib.h"
#include "shell_config.h"
#include "api_access.h"
#include "fal_uk_if.h"

a_ulong_t *ioctl_buf = NULL;
ssdk_init_cfg init_cfg = def_init_cfg;
ssdk_cfg_t ssdk_cfg;
static a_uint32_t flag = 0;

static a_ulong_t *ioctl_argp;
static FILE * out_fd;
char dev_id_path[] = "/sys/ssdk/dev_id";
#ifndef SSDK_STR
#define SSDK_STR  "SSDK"
#endif
static char *err_info[] =
{
    "Operation succeeded",                 /*SW_OK*/
    "Operation failed",                    /*SW_FAIL*/
    "Illegal value ",                      /*SW_BAD_VALUE*/
    "Value is out of range ",              /*SW_OUT_OF_RANGE*/
    "Illegal parameter(s) ",               /*SW_BAD_PARAM*/
    "Illegal pointer value ",              /*SW_BAD_PTR*/
    "Wrong length",                        /*SW_BAD_LEN*/
    "Wrong state of state machine ",       /*SW_BAD_STATE*/
    "Read operation failed  ",             /*SW_READ_ERROR*/
    "Write operation failed ",             /*SW_WRITE_ERROR*/
    "Fail in creating an entry ",          /*SW_CREATE_ERROR*/
    "Fail in deleteing an entry ",         /*SW_DELETE_ERROR*/
    "Entry not found     ",                /*SW_NOT_FOUND*/
    "The parameter(s) is the same  ",      /*SW_NO_CHANGE*/
    "No more entry found  ",               /*SW_NO_MORE*/
    "No such entry   ",                    /*SW_NO_SUCH*/
    "Tried to create existing entry ",     /*SW_ALREADY_EXIST*/
    "Table is full  ",                     /*SW_FULL*/
    "Table is empty ",                     /*SW_EMPTY*/
    "This request is not support    ",     /*SW_NOT_SUPPORTED*/
    "This request is not implemented",     /*SW_NOT_IMPLEMENTED*/
    "The item is not initialized ",        /*SW_NOT_INITIALIZED*/
    "Operation is still running",          /*SW_BUSY*/
    "Operation Time Out ",                 /*SW_TIMEOUT*/
    "Operation is disabled ",              /*SW_DISABLE*/
    "Resource not available (memory ...)", /*SW_NO_RESOURCE*/
    "Error occured while INIT process",    /*SW_INIT_ERROR*/
    "The other side is not ready yet",     /*SW_NOT_READY */
    "Cpu memory allocation failed. ",      /*SW_OUT_OF_MEM */
    "Operation has been aborted. ",        /*SW_ABORTED*/
} ;

void
cmd_print_error(sw_error_t rtn)
{
    dprintf("\n%s\n\n", err_info[abs(rtn)]);
}

void
cmd_print(char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    if(out_fd)
        vfprintf(out_fd, fmt, args);
    else
        vfprintf(stdout, fmt, args);
    va_end(args);
}

static sw_error_t
cmd_input_parser(a_ulong_t *arg_val, a_uint32_t arg_index, sw_api_param_t *pp)
{
    a_int16_t i;
    a_ulong_t *pbuf;
    a_uint16_t rtn_size = 1;
    sw_api_param_t *pptmp = pp;

    pbuf = ioctl_buf + rtn_size;    /*reserve for return value */

    for (i = 0; i < arg_index; i++)
    {
        pptmp = pp + i;
        if (pptmp->param_type & SW_PARAM_PTR)
        {
            pbuf += (pptmp->data_size + 3) / 4;
        }
    }
    if ((pbuf - ioctl_buf + (pptmp->data_size + 3) / 4) > (IOCTL_BUF_SIZE/4))
    {
        return SW_NO_RESOURCE;
    }

    *arg_val = (a_ulong_t) pbuf;

    return SW_OK;
}

static sw_error_t
cmd_api_func(sw_api_func_t *fp, a_uint32_t nr_param, a_ulong_t * args)
{
    a_ulong_t *p = &args[2];
    sw_error_t rv;
    sw_error_t(*func) ();

    func = fp->func;

    switch (nr_param)
    {
        case 0:
            rv = (func) ();
            break;
        case 1:
            rv = (func) (p[0]);
            break;
        case 2:
            rv = (func) (p[0], p[1]);
            break;
        case 3:
            rv = (func) (p[0], p[1], p[2]);
            break;
        case 4:
            rv = (func) (p[0], p[1], p[2], p[3]);
            break;
        case 5:
            rv = (func) (p[0], p[1], p[2], p[3], p[4]);
            break;
        case 6:
            rv = (func) (p[0], p[1], p[2], p[3], p[4], p[5]);
            break;
        case 7:
            rv = (func) (p[0], p[1], p[2], p[3], p[4], p[5], p[6]);
            break;
        case 8:
            rv = (func) (p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7]);
            break;
        case 9:
            rv = (func) (p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7], p[8]);
            break;
        case 10:
            rv = (func) (p[0], p[1], p[2], p[3], p[4], p[5],
                         p[6], p[7], p[8], p[9]);
            break;
        default:
            rv = SW_OUT_OF_RANGE;
    }

    *(a_ulong_t *) args[1] = rv;

    return rv;
}

static sw_error_t
cmd_api_output(sw_api_param_t *pp, a_uint32_t nr_param, a_ulong_t * args)
{
    a_uint16_t i;
    a_ulong_t *pbuf;
    a_uint16_t rtn_size = 1;
    sw_error_t rtn = (sw_error_t) (*ioctl_buf);
    sw_api_param_t *pptmp = NULL;

    if (rtn != SW_OK)
    {
        cmd_print_error(rtn);
        return rtn;
    }

    pbuf = ioctl_buf + rtn_size;
    for (i = 0; i < nr_param; i++)
    {
        pptmp = pp + i;
        if (pptmp->param_type & SW_PARAM_PTR)
        {

            if (pptmp->param_type & SW_PARAM_OUT)
            {

                sw_data_type_t *data_type;
                if (!(data_type = cmd_data_type_find(pptmp->data_type)))
                    return SW_NO_SUCH;

                if (data_type->show_func)
                {
                    data_type->show_func(pptmp->param_name, pbuf, pptmp->data_size);
/*qca808x_end*/
                    if(strcmp((a_char_t *)pptmp->param_name, "Function bitmap") == 0)
                    {
			cmd_data_print_module_func_ctrl(args[3], (fal_func_ctrl_t *)pbuf);
                    }
/*qca808x_start*/
                }
                else
                {
                    dprintf("\n Error, not define output print function!");
                }
            }

            if ((pbuf - ioctl_buf +
                    (pptmp->data_size + 3) / 4) > (IOCTL_BUF_SIZE/4))
                return SW_NO_RESOURCE;

            pbuf += (pptmp->data_size + 3) / 4;

        }
    }
    return SW_OK;
}

void
cmd_strtol(char *str, a_uint32_t * arg_val)
{
    if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X'))
        sscanf(str, "%x", arg_val);
    else
        sscanf(str, "%d", arg_val);
}

static sw_error_t
cmd_parse_api(char **cmd_str, a_ulong_t * arg_val)
{
    char *tmp_str;
    a_uint32_t arg_index, arg_start = 2, reserve_index = 1; /*reserve for dev_id */
    a_uint32_t last_param_in = 0;
    a_ulong_t *temp;
    void *pentry;
    sw_api_param_t *pptmp = NULL;
    sw_api_t sw_api;
    a_uint32_t ignorecnt = 0, jump = 0;
    sw_api.api_id = arg_val[0];
    SW_RTN_ON_ERROR(sw_api_get(&sw_api));

    /*set device id */
    arg_val[arg_start] = get_devid();

    for (arg_index = reserve_index; arg_index < sw_api.api_nr; arg_index++)
    {
        tmp_str = NULL;
        pptmp = sw_api.api_pp + arg_index;

        if (!(pptmp->param_type & SW_PARAM_IN))
        {
            ignorecnt++;
        }

        if (pptmp->param_type & SW_PARAM_IN)
        {
            tmp_str = cmd_str[arg_index - reserve_index - ignorecnt + jump];
            last_param_in = arg_index;
            if((pptmp->api_id == 314) && last_param_in == 2) last_param_in = 4;//SW_API_FDB_EXTEND_NEXT wr
            if((pptmp->api_id == 327) && last_param_in == 2) last_param_in = 4;//SW_API_FDB_EXTEND_FIRST wr
        }
        temp = &arg_val[arg_start + arg_index];

        sw_data_type_t *data_type;
        if (!(data_type = cmd_data_type_find(pptmp->data_type)))
            return SW_NO_SUCH;

        pentry = temp;
        if (pptmp->param_type & SW_PARAM_PTR)
        {
            if (cmd_input_parser(temp, arg_index, sw_api.api_pp) != SW_OK)
                return SW_NO_RESOURCE;

            pentry = (void *) *temp;
        }

        if (pptmp->param_type & SW_PARAM_IN)
        {
#if 1
            if(pptmp->param_type & SW_PARAM_PTR)   //quiet mode
            {
                if(!get_talk_mode())
                    set_full_cmdstrp((char **)(cmd_str + (last_param_in - reserve_index) + jump));
            }
#endif
            /*check and convert input param */
            if (data_type->param_check != NULL)
            {
                if (data_type->param_check(tmp_str, pentry, pptmp->data_size) != SW_OK)
                    return SW_BAD_PARAM;
	    if(!get_talk_mode() && (pptmp->param_type & SW_PARAM_PTR)) {
	    	if (get_jump())
	    		jump += get_jump() -1;
	    }

            }
        }
    }

    /*superfluous args */
    /*
       if(cmd_str[last_param_in] != NULL)
       return SW_BAD_PARAM;
     */

    return SW_OK;
}

static sw_error_t
cmd_parse_sw(char **cmd_str, a_ulong_t * arg_val)
{
    char *tmp_str;
    a_uint32_t arg_index = 0, tmp = 0;
    a_uint32_t api_id = arg_val[0];

    tmp_str = cmd_str[arg_index];
    while (tmp_str)
    {
        arg_index++;
        cmd_strtol(tmp_str, &tmp);
        arg_val[arg_index] = tmp;
        tmp_str = cmd_str[arg_index];
    }

    /*args number check */
    if ( (arg_index == 0 && ( api_id == SW_CMD_VLAN_SHOW ||
                              api_id == SW_CMD_FDB_SHOW ||
                              api_id == SW_CMD_RESV_FDB_SHOW ||
                              api_id == SW_CMD_HOST_SHOW ||
                              api_id == SW_CMD_HOST_IPV4_SHOW ||
                              api_id == SW_CMD_HOST_IPV6_SHOW ||
                              api_id == SW_CMD_HOST_IPV4M_SHOW ||
                              api_id == SW_CMD_HOST_IPV6M_SHOW ||
                              api_id == SW_CMD_FLOW_IPV43T_SHOW ||
                              api_id == SW_CMD_FLOW_IPV63T_SHOW ||
                              api_id == SW_CMD_FLOW_IPV45T_SHOW ||
                              api_id == SW_CMD_FLOW_IPV65T_SHOW ||
                              api_id == SW_CMD_NAT_SHOW ||
                              api_id == SW_CMD_NAPT_SHOW ||
                              api_id == SW_CMD_FLOW_SHOW ||
                              api_id == SW_CMD_CTRLPKT_SHOW ||
                              api_id == SW_CMD_INTFMAC_SHOW ||
                              api_id == SW_CMD_PUBADDR_SHOW )) ||
		    ( arg_index == 1 && api_id == SW_CMD_SET_DEVID) ||
		    ( arg_index == 2 && api_id == SW_CMD_PT_VLAN_TRANS_ADV_SHOW) )
        return SW_OK;

    return SW_BAD_PARAM;
}

/*user command api*/
sw_error_t
cmd_exec_api(a_ulong_t *arg_val)
{
    sw_error_t rv;
    sw_api_t sw_api;

    sw_api.api_id = arg_val[0];
    SW_RTN_ON_ERROR(sw_api_get(&sw_api));

    /*save cmd return value */
    arg_val[1] = (a_ulong_t) ioctl_buf;
    /*save set device id */
    arg_val[2] = get_devid();

    rv = cmd_api_func(sw_api.api_fp, sw_api.api_nr, arg_val);
    SW_RTN_ON_ERROR(rv);

    rv = cmd_api_output(sw_api.api_pp, sw_api.api_nr, arg_val);
    SW_RTN_ON_ERROR(rv);

    return rv;
}


void
cmd_print_usage (int cmd_index, int cmd_index_sub)
{
    if(GCMD_NAME(cmd_index))
        dprintf("usage: %s", GCMD_NAME(cmd_index));

    if (GCMD_SUB_NAME(cmd_index, cmd_index_sub))
        dprintf(" %s", GCMD_SUB_NAME(cmd_index, cmd_index_sub));

    if(GCMD_SUB_ACT(cmd_index, cmd_index_sub) && GCMD_SUB_USAGE(cmd_index, cmd_index_sub))
        dprintf(" %s %s\n\n", GCMD_SUB_ACT(cmd_index, cmd_index_sub),
                GCMD_SUB_USAGE(cmd_index, cmd_index_sub));
}
/*
    main function
    input args:
        arg_val[0] = cmd_num
        arg_val[1] = rtn_code
        arg_val[2] = dev_id
        arg_val[3] = dbg_cmd_num or other
*/

/*command string lookup*/
a_uint32_t
cmd_lookup(char **cmd_str, int *cmd_index, int *cmd_index_sub)
{
    a_uint32_t no, sub_no;
    a_uint32_t cmd_deepth = 0;

    *cmd_index = GCMD_DESC_NO_MATCH;
    *cmd_index_sub = GCMD_DESC_NO_MATCH;

    if (cmd_str[0] == NULL)
        return cmd_deepth;

    for (no = 0; GCMD_DESC_VALID(no); no++)
    {
        if (strcasecmp(cmd_str[0], GCMD_NAME(no)))
            continue;

        for (sub_no = 0; GCMD_SUB_DESC_VALID(no, sub_no); sub_no++)
        {
            if (cmd_str[1] != NULL && cmd_str[2] != NULL)
            {

                if (GCMD_SUB_NAME(no, sub_no) && GCMD_SUB_ACT(no, sub_no)
                        && !strcasecmp(cmd_str[1], GCMD_SUB_NAME(no, sub_no))
                        && !strcasecmp(cmd_str[2], GCMD_SUB_ACT(no, sub_no)))
                {
                    *cmd_index = no;
                    *cmd_index_sub = sub_no;
                    cmd_deepth = 3;
                    return cmd_deepth;
                }

                else if (!GCMD_SUB_NAME(no, sub_no) && GCMD_SUB_ACT(no, sub_no)
                         && !strcasecmp(cmd_str[1], GCMD_SUB_ACT(no, sub_no)))
                {
                    *cmd_index = no;
                    *cmd_index_sub = sub_no;
                    cmd_deepth = 2;
                    return cmd_deepth;
                }
            }
            else if (cmd_str[1] != NULL && cmd_str[2] == NULL)
            {

                if (!GCMD_SUB_NAME(no, sub_no) && GCMD_SUB_ACT(no, sub_no)
                        && !strcasecmp(cmd_str[1], GCMD_SUB_ACT(no, sub_no)))
                {
                    *cmd_index = no;
                    *cmd_index_sub = sub_no;
                    cmd_deepth = 2;
                    return cmd_deepth;
                }
            }
        }
    }

    return cmd_deepth;
}

static a_ulong_t *
cmd_parse(char *cmd_str, int *cmd_index, int *cmd_index_sub)
{
    int cmd_nr = 0;
    a_ulong_t *arg_val = ioctl_argp;
    char *tmp_str[CMDSTR_ARGS_MAX], *str_save;

    if (cmd_str == NULL)
        return NULL;

    memset(arg_val, 0, CMDSTR_ARGS_MAX * sizeof (a_ulong_t));

    /* split string into array */
    if ((tmp_str[cmd_nr] = (void *) strtok_r(cmd_str, " ", &str_save)) == NULL)
        return NULL;

    /*handle help */
    if (!strcasecmp(tmp_str[cmd_nr], "help"))
    {
        dprintf("input ? get help\n\n");
        return NULL;
    }

    while (tmp_str[cmd_nr])
    {
        if (++cmd_nr == 3)
            break;
        tmp_str[cmd_nr] = (void *) strtok_r(NULL, " ", &str_save);
    }

    /*commond string lookup */
    int cmd_depth = cmd_lookup(tmp_str, cmd_index, cmd_index_sub);

    if (*cmd_index == GCMD_DESC_NO_MATCH || *cmd_index_sub == GCMD_DESC_NO_MATCH)
    {
        dprintf("invalid or incomplete command.\n\n");
        return NULL;
    }

    /*parse param */
    cmd_nr = 0;
    if (cmd_depth == 2)
    {
        tmp_str[cmd_nr] = tmp_str[2];
        cmd_nr++;
    }

    tmp_str[cmd_nr] = (void *) strtok_r(NULL, " ", &str_save);
    while (tmp_str[cmd_nr])
    {
        if (++cmd_nr == CMDSTR_ARGS_MAX)
            break;
        tmp_str[cmd_nr] = (void *) strtok_r(NULL, " ", &str_save);
    }

    arg_val[0] = GCMD_SUB_API(*cmd_index, *cmd_index_sub);
    arg_val[1] = (a_ulong_t) ioctl_buf;

    int rtn_code;
    if (arg_val[0] < SW_API_MAX)
    {
        /*api command parse */
        rtn_code = cmd_parse_api(tmp_str, arg_val);

    }
    else if (arg_val[0] > SW_API_MAX)
    {
        /*user command parse */
        rtn_code = cmd_parse_sw(tmp_str, arg_val);

    }
    else
    {
        rtn_code = SW_BAD_PARAM;
    }

    if(rtn_code != SW_OK)
    {
        cmd_print_error(rtn_code);

        if(rtn_code == SW_BAD_PARAM)
            cmd_print_usage(*cmd_index, *cmd_index_sub);

        return NULL;
    }

    return arg_val;
}

static int
cmd_exec(a_ulong_t *arg_val, int cmd_index, int cmd_index_sub)
{
    a_uint32_t api_id = arg_val[0];
    sw_error_t rtn = SW_OK;

    if( api_id < SW_API_MAX )
    {
        rtn = cmd_exec_api(arg_val);

    }
    else if ((api_id > SW_API_MAX ) && (api_id < SW_CMD_MAX))
    {
        if (GCMD_SUB_FUNC(cmd_index, cmd_index_sub))
            rtn = GCMD_SUB_FUNC(cmd_index, cmd_index_sub)(arg_val);
    }
    else
    {
        rtn = SW_BAD_PARAM;
    }

    if(rtn != SW_OK)
        cmd_print_error(rtn);
    else
        dprintf("\noperation done.\n\n");

    return 0;
}

static sw_error_t
cmd_socket_init(int dev_id)
{
    sw_error_t rv;

    init_cfg.cpu_mode = HSL_CPU_1;
    init_cfg.reg_mode = HSL_MDIO;
#if defined UK_MINOR_DEV
    init_cfg.nl_prot  = UK_MINOR_DEV;
#else
    init_cfg.nl_prot  = 30;
#endif
    init_cfg.chip_type=CHIP_UNSPECIFIED;
/*qca808x_end*/
    init_cfg.reg_func.mdio_set = NULL;
    init_cfg.reg_func.mdio_get = NULL;
/*qca808x_start*/
    rv = ssdk_init(dev_id, &init_cfg);
    if (SW_OK == rv)
    {
        dprintf("\n %s Init OK!", SSDK_STR);
    }
    else
    {
        dprintf("\n %s Init Fail! RV[%d]", SSDK_STR, rv);
    }

    if (flag == 0)
    {
        aos_mem_set(&ssdk_cfg, 0 ,sizeof(ssdk_cfg_t));
        rv = sw_uk_exec(SW_API_SSDK_CFG, dev_id, &ssdk_cfg);
        flag = 1;
    }
    return rv;
}

static sw_error_t
cmd_init(void)
{
    ioctl_buf = (a_ulong_t *) malloc(IOCTL_BUF_SIZE);
    ioctl_argp = (a_ulong_t *) malloc(CMDSTR_ARGS_MAX * sizeof (a_ulong_t));
    FILE *dev_id_fd = NULL;
    int dev_id_value = 0;
    if((dev_id_fd = fopen(dev_id_path, "r")) != NULL)
    {
        fscanf(dev_id_fd, "%d", &dev_id_value);
    }

    set_devid(dev_id_value);
    cmd_socket_init(dev_id_value);

    return SW_OK;
}

static sw_error_t
cmd_exit(void)
{
    free(ioctl_buf);
    free(ioctl_argp);
    ssdk_cleanup();
    flag = 0;
    return SW_OK;
}

static sw_error_t
cmd_run_one(char *cmd_str)
{
    a_ulong_t *arg_list;
    int cmd_index = 0, cmd_index_sub = 0;

    if ((arg_list = cmd_parse(cmd_str, &cmd_index, &cmd_index_sub)) != NULL)
    {
        cmd_exec(arg_list, cmd_index, cmd_index_sub);
    }

    return SW_OK;
}

int
cmd_is_batch(const char *cmd_str)
{
    char batch_cmd[] = "run";

    if(!strncmp(cmd_str, batch_cmd, strlen(batch_cmd)))
        return 1;
    return 0;
}

static void
cmd_batch_help(void)
{
    dprintf("usage:run <cmd_file> <result_file>\n");
}

static sw_error_t
cmd_run_batch (char *cmd_str)
{
    FILE *in_fd = NULL;
    char * line = NULL, *str_save;
    char *tmp_str[3];

    if (cmd_str == NULL)
        return SW_BAD_PARAM;

    /*usage: run cmd result*/
    if((tmp_str[0] = (void *) strtok_r(cmd_str, " ", &str_save)) == NULL)
        return SW_BAD_PARAM;

    /*check again*/
    if(!cmd_is_batch(tmp_str[0]))
        return SW_BAD_PARAM;

    if((tmp_str[1] = (void *) strtok_r(NULL, " ", &str_save))== NULL)
        return SW_BAD_PARAM;
    if((tmp_str[2] = (void *) strtok_r(NULL, " ", &str_save))== NULL)
        return SW_BAD_PARAM;

    if((in_fd = fopen(tmp_str[1], "r")) == NULL)
    {
        dprintf("can't open cmd file %s\n", tmp_str[1]);
        return SW_FAIL;
    }
    if((out_fd = fopen(tmp_str[2], "w+")) == NULL)
    {
        dprintf("can't open result file %s\n", tmp_str[2]);
        return SW_FAIL;
    }

    size_t len = 0;
    ssize_t read;

    set_talk_mode(0);
    while ((read = getline(&line, &len, in_fd)) != -1)
    {
        //dprintf("(%d)%s",read, line);
        if (read <= 1 )
	{
	    continue;
	}

	if (line[strlen(line)-1] == '\n')
	{
	    line[strlen(line)-1] = '\0';
	}

        if (!strncmp(line, "echo", 4))
        {
            dprintf("%s\n", line+strlen("echo "));
            continue;
        }
        else
        {
            dprintf("%s\n", line);
        }
        cmd_run_one(line);
    }
    set_talk_mode(1);

    if (line) free(line);

    fclose(out_fd);
    fclose(in_fd);
    out_fd = 0;
    in_fd =0;

    return SW_OK;

}

static sw_error_t
cmd_args(char *cmd_str, int argc, const char *argv[])
{
    /*quiet mode*/
    set_talk_mode(0);

    if(cmd_is_batch(argv[1]))
    {
        if(argc != 4)
        {
            cmd_batch_help();
            return SW_FAIL;
        }

        snprintf(cmd_str, CMDSTR_BUF_SIZE, "%s %s %s", argv[1], argv[2], argv[3]);
        cmd_run_batch(cmd_str);

    }
    else
    {
        int argi;
        for(argi = 1; argi < argc; argi++)
        {
            strlcat(cmd_str, argv[argi], CMDSTR_BUF_SIZE);
            strlcat(cmd_str, " ", CMDSTR_BUF_SIZE);
        }
        cmd_run_one(cmd_str);
    }

    return SW_OK;
}

int
cmd_is_exit(char *cmd_str)
{
    if ((!strcasecmp(cmd_str, "q")) || (!strcasecmp(cmd_str, "quit")))
    {
        return 1;
    }
    return 0;
}

void cmd_welcome()
{
    char *ver = "", *date = "";
#ifdef VERSION
    ver = VERSION;
#endif

#ifdef BUILD_DATE
    date = BUILD_DATE;
#endif
    dprintf("\n Welcome to %s Shell version: %s, at %s.\n", SSDK_STR, ver, date);
}

/* Dummy function to avoid linker complaints */
void __aeabi_unwind_cpp_pr0(void)
{
};
void __aeabi_unwind_cpp_pr1(void)
{
};

int
main(int argc, const char *argv[])
{
    char cmd_str[CMDSTR_BUF_SIZE];
    cmd_init();

    if(argc > 1)
    {
        memset(cmd_str, 0, sizeof(cmd_str));
        cmd_args(cmd_str, argc, argv);
        cmd_exit();
        return 0;
    }

    cmd_welcome();

    /*main loop*/
    while (1)
    {
        memset(cmd_str, 0, sizeof(cmd_str));

        if(next_cmd(cmd_str) == 0)/*loop through if '\n'*/
            continue;

        if (cmd_is_exit(cmd_str))
            break;

        if(cmd_is_batch(cmd_str))
        {
            if(cmd_run_batch(cmd_str)!= SW_OK)
                cmd_batch_help();
        }
        else
        {
            cmd_run_one(cmd_str);
        }
    }

    cmd_exit();
    return 0;
}
/*qca808x_end*/
