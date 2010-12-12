#ifndef IFXMIPS_ATM_FW_REGS_COMMON_H
#define IFXMIPS_ATM_FW_REGS_COMMON_H



#if defined(CONFIG_DANUBE)
  #include "ifxmips_atm_fw_regs_danube.h"
#elif defined(CONFIG_AMAZON_SE)
  #include "ifxmips_atm_fw_regs_amazon_se.h"
#elif defined(CONFIG_AR9)
  #include "ifxmips_atm_fw_regs_ar9.h"
#elif defined(CONFIG_VR9)
  #include "ifxmips_atm_fw_regs_vr9.h"
#else
  #error Platform is not specified!
#endif



/*
 *  PPE ATM Cell Header
 */
#if defined(__BIG_ENDIAN)
    struct uni_cell_header {
        unsigned int        gfc     :4;
        unsigned int        vpi     :8;
        unsigned int        vci     :16;
        unsigned int        pti     :3;
        unsigned int        clp     :1;
    };
#else
    struct uni_cell_header {
        unsigned int        clp     :1;
        unsigned int        pti     :3;
        unsigned int        vci     :16;
        unsigned int        vpi     :8;
        unsigned int        gfc     :4;
    };
#endif  //  defined(__BIG_ENDIAN)

/*
 *  Inband Header and Trailer
 */
#if defined(__BIG_ENDIAN)
    struct rx_inband_trailer {
        /*  0 - 3h  */
        unsigned int        uu      :8;
        unsigned int        cpi     :8;
        unsigned int        stw_res1:4;
        unsigned int        stw_clp :1;
        unsigned int        stw_ec  :1;
        unsigned int        stw_uu  :1;
        unsigned int        stw_cpi :1;
        unsigned int        stw_ovz :1;
        unsigned int        stw_mfl :1;
        unsigned int        stw_usz :1;
        unsigned int        stw_crc :1;
        unsigned int        stw_il  :1;
        unsigned int        stw_ra  :1;
        unsigned int        stw_res2:2;
        /*  4 - 7h  */
        unsigned int        gfc     :4;
        unsigned int        vpi     :8;
        unsigned int        vci     :16;
        unsigned int        pti     :3;
        unsigned int        clp     :1;
    };

    struct tx_inband_header {
        /*  0 - 3h  */
        unsigned int        gfc     :4;
        unsigned int        vpi     :8;
        unsigned int        vci     :16;
        unsigned int        pti     :3;
        unsigned int        clp     :1;
        /*  4 - 7h  */
        unsigned int        uu      :8;
        unsigned int        cpi     :8;
        unsigned int        pad     :8;
        unsigned int        res1    :8;
    };
#else
    struct rx_inband_trailer {
        /*  0 - 3h  */
        unsigned int        stw_res2:2;
        unsigned int        stw_ra  :1;
        unsigned int        stw_il  :1;
        unsigned int        stw_crc :1;
        unsigned int        stw_usz :1;
        unsigned int        stw_mfl :1;
        unsigned int        stw_ovz :1;
        unsigned int        stw_cpi :1;
        unsigned int        stw_uu  :1;
        unsigned int        stw_ec  :1;
        unsigned int        stw_clp :1;
        unsigned int        stw_res1:4;
        unsigned int        cpi     :8;
        unsigned int        uu      :8;
        /*  4 - 7h  */
        unsigned int        clp     :1;
        unsigned int        pti     :3;
        unsigned int        vci     :16;
        unsigned int        vpi     :8;
        unsigned int        gfc     :4;
    };

    struct tx_inband_header {
        /*  0 - 3h  */
        unsigned int        clp     :1;
        unsigned int        pti     :3;
        unsigned int        vci     :16;
        unsigned int        vpi     :8;
        unsigned int        gfc     :4;
        /*  4 - 7h  */
        unsigned int        res1    :8;
        unsigned int        pad     :8;
        unsigned int        cpi     :8;
        unsigned int        uu      :8;
    };
#endif  //  defined(__BIG_ENDIAN)

/*
 *  MIB Table Maintained by Firmware
 */
struct wan_mib_table {
    u32                     res1;
    u32                     wrx_drophtu_cell;
    u32                     wrx_dropdes_pdu;
    u32                     wrx_correct_pdu;
    u32                     wrx_err_pdu;
    u32                     wrx_dropdes_cell;
    u32                     wrx_correct_cell;
    u32                     wrx_err_cell;
    u32                     wrx_total_byte;
    u32                     res2;
    u32                     wtx_total_pdu;
    u32                     wtx_total_cell;
    u32                     wtx_total_byte;
};

/*
 *  Host-PPE Communication Data Structure
 */

