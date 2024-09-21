#include "shared_data.h"
#include "adxl363.h"

uint8_t sensor_pack_buffer[SENSOR_PACK_BUFF_LENGTH];
uint16_t last_send_pack_idx = 0;
uint8_t ready_flag = 0;
uint8_t sem_ready_flag = PACK_READY_SEM_ID;