#include "adxl363_test.h"
#include "mxc_device.h"
#include "led.h"
#include "board.h"
#include "mxc_delay.h"
#include "nvic_table.h"
#include "lp.h"

#define INTMAP1_ACTIVE_LOW 0
#define INTMAP1_PIN MXC_GPIO_PIN_6

#define INTMAP2_ACTIVE_LOW 1
#define INTMAP2_PIN MXC_GPIO_PIN_7

#define INTERRUPT_GPIO_PORT MXC_GPIO1
#define INT_SAMPLE_COUNT 96
#define SPI_SPEED 4000000

// void GPIO1_IRQHandler(void){
//     MXC_GPIO_Handler(1);
// }

static int interrupt_flag = 0;
void gpio_interrupt_callback_function(void* callback_param){
    interrupt_flag = 1;
}

void prepare_interrupt_read_pin(void){
    static mxc_gpio_cfg_t gpio_pin_to_get_interrupt = {0};

    // Disable interrupt for P1.6
    MXC_GPIO_DisableInt(INTERRUPT_GPIO_PORT, INTMAP1_PIN);

    // Clear pending interrupts.
    MXC_GPIO_ClearFlags(INTERRUPT_GPIO_PORT, INTMAP1_PIN);

    // Configure the pin for input interrupt
    gpio_pin_to_get_interrupt.drvstr = MXC_GPIO_DRVSTR_0;
    gpio_pin_to_get_interrupt.func = MXC_GPIO_FUNC_IN;
    gpio_pin_to_get_interrupt.mask = INTMAP1_PIN;
    gpio_pin_to_get_interrupt.port = INTERRUPT_GPIO_PORT;
    gpio_pin_to_get_interrupt.pad = MXC_GPIO_PAD_PULL_UP;
    gpio_pin_to_get_interrupt.vssel = MXC_GPIO_VSSEL_VDDIOH;
    MXC_GPIO_Config(&gpio_pin_to_get_interrupt);

    // Configure interrupt polarity to FALLING edge. INTMODE is set 
    // to edge triggered automatically.
    MXC_GPIO_IntConfig(&gpio_pin_to_get_interrupt, MXC_GPIO_INT_FALLING);

    // Register callback function to handle interrupt.
    MXC_GPIO_RegisterCallback(&gpio_pin_to_get_interrupt, gpio_interrupt_callback_function, NULL);

    // Enable interrupt for MXC_GPIO1.
    NVIC_EnableIRQ(MXC_GPIO_GET_IRQ(MXC_GPIO_GET_IDX(INTERRUPT_GPIO_PORT)));
    MXC_GPIO_EnableInt(INTERRUPT_GPIO_PORT, INTMAP1_PIN);

}

/**
 * @brief This case we enable Watermark interrupt and follow the signal 
 * on GPIO pin which is set to catch interrupts.
*/
int test_adxl363_int_conf(void)
{
    printf("Interrupt test started\r\n");
    uint16_t fifo_sample_count = 0;
    adxl_363_controller_t controller;
    int ret_val = 0;

    // P1.6 for INTMAP1.
    prepare_interrupt_read_pin();

    controller.odr = ADXL_363_ODR_100;
    controller.range = ADXL_363_RANGE_8G;

    adxl_363_spi_t spi_cfg;

    spi_cfg.spi_id = 0;
    spi_cfg.spi_ssid = 0;
    spi_cfg.spi_speed = SPI_SPEED;
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

    adxl363_fifo_set_sample_count(INT_SAMPLE_COUNT);

    adxl363_fifo_get_sample_count(&fifo_sample_count);

    if (INT_SAMPLE_COUNT != fifo_sample_count)
    {
        printf("Interrupt Sample set failed, %.2x\n",fifo_sample_count);
        return E_INVALID;
    }

    ret_val = adxl363_init(spi_cfg, controller);
    if(E_NO_ERROR != ret_val){
        return ret_val;
    }

    adxl363_intmap_conf(
            (ADXL_363_REG_INTMAP1_FIFO_WATERMARK),
        1);

    // printf("Sleep before fifo enable\r\n");
    // MXC_Delay(MXC_DELAY_SEC(5));
    adxl363_fifo_enable_mode(ADXL_363_REG_FIFO_CTL_MODE_STREAM);

    // Enable temperature measurement adding to fifo packets.
    adxl363_enable_measurement(1);

    /**
     * @note Wait 4/ODR for data to settle before reading.
    */

    uint8_t fifo_read_buff[fifo_sample_count*2];
    uint8_t status_reg_val = {0};

    while(1){
        if(interrupt_flag == 1){
            // MXC_Delay(MXC_DELAY_MSEC(2000));
            adxl363_fifo_get_sample_count(&fifo_sample_count);
            adxl363_reg_read(ADXL_363_REG_STATUS, &status_reg_val);
            // printf("B: 0x%.2x\r\n",status_reg_val);
            adxl363_fifo_read(fifo_read_buff, fifo_sample_count);
            adxl363_reg_read(ADXL_363_REG_STATUS, &status_reg_val);
            printf("Fifo read after interrupt\r\n");
            interrupt_flag = 0;
        }
    }

    return E_NO_ERROR;
}
