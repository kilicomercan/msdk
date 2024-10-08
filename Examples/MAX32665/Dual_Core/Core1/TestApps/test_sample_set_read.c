#include "adxl363_test.h"
#include "string.h"
#include "mxc_device.h"
#include "led.h"
#include "board.h"
#include "mxc_delay.h"
#include "rtc.h"
#include "helper.h"

#define MAX_SAMPLE_SET_COUNT 170
#define SINGLE_SAMPLE_SIZE 2
#define SAMPLE_COUNT_IN_SET_WITH_TEMP 4
#define SAMPLE_COUNT_IN_SET_NO_TEMP 3
#define MAX_SAMPLE_COUNT_IN_FIFO 80

static uint16_t read_buff[SAMPLE_COUNT_IN_SET_WITH_TEMP] = { 0 };
static adxl363_sample_pkg_t sample_set_pack = { 0 };

int test_sample_set_read(int read_sample_set_count)
{
    int ret_val = E_NO_ERROR;
    uint16_t fifo_sample_count = 0;
    adxl_363_controller_t controller;
    adxl_363_spi_t spi_cfg;

    /* Configure controller parameters. */
    controller.odr = ADXL_363_ODR_100;
    controller.range = ADXL_363_RANGE_2G;

    /* Configure SPI parameters. */
    spi_cfg.spi_id = 0;
    spi_cfg.spi_ssid = 0;
    spi_cfg.spi_speed = 2400000;
#if TARGET == MAX32655
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
    /* Initialize RTC to get timestamp for packets */
    ret_val = MXC_RTC_Init(0, 0);
    ret_val = MXC_RTC_Start();

    /**
     * Initialize adxl sensor. We send required commands to sensor
     * to make it run.
     */
    ret_val = adxl363_init(spi_cfg, controller);

    if (E_NO_ERROR != ret_val) {
        return ret_val;
    }

    /* MAX sample count to be hold in adxl fifo. */
    adxl363_fifo_set_sample_count(MAX_SAMPLE_COUNT_IN_FIFO);
    adxl363_fifo_get_sample_count(&fifo_sample_count);

    if (MAX_SAMPLE_COUNT_IN_FIFO != fifo_sample_count) {
        printf("Sample set failed\n");
        return E_INVALID;
    }

    adxl363_fifo_enable_mode(ADXL_363_REG_FIFO_CTL_MODE_STREAM);
    adxl363_enable_measurement(1);
    int initial_pack = 1;
    while (read_sample_set_count--) {
        adxl363_fifo_read_sample_set((uint8_t*)read_buff, 0);
        if (initial_pack != 1) {
            sample_set_pack = adxl363_parse_sample_set(read_buff);
            printf("0x%.4x%.4x%.4x\r\n", read_buff[0],read_buff[1], read_buff[2]);
            // while(E_NO_ERROR !=MXC_RTC_GetTime(&sample_set_pack.sec, &sample_set_pack.subsec)){}
            // send_pack_to_host(&sample_set_pack);
        } else {
            initial_pack = 0;
        }
        MXC_Delay(MXC_DELAY_MSEC(10));
    }
    adxl363_shutdown();
}