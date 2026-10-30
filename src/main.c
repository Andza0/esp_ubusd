#include <stdio.h>
#include <string.h>
#include "esp_ubus.h"

#define VID 0x10c4
#define PID 0xea60

int main(void){
    uloop_init();

    struct ubus_context *ctx = ubus_connect(NULL);
    if (!ctx) {
        fprintf(stderr, "Failed to connect to ubus\n");
        uloop_done();
        return 1;
    }

    ubus_add_uloop(ctx);

    int ret = init_objects(ctx);
    if (ret) {
        fprintf(stderr, "Registration failed: %s\n",
                ubus_strerror(ret));
        ubus_free(ctx);
        uloop_done();
        return 1;
    }

    printf("Object registered!\n");

    uloop_run();

    ubus_free(ctx);
    uloop_done();
    return 0;
}
