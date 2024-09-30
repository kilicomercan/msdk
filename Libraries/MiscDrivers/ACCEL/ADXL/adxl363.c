#include "adxl363.h"

#define SAMPLE_SIZE_IN_BYTES 2
#define MAX_FIFO_SAMPLE_COUNT 511
#define ADXL_363_FIFO_TX_BUFF_SIZE (MAX_FIFO_SAMPLE_COUNT * SAMPLE_SIZE_IN_BYTES + 1)
#define ADXL_363_FIFO_RX_BUFF_SIZE (MAX_FIFO_SAMPLE_COUNT * SAMPLE_SIZE_IN_BYTES + 1)

static uint8_t read_reg_pack[3];
static uint8_t write_reg_pack[3];
static uint8_t adxl_363_fifo_tx_buff[ADXL_363_FIFO_TX_BUFF_SIZE];
static uint8_t adxl_363_fifo_rx_buff[ADXL_363_FIFO_RX_BUFF_SIZE];

static mxc_spi_req_t read_reg_req;
static mxc_spi_req_t write_reg_req;
static mxc_spi_req_t read_fifo_req;

int __adxl363_reg_read(uint8_t reg_addr, uint8_t *read_buff, uint32_t size)
{
    int ret_val = 0;
    read_reg_pack[0] = ADXL363_CMD_READ;
    read_reg_pack[1] = reg_addr;
    read_reg_pack[2] = 0;

    read_reg_req.txData = read_reg_pack;
    read_reg_req.txLen = size + 2;
    read_reg_req.ssDeassert = 1;
    read_reg_req.rxData = read_buff;
    read_reg_req.rxLen = read_reg_req.txLen;
    ret_val = MXC_SPI_MasterTransaction(&read_reg_req);
    return ret_val;
}

int __initialize_mxc_spi(adxl_363_spi_t spi_conf, adxl_363_controller_t adxl_conf)
{
    mxc_spi_regs_t *spi;
    spi = MXC_SPI_GET_SPI(spi_conf.spi_id);

    /**
     * @warning MXC_SPI_Init is defined differently for different part numbers.
     * We call the implementation of it for MAX32655. 
     * 
     * @todo Update this part of the function to make it compatible for other
     * part numbers.
    */
    return MXC_SPI_Init(spi, 1, 0, 1, 0, spi_conf.spi_speed, spi_conf.spi_pins);
}

int16_t __adxl363_read_axis_data(uint8_t lsig_addr, uint8_t msig_addr, int *err)
{
    int ret_val = 0;
    uint8_t most_sig = 0;
    uint8_t least_sig = 0;

    ret_val = adxl363_reg_read(lsig_addr, &least_sig);
    *err = ret_val;
    if (ret_val == 0) {
        ret_val = adxl363_reg_read(msig_addr, &most_sig);
        *err = ret_val;
    }
    /**
     * @todo check concatenation.
    */
    return (most_sig << 8 | least_sig);
}

void __reset_variables(void)
{
    memset(&read_reg_req, 0, sizeof(mxc_spi_req_t));
    memset(&write_reg_req, 0, sizeof(mxc_spi_req_t));
    memset(&read_fifo_req, 0, sizeof(mxc_spi_req_t));

    write_reg_pack[1] = 0;
    write_reg_pack[2] = 0;
    read_reg_pack[1] = 0;
    memset(adxl_363_fifo_tx_buff + 1, 0, ADXL_363_FIFO_TX_BUFF_SIZE - 1);
    memset(adxl_363_fifo_rx_buff, 0, ADXL_363_FIFO_RX_BUFF_SIZE);
    return;
}

int adxl363_axis_read(int16_t *axis_data, adxl_axis_t axis_id)
{
    int16_t local_axis_val = 0;
    int ret_val = 0;

    if (axis_id == X_AXIS) {
        local_axis_val =
            __adxl363_read_axis_data(ADXL_363_REG_XDATA_L, ADXL_363_REG_XDATA_H, &ret_val);
        *axis_data = local_axis_val;
    } else if (axis_id == Y_AXIS) {
        local_axis_val =
            __adxl363_read_axis_data(ADXL_363_REG_YDATA_L, ADXL_363_REG_YDATA_H, &ret_val);
        *axis_data = local_axis_val;
    } else if (axis_id == Z_AXIS) {
        local_axis_val =
            __adxl363_read_axis_data(ADXL_363_REG_ZDATA_L, ADXL_363_REG_ZDATA_H, &ret_val);
        *axis_data = local_axis_val;
    } else {
        *axis_data = -1;
        ret_val = -4;
    }
    return ret_val;
}

