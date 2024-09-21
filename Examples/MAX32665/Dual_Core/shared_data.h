#ifndef __SHARED_DATA_H
#define __SHARED_DATA_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"
#include "stdio.h"
#include "mxc_sys.h"
#include "sema.h"
#include "adxl363.h"

#define SENSOR_PACK_TYPE_UNDEF (1<<0)
#define SENSOR_PACK_TYPE_TRAINING (1<<1)
#define SENSOR_PACK_TYPE_DATA (1<<2)
#define SENSOR_PACK_SEM_ID 0
#define PACK_READY_SEM_ID 1
#define SHARED_SENSOR_ODR 200 // In terms of SAMPLE SET COUNT
#define SENSOR_SET_LENGTH 7
#define SENSOR_IND_PACK_COUNT 2
#define SENSOR_DATA_TRANSFER (SENSOR_SET_LENGTH*SENSOR_IND_PACK_COUNT)
/**
 * 1 pack is 7-byte long.
 * First byte is to represent the pack type. Other 6 bytes 
 * is to store the X,Y,Z sensor value.
 * 
 */
#define SENSOR_PACK_BUFF_LENGTH (SENSOR_SET_LENGTH*SHARED_SENSOR_ODR) 
extern uint16_t last_send_pack_idx;
// struct sensor_pack_t{
//     uint8_t length; // Number of sets (x,y,z).
//     uint8_t *pack_list;
// };

extern struct sensor_pack_t sensor_pack;
extern uint8_t sensor_pack_buffer[];
extern uint8_t ready_flag;
extern uint8_t sem_ready_flag;
#ifdef __cplusplus
}
#endif

#endif // __SHARED_DATA_H