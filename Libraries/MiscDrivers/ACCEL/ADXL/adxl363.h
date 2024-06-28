#ifndef _ADXL363_H
#define _ADXL363_H

#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "spi.h"

typedef enum{
    E_ADXL363_NULL_PARAM,
    E_ADXL363_INVALID_PARAM,
    E_ADXL363_NO_ERR = 0
}E_ADXL363_ERR;

/**
 * @brief Command identifiers
*/
#define ADXL363_CMD_WRITE 0x0A
#define ADXL363_CMD_READ 0x0B
#define ADXL363_CMD_READ_FIFO 0x0D

// Register definitions
#define ADXL_363_REG_DEVID_AD             (0x00)
#define ADXL_363_REG_DEVID_MST            (0x01)
#define ADXL_363_REG_DEVID                (0x02)
#define ADXL_363_REG_REVID                (0x03)
#define ADXL_363_REG_XDATA                (0x08)
#define ADXL_363_REG_YDATA                (0x09)
#define ADXL_363_REG_ZDATA                (0x0a)
#define ADXL_363_REG_STATUS               (0x0b)
#define ADXL_363_REG_FIFO_ENTRIES_L       (0x0c)
#define ADXL_363_REG_FIFO_ENTRIES_H       (0x0d)
#define ADXL_363_REG_XDATA_L              (0x0e)
#define ADXL_363_REG_XDATA_H              (0x0f)
#define ADXL_363_REG_YDATA_L              (0x10)
#define ADXL_363_REG_YDATA_H              (0x11)
#define ADXL_363_REG_ZDATA_L              (0x12)
#define ADXL_363_REG_ZDATA_H              (0x13)
#define ADXL_363_REG_TEMP_L               (0x14)
#define ADXL_363_REG_TEMP_H               (0x15)
#define ADXL_363_REG_ADC_DATA_L           (0x16)
#define ADXL_363_REG_ADC_DATA_H           (0x17)
#define ADXL_363_REG_SOFT_RESET           (0x1f)
#define ADXL_363_REG_THRESH_ACT_L         (0x20)
#define ADXL_363_REG_THRESH_ACT_H         (0x21)
#define ADXL_363_REG_TIME_ACT             (0x22)
#define ADXL_363_REG_THRESH_INACT_L       (0x23)
#define ADXL_363_REG_THRESH_INACT_H       (0x24)
#define ADXL_363_REG_TIME_INACT_L         (0x25)
#define ADXL_363_REG_TIME_INACT_H         (0x26)
#define ADXL_363_REG_ACT_INACT_CTL        (0x27)
#define ADXL_363_REG_FIFO_CONTROL         (0x28)
#define ADXL_363_REG_FIFO_SAMPLES         (0x29)
#define ADXL_363_REG_INTMAP1              (0x2a)
#define ADXL_363_REG_INTMAP2              (0x2b)
#define ADXL_363_REG_FILTER_CTL           (0x2c)
#define ADXL_363_REG_POWER_CTL            (0x2d)
#define ADXL_363_REG_SELF_TEST            (0x2e)

// Defining a fake register for reading from the FIFO
#define ADXL_363_PSEUDOREG_FIFO           (0x30)

// 0x22 TIME_ACT
// Activity Time is measured in units of inverse ODR
// (i.e. 1 unit = 2.5ms for ODR=400Hz)
#define ADXL_363_REG_TIME_ACT_DEFAULT           (0x01)

// 0x27 ACT_INACT_CTL
#define ADXL_363_REG_ACT_INACT_CTL_ACT_EN       (0x01)
#define ADXL_363_REG_ACT_INACT_CTL_ACT_REF      (0x02)
#define ADXL_363_REG_ACT_INACT_CTL_INACT_EN     (0x04)
#define ADXL_363_REG_ACT_INACT_CTL_INACT_REF    (0x08)
#define ADXL_363_REG_ACT_INACT_CTL_LINKLOOP_MASK    (0x30)
#define ADXL_363_REG_ACT_INACT_CTL_LINKLOOP_DEFAULT (0x00)
#define ADXL_363_REG_ACT_INACT_CTL_LINKLOOP_LINKED  (0x10)
#define ADXL_363_REG_ACT_INACT_CTL_LINKLOOP_LOOP    (0x30)
#define ADXL_363_REG_ACT_INACT_CTL_DEFAULT      (0x00)

// 0x28 FIFO_CONTROL
#define ADXL_363_REG_FIFO_CTL_MODE_MASK         (0x03)
#define ADXL_363_REG_FIFO_CTL_MODE_DIS          (0x00)
#define ADXL_363_REG_FIFO_CTL_MODE_OLDEST       (0x01)
#define ADXL_363_REG_FIFO_CTL_MODE_STREAM       (0x02)
#define ADXL_363_REG_FIFO_CTL_MODE_TRIG         (0x03)

