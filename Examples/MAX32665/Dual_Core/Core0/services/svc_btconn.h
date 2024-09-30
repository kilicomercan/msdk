/*************************************************************************************************/
/*!
 *  \file
 *
 *  \brief  Maxim Custom service server.
 *
 *  Copyright (c) 2012-2018 Arm Ltd. All Rights Reserved.
 *
 *  Copyright (c) 2022-2023 Analog Devices, Inc.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
/*************************************************************************************************/

#ifndef EXAMPLES_MAX32655_BLUETOOTH_BLE_MCS_SERVICES_SVC_MCS_H_
#define EXAMPLES_MAX32655_BLUETOOTH_BLE_MCS_SERVICES_SVC_MCS_H_

#include "wsf_types.h"
#include "att_api.h"
#include "util/bstream.h"
#include "svc_cfg.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! \addtogroup Mcs
 *  \{ */
/**************************************************************************************************
  Macros
**************************************************************************************************/
/*MCS service UUID*/
#define ATT_UUID_CUSTOM_SERVICE                                                                  \
    0xBE, 0xC5, 0xD1, 0x24, 0x99, 0x33, 0xC6, 0x87, 0x85, 0x41, 0xD9, 0x31, 0x7D, 0x56, 0xFC,0x85

/* MCS service GATT characteristic UUIDs*/
#define ATT_UUID_CUSTOM_I2C \
  0xBE, 0xC5, 0xD1, 0x24, 0x99, 0x33, 0xC6, 0x87, 0x85, 0x41, 0xD9, 0x31, 0x7E, 0x56, 0xFC, 0x86
#define ATT_UUID_CUSTOM_I2C_PERIOD \
    0xBE, 0xC5, 0xD1, 0x24, 0x99, 0x33, 0xC6, 0x87, 0x85, 0x41, 0xD9, 0x31, 0x7F, 0x56, 0xFC, 0x87

/**************************************************************************************************
 Handle Ranges
**************************************************************************************************/

/** \name Maxim custom Service Handles
 *
 */
/**@{*/
#define CUSTOM_SVC_START_HDL 0x1500 /*!< \brief Start handle. */
#define CUSTOM_SVC_END_HDL (CUSTOM_SVC_MAX_HDL - 1) /*!< \brief End handle. */

/**************************************************************************************************
 Handles
**************************************************************************************************/

/*! \brief Maxim custom Service Handles */
enum {
    CUSTOM_SVC_HDL = CUSTOM_SVC_START_HDL, /*!< \brief Maxim custom service declaration */
    CUSTOM_I2C_CH_HDL,
    CUSTOM_I2C_HDL,
    CUSTOM_I2C_CH_CCC_HDL,

    CUSTOM_I2C_PERIOD_CH_HDL,
    CUSTOM_I2C_PERIOD_HDL,
    
    CUSTOM_SVC_MAX_HDL /*!< \brief Maximum handle. */
};
/**@}*/

/**************************************************************************************************
  Function Declarations
**************************************************************************************************/

/*************************************************************************************************/
/*!
 *  \brief  Add the services to the attribute server.
 *
 *  \return None.
 */
/*************************************************************************************************/
void SvcMcsAddGroup(void);

/*************************************************************************************************/
/*!
 *  \brief  Remove the services from the attribute server.
 *
 *  \return None.
 */
/*************************************************************************************************/
void SvcMcsRemoveGroup(void);

/*************************************************************************************************/
/*!
 *  \brief  Register callbacks for the service.
 *
 *  \param  readCback   Read callback function.
 *  \param  writeCback  Write callback function.
 *
 *  \return None.
 */
/*************************************************************************************************/
void SvcMcsCbackRegister(attsReadCback_t readCback, attsWriteCback_t writeCback);

/*! \} */ /* TEST_SERVICE */

#ifdef __cplusplus
};
#endif

#endif // EXAMPLES_MAX32655_BLUETOOTH_BLE_MCS_SERVICES_SVC_MCS_H_
