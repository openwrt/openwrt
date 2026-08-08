/*
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * Copyright (c) 2025 Realtek Semiconductor Corp. All rights reserved.
 *
 * This software is a confidential and proprietary property of Realtek
 * Semiconductor Corp. Disclosure, reproduction, redistribution, in
 * whole or in part, of this work and its derivatives without express
 * permission is prohibited.
 *
 * Realtek Semiconductor Corp. reserves the right to update, modify, or
 * discontinue this software at any time without notice. This software is
 * provided as is and any express or implied warranties, including, but
 * not limited to, the implied warranties of merchantability and fitness for
 * a particular purpose are disclaimed. In no event shall Realtek
 * Semiconductor Corp. be liable for any direct, indirect, incidental,
 * special, exemplary, or consequential damages (including, but not limited
 * to, procurement of substitute goods or services; loss of use, data, or
 * profits; or business interruption) however caused and on any theory of
 * liability, whether in contract, strict liability, or tort (including
 * negligence or otherwise) arising in any way out of the use of this software,
 * even if advised of the possibility of such damage.
 */

#ifndef __RTKSW_API_MIRROR_H__
#define __RTKSW_API_MIRROR_H__

typedef enum rtksw_mirror_keep_e
{
    MIRROR_FOLLOW_VLAN = 0,
    MIRROR_KEEP_ORIGINAL,
    MIRROR_KEEP_END
}rtksw_mirror_keep_t;


/* Function Name:
 *      rtksw_mirror_portBased_set
 * Description:
 *      Set port mirror function.
 * Input:
 *      unit                    - Unit ID
 *      mirroring_port          - Monitor port.
 *      pMirrored_rx_portmask   - Rx mirror port mask.
 *      pMirrored_tx_portmask   - Tx mirror port mask.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number
 *      RT_ERR_PORT_MASK    - Invalid portmask.
 * Note:
 *      The API is to set mirror function of source port and mirror port.
 *      The mirror port can only be set to one port and the TX and RX mirror ports
 *      should be identical.
 */
extern rtksw_api_ret_t rtksw_mirror_portBased_set(rtksw_uint32 unit, rtksw_port_t mirroring_port, rtksw_portmask_t *pMirrored_rx_portmask, rtksw_portmask_t *pMirrored_tx_portmask);

/* Function Name:
 *      rtksw_mirror_portBased_get
 * Description:
 *      Get port mirror function.
 * Input:
 *      unit                    - Unit ID
 * Output:
 *      pMirroring_port         - Monitor port.
 *      pMirrored_rx_portmask   - Rx mirror port mask.
 *      pMirrored_tx_portmask   - Tx mirror port mask.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      The API is to get mirror function of source port and mirror port.
 */
extern rtksw_api_ret_t rtksw_mirror_portBased_get(rtksw_uint32 unit, rtksw_port_t *pMirroring_port, rtksw_portmask_t *pMirrored_rx_portmask, rtksw_portmask_t *pMirrored_tx_portmask);

/* Function Name:
 *      rtksw_mirror_portIso_set
 * Description:
 *      Set mirror port isolation.
 * Input:
 *      unit        - Unit ID
 *      enable      - Mirror isolation status.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_ENABLE       - Invalid enable input
 * Note:
 *      The API is to set mirror isolation function that prevent normal forwarding packets to miror port.
 */
extern rtksw_api_ret_t rtksw_mirror_portIso_set(rtksw_uint32 unit, rtksw_enable_t enable);

/* Function Name:
 *      rtksw_mirror_portIso_get
 * Description:
 *      Get mirror port isolation.
 * Input:
 *      None
 * Output:
 *      unit        - Unit ID
 *      pEnable     - Mirror isolation status.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      The API is to get mirror isolation status.
 */
extern rtksw_api_ret_t rtksw_mirror_portIso_get(rtksw_uint32 unit, rtksw_enable_t *pEnable);

/* Function Name:
 *      rtksw_mirror_vlanLeaky_set
 * Description:
 *      Set mirror VLAN leaky.
 * Input:
 *      unit        - Unit ID
 *      txenable    - TX leaky enable.
 *      rxenable    - RX leaky enable.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_ENABLE       - Invalid enable input
 * Note:
 *      The API is to set mirror VLAN leaky function forwarding packets to miror port.
 */
extern rtksw_api_ret_t rtksw_mirror_vlanLeaky_set(rtksw_uint32 unit, rtksw_enable_t txenable, rtksw_enable_t rxenable);

