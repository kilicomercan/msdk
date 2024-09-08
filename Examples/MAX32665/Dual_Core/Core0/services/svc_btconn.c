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

#include "svc_btconn.h"

/**************************************************************************************************
  Macros
**************************************************************************************************/

/*! Characteristic read permissions */
#ifndef MCS_SEC_PERMIT_READ
#define MCS_SEC_PERMIT_READ SVC_SEC_PERMIT_READ
#endif

/*! Characteristic write permissions */
#ifndef MCS_SEC_PERMIT_WRITE
#define MCS_SEC_PERMIT_WRITE SVC_SEC_PERMIT_WRITE
#endif

/**************************************************************************************************
 Service variables
**************************************************************************************************/

/*Service variables declaration*/
const uint8_t attMcsSvcUuid[ATT_128_UUID_LEN] = {ATT_UUID_CUSTOM_SERVICE};

/*Characteristic variables declaration*/
const uint8_t svcCustomI2CUuid[ATT_128_UUID_LEN] = {ATT_UUID_CUSTOM_I2C};
const uint8_t svcCustomI2CPeriodUuid[ATT_128_UUID_LEN] = {ATT_UUID_CUSTOM_I2C_PERIOD};
const uint8_t svcCustomSPIUuid[ATT_128_UUID_LEN] = {ATT_UUID_CUSTOM_SPI};
const uint8_t svcCustomSPIPeriodUuid[ATT_128_UUID_LEN] = {ATT_UUID_CUSTOM_SPI_PERIOD};

static const uint8_t customSvc[] = {ATT_UUID_CUSTOM_SERVICE};
static const uint16_t customLenSvc = sizeof(customSvc);

static const uint8_t customI2CCh[] = {ATT_PROP_READ | ATT_PROP_NOTIFY,
                                      UINT16_TO_BYTES(CUSTOM_I2C_HDL), ATT_UUID_CUSTOM_I2C};
static const uint16_t customI2CLenCh = sizeof(customI2CCh);

static const uint8_t customI2CPeriodCh[] = {ATT_PROP_WRITE | ATT_PROP_READ,
                                            UINT16_TO_BYTES(CUSTOM_I2C_PERIOD_HDL), ATT_UUID_CUSTOM_I2C_PERIOD};
static const uint16_t customI2CPeriodLenCh = sizeof(customI2CPeriodCh);

static const uint8_t customSPICh[] = {ATT_PROP_READ | ATT_PROP_NOTIFY,
                                      UINT16_TO_BYTES(CUSTOM_SPI_HDL), ATT_UUID_CUSTOM_SPI};
static const uint16_t customSPILenCh = sizeof(customSPICh);

static const uint8_t customSPIPeriodCh[] = {ATT_PROP_WRITE | ATT_PROP_READ,
                                            UINT16_TO_BYTES(CUSTOM_SPI_PERIOD_HDL), ATT_UUID_CUSTOM_SPI_PERIOD};
static const uint16_t customSPIPeriodLenCh = sizeof(customSPIPeriodCh);

/*Characteristic values declaration*/
static uint8_t customI2CVal[] = {0};
static const uint16_t customI2CValLen = sizeof(customI2CVal);

static uint8_t customI2CChCcc[] = {UINT16_TO_BYTES(0x0000)};
static const uint16_t customI2CChLenCcc = sizeof(customI2CChCcc);

static uint8_t customI2CPeriodVal[] = {UINT16_TO_BYTES(500)};
static const uint16_t customI2CPeriodValLen = sizeof(customI2CPeriodVal);

static uint8_t customSPIVal[] = {0};
static const uint16_t customSPIValLen = sizeof(customSPIVal);

static uint8_t customSPIChCcc[] = {UINT16_TO_BYTES(0x0000)};
static const uint16_t customSPIChLenCcc = sizeof(customSPIChCcc);

static uint8_t customSPIPeriodVal[] = {UINT16_TO_BYTES(500)};
static const uint16_t customSPIPeriodValLen = sizeof(customSPIPeriodVal);

/**************************************************************************************************
 Custom Service group
**************************************************************************************************/

