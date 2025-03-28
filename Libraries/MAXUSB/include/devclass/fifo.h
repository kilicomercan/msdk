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

/**
* \file   fifo.h
* \brief  Driver for management of a software FIFO consisting of 8- or 16-bit elements.
*
*         This file defines the driver API including all types and function prototypes.
*/

#ifndef LIBRARIES_MAXUSB_INCLUDE_DEVCLASS_FIFO_H_
#define LIBRARIES_MAXUSB_INCLUDE_DEVCLASS_FIFO_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/***** Definitions *****/

/// Structure used for FIFO management
typedef struct {
  unsigned int length;  ///< FIFO size (number of elements)
  void * data;          ///< pointer to the FIFO buffer
  unsigned int rindex;  ///< current FIFO read index
  unsigned int windex;  ///< current FIFO write index
} fifo_t;

/// Function alias
/// \sa fifo_put8
#define fifo_put   fifo_put8

/// Function alias
/// \sa fifo_get8
#define fifo_get   fifo_get8


/***** Function Prototypes *****/

/// Initializes the specified FIFO
/**
*   \param    fifo     FIFO on which to perform the operation
*   \param    mem      memory buffer to use for FIFO element storage
*   \param    length   number of elements that the memory buffer can contain
*   \returns  0 if successful, -1 upon failure
*/
void fifo_init(fifo_t * fifo, void * mem, unsigned int length);

/**
*   \brief    Adds and 8-bit element to the FIFO
*   \param    fifo     FIFO on which to perform the operation
*   \param    element  element to add to the FIFO
*   \returns  0 if successful, -1 upon failure
*/
int fifo_put8(fifo_t * fifo, uint8_t element);

/**
*   \brief    Gets the next 8-bit element to the FIFO
*   \param    fifo     FIFO on which to perform the operation
*   \param    element  pointer to where to store the element from the FIFO
*   \returns  0 if successful, -1 upon failure
*/
int fifo_get8(fifo_t * fifo, uint8_t * element);

/**
*   \brief    Adds the next 16-bit element to the FIFO
*   \param    fifo     FIFO on which to perform the operation
*   \param    element  element to add to the FIFO
*   \returns  0 if successful, -1 upon failure
*/
int fifo_put16(fifo_t * fifo, uint16_t element);

/**
*   \brief    Gets the next 16-bit element to the FIFO
*   \param    fifo     FIFO on which to perform the operation
*   \param    element  pointer to where to store the element from the FIFO
*   \returns  0 if successful, -1 upon failure
*/
int fifo_get16(fifo_t * fifo, uint16_t * element);

/**
*   \brief    Immediately resets the FIFO to the empty state
*   \param    fifo   FIFO on which to perform the operation
*/
void fifo_clear(fifo_t * fifo);

/**
*   \brief    Determines if the FIFO is empty
*   \param    fifo   FIFO on which to perform the operation
*   \returns  #TRUE if FIFO is empty, #FALSE otherwise
*/
int fifo_empty(fifo_t * fifo);

/**
*   \brief    FIFO status function
*   \param    fifo   FIFO on which to perform the operation
*   \returns  #TRUE if FIFO is full, #FALSE otherwise
*/
int fifo_full(fifo_t * fifo);

/**
*   \brief    FIFO status function
*   \param    fifo   FIFO on which to perform the operation
*   \returns  the number of elements currently in the FIFO
*/
unsigned int fifo_level(fifo_t * fifo);

/**
*   \brief    FIFO status function
*   \param    fifo   FIFO on which to perform the operation
*   \returns  the remaining elements that can be added to the FIFO
*/
unsigned int fifo_remaining(fifo_t * fifo);

#ifdef __cplusplus
}
#endif

#endif //LIBRARIES_MAXUSB_INCLUDE_DEVCLASS_FIFO_H_