/* Function Name:
 *      rtksw_mirror_vlanLeaky_get
 * Description:
 *      Get mirror VLAN leaky.
 * Input:
 *      unit        - Unit ID
 * Output:
 *      pTxenable   - TX leaky enable.
 *      pRxenable   - RX leaky enable.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      The API is to get mirror VLAN leaky status.
 */
extern rtksw_api_ret_t rtksw_mirror_vlanLeaky_get(rtksw_uint32 unit, rtksw_enable_t *pTxenable, rtksw_enable_t *pRxenable);

/* Function Name:
 *      rtksw_mirror_isolationLeaky_set
 * Description:
 *      Set mirror Isolation leaky.
 * Input:
 *      unit        - Unit ID
 *      txenable    - TX leaky enable.
 *      rxenable    - RX leaky enable.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_ENABLE       - Invalid enable input
 * Note:
 *      The API is to set mirror VLAN leaky function forwarding packets to miror port.
 */
extern rtksw_api_ret_t rtksw_mirror_isolationLeaky_set(rtksw_uint32 unit, rtksw_enable_t txenable, rtksw_enable_t rxenable);

/* Function Name:
 *      rtksw_mirror_isolationLeaky_get
 * Description:
 *      Get mirror isolation leaky.
 * Input:
 *      unit        - Unit ID
 * Output:
 *      pTxenable   - TX leaky enable.
 *      pRxenable   - RX leaky enable.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      The API is to get mirror isolation leaky status.
 */
extern rtksw_api_ret_t rtksw_mirror_isolationLeaky_get(rtksw_uint32 unit, rtksw_enable_t *pTxenable, rtksw_enable_t *pRxenable);

/* Function Name:
 *      rtksw_mirror_keep_set
 * Description:
 *      Set mirror packet format keep.
 * Input:
 *      unit        - Unit ID
 *      mode        - mirror keep mode.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_ENABLE       - Invalid enable input
 * Note:
 *      The API is to set  -mirror keep mode.
 *      The mirror keep mode is as following:
 *      - MIRROR_FOLLOW_VLAN
 *      - MIRROR_KEEP_ORIGINAL
 *      - MIRROR_KEEP_END
 */
extern rtksw_api_ret_t rtksw_mirror_keep_set(rtksw_uint32 unit, rtksw_mirror_keep_t mode);

/* Function Name:
 *      rtksw_mirror_keep_get
 * Description:
 *      Get mirror packet format keep.
 * Input:
 *      unit        - Unit ID
 * Output:
 *      pMode       - mirror keep mode.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      The API is to get mirror keep mode.
  *      The mirror keep mode is as following:
 *      - MIRROR_FOLLOW_VLAN
 *      - MIRROR_KEEP_ORIGINAL
 *      - MIRROR_KEEP_END
 */
extern rtksw_api_ret_t rtksw_mirror_keep_get(rtksw_uint32 unit, rtksw_mirror_keep_t *pMode);

/* Function Name:
 *      rtksw_mirror_override_set
 * Description:
 *      Set port mirror override function.
 * Input:
 *      unit            - Unit ID
 *      rxMirror        - 1: output mirrored packet, 0: output normal forward packet
 *      txMirror        - 1: output mirrored packet, 0: output normal forward packet
 *      aclMirror       - 1: output mirrored packet, 0: output normal forward packet
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 * Note:
 *      The API is to set mirror override function.
 *      This function control the output format when a port output
 *      normal forward & mirrored packet at the same time.
 */
extern rtksw_api_ret_t rtksw_mirror_override_set(rtksw_uint32 unit, rtksw_enable_t rxMirror, rtksw_enable_t txMirror, rtksw_enable_t aclMirror);

/* Function Name:
 *      rtksw_mirror_override_get
 * Description:
 *      Get port mirror override function.
 * Input:
 *      unit            - Unit ID
 * Output:
 *      pRxMirror       - 1: output mirrored packet, 0: output normal forward packet
 *      pTxMirror       - 1: output mirrored packet, 0: output normal forward packet
 *      pAclMirror      - 1: output mirrored packet, 0: output normal forward packet
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_NULL_POINTER - Null Pointer
 * Note:
 *      The API is to Get mirror override function.
 *      This function control the output format when a port output
 *      normal forward & mirrored packet at the same time.
 */
extern rtksw_api_ret_t rtksw_mirror_override_get(rtksw_uint32 unit, rtksw_enable_t *pRxMirror, rtksw_enable_t *pTxMirror, rtksw_enable_t *pAclMirror);

