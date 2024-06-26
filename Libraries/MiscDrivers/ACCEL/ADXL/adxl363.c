
#include "adxl363.h"

int __adxl363_reg_read(uint8_t reg_addr, uint8_t *read_buff,uint size)
{
  int iVar1;
  
  read_reg_pack[0] = '\v';
  read_reg_req.txData = read_reg_pack;
  read_reg_req.txLen = size + 2;
  read_reg_req.ssDeassert = 1;
  read_reg_pack[1] = reg_addr;
  read_reg_req.rxData = read_buff;
  read_reg_req.rxLen = read_reg_req.txLen;
  iVar1 = MXC_SPI_MasterTransaction(&read_reg_req);
  return iVar1;
}

int __initialize_mxc_spi(adxl_363_spi_t spi_conf,adxl_363_controller_t adxl_conf)
{
  mxc_spi_pins_t pins;
  int iVar1;
  uint in_r1;
  undefined4 in_r2;
  undefined4 in_r3;
  mxc_spi_regs_t *spi;
  
  if (((uint8_t)adxl_conf & 0xff) == 0) {
    spi = (mxc_spi_regs_t *)0x40046000;
  }
  else if (((uint8_t)adxl_conf & 0xff) == 1) {
    spi = (mxc_spi_regs_t *)0x400be000;
  }
  else {
    spi = (mxc_spi_regs_t *)0x0;
  }
  pins._4_4_ = in_r3;
  pins._0_4_ = in_r2;
  pins.vddioh = (_Bool)spi_conf.spi_id;
  iVar1 = MXC_SPI_Init(spi,1,0,1,0,in_r1,pins);
  return iVar1;
}

int16_t __adxl363_read_axis_data(uint8_t lsig, uint8_t msig, int *err)
{
  int iVar1;
  uint8_t most_sig;
  uint8_t least_sig;
  
  least_sig = '\0';
  most_sig = '\0';
  iVar1 = adxl363_reg_read(lsig, &least_sig);
  *err = iVar1;
  if (iVar1 == 0) {
    iVar1 = adxl363_reg_read(msig, &most_sig);
    *err = iVar1;
  }
  return CONCAT11(most_sig,least_sig);
}

int adxl363_axis_read(int16_t *axis_data, adxl_axis_t axis_id)
{
  int16_t iVar1;
  int ret_val;
  
  ret_val = 0;
  if (axis_id == X_AXIS) {
    iVar1 = __adxl363_read_axis_data('\x0e','\x0f',&ret_val);
    *axis_data = iVar1;
  }
  else if (axis_id == Y_AXIS) {
    iVar1 = __adxl363_read_axis_data('\x10','\x11',&ret_val);
    *axis_data = iVar1;
  }
  else if (axis_id == Z_AXIS) {
    iVar1 = __adxl363_read_axis_data('\x12','\x13',&ret_val);
    *axis_data = iVar1;
  }
  else {
    *axis_data = -1;
    ret_val = -4;
  }
  return ret_val;
}

int adxl363_enable_measurement(uint8_t enable)
{
  int iVar1;
  uint8_t meas_mode;
  
  meas_mode = '\0';
  iVar1 = adxl363_reg_read('-',&meas_mode);
  if (iVar1 == 0) {
    if (enable == '\x01') {
      meas_mode = meas_mode & 0xfc | 2;
    }
    else {
      meas_mode = meas_mode & 0xfc;
    }
    iVar1 = adxl363_reg_write('-',meas_mode);
  }
  return iVar1;
}

int adxl363_fifo_enable_mode(uint8_t mode)
{
  int iVar1;
  uint8_t current_status;
  
  current_status = '\0';
  if ((((mode == '\0') || (mode == '\x02')) || (mode == '\x03')) || (mode == '\x01')) {
    iVar1 = adxl363_reg_read('(',&current_status);
    if (iVar1 == 0) {
      current_status = current_status & 0xfc;
      iVar1 = adxl363_reg_write('(',current_status | mode);
    }
  }
  else {
    iVar1 = -4;
  }
  return iVar1;
}

