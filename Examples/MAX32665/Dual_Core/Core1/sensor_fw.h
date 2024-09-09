
#ifndef __SENSOR_FW_H
#define __SENSOR_FW_H

#include "shared_data.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SENSOR_SAMPLE_COUNT_IN_FIFO 150

/**
 * @brief This function initializes SPI for the targe and
 * configures the related registers of the sensor according
 * the parameters 
 * 
 * @retval E_NO_ERROR, on success.
 * @retval error code, on fail.
 */
int init_sensor(void);

/**
 * @brief This function stops sen
 */
void stop_sensor(void);

#ifdef __cplusplus
}
#endif

#endif // __SENSOR_FW_H