/* Function Name:
 *      rtksw_mirror_sampling_set
 * Description:
 *      Set port mirror sampling function.
 * Input:
 *      unit            - Unit ID
 *      state           - Enable/Disable mirror sampling
 *      sampleNumber    - TX one mirrored packet after sampleNumber packets
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 * Note:
 *      The API is used to set mirror sampling function.
 */
extern rtksw_api_ret_t rtksw_mirror_sampling_set(rtksw_uint32 unit, rtksw_enable_t state, rtksw_uint32 sampleNumber);

/* Function Name:
 *      rtksw_mirror_sampling_get
 * Description:
 *      get port mirror sampling function.
 * Input:
 *      unit            - Unit ID
 * Output:
 *      pState          - Enable/Disable mirror sampling
 *      pSampleNumber   - TX one mirrored packet after sampleNumber packets
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 * Note:
 *      The API is used to get mirror sampling function.
 */
extern rtksw_api_ret_t rtksw_mirror_sampling_get(rtksw_uint32 unit, rtksw_enable_t *pState, rtksw_uint32 *pSampleNumber);

/* Function Name:
 *      rtksw_mirror_samplingCounter_get
 * Description:
 *      get port mirror sampling counter function.
 * Input:
 *      unit            - Unit ID
 * Output:
 *      pState          - Enable/Disable mirror sampling
 *      pSampleNumber   - TX one mirrored packet after sampleNumber packets
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 * Note:
 *      The API is used to get mirror sampling counter function.
 */
extern rtksw_api_ret_t rtksw_mirror_samplingCounter_get(rtksw_uint32 unit, rtksw_uint32 *pMirroredCounter, rtksw_uint32 *pSampleCounter);

#if defined(CONFIG_COMPATIBLE_V1_4)

#define rtk_mirror_portBased_set(mirroring_port, pMirrored_rx_portmask, pMirrored_tx_portmask)      rtksw_mirror_portBased_set(0, mirroring_port, pMirrored_rx_portmask, pMirrored_tx_portmask)
#define rtk_mirror_portBased_get(pMirroring_port, pMirrored_rx_portmask, pMirrored_tx_portmask)     rtksw_mirror_portBased_get(0, pMirroring_port, pMirrored_rx_portmask, pMirrored_tx_portmask)
#define rtk_mirror_portIso_set(enable)                                                              rtksw_mirror_portIso_set(0, enable)
#define rtk_mirror_portIso_get(pEnable)                                                             rtksw_mirror_portIso_get(0, pEnable)
#define rtk_mirror_vlanLeaky_set(txenable, rxenable)                                                rtksw_mirror_vlanLeaky_set(0, txenable, rxenable)
#define rtk_mirror_vlanLeaky_get(pTxenable, pRxenable)                                              rtksw_mirror_vlanLeaky_get(0, pTxenable, pRxenable)
#define rtk_mirror_isolationLeaky_set(txenable, rxenable)                                           rtksw_mirror_isolationLeaky_set(0, txenable, rxenable)
#define rtk_mirror_isolationLeaky_get(pTxenable, pRxenable)                                         rtksw_mirror_isolationLeaky_get(0, pTxenable, pRxenable)
#define rtk_mirror_keep_set(mode)                                                                   rtksw_mirror_keep_set(0, mode)
#define rtk_mirror_keep_get(pMode)                                                                  rtksw_mirror_keep_get(0, pMode)
#define rtk_mirror_override_set(rxMirror, txMirror, aclMirror)                                      rtksw_mirror_override_set(0, rxMirror, txMirror, aclMirror)
#define rtk_mirror_override_get(pRxMirror, pTxMirror, pAclMirror)                                   rtksw_mirror_override_get(0, pRxMirror, pTxMirror, pAclMirror)
#define rtk_mirror_sampling_set(state, sampleNumber)                                                rtksw_mirror_sampling_set(0, state, sampleNumber)
#define rtk_mirror_sampling_get(pState, pSampleNumber)                                              rtksw_mirror_sampling_get(0, pState, pSampleNumber)
#define rtk_mirror_samplingCounter_get(pMirroredCounter, pSampleCounter)                            rtksw_mirror_samplingCounter_get(0, pMirroredCounter, pSampleCounter)

#define rtk_mirror_keep_t       rtksw_mirror_keep_t

#endif /* CONFIG_COMPATIBLE_V1_4 */

#endif /* __RTKSW_API_MIRROR_H__ */

