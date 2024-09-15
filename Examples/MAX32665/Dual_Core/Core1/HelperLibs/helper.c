#include "helper.h"

void send_pack_to_host(adxl363_sample_pkg_t *sample_pack)
{
    /**
     * This function will support different protocols to communicate
     * with Host computer.
     * @todo BLE support will be added. For now, we use only usb serial
     * communication.
     * */

    printf("%u.%u %hd %hd %hd\r\n", sample_pack->sec, sample_pack->subsec, sample_pack->x_val,
           sample_pack->y_val, sample_pack->z_val);
}


void print_complete_pack(uint8_t *pack){
    int sizeof_pack = pack[0];
    int counter = 0;

                
    while(counter < sizeof_pack){
        send_pack_to_host((adxl363_sample_pkg_t*)&pack[counter*(1+(sizeof(adxl363_sample_pkg_t)))+1]);
        counter +=1;
    }
}