// 0x2A INTMAP1
#define ADXL_363_REG_INTMAP1_DATA_RDY           (0x01)
#define ADXL_363_REG_INTMAP1_FIFO_RDY           (0x02)
#define ADXL_363_REG_INTMAP1_FIFO_WATERMARK     (0x04)
#define ADXL_363_REG_INTMAP1_FIFO_OVERRUN       (0x08)
#define ADXL_363_REG_INTMAP1_ACT                (0x10)
#define ADXL_363_REG_INTMAP1_INACT              (0x20)
#define ADXL_363_REG_INTMAP1_AWAKE              (0x40)
#define ADXL_363_REG_INTMAP1_ACTIVE_LOW         (0x80)

// 0x2B INTMAP2
#define ADXL_363_REG_INTMAP2_DATA_RDY           (0x01)
#define ADXL_363_REG_INTMAP2_FIFO_RDY           (0x02)
#define ADXL_363_REG_INTMAP2_FIFO_WATERMARK     (0x04)
#define ADXL_363_REG_INTMAP2_FIFO_OVERRUN       (0x08)
#define ADXL_363_REG_INTMAP2_ACT                (0x10)
#define ADXL_363_REG_INTMAP2_INACT              (0x20)
#define ADXL_363_REG_INTMAP2_AWAKE              (0x40)
#define ADXL_363_REG_INTMAP2_ACTIVE_LOW         (0x80)

// 0x2C FILTER_CTL
#define ADXL_363_REG_FILTER_CTL_ODR_MASK        (0x07)
#define ADXL_363_REG_FILTER_CTL_ODR_12HZ        (0x00)
#define ADXL_363_REG_FILTER_CTL_ODR_25HZ        (0x01)
#define ADXL_363_REG_FILTER_CTL_ODR_50HZ        (0x02)
#define ADXL_363_REG_FILTER_CTL_ODR_100HZ       (0x03)
#define ADXL_363_REG_FILTER_CTL_ODR_200HZ       (0x04)
#define ADXL_363_REG_FILTER_CTL_ODR_400HZ       (0x05)
#define ADXL_363_REG_FILTER_CTL_EXT_SAMPLE_EN   (0x08)
#define ADXL_363_REG_FILTER_CTL_EXT_SAMPLE_DIS  (0x00)
#define ADXL_363_REG_FILTER_CTL_HALF_BW_EN      (0x10)
#define ADXL_363_REG_FILTER_CTL_HALF_BW_DIS     (0x00)
#define ADXL_363_REG_FILTER_CTL_RANGE_MASK      (0xC0)
#define ADXL_363_REG_FILTER_CTL_RANGE_2G        (0x00)
#define ADXL_363_REG_FILTER_CTL_RANGE_4G        (0x40)
#define ADXL_363_REG_FILTER_CTL_RANGE_8G        (0x80)
#define ADXL_363_REG_FILTER_CTL_DEFAULT   (ADXL_363_REG_FILTER_CTL_ODR_400HZ | ADXL_363_REG_FILTER_CTL_EXT_SAMPLE_EN | \
                                       ADXL_363_REG_FILTER_CTL_HALF_BW_DIS | ADXL_363_REG_FILTER_CTL_RANGE_8G)

// 0x2D POWER_CTL
#define ADXL_363_REG_POWER_CTL_MODE_MASK        (0x03)
#define ADXL_363_REG_POWER_CTL_MODE_STBY        (0x00)
#define ADXL_363_REG_POWER_CTL_MODE_MEAS        (0x02)
#define ADXL_363_REG_POWER_CTL_AUTOSLEEP        (0x04)
#define ADXL_363_REG_POWER_CTL_WAKEUP           (0x08)
#define ADXL_363_REG_POWER_CTL_LOW_NOISE_MASK   (0x30)
#define ADXL_363_REG_POWER_CTL_LOW_NOISE_NORMAL (0x00)
#define ADXL_363_REG_POWER_CTL_LOW_NOISE_LOW    (0x10)
#define ADXL_363_REG_POWER_CTL_LOW_NOISE_ULOW   (0x20)
#define ADXL_363_REG_POWER_CTL_EXT_CLK_EN       (0x40)
#define ADXL_363_REG_POWER_CTL_EXT_CLK_DIS      (0x00)
#define ADXL_363_REG_POWER_CTL_EXT_ADC_EN       (0x80)
#define ADXL_363_REG_POWER_CTL_EXT_ADC_DIS      (0x00)
#define ADXL_363_REG_POWER_CTL_DEFAULT    (ADXL_363_REG_POWER_CTL_MODE_MEAS | ADXL_363_REG_POWER_CTL_LOW_NOISE_NORMAL | \
                                       ADXL_363_REG_POWER_CTL_EXT_CLK_DIS | ADXL_363_REG_POWER_CTL_EXT_ADC_EN)