int adxl363_fifo_get_sample_count(uint8_t *sample_count)
{
  int iVar1;
  uint8_t reg_fifo_sample;
  uint8_t reg_fifo_cont;
  
  if (sample_count != (uint8_t *)0x0) {
    reg_fifo_cont = '\0';
    reg_fifo_sample = '\0';
    iVar1 = adxl363_reg_read('(',&reg_fifo_cont);
    if (iVar1 == 0) {
      iVar1 = adxl363_reg_read(')',&reg_fifo_sample);
    }
    *sample_count = (reg_fifo_cont & 8) << 5 | (uint8_t)reg_fifo_sample;
    return iVar1;
  }
  return -3;
}

int adxl363_fifo_read(uint8_t *buff,uint8_t sample_count)

{
  int iVar1;
  size_t __n;
  
  if (sample_count < 0x201) {
    __n = (uint)sample_count * 2;
    if (buff == (uint8_t *)0x0) {
      iVar1 = -3;
    }
    else {
      read_fifo_req.rxData = adxl_363_fifo_rx_buff;
      read_fifo_req.txLen = __n + 1;
      read_fifo_req.txData = adxl_363_fifo_tx_buff;
      read_fifo_req.rxLen = read_fifo_req.txLen;
      iVar1 = MXC_SPI_MasterTransaction(&read_fifo_req);
      if (iVar1 == 0) {
        memcpy(buff,adxl_363_fifo_rx_buff + 1,__n);
      }
      memset(adxl_363_fifo_rx_buff,0,0x401);
    }
  }
  else {
    iVar1 = -3;
  }
  return iVar1;
}

int adxl363_fifo_set_sample_count(uint8_t sample_count)
{
  int iVar1;
  uint8_t fifo_ctrl_reg_val;
  
  fifo_ctrl_reg_val = '\0';
  if (sample_count < 0x201) {
    if ((0xff < sample_count) && (iVar1 = adxl363_reg_read('(',&fifo_ctrl_reg_val), iVar1 == 0)) {
      adxl363_reg_write('(',fifo_ctrl_reg_val | 8);
    }
    iVar1 = adxl363_reg_write(')',(uint8_t)sample_count);
  }
  else {
    iVar1 = -3;
  }
  return iVar1;
}


