#include "adxl363_test.h"
#include "mxc_device.h"
#include "led.h"
#include "board.h"
#include "mxc_delay.h"

int test_adxl363_read_reg(uint8_t reg_id, uint8_t expected_val)
{
    int ret_val = E_NO_ERROR;

    adxl_363_controller_t controller;
    controller.odr = ADXL_363_ODR_50;
    controller.range = ADXL_363_RANGE_8G;

    adxl_363_spi_t spi_cfg;
    spi_cfg.spi_ssid = 0;
    spi_cfg.spi_speed = 2400000;

#if TARGET == MAX32655
    spi_cfg.spi_id = 0;
    spi_cfg.spi_pins.clock = TRUE;
    spi_cfg.spi_pins.miso = TRUE;
    spi_cfg.spi_pins.mosi = TRUE;

    spi_cfg.spi_pins.ss0 = TRUE;
    spi_cfg.spi_pins.ss1 = FALSE;
    spi_cfg.spi_pins.ss2 = FALSE;

    spi_cfg.spi_pins.sdio2 = FALSE;
    spi_cfg.spi_pins.sdio3 = FALSE;
#else
    spi_cfg.spi_id = 1;
    spi_cfg.spi_pins = MAP_A;
#endif
    ret_val = adxl363_init(spi_cfg, controller);
    
    if (E_NO_ERROR != ret_val)
    {
        return ret_val;
    }

    uint8_t dev_id = 0;

    if(E_NO_ERROR == adxl363_reg_read(reg_id, &dev_id))
    {
        if (dev_id == expected_val)
        {
            printf("Device initialization SUCCESFUL %.2X\r\n",dev_id);
        }else{
            printf("Device initialization FAILED :%.2X -> Check pin connections\r\n", dev_id);
        }
    }

    adxl363_shutdown();

    return ret_val;
}