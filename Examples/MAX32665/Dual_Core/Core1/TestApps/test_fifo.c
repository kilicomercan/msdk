#include "adxl363_test.h"
#include "string.h"
#include "mxc_device.h"
#include "led.h"
#include "board.h"
#include "mxc_delay.h"

#define SAMPLE_COUNT 100
int read_fifo_test(void)
{
    uint16_t fifo_sample_count = 0;
    uint16_t fifo_read_buff[SAMPLE_COUNT];
    int test_size = 10;
    int ret_val = E_NO_ERROR;

    adxl_363_controller_t controller;
    controller.odr = ADXL_363_ODR_100;
    controller.range = ADXL_363_RANGE_8G;

    adxl_363_spi_t spi_cfg;

    spi_cfg.spi_id = 0;
    spi_cfg.spi_ssid = 0;
    spi_cfg.spi_speed = 2400000;
#if TARGET==MAX32655
    spi_cfg.spi_pins.clock = TRUE;
    spi_cfg.spi_pins.miso = TRUE;
    spi_cfg.spi_pins.mosi = TRUE;

    spi_cfg.spi_pins.ss0 = TRUE;
    spi_cfg.spi_pins.ss1 = FALSE;
    spi_cfg.spi_pins.ss2 = FALSE;

    spi_cfg.spi_pins.sdio2 = FALSE;
    spi_cfg.spi_pins.sdio3 = FALSE;
#elif TARGET == MAX32665
    spi_cfg.spi_id = 1;
    spi_cfg.spi_pins = MAP_A;
#else
    return -1;
#endif

    ret_val = adxl363_init(spi_cfg, controller);

    if(E_NO_ERROR != ret_val ){
        return ret_val;
    }

    adxl363_fifo_set_sample_count(SAMPLE_COUNT);

    adxl363_fifo_get_sample_count(&fifo_sample_count);

    if (SAMPLE_COUNT != fifo_sample_count)
    {
        printf("Sample set failed\n");
        return E_INVALID;
    }

    adxl363_fifo_enable_mode(ADXL_363_REG_FIFO_CTL_MODE_STREAM);
    // adxl363_intmap_conf(
    //          ADXL_363_REG_INTMAP_ACTIVE_LOW_MASK |
    //         ADXL_363_REG_INTMAP_FIFO_WATERMARK_MASK,
    //     1);
    uint16_t final_data[480] = {0};

    adxl363_enable_measurement(1);
    while (test_size) {
        MXC_Delay(MXC_DELAY_MSEC(300));
        adxl363_fifo_read((uint8_t*)fifo_read_buff, SAMPLE_COUNT);
        memcpy(&final_data[(10-test_size)*SAMPLE_COUNT], fifo_read_buff, SAMPLE_COUNT*2);
        memset(fifo_read_buff,0,SAMPLE_COUNT*2);
        test_size--;
    }
    int i  = 0;
    while(i < 480){
        printf("%d : %d\r\n", i%SAMPLE_COUNT, (int16_t)final_data[i]);
        i++;
    }

    adxl363_shutdown();

    return E_NO_ERROR;
}