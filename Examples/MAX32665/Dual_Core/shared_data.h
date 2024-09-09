#ifndef __SHARED_DATA_H
#define __SHARED_DATA_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"
#include "stdio.h"
#include "mxc_sys.h"
#include "sema.h"

#define SENSOR_PACK_TYPE_UNDEF (1<<0)
#define SENSOR_PACK_TYPE_TRAINING (1<<1)
#define SENSOR_PACK_TYPE_DATA (1<<2)
#define SENSOR_PACK_SEM_ID 0
#define PACK_READY_SEM_ID 1

/**
 * 1 pack is 7-byte long.
 * 1 byte to determine the packet type.
 * 2 bytes per every axis data.
 * 3 bytes per a set(X,Y,Z).
 * 100 is ODR.
 */
#define SENSOR_PACK_BUFF_LENGTH ((1 + sizeof(adxl363_sample_pkg_t))*100) 

struct pack_t{
    uint8_t type;
    uint16_t raw_data[3];
};

struct sensor_pack_t{
    uint8_t sem_id;
    uint8_t length; // Number of axis pack. 3*100 in case of 100 ODR.
    uint8_t *pack_list;
};

extern struct sensor_pack_t sensor_pack;
extern uint8_t sensor_pack_buffer[];
extern uint8_t ready_flag;
extern uint8_t sem_ready_flag;
#ifdef __cplusplus
}
#endif

#endif // __SHARED_DATA_H