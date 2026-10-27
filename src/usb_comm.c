#include "usb_comm.h"

int get_port_vidpid(int vid, int pid, struct sp_port *port)
{
    struct sp_port **port_list;

    if(sp_list_port_list(&port_list) != SP_OK){
        return -1;
    }

    struct sp_port *current;
    for(current = port_list[0]; current != NULL; current = port_list++){

        if(sp_get_port_transport(current) != SP_TRANSPORT_USB){
            continue;
        }

        int dev_vid, dev_pid;
        sp_get_port_usb_vid_pid(current, &dev_vid, &dev_pid);

        if(vid == NULL || pid == NULL){
            continue;
        }

        if(dev_vid != vid || dev_pid != pid){
            continue;
        }

        

    }

    sp_free_port_list(port_list);

}