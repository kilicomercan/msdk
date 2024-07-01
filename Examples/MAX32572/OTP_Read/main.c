#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "mxc_device.h"
#include "otp.h"
#include "otp_regs.h"
#include "mxc_delay.h"

/* Macro Definitions */
#define OTP_MANUFACTURER_MEM_BASE_ADDR   0x0000
#define OTP_USER_AVAILABLE_MEM_BASE_ADDR 0x0400

/* Global variables */
static uint8_t otp_buff[MXC_OTP_MEM_SIZE / 2];

/* Functions */
int otp_read_test(uint16_t addr, uint32_t *read_data, uint16_t size){

    int ret_val = 0;
    // Initialize OTP peripheral
    ret_val = MXC_OTP_Init(MXC_OTP_CLK_DIV2);
    if(E_NO_ERROR != ret_val){
        return ret_val;
    }

    // Unlock OTP region to read.
    MXC_OTP_Unlock();

    ret_val = MXC_OTP_IsLocked();
    if(ret_val != 0){
        // If OTP is locked, unlock.
        return -1;
    }

    ret_val = MXC_OTP_Read(addr, read_data, size);
    
    MXC_OTP_Lock();

    return ret_val;
}


//******************************************************************************
int main(void)
{
    printf("\n\n***** OTP Memory Read Example *****\n");
    printf("***** This example demonstrates how to read OTP memory *****\n");
    //command added
    /* Dump manufacturer section */
    printf("\n\n***** MANUFACTURER AREA *****\n");
    otp_read_test(OTP_MANUFACTURER_MEM_BASE_ADDR, (uint32_t*)otp_buff, sizeof(otp_buff));

    int i = 0;
    while( i< (MXC_OTP_MEM_SIZE/2)) {
        printf("0x%X", otp_buff[i]);
        i++;
    }

    /* Clean buffer after print */
    memset(otp_buff, 0, sizeof(otp_buff));
    MXC_Delay(MXC_DELAY_MSEC(1000));


    printf("\n\n***** USER AREA *****\n");
    otp_read_test(OTP_USER_AVAILABLE_MEM_BASE_ADDR, (uint32_t*)otp_buff, sizeof(otp_buff));
    i = 0;
    while( i< (MXC_OTP_MEM_SIZE/2)) {
        printf("0x%X", otp_buff[i]);
        i++;
    }

    return 0;
}
