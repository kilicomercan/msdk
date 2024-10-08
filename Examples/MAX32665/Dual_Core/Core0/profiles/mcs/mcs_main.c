/*************************************************************************************************/
/*!
 *  \file
 *
 *  \brief  Maxim Custom service server.
 *
 *  Copyright (c) 2012-2019 Arm Ltd. All Rights Reserved.
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

#include <stdbool.h>
#include "mcs_api.h"
#include "app_ui.h"
#include "pal_led.h"
#include "shared_data.h"

/**************************************************************************************************
  Macros
**************************************************************************************************/
#define DEAULT_TIMER_PERIOD (((1000/SHARED_SENSOR_ODR) * SENSOR_IND_PACK_COUNT)+1)
/**************************************************************************************************
  Local Variables
**************************************************************************************************/

/*! \brief Connection control block */
typedef struct
{
    dmConnId_t connId;       /*! \brief Connection ID */
    bool_t mcsToSend;        /*! \brief mcs measurement ready to be sent on this channel */
    uint8_t sentMcsBtnState; /*! \brief value of last sent mcs button state */
} mcsConn_t;

/*! \brief Control block */
static struct
{
    mcsConn_t conn[DM_CONN_MAX]; /*! \brief connection control block */
    wsfTimer_t I2CTimer;         /*! \brief I2C periodic check timer */
    mcsCfg_t cfg;                /*! \brief configurable parameters */
    uint16_t currCount;          /*! \brief current measurement period count */
    bool_t txReady;              /*! \brief TRUE if ready to send notifications */
    uint8_t btnState;            /*! \brief value of last button state */
    uint16_t I2CPeriod;
} mcsCb = {.I2CPeriod = DEAULT_TIMER_PERIOD};

/*************************************************************************************************/
/*!
 *  \brief  Return TRUE if no connections with active measurements.
 *
 *  \return TRUE if no connections active.
 */
/*************************************************************************************************/
static bool_t mcsNoConnActive(void)
{
    mcsConn_t *pConn = mcsCb.conn;
    uint8_t i;

    for (i = 0; i < DM_CONN_MAX; i++, pConn++)
    {
        if (pConn->connId != DM_CONN_ID_NONE)
        {
            return FALSE;
        }
    }
    return TRUE;
}

/*************************************************************************************************/
/*!
 *  \brief  ATTS write callback for maxim custom service.  Use this function as a parameter
 *          to SvcMcsCbackRegister().
 *
 *  \return ATT status.
 */
/*************************************************************************************************/
uint8_t McsWriteCback(dmConnId_t connId, uint16_t handle, uint8_t operation, uint16_t offset,
                      uint16_t len, uint8_t *pValue, attsAttr_t *pAttr)
{
    AttsSetAttr(handle, sizeof(*pValue), (uint8_t *)pValue);
    uint16_t period = 0;

    /* Get LED ID */
    switch (handle)
    {
    case CUSTOM_I2C_PERIOD_HDL:
        // Change I2C period = 0;
        BYTES_BE_TO_UINT16(period, pValue);
        mcsCb.I2CPeriod = period;
        break;
    }

    return ATT_SUCCESS;
}

/*************************************************************************************************/

/*!
 *  \brief  Setting characteristic value and send the button value to the peer device
 *
 *  \return None.
 */
/*************************************************************************************************/
void McsSetFeatures(uint8_t features)
{
    AttsSetAttr(CUSTOM_I2C_HDL, sizeof(features),
                (uint8_t *)&features); /*Setting mcsButtonVal characteristic value */

    dmConnId_t connId = AppConnIsOpen(); /*Getting connected */
    if (connId != DM_CONN_ID_NONE)
    {
        AttsHandleValueNtf(connId, CUSTOM_I2C_HDL, sizeof(features),
                           (uint8_t *)&features); /*Send notify */
                           
    }
}

/*************************************************************************************************/
/*!
 *  \brief  Initialize the mcs server.
 *
 *  \param  handerId    WSF handler ID of the application using this service.
 *  \param  pCfg        mcs configurable parameters.
 *
 *  \return None.
 */
/*************************************************************************************************/
void McsInit(wsfHandlerId_t handlerId, mcsCfg_t *pCfg)
{
    mcsCb.I2CTimer.handlerId = handlerId;
    mcsCb.cfg = *pCfg;

    /* De-init the PAL LEDs so we can control them here */
    PalLedDeInit();
}