#if defined(__BIG_ENDIAN)
    struct wrx_queue_config {
        /*  0h  */
        unsigned int    res2        :27;
        unsigned int    dmach       :4;
        unsigned int    errdp       :1;
        /*  1h  */
        unsigned int    oversize    :16;
        unsigned int    undersize   :16;
        /*  2h  */
        unsigned int    res1        :16;
        unsigned int    mfs         :16;
        /*  3h  */
        unsigned int    uumask      :8;
        unsigned int    cpimask     :8;
        unsigned int    uuexp       :8;
        unsigned int    cpiexp      :8;
    };

    struct wtx_port_config {
        unsigned int    res1        :27;
        unsigned int    qid         :4;
        unsigned int    qsben       :1;
    };

    struct wtx_queue_config {
        unsigned int    res1        :25;
        unsigned int    sbid        :1;
        unsigned int    res2        :3;
        unsigned int    type        :2;
        unsigned int    qsben       :1;
    };

    struct wrx_dma_channel_config {
        /*  0h  */
        unsigned int    res1        :1;
        unsigned int    mode        :2;
        unsigned int    rlcfg       :1;
        unsigned int    desba       :28;
        /*  1h  */
        unsigned int    chrl        :16;
        unsigned int    clp1th      :16;
        /*  2h  */
        unsigned int    deslen      :16;
        unsigned int    vlddes      :16;
    };

    struct wtx_dma_channel_config {
        /*  0h  */
        unsigned int    res2        :1;
        unsigned int    mode        :2;
        unsigned int    res3        :1;
        unsigned int    desba       :28;
        /*  1h  */
        unsigned int    res1        :32;
        /*  2h  */
        unsigned int    deslen      :16;
        unsigned int    vlddes      :16;
    };

    struct htu_entry {
        unsigned int    res1        :1;
        unsigned int    clp         :1;
        unsigned int    pid         :2;
        unsigned int    vpi         :8;
        unsigned int    vci         :16;
        unsigned int    pti         :3;
        unsigned int    vld         :1;
    };

    struct htu_mask {
        unsigned int    set         :1;
        unsigned int    clp         :1;
        unsigned int    pid_mask    :2;
        unsigned int    vpi_mask    :8;
        unsigned int    vci_mask    :16;
        unsigned int    pti_mask    :3;
        unsigned int    clear       :1;
    };

   struct htu_result {
        unsigned int    res1        :12;
        unsigned int    cellid      :4;
        unsigned int    res2        :5;
        unsigned int    type        :1;
        unsigned int    ven         :1;
        unsigned int    res3        :5;
        unsigned int    qid         :4;
    };

    struct rx_descriptor {
        /*  0 - 3h  */
        unsigned int    own         :1;
        unsigned int    c           :1;
        unsigned int    sop         :1;
        unsigned int    eop         :1;
        unsigned int    res1        :3;
        unsigned int    byteoff     :2;
        unsigned int    res2        :2;
        unsigned int    id          :4;
        unsigned int    err         :1;
        unsigned int    datalen     :16;
        /*  4 - 7h  */
        unsigned int    res3        :4;
        unsigned int    dataptr     :28;
    };

    struct tx_descriptor {
        /*  0 - 3h  */
        unsigned int    own         :1;
        unsigned int    c           :1;
        unsigned int    sop         :1;
        unsigned int    eop         :1;
        unsigned int    byteoff     :5;
        unsigned int    res1        :5;
        unsigned int    iscell      :1;
        unsigned int    clp         :1;
        unsigned int    datalen     :16;
        /*  4 - 7h  */
        unsigned int    res2        :4;
        unsigned int    dataptr     :28;
    };
#else
    struct wrx_queue_config {
        /*  0h  */
        unsigned int    errdp       :1;
        unsigned int    dmach       :4;
        unsigned int    res2        :27;
        /*  1h  */
        unsigned int    undersize   :16;
        unsigned int    oversize    :16;
        /*  2h  */
        unsigned int    mfs         :16;
        unsigned int    res1        :16;
        /*  3h  */
        unsigned int    cpiexp      :8;
        unsigned int    uuexp       :8;
        unsigned int    cpimask     :8;
        unsigned int    uumask      :8;
    };

    struct wtx_port_config {
        unsigned int    qsben       :1;
        unsigned int    qid         :4;
        unsigned int    res1        :27;
    };

    struct wtx_queue_config {
        unsigned int    qsben       :1;
        unsigned int    type        :2;
        unsigned int    res2        :3;
        unsigned int    sbid        :1;
        unsigned int    res1        :25;
    };

    struct wrx_dma_channel_config
    {
        /*  0h  */
        unsigned int    desba       :28;
        unsigned int    rlcfg       :1;
        unsigned int    mode        :2;
        unsigned int    res1        :1;
        /*  1h  */
        unsigned int    clp1th      :16;
        unsigned int    chrl        :16;
        /*  2h  */
        unsigned int    vlddes      :16;
        unsigned int    deslen      :16;
    };

    struct wtx_dma_channel_config {
        /*  0h  */
        unsigned int    desba       :28;
        unsigned int    res3        :1;
        unsigned int    mode        :2;
        unsigned int    res2        :1;
        /*  1h  */
        unsigned int    res1        :32;
        /*  2h  */
        unsigned int    vlddes      :16;
        unsigned int    deslen      :16;
    };

    struct rx_descriptor {
        /*  4 - 7h  */
        unsigned int    dataptr     :28;
        unsigned int    res3        :4;
        /*  0 - 3h  */
        unsigned int    datalen     :16;
        unsigned int    err         :1;
        unsigned int    id          :4;
        unsigned int    res2        :2;
        unsigned int    byteoff     :2;
        unsigned int    res1        :3;
        unsigned int    eop         :1;
        unsigned int    sop         :1;
        unsigned int    c           :1;
        unsigned int    own         :1;
    };

    struct tx_descriptor {
        /*  4 - 7h  */
        unsigned int    dataptr     :28;
        unsigned int    res2        :4;
        /*  0 - 3h  */
        unsigned int    datalen     :16;
        unsigned int    clp         :1;
        unsigned int    iscell      :1;
        unsigned int    res1        :5;
        unsigned int    byteoff     :5;
        unsigned int    eop         :1;
        unsigned int    sop         :1;
        unsigned int    c           :1;
        unsigned int    own         :1;
    };
#endif  //  defined(__BIG_ENDIAN)



#endif  //  IFXMIPS_ATM_FW_REGS_COMMON_H
