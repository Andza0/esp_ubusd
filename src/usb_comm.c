#include "usb_comm.h"

int get_port_vidpid(int vid, int pid, struct sp_port **port)
{
    struct sp_port **port_list;

    if(sp_list_ports(&port_list) != SP_OK){
        return ret;
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
            return ret;
        }

        sp_free_port_list(port_list);
        printf("port found\n");
        return 1;
    }

    sp_free_port_list(port_list);
    return 0;

}

int init_port(struct sp_port *port, int baudrate, int bits, int parity_bit, int stop_bit, int flowcontrol)
{   
    int ret;
    if((ret = sp_open(port, SP_MODE_READ_WRITE)) != SP_OK){
        return ret;
    }

    if((ret = sp_set_baudrate(port, baudrate)) != SP_OK){
        return ret;
    }

    if((ret = sp_set_bits(port, bits)) != SP_OK){
        return ret;
    }

    if((ret = sp_set_parity(port, parity_bit)) != SP_OK){
        return ret;
    }

    if((ret = sp_set_stopbits(port, stop_bit)) != SP_OK){
        return ret;
    }

    if((ret = sp_set_flowcontrol(port, flowcontrol)) != SP_OK){
        return ret;
    }

    return 1;
}

int iterate_port_list(struct blob_buf *buf)
{
    struct sp_port **port_list;

    if(sp_list_ports(&port_list) != SP_OK){
        blobmsg_add_string(buf, "ERROR", "Can't get port list");
        return ret;
    }
    char *name;
    int vid, pid;
    struct sp_port *current;

    void *dev_table = blobmsg_open_table(buf, "devices");
    for(int i = 0; port_list[i] != NULL; i++){

        current = port_list[i];
        name = sp_get_port_name(current);
        sp_get_port_usb_vid_pid(current, &vid, &pid);

        if((name = sp_get_port_name(current)) == NULL || sp_get_port_usb_vid_pid(current, &vid, &pid) == 0){
            blobmsg_add_string(buf, "ERROR", "Incorrect port");
        }

        void *port_table = blobmsg_open_table(buf, name);
        blobmsg_add_u32(buf, "Vendor ID", vid);
        blobmsg_add_u32(buf, "Product ID", pid);
        blobmsg_close_table(buf, port_table);

    }

    blobmsg_close_table(buf, dev_table);

    sp_free_port_list(port_list);
    return 1;
}