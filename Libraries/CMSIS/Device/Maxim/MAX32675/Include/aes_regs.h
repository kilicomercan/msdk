/**
 * @file    aes_regs.h
 * @brief   Registers, Bit Masks and Bit Positions for the AES Peripheral Module.
 * @note    This file is @generated.
 * @ingroup aes_registers
 */

/******************************************************************************
 *
 * Copyright (C) 2022-2023 Maxim Integrated Products, Inc. (now owned by 
 * Analog Devices, Inc.),
 * Copyright (C) 2023-2024 Analog Devices, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************************/

#ifndef LIBRARIES_CMSIS_DEVICE_MAXIM_MAX32675_INCLUDE_AES_REGS_H_
#define LIBRARIES_CMSIS_DEVICE_MAXIM_MAX32675_INCLUDE_AES_REGS_H_

/* **** Includes **** */
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined (__ICCARM__)
  #pragma system_include
#endif

#if defined (__CC_ARM)
  #pragma anon_unions
#endif
/// @cond
/*
    If types are not defined elsewhere (CMSIS) define them here
*/
#ifndef __IO
#define __IO volatile
#endif
#ifndef __I
#ifdef __cplusplus
#define __I volatile
#else
#define __I volatile const
#endif
#endif
#ifndef __O
#define __O  volatile
#endif
#ifndef __R
#define __R  volatile const
#endif
/// @endcond

/* **** Definitions **** */

/**
 * @ingroup     aes
 * @defgroup    aes_registers AES_Registers
 * @brief       Registers, Bit Masks and Bit Positions for the AES Peripheral Module.
 * @details     AES Keys.
 */

/**
 * @ingroup aes_registers
 * Structure type to access the AES Registers.
 */
typedef struct {
    __IO uint32_t ctrl;                 /**< <tt>\b 0x0000:</tt> AES CTRL Register */
    __IO uint32_t status;               /**< <tt>\b 0x0004:</tt> AES STATUS Register */
    __IO uint32_t intfl;                /**< <tt>\b 0x0008:</tt> AES INTFL Register */
    __IO uint32_t inten;                /**< <tt>\b 0x000C:</tt> AES INTEN Register */
    __IO uint32_t fifo;                 /**< <tt>\b 0x0010:</tt> AES FIFO Register */
} mxc_aes_regs_t;

/* Register offsets for module AES */
/**
 * @ingroup    aes_registers
 * @defgroup   AES_Register_Offsets Register Offsets
 * @brief      AES Peripheral Register Offsets from the AES Base Peripheral Address.
 * @{
 */
#define MXC_R_AES_CTRL                     ((uint32_t)0x00000000UL) /**< Offset from AES Base Address: <tt> 0x0000</tt> */
#define MXC_R_AES_STATUS                   ((uint32_t)0x00000004UL) /**< Offset from AES Base Address: <tt> 0x0004</tt> */
#define MXC_R_AES_INTFL                    ((uint32_t)0x00000008UL) /**< Offset from AES Base Address: <tt> 0x0008</tt> */
#define MXC_R_AES_INTEN                    ((uint32_t)0x0000000CUL) /**< Offset from AES Base Address: <tt> 0x000C</tt> */
#define MXC_R_AES_FIFO                     ((uint32_t)0x00000010UL) /**< Offset from AES Base Address: <tt> 0x0010</tt> */
/**@} end of group aes_registers */

/**
 * @ingroup  aes_registers
 * @defgroup AES_CTRL AES_CTRL
 * @brief    AES Control Register
 * @{
 */
#define MXC_F_AES_CTRL_EN_POS                          0 /**< CTRL_EN Position */
#define MXC_F_AES_CTRL_EN                              ((uint32_t)(0x1UL << MXC_F_AES_CTRL_EN_POS)) /**< CTRL_EN Mask */

#define MXC_F_AES_CTRL_DMA_RX_EN_POS                   1 /**< CTRL_DMA_RX_EN Position */
#define MXC_F_AES_CTRL_DMA_RX_EN                       ((uint32_t)(0x1UL << MXC_F_AES_CTRL_DMA_RX_EN_POS)) /**< CTRL_DMA_RX_EN Mask */

#define MXC_F_AES_CTRL_DMA_TX_EN_POS                   2 /**< CTRL_DMA_TX_EN Position */
#define MXC_F_AES_CTRL_DMA_TX_EN                       ((uint32_t)(0x1UL << MXC_F_AES_CTRL_DMA_TX_EN_POS)) /**< CTRL_DMA_TX_EN Mask */

