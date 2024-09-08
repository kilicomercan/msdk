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