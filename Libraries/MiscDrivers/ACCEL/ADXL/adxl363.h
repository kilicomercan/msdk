#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"
#include "spi.h"

typedef enum adxl_odr_t {
    ADXL_363_ODR_12_5=0,
    ADXL_363_ODR_25=1,
    ADXL_363_ODR_50=2,
    ADXL_363_ODR_100=3,
    ADXL_363_ODR_200=4,
    ADXL_363_ODR_400=5
} adxl_odr_t;

typedef enum adxl_range_t {
    ADXL_363_RANGE_2G=0,
    ADXL_363_RANGE_4G=64,
    ADXL_363_RANGE_8G=128
} adxl_range_t;

typedef struct  {
    enum adxl_odr_t odr;
    enum adxl_range_t range;
};

struct adxl_363_spi_t {
    uint8_t spi_id;
    uint8_t spi_ssid;
    uint32_t spi_speed;
    mxc_spi_pins_t spi_pins;
};

typedef enum adxl_axis_t {
    X_AXIS=88,
    Y_AXIS=89,
    Z_AXIS=90
} adxl_axis_t;

int adxl363_axis_read(short * axis_data, adxl_axis_t axis_id);
int adxl363_enable_measurement(uint8_t enable);
int adxl363_fifo_enable_mode(uint8_t mode);
int adxl363_fifo_get_sample_count(uint16_t * sample_count);
int adxl363_fifo_read(uint16_t * buff, uint16_t sample_count);
int adxl363_fifo_set_sample_count(uint16_t sample_count);
int adxl363_init(adxl_363_spi_t spi_conf, adxl_363_controller_t adxl_conf);
int adxl363_intmap_conf(uint8_t int_mask, uint8_t intpin_id);
int adxl363_reg_read(adxl363_reg_t reg_addr, uint8_t * read_buff);
int adxl363_reg_write(adxl363_reg_t reg_addr, uint8_t val);
int adxl363_shutdown(void);