int adxl363_enable_measurement(uint8_t enable)
{
    int ret_val;
    uint8_t meas_mode = 0;
    ret_val = adxl363_reg_read(ADXL_363_REG_POWER_CTL, &meas_mode);
    if (ret_val == 0) {
        if (enable == 1) {
            meas_mode = (meas_mode & 0xFC) | 2;
        } else {
            meas_mode = (meas_mode & 0xFC);
        }
        ret_val = adxl363_reg_write(ADXL_363_REG_POWER_CTL, meas_mode);
    }
    return ret_val;
}

int adxl363_fifo_enable_mode(uint8_t mode)
{
    int ret_val;
    uint8_t current_status = 0;

    // If the mode is between [0,3]
    if ((mode & 0xFC) == 0) {
        ret_val = adxl363_reg_read(ADXL_363_REG_FIFO_CONTROL, &current_status);
        if (ret_val == E_NO_ERROR) {
            current_status = current_status & 0xfc;
            ret_val = adxl363_reg_write(ADXL_363_REG_FIFO_CONTROL, current_status | mode);
        }
    } else {
        // If mode is not in valid range, return error.
        ret_val = -4;
    }
    return ret_val;
}

int adxl363_fifo_get_sample_count(uint16_t *sample_count)
{
    int ret_val = 0;
    uint8_t reg_fifo_sample = 0;
    uint8_t reg_fifo_cont = 0;

    if (sample_count != NULL) {
        ret_val = adxl363_reg_read(ADXL_363_REG_FIFO_CONTROL, &reg_fifo_cont);
        if (ret_val == 0) {
            ret_val = adxl363_reg_read(ADXL_363_REG_FIFO_SAMPLES, &reg_fifo_sample);
        }
        *sample_count = ((reg_fifo_cont & 8) << 5) | (uint8_t)reg_fifo_sample;
        return ret_val;
    }
    return ret_val;
}

int adxl363_fifo_read(uint8_t *buff, uint16_t sample_count)
{
    int ret_val = 0;
    int transaction_size = 0;

    if (sample_count < 0x201 || NULL != buff) {
        //Each sample is 2 bytes. So, multiply by 2.
        transaction_size = sample_count * 2;
        if (buff == NULL) {
            ret_val = E_ADXL363_NULL_PARAM;
        } else {
            read_fifo_req.rxData = adxl_363_fifo_rx_buff;

            // 1 byte is spent during transaction to send READ_FIFO command.
            read_fifo_req.txLen = transaction_size + 1;
            read_fifo_req.txData = adxl_363_fifo_tx_buff;
            read_fifo_req.rxLen = read_fifo_req.txLen;
            ret_val = MXC_SPI_MasterTransaction(&read_fifo_req);
            if (ret_val == 0) {
                memcpy(buff, adxl_363_fifo_rx_buff + 1, transaction_size);
            }
            memset(adxl_363_fifo_rx_buff, 0, ADXL_363_FIFO_RX_BUFF_SIZE);
        }
    } else {
        ret_val = -3;
    }
    return ret_val;
}

int adxl363_fifo_set_sample_count(uint16_t sample_count)
{
    int ret_val = 0;
    uint8_t fifo_ctrl_reg_val = 0;

    if (sample_count <= MAX_FIFO_SAMPLE_COUNT) {
        if (0xff < sample_count) {
            // We need to set AH bit to 1. However, we need to read
            // FIFO control resister first not
            adxl363_reg_read(ADXL_363_REG_FIFO_CONTROL, &fifo_ctrl_reg_val);

            adxl363_reg_write(ADXL_363_REG_FIFO_CONTROL, fifo_ctrl_reg_val | 8);
        }
        ret_val = adxl363_reg_write(ADXL_363_REG_FIFO_SAMPLES, sample_count);
    } else {
        ret_val = E_ADXL363_INVALID_PARAM;
    }
    return ret_val;
}