/* Attribute list for mcs group */
static const attsAttr_t customList[] = {
    /*-----------------------------*/
    /* Service declaration */
    {attPrimSvcUuid, (uint8_t *)customSvc, (uint16_t *)&customLenSvc, sizeof(customSvc), 0,
     MCS_SEC_PERMIT_READ},

    /*----------------------------*/
    /* I2C characteristic declaration */
    {attChUuid, (uint8_t *)customI2CCh, (uint16_t *)&customI2CLenCh, sizeof(customI2CCh), 0,
     MCS_SEC_PERMIT_READ},
    /* I2C characteristic value */
    {svcCustomI2CUuid, (uint8_t *)customI2CVal, (uint16_t *)&customI2CValLen, sizeof(customI2CVal),
     0, MCS_SEC_PERMIT_READ},
    /*I2C characteristic value descriptor*/
    {attCliChCfgUuid, (uint8_t *)customI2CChCcc, (uint16_t *)&customI2CChLenCcc,
     sizeof(customI2CChCcc), ATTS_SET_CCC, (ATTS_PERMIT_READ | SVC_SEC_PERMIT_WRITE)},

    /*-----------------------------*/
    /* I2C Period characteristic declaration */
    {attChUuid, (uint8_t *)customI2CPeriodCh, (uint16_t *)&customI2CPeriodLenCh, sizeof(customI2CPeriodCh),
     ATTS_SET_WRITE_CBACK, (MCS_SEC_PERMIT_READ | MCS_SEC_PERMIT_WRITE)},
    /* I2C Period characteristic characteristic value */
    {svcCustomI2CPeriodUuid, (uint8_t *)customI2CPeriodVal, (uint16_t *)&customI2CPeriodValLen, sizeof(customI2CPeriodVal),
     ATTS_SET_WRITE_CBACK, (MCS_SEC_PERMIT_READ | MCS_SEC_PERMIT_WRITE)},

    /*----------------------------*/
    /* SPI characteristic declaration */
    {attChUuid, (uint8_t *)customSPICh, (uint16_t *)&customSPILenCh, sizeof(customSPICh), 0,
     MCS_SEC_PERMIT_READ},
    /* SPI characteristic value */
    {svcCustomSPIUuid, (uint8_t *)customSPIVal, (uint16_t *)&customSPIValLen, sizeof(customSPIVal),
     0, MCS_SEC_PERMIT_READ},
    /*SPI characteristic value descriptor*/
    {attCliChCfgUuid, (uint8_t *)customSPIChCcc, (uint16_t *)&customSPIChLenCcc,
     sizeof(customSPIChCcc), ATTS_SET_CCC, (ATTS_PERMIT_READ | SVC_SEC_PERMIT_WRITE)},

    /*-----------------------------*/
    /* SPI Period characteristic declaration */
    {attChUuid, (uint8_t *)customSPIPeriodCh, (uint16_t *)&customSPIPeriodLenCh, sizeof(customSPIPeriodCh),
     ATTS_SET_WRITE_CBACK, (MCS_SEC_PERMIT_READ | MCS_SEC_PERMIT_WRITE)},
    /* SPI Period characteristic characteristic value */
    {svcCustomSPIPeriodUuid, (uint8_t *)customSPIPeriodVal, (uint16_t *)&customSPIPeriodValLen, sizeof(customSPIPeriodVal),
     ATTS_SET_WRITE_CBACK, (MCS_SEC_PERMIT_READ | MCS_SEC_PERMIT_WRITE)}};

/* Test group structure */
static attsGroup_t svcCustomGroup = {NULL, (attsAttr_t *)customList, NULL,
                                     NULL, CUSTOM_SVC_START_HDL, CUSTOM_SVC_END_HDL};

/*************************************************************************************************/
/*!
 *  \brief  Add the services to the attribute server.
 *
 *  \return None.
 */
/*************************************************************************************************/
void SvcMcsAddGroup(void)
{
  AttsAddGroup(&svcCustomGroup);
}

/*************************************************************************************************/
/*!
 *  \brief  Remove the services from the attribute server.
 *
 *  \return None.
 */
/*************************************************************************************************/
void SvcMcsRemoveGroup(void)
{
  AttsRemoveGroup(CUSTOM_SVC_START_HDL);
}

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
void SvcMcsCbackRegister(attsReadCback_t readCback, attsWriteCback_t writeCback)
{
  svcCustomGroup.readCback = readCback;
  svcCustomGroup.writeCback = writeCback;
}
