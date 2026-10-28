#include <stdio.h>
#include "usb_comm.h"

#define VID 0x10c4
#define PID 0xea60

int main(void){

    struct sp_port *port = {0};

    if(get_port_vidpid(VID, PID, &port) != 1){
        printf("Failed to find port\n");
    }

    return 1;
}