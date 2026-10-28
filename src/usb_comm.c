#include "usb_comm.h"

int get_port_vidpid(int vid, int pid, struct sp_port **port)
{
    struct sp_port **port_list;

    if(sp_list_ports(&port_list) != SP_OK){
        return -1;
    }

    struct sp_port *current;
    for(int i = 0; port_list[i] != NULL; i++){

        current = port_list[i];
        if(sp_get_port_transport(current) != SP_TRANSPORT_USB){
            continue;
        }

        int dev_vid, dev_pid;
        if(sp_get_port_usb_vid_pid(current, &dev_vid, &dev_pid) != SP_OK){
            printf("Unable to get port's VID and PID");
            continue;
        }

        if(dev_vid != vid || dev_pid != pid){
            continue;
        }

        if(sp_copy_port(current, port) != SP_OK){
            printf("Cannot copy port.\n");
            sp_free_port_list(port_list);
            return -1;
        }

        sp_free_port_list(port_list);
        printf("port found\n");
        return 1;
    }

    sp_free_port_list(port_list);
    return 0;

}