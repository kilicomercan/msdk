#include "sensor_fw.h"
#include "adxl363.h"
#include "gpio.h"

static void __init_sensor_pack(void){
    // sensor_pack.pack_list = sensor_pack_buffer;
    // sensor_pack.length = 0;
    memset(sensor_pack_buffer, 0, SENSOR_PACK_BUFF_LENGTH);
    last_send_pack_idx = 0;
}

static int __init_test(void){
    int ret_val = E_NO_ERROR;
    uint8_t dev_id = 0;
    uint8_t expected_val = 0xF3;
    uint8_t reg_id = ADXL_363_REG_DEVID;

    printf("Read test val : 0x%X\r\n", dev_id);
    if(E_NO_ERROR == adxl363_reg_read(reg_id, &dev_id))
    {
        printf("Read test val : 0x%X\r\n", dev_id);
        if (dev_id != expected_val)
        {
            ret_val = E_FAIL;
        }
    }

    return ret_val;
}

int init_sensor(void){
    int ret_val = E_NO_ERROR;
    
    adxl_363_controller_t controller = {0};
    controller.odr = SENSOR_ODR;
    controller.range = ADXL_363_RANGE_8G;

    adxl_363_spi_t spi_cfg = {0};
    spi_cfg.spi_ssid = 0;
    spi_cfg.spi_speed = SENSOR_SPI_COMM_SPEED;

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

    __init_sensor_pack();
    ret_val = adxl363_init(spi_cfg, controller);

    if(E_NO_ERROR == ret_val){
        // Apply reading register test
        ret_val = __init_test();

        if(E_NO_ERROR != ret_val){
            adxl363_shutdown();        
            return ret_val;
        }
    }

    /* MAX sample count to be hold in adxl fifo. */
    uint16_t fifo_sample_count = 0;
    adxl363_fifo_set_sample_count(SENSOR_SAMPLE_COUNT_IN_FIFO);
    adxl363_fifo_get_sample_count(&fifo_sample_count);
    
    if (SENSOR_SAMPLE_COUNT_IN_FIFO != fifo_sample_count) {
        adxl363_shutdown();
        return E_INVALID;
    }

    adxl363_fifo_enable_mode(ADXL_363_REG_FIFO_CTL_MODE_STREAM);
    adxl363_enable_measurement(1);

    return ret_val;
}

void stop_sensor(void){
    adxl363_shutdown();
}