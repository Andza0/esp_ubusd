#include <stdio.h>
#include <string.h>
#include <syslog.h>
#include "esp_ubus.h"

int main(void){
    openlog("esp_ubusd", LOG_PID, LOG_DAEMON);
    
    uloop_init();

    struct ubus_context *ctx = ubus_connect(NULL);
    if (!ctx) {
        syslog(LOG_ERR, "Failed to connect to ubus");
        uloop_done();
        return 1;
    }

    ubus_add_uloop(ctx);

    int ret = init_objects(ctx);
    if (ret) {
        syslog(LOG_ERR, "Registration failed: %s\n",
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