int adxl363_init(adxl_363_spi_t spi_conf,adxl_363_controller_t adxl_conf)
{
  adxl_363_spi_t spi_conf_00;
  int iVar1;
  uint in_r1;
  undefined4 in_r2;
  undefined4 in_r3;
  undefined4 unaff_r4;
  mxc_spi_regs_t *spi;
  undefined4 unaff_r5;
  undefined6 in_stack_ffffffde;
  adxl_range_t aStack_f;
  
  adxl363_spi_conf.spi_id = '\0';
  adxl363_spi_conf.spi_ssid = '\0';
  adxl363_spi_conf._2_2_ = 0;
  adxl363_spi_conf.spi_speed = 0;
  adxl363_spi_conf.spi_pins.clock = false;
  adxl363_spi_conf.spi_pins.ss0 = false;
  adxl363_spi_conf.spi_pins.ss1 = false;
  adxl363_spi_conf.spi_pins.ss2 = false;
  adxl363_spi_conf.spi_pins.miso = false;
  adxl363_spi_conf.spi_pins.mosi = false;
  adxl363_spi_conf.spi_pins.sdio2 = false;
  adxl363_spi_conf.spi_pins.sdio3 = false;
  adxl363_spi_conf._16_4_ = 0;
  adxl363_conf.odr = ADXL_363_ODR_12_5;
  adxl363_conf.range = ADXL_363_RANGE_2G;
  if (in_r1 - 2400000 < 0x557301) {
    spi_conf_00._6_6_ = in_stack_ffffffde;
    spi_conf_00._0_6_ = spi_conf._0_6_;
    spi_conf_00.spi_pins._4_4_ = unaff_r4;
    spi_conf_00._16_4_ = unaff_r5;
    iVar1 = __initialize_mxc_spi(spi_conf_00,adxl_conf);
    if (iVar1 == 0) {
      if (((uint8_t)adxl_conf & 0xff) == 0) {
        spi = (mxc_spi_regs_t *)0x40046000;
      }
      else if (((uint8_t)adxl_conf & 0xff) == 1) {
        spi = (mxc_spi_regs_t *)0x400be000;
      }
      else {
        spi = (mxc_spi_regs_t *)0x0;
      }
      MXC_SPI_SetDataSize(spi,8);
      read_reg_req.txData = read_reg_pack;
      read_reg_req.ssDeassert = 1;
      aStack_f = adxl_conf.range;
      read_fifo_req.ssIdx = (int)aStack_f;
      write_reg_req.txData = write_reg_pack;
      write_reg_req.ssDeassert = 1;
      read_fifo_req.ssDeassert = 1;
      read_fifo_req.spi = spi;
      read_reg_req.spi = spi;
      read_reg_req.ssIdx = read_fifo_req.ssIdx;
      write_reg_req.spi = spi;
      write_reg_req.ssIdx = read_fifo_req.ssIdx;
      memset(adxl_363_fifo_tx_buff,0,0x401);
      memset(adxl_363_fifo_rx_buff,0,0x401);
      adxl_363_fifo_tx_buff[0] = '\r';
      iVar1 = adxl363_reg_write(',',(byte)spi_conf.spi_speed | spi_conf.spi_speed._1_1_);
      adxl363_spi_conf._16_4_ = spi_conf._0_4_;
      adxl363_conf = spi_conf.spi_speed;
      adxl363_spi_conf._0_4_ = (uint)(uint8_t)adxl_conf;
      adxl363_spi_conf.spi_speed = in_r1;
      adxl363_spi_conf.spi_pins._0_4_ = in_r2;
      adxl363_spi_conf.spi_pins._4_4_ = in_r3;
    }
  }
  else {
    iVar1 = -3;
  }
  return iVar1;
}


void adxl363_intmap1_cb(void *cb_param)

{
  map1_int = '\0';
  return;
}

int adxl363_intmap_conf(uint8_t int_mask,uint8_t intpin_id)

{
  adxl363_reg_t reg_addr;
  int iVar1;
  
  if ((byte)(intpin_id - 1) < 2) {
    if (intpin_id == '\x01') {
      reg_addr = '*';
    }
    else if (intpin_id == '\x02') {
      reg_addr = '+';
    }
    else {
      reg_addr = 0xff;
    }
    iVar1 = adxl363_reg_write(reg_addr,int_mask);
  }
  else {
    iVar1 = -3;
  }
  return iVar1;
}

int adxl363_reg_read(adxl363_reg_t reg_addr,uint8_t *read_buff)

{
  int iVar1;
  uint8_t read_data [3];
  
  if (read_buff != (uint8_t *)0x0) {
    read_data[0] = '\0';
    read_data[1] = '\0';
    read_data[2] = '\0';
    iVar1 = __adxl363_reg_read(reg_addr,read_data,1);
    *read_buff = read_data[2];
    return iVar1;
  }
  return -3;
}

int adxl363_reg_write(adxl363_reg_t reg_addr,uint8_t val)

{
  int iVar1;
  
  write_reg_req.txData = write_reg_pack;
  write_reg_req.txLen = 3;
  write_reg_pack[1] = reg_addr;
  write_reg_pack[2] = val;
  iVar1 = MXC_SPI_MasterTransaction(&write_reg_req);
  return iVar1;
}

int adxl363_shutdown(void)

{
  int iVar1;
  
  adxl363_reg_write('\x1f','R');
  iVar1 = MXC_SPI_Shutdown(write_reg_req.spi);
  __reset_variables();
  return iVar1;
}