/*************************************************************************************************/
/*!
 *  \brief  Start periodic mcs button state check.  This function starts a timer to perform
 *          periodic button checks.
 *
 *  \param  connId      DM connection identifier.
 *  \param  timerEvt    WSF event designated by the application for the timer.
 *  \param  mcsCccIdx   Index of mcs button state CCC descriptor in CCC descriptor handle table.
 *
 *  \return None.
 */
/*************************************************************************************************/
void McsI2CCheckStart(dmConnId_t connId, uint8_t timerEvt, uint8_t mcsCccIdx)
{
    /* initialize control block */
    mcsCb.I2CTimer.msg.event = timerEvt;
    mcsCb.I2CTimer.msg.status = mcsCccIdx;
    mcsCb.currCount = mcsCb.cfg.count;

    /* start timer */
    WsfTimerStartMs(&mcsCb.I2CTimer, 500);

    /* set conn id and last sent button level */
    mcsCb.conn[connId - 1].connId = connId;
}

/*************************************************************************************************/
/*!
 *  \brief  Stop periodic button state check.
 *
 *  \param  connId      DM connection identifier.
 *
 *  \return None.
 */
/*************************************************************************************************/
void McsI2CCheckStop(dmConnId_t connId)
{
    /* stop timer */
    WsfTimerStop(&mcsCb.I2CTimer);
    /* if no remaining connections */
    if (mcsNoConnActive())
    {
        /* clear connection */
        mcsCb.conn[connId - 1].connId = DM_CONN_ID_NONE;
        mcsCb.conn[connId - 1].mcsToSend = FALSE;
    }
}

/*************************************************************************************************/
/*!
 *  \brief  Stop periodic button state check.
 *
 *  \param  connId      DM connection identifier.
 *
 *  \return None.
 */

/*************************************************************************************************/
/*!
 *  \brief  Find next connection with measurement to send.
 *
 *  \param  cccIdx  measurement CCC descriptor index.
 *
 *  \return Connection control block.
 */
/*************************************************************************************************/
static mcsConn_t *cgmpsFindNextToSend(uint8_t cccIdx)
{
    mcsConn_t *pConn = mcsCb.conn;
    uint8_t i;

    for (i = 0; i < DM_CONN_MAX; i++, pConn++)
    {
        if (pConn->connId != DM_CONN_ID_NONE)
        {
            if (AttsCccEnabled(pConn->connId, cccIdx))
            {
                return pConn;
            }
        }
    }
    return NULL;
}

// static uint16_t data_prt[3];
static int counter_test = 0;
static uint8_t data_set_core1[SENSOR_DATA_TRANSFER] = {0};
void McsI2CTimerExpired(wsfMsgHdr_t *pMsg)
{
    mcsConn_t *pConn;
    memset(data_set_core1, 0, SENSOR_DATA_TRANSFER);
    while(MXC_SEMA_GetSema(PACK_READY_SEM_ID));
    if(ready_flag){
        memcpy(data_set_core1, &sensor_pack_buffer[last_send_pack_idx*SENSOR_DATA_TRANSFER], SENSOR_DATA_TRANSFER);
        memset(&sensor_pack_buffer[last_send_pack_idx*SENSOR_DATA_TRANSFER], 0, SENSOR_DATA_TRANSFER);
        ready_flag -= SENSOR_IND_PACK_COUNT;
        MXC_SEMA_FreeSema(PACK_READY_SEM_ID);

        // Sensor data transfer size is defined according to 
        last_send_pack_idx += 1;
        // We are sending sets 2 by 2. So modulate it with odr/set_count_in_pack
        last_send_pack_idx %= (SHARED_SENSOR_ODR/SENSOR_IND_PACK_COUNT);
    }else{
        MXC_SEMA_FreeSema(PACK_READY_SEM_ID);
    }

    // printf("B:0x%.2x%.2x%.2x%.2x%.2x%.2x\r\n",data_set_core1[9],data_set_core1[10],data_set_core1[11],data_set_core1[12],data_set_core1[13],data_set_core1[14]);
    if (mcsNoConnActive() == FALSE)
    {
        /* find next connection to send (note ccc idx is stored in timer status) */
        if ((pConn = cgmpsFindNextToSend(pMsg->status)) != NULL)
        {
            AttsHandleValueNtf(pConn->connId, CUSTOM_I2C_HDL,sizeof(data_set_core1), (uint8_t*)data_set_core1);
            mcsCb.txReady = FALSE;
            pConn->mcsToSend = FALSE;
        }

        /* restart timer */
        WsfTimerStartMs(&mcsCb.I2CTimer, mcsCb.I2CPeriod);
    }
}