#define MXC_F_AES_CTRL_START_POS                       3 /**< CTRL_START Position */
#define MXC_F_AES_CTRL_START                           ((uint32_t)(0x1UL << MXC_F_AES_CTRL_START_POS)) /**< CTRL_START Mask */

#define MXC_F_AES_CTRL_INPUT_FLUSH_POS                 4 /**< CTRL_INPUT_FLUSH Position */
#define MXC_F_AES_CTRL_INPUT_FLUSH                     ((uint32_t)(0x1UL << MXC_F_AES_CTRL_INPUT_FLUSH_POS)) /**< CTRL_INPUT_FLUSH Mask */

#define MXC_F_AES_CTRL_OUTPUT_FLUSH_POS                5 /**< CTRL_OUTPUT_FLUSH Position */
#define MXC_F_AES_CTRL_OUTPUT_FLUSH                    ((uint32_t)(0x1UL << MXC_F_AES_CTRL_OUTPUT_FLUSH_POS)) /**< CTRL_OUTPUT_FLUSH Mask */

#define MXC_F_AES_CTRL_KEY_SIZE_POS                    6 /**< CTRL_KEY_SIZE Position */
#define MXC_F_AES_CTRL_KEY_SIZE                        ((uint32_t)(0x3UL << MXC_F_AES_CTRL_KEY_SIZE_POS)) /**< CTRL_KEY_SIZE Mask */
#define MXC_V_AES_CTRL_KEY_SIZE_AES128                 ((uint32_t)0x0UL) /**< CTRL_KEY_SIZE_AES128 Value */
#define MXC_S_AES_CTRL_KEY_SIZE_AES128                 (MXC_V_AES_CTRL_KEY_SIZE_AES128 << MXC_F_AES_CTRL_KEY_SIZE_POS) /**< CTRL_KEY_SIZE_AES128 Setting */
#define MXC_V_AES_CTRL_KEY_SIZE_AES192                 ((uint32_t)0x1UL) /**< CTRL_KEY_SIZE_AES192 Value */
#define MXC_S_AES_CTRL_KEY_SIZE_AES192                 (MXC_V_AES_CTRL_KEY_SIZE_AES192 << MXC_F_AES_CTRL_KEY_SIZE_POS) /**< CTRL_KEY_SIZE_AES192 Setting */
#define MXC_V_AES_CTRL_KEY_SIZE_AES256                 ((uint32_t)0x2UL) /**< CTRL_KEY_SIZE_AES256 Value */
#define MXC_S_AES_CTRL_KEY_SIZE_AES256                 (MXC_V_AES_CTRL_KEY_SIZE_AES256 << MXC_F_AES_CTRL_KEY_SIZE_POS) /**< CTRL_KEY_SIZE_AES256 Setting */

#define MXC_F_AES_CTRL_TYPE_POS                        8 /**< CTRL_TYPE Position */
#define MXC_F_AES_CTRL_TYPE                            ((uint32_t)(0x3UL << MXC_F_AES_CTRL_TYPE_POS)) /**< CTRL_TYPE Mask */

/**@} end of group AES_CTRL_Register */

/**
 * @ingroup  aes_registers
 * @defgroup AES_STATUS AES_STATUS
 * @brief    AES Status Register
 * @{
 */
#define MXC_F_AES_STATUS_BUSY_POS                      0 /**< STATUS_BUSY Position */
#define MXC_F_AES_STATUS_BUSY                          ((uint32_t)(0x1UL << MXC_F_AES_STATUS_BUSY_POS)) /**< STATUS_BUSY Mask */

#define MXC_F_AES_STATUS_INPUT_EM_POS                  1 /**< STATUS_INPUT_EM Position */
#define MXC_F_AES_STATUS_INPUT_EM                      ((uint32_t)(0x1UL << MXC_F_AES_STATUS_INPUT_EM_POS)) /**< STATUS_INPUT_EM Mask */

#define MXC_F_AES_STATUS_INPUT_FULL_POS                2 /**< STATUS_INPUT_FULL Position */
#define MXC_F_AES_STATUS_INPUT_FULL                    ((uint32_t)(0x1UL << MXC_F_AES_STATUS_INPUT_FULL_POS)) /**< STATUS_INPUT_FULL Mask */

#define MXC_F_AES_STATUS_OUTPUT_EM_POS                 3 /**< STATUS_OUTPUT_EM Position */
#define MXC_F_AES_STATUS_OUTPUT_EM                     ((uint32_t)(0x1UL << MXC_F_AES_STATUS_OUTPUT_EM_POS)) /**< STATUS_OUTPUT_EM Mask */

