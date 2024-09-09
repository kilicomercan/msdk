#include "shared_data.h"
#include "adxl363.h"

uint8_t sensor_pack_buffer[SENSOR_PACK_BUFF_LENGTH];
struct sensor_pack_t sensor_pack = {.pack_list = sensor_pack_buffer, .length=0, .sem_id = SENSOR_PACK_SEM_ID};
uint8_t ready_flag = 0;
uint8_t sem_ready_flag = PACK_READY_SEM_ID;