#include <libserialport.h>
#include <stdio.h>
#include <string.h>
#include <libubox/blobmsg.h>
#include <libubox/blobmsg_json.h>

int get_port_vidpid(int vid, int pid, struct sp_port **port);
int init_port(struct sp_port *port, int baudrate, int bits, int parity_bit, int stop_bit, int flowcontrol);
int iterate_port_list(struct blob_buf *buf);