
#ifndef _ADXL363_TEST_H
#define _ADXL363_TEST_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>
#include "adxl363.h"

int read_fifo_test(void);

int test_adxl363_read_reg(uint8_t reg_id, uint8_t expected_val);

int test_adxl363_int_conf(void);

int test_adxl363_read_axis(void);

int test_sample_set_read(int read_sample_set_count);

#ifdef __cplusplus
}
#endif

#endif // _ADXL363_TEST_H