#define MXC_F_AES_STATUS_OUTPUT_FULL_POS               4 /**< STATUS_OUTPUT_FULL Position */
#define MXC_F_AES_STATUS_OUTPUT_FULL                   ((uint32_t)(0x1UL << MXC_F_AES_STATUS_OUTPUT_FULL_POS)) /**< STATUS_OUTPUT_FULL Mask */

/**@} end of group AES_STATUS_Register */

/**
 * @ingroup  aes_registers
 * @defgroup AES_INTFL AES_INTFL
 * @brief    AES Interrupt Flag Register
 * @{
 */
#define MXC_F_AES_INTFL_DONE_POS                       0 /**< INTFL_DONE Position */
#define MXC_F_AES_INTFL_DONE                           ((uint32_t)(0x1UL << MXC_F_AES_INTFL_DONE_POS)) /**< INTFL_DONE Mask */

#define MXC_F_AES_INTFL_KEY_CHANGE_POS                 1 /**< INTFL_KEY_CHANGE Position */
#define MXC_F_AES_INTFL_KEY_CHANGE                     ((uint32_t)(0x1UL << MXC_F_AES_INTFL_KEY_CHANGE_POS)) /**< INTFL_KEY_CHANGE Mask */

#define MXC_F_AES_INTFL_KEY_ZERO_POS                   2 /**< INTFL_KEY_ZERO Position */
#define MXC_F_AES_INTFL_KEY_ZERO                       ((uint32_t)(0x1UL << MXC_F_AES_INTFL_KEY_ZERO_POS)) /**< INTFL_KEY_ZERO Mask */

#define MXC_F_AES_INTFL_OV_POS                         3 /**< INTFL_OV Position */
#define MXC_F_AES_INTFL_OV                             ((uint32_t)(0x1UL << MXC_F_AES_INTFL_OV_POS)) /**< INTFL_OV Mask */

#define MXC_F_AES_INTFL_KEY_ONE_POS                    4 /**< INTFL_KEY_ONE Position */
#define MXC_F_AES_INTFL_KEY_ONE                        ((uint32_t)(0x1UL << MXC_F_AES_INTFL_KEY_ONE_POS)) /**< INTFL_KEY_ONE Mask */

/**@} end of group AES_INTFL_Register */

/**
 * @ingroup  aes_registers
 * @defgroup AES_INTEN AES_INTEN
 * @brief    AES Interrupt Enable Register
 * @{
 */
#define MXC_F_AES_INTEN_DONE_POS                       0 /**< INTEN_DONE Position */
#define MXC_F_AES_INTEN_DONE                           ((uint32_t)(0x1UL << MXC_F_AES_INTEN_DONE_POS)) /**< INTEN_DONE Mask */

#define MXC_F_AES_INTEN_KEY_CHANGE_POS                 1 /**< INTEN_KEY_CHANGE Position */
#define MXC_F_AES_INTEN_KEY_CHANGE                     ((uint32_t)(0x1UL << MXC_F_AES_INTEN_KEY_CHANGE_POS)) /**< INTEN_KEY_CHANGE Mask */

#define MXC_F_AES_INTEN_KEY_ZERO_POS                   2 /**< INTEN_KEY_ZERO Position */
#define MXC_F_AES_INTEN_KEY_ZERO                       ((uint32_t)(0x1UL << MXC_F_AES_INTEN_KEY_ZERO_POS)) /**< INTEN_KEY_ZERO Mask */

#define MXC_F_AES_INTEN_OV_POS                         3 /**< INTEN_OV Position */
#define MXC_F_AES_INTEN_OV                             ((uint32_t)(0x1UL << MXC_F_AES_INTEN_OV_POS)) /**< INTEN_OV Mask */

#define MXC_F_AES_INTEN_KEY_ONE_POS                    4 /**< INTEN_KEY_ONE Position */
#define MXC_F_AES_INTEN_KEY_ONE                        ((uint32_t)(0x1UL << MXC_F_AES_INTEN_KEY_ONE_POS)) /**< INTEN_KEY_ONE Mask */

/**@} end of group AES_INTEN_Register */

/**
 * @ingroup  aes_registers
 * @defgroup AES_FIFO AES_FIFO
 * @brief    AES Data Register
 * @{
 */
#define MXC_F_AES_FIFO_DATA_POS                        0 /**< FIFO_DATA Position */
#define MXC_F_AES_FIFO_DATA                            ((uint32_t)(0x1UL << MXC_F_AES_FIFO_DATA_POS)) /**< FIFO_DATA Mask */

/**@} end of group AES_FIFO_Register */

#ifdef __cplusplus
}
#endif

#endif // LIBRARIES_CMSIS_DEVICE_MAXIM_MAX32675_INCLUDE_AES_REGS_H_