// PATCH LAYER
// These defines are to make the ADXL363 look like the ADXL372 as much as possible
#define ADXL_363_REG_PARTID               ADXL_363_REG_DEVID
#define ADXL_363_REG_FIFO_ENTRIES2        ADXL_363_REG_FIFO_ENTRIES_H
#define ADXL_363_REG_FIFO_ENTRIES         ADXL_363_REG_FIFO_ENTRIES_L
#define ADXL_363_REG_FIFO_ENTRIES_PATCH   ADXL_363_REG_FIFO_ENTRIES_L
#define ADXL_363_REG_FIFO_CTL             ADXL_363_REG_FIFO_CONTROL
#define ADXL_363_REG_RESET                ADXL_363_REG_SOFT_RESET
#define ADXL_363_REG_FIFO_DATA            ADXL_363_PSEUDOREG_FIFO
#define ADXL_363_REG_THRESH_ACT_X_PATCH   ADXL_363_REG_THRESH_ACT_L
#define ADXL_363_REG_THRESH_ACT_Y_PATCH   ADXL_363_REG_THRESH_ACT_L
#define ADXL_363_REG_THRESH_ACT_Z_PATCH   ADXL_363_REG_THRESH_ACT_L
#define ADXL_363_REG_TIME_INACT_PATCH     ADXL_363_REG_TIME_INACT_L
#define ADXL_363_REG_THRESH_INACT_X_PATCH ADXL_363_REG_THRESH_INACT_L
#define ADXL_363_REG_THRESH_INACT_Y_PATCH ADXL_363_REG_THRESH_INACT_L
#define ADXL_363_REG_THRESH_INACT_Z_PATCH ADXL_363_REG_THRESH_INACT_L
#define ADXL_363_REG_TIME_INACT_PATCH     ADXL_363_REG_TIME_INACT_L

// Different devices order the bytes H/L in different directions.
#define ADXL_363_REG_XDATA_PATCH          ADXL_363_REG_XDATA_L
#define ADXL_363_REG_YDATA_PATCH          ADXL_363_REG_YDATA_L
#define ADXL_363_REG_ZDATA_PATCH          ADXL_363_REG_ZDATA_L

// 0x1F SOFT RESET
#define ADXL_363_REG_RESET_CODE (0x52)

typedef uint8_t adxl363_reg_t;

typedef enum adxl_odr_t{
    ADXL_363_ODR_12_5=0,
    ADXL_363_ODR_25=1,
    ADXL_363_ODR_50=2,
    ADXL_363_ODR_100=3,
    ADXL_363_ODR_200=4,
    ADXL_363_ODR_400=5
} adxl_odr_t;

typedef enum {
    ADXL_363_RANGE_2G=0,
    ADXL_363_RANGE_4G=64,
    ADXL_363_RANGE_8G=128
} adxl_range_t;

typedef struct adxl_363_controller_t{
    adxl_odr_t odr;
    adxl_range_t range;
}adxl_363_controller_t;

typedef struct adxl_363_spi_t{
    uint8_t spi_id;
    uint8_t spi_ssid;
    uint32_t spi_speed;
    mxc_spi_pins_t spi_pins;
}adxl_363_spi_t;

typedef enum adxl_axis_t {
    X_AXIS=88,
    Y_AXIS=89,
    Z_AXIS=90
} adxl_axis_t;

#define COMMENT_OUT 0
#if COMMENT_OUT
    #define CODE_TO_COMMENT_OUT(code)
#else
    #define CODE_TO_COMMENT_OUT(code) code
#endif

/** API Declarations */
int adxl363_axis_read(int16_t * axis_data, adxl_axis_t axis_id);
int adxl363_enable_measurement(uint8_t enable);
int adxl363_fifo_enable_mode(uint8_t mode);
int adxl363_fifo_get_sample_count(uint16_t *sample_count);
int adxl363_fifo_read(uint8_t *buff, uint16_t sample_count);
int adxl363_fifo_set_sample_count(uint16_t sample_count);

int adxl363_init(adxl_363_spi_t spi_conf, adxl_363_controller_t adxl_conf);

CODE_TO_COMMENT_OUT(int adxl363_intmap_conf(uint8_t int_mask, uint8_t intpin_id);)

int adxl363_reg_read(adxl363_reg_t reg_addr, uint8_t * read_buff);
int adxl363_fifo_read_sample_set(uint8_t *buff, bool temp_data);
int adxl363_reg_write(adxl363_reg_t reg_addr, uint8_t val);
int adxl363_shutdown(void);


#endif