int adxl363_init(adxl_363_spi_t spi_conf, adxl_363_controller_t adxl_conf)
{
    int ret_val = 0;
    mxc_spi_regs_t *spi = NULL;
    spi = MXC_SPI_GET_SPI(spi_conf.spi_id);

    ret_val = __initialize_mxc_spi(spi_conf, adxl_conf);
    if (ret_val == E_NO_ERROR) {
        MXC_SPI_SetDataSize(spi, 8);

        write_reg_req.txData = write_reg_pack;
        write_reg_req.ssDeassert = 1;
        write_reg_req.spi = spi;
        write_reg_req.ssIdx = read_fifo_req.ssIdx;

        read_fifo_req.ssIdx = spi_conf.spi_ssid;
        read_fifo_req.ssDeassert = 1;
        read_fifo_req.spi = spi;

        read_reg_req.spi = spi;
        read_reg_req.txData = read_reg_pack;
        read_reg_req.ssDeassert = 1;
        read_reg_req.ssIdx = read_fifo_req.ssIdx;

        // Clear fifo TX transaction buffer.
        memset(adxl_363_fifo_tx_buff, 0, ADXL_363_FIFO_TX_BUFF_SIZE);
        adxl_363_fifo_tx_buff[0] = ADXL363_CMD_READ_FIFO;

        // Clear fifo RX transaction buffer.
        memset(adxl_363_fifo_rx_buff, 0, ADXL_363_FIFO_RX_BUFF_SIZE);

        //
        ret_val = adxl363_reg_write(ADXL_363_REG_FILTER_CTL, adxl_conf.odr | adxl_conf.range);
    }

    return ret_val;
}

CODE_TO_COMMENT_OUT(int adxl363_intmap_conf(uint8_t int_mask, uint8_t intpin_id) {
    adxl363_reg_t reg_addr;
    int ret_val = 0;

    if (intpin_id == 1 || intpin_id == 2) {
        if (intpin_id == 1) {
            reg_addr = ADXL_363_REG_INTMAP1;
        } else {
            reg_addr = ADXL_363_REG_INTMAP2;
        }

        ret_val = adxl363_reg_write(reg_addr, int_mask);
    } else {
        ret_val = E_ADXL363_INVALID_PARAM;
    }
    return ret_val;
})

int adxl363_reg_read(adxl363_reg_t reg_addr, uint8_t *read_buff)
{
    int ret_val = 0;
    uint8_t read_data[3] = { 0 };

    if (read_buff != NULL) {
        ret_val = __adxl363_reg_read(reg_addr, read_data, 1);
        *read_buff = read_data[2];
    } else {
        ret_val = E_ADXL363_NULL_PARAM;
    }

    return ret_val;
}

int adxl363_reg_write(adxl363_reg_t reg_addr, uint8_t val)
{
    write_reg_pack[0] = ADXL363_CMD_WRITE;
    write_reg_pack[1] = reg_addr;
    write_reg_pack[2] = val;

    write_reg_req.txData = write_reg_pack;
    write_reg_req.txLen = 3;
    return MXC_SPI_MasterTransaction(&write_reg_req);
}

int adxl363_fifo_read_sample_set(uint8_t *buff, bool temp_data)
{
    int ret_val = 0;
    uint8_t reg_data = 0;
    uint16_t sample[4] = { 0 };

    adxl363_reg_read(ADXL_363_REG_FIFO_CONTROL, &reg_data);

    if (reg_data & (1 << 2)) {
        // Temperature data will be in sample set. So, read 4 bytes.
        ret_val = adxl363_fifo_read((uint8_t *)&sample, 4);
    } else {
        // Temperature data is not in sample set. So, read 3 bytes.
        ret_val = adxl363_fifo_read((uint8_t *)&sample, 3);
    }
    if (E_NO_ERROR == ret_val) {
        if (temp_data) {
            memcpy(buff, &sample, 8);
        } else {
            memcpy(buff, &sample, 6);
        }
    }
    
    return ret_val;
}

int adxl363_shutdown(void)
{
    int ret_val = 0;
    adxl363_reg_write(ADXL_363_REG_RESET, ADXL_363_REG_RESET_CODE);
    ret_val = MXC_SPI_Shutdown(write_reg_req.spi);
    __reset_variables();
    return ret_val;
}

/**
 * @brief This function parses the 16-bits data coming from
 * accelerometer fifo.
 */
int16_t sign_extension;
int16_t type;
int16_t scalar_val;
adxl363_sample_pkg_t adxl363_parse_sample_set(uint16_t *data){
    int i = 0;
    adxl363_sample_pkg_t pkg = {0};

    for(; i <= 2; i++){
        sign_extension = (data[i] >> 12) & 0b11;
        type = (data[i] >> 14) & 0b11;
        scalar_val = (data[i] & 0xfff);
        if(sign_extension & 0b10 ){
            scalar_val |= 0xf000;
        }
        switch (type){
            case type_x:
                pkg.x_val = scalar_val;
                break;
            case type_y:
                pkg.y_val = scalar_val;
                break;
            case type_z:
                pkg.z_val = scalar_val;
                break;
            case type_temp:
                pkg.temp_val = scalar_val;
                break;
            default:
                printf("Parsing error\r\n");
                break;
        }
    }
    return pkg;
}