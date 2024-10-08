#include "adxl363_test.h"
#include "string.h"
#include "mxc_device.h"
#include "led.h"
#include "board.h"
#include "mxc_delay.h"
#include "helper.h"


static uint16_t x_axis_data = 0;
static uint16_t y_axis_data = 0;
static uint16_t z_axis_data = 0;
int test_adxl363_read_axis(void)
{
    int testCount = 50;
    printf("Read Axis test started\r\n");

    adxl_363_controller_t controller;
    controller.odr = ADXL_363_ODR_100;
    controller.range = ADXL_363_RANGE_2G;

    adxl_363_spi_t spi_cfg;
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

    adxl363_init(spi_cfg, controller);

    while (testCount--)
    {
        // memset(axis_buff,0,sizeof(axis_buff));
        x_axis_data = 0;
        y_axis_data = 0;
        z_axis_data = 0;
        adxl363_axis_read(&y_axis_data, Y_AXIS);
        adxl363_axis_read(&z_axis_data, Z_AXIS);
        adxl363_axis_read(&x_axis_data, X_AXIS);
        printf("0x%.4x %.4x %.4x\r\n",x_axis_data, y_axis_data, z_axis_data);
        // adxl363_sample_pkg_t pkg = adxl363_parse_sample_set(axis_buff);
        // send_pack_to_host(&pkg);
        // printf("x_val: %d\n", axis_buff[0]);
        // printf("y_val: %d\n", axis_buff[1]);
        // printf("z_val: %d\n\n", axis_buff[2]);
        MXC_Delay(MXC_DELAY_MSEC(10));
    }

    return adxl363_shutdown();
}