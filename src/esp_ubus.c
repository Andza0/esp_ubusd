#include "esp_ubus.h"

int get_devices(struct ubus_context *ctx,
                      struct ubus_object *obj,
                      struct ubus_request_data *req,
                      const char *method,
                      struct blob_attr *msg)
{
    struct blob_buf buf = {0};
    blob_buf_init(&buf, 0);

    iterate_port_list(&buf);

    int ret = ubus_send_reply(ctx, req, buf.head);
    blob_buf_free(&buf);
    return ret;
}

int led_on(struct ubus_context *ctx,
            struct ubus_object *obj,
            struct ubus_request_data *req,
            const char *method,
            struct blob_attr *msg)
{
    struct blob_attr *tb[__LED_MAX] = {0};

    if (!msg) {
        return UBUS_STATUS_INVALID_ARGUMENT;
    }

    blobmsg_parse(led_policy, __LED_MAX, tb,
                  blob_data(msg), blob_len(msg));

    if (!tb[PORT] || !tb[PIN])
        return UBUS_STATUS_INVALID_ARGUMENT;

    char *port_name = blobmsg_get_string(tb[PORT]);
    int pin = blobmsg_get_int16(tb[PIN]);

    struct blob_buf buf = {0};
    blob_buf_init(&buf, 0);
    struct sp_port *port = {0};
    if(sp_get_port_by_name(port_name, &port) != SP_OK){
        blobmsg_add_string(&buf, "ERROR", "Cannot find a port by given name");
        int ret = ubus_send_reply(ctx, req, buf.head);
        blob_buf_free(&buf);
        return UBUS_STATUS_INVALID_ARGUMENT;
    }

    int ret;
    int baudrate = 9600, bits = 8, parity_bit = SP_PARITY_NONE;
    int stop_bit = 1, flowcontrol = SP_FLOWCONTROL_NONE;
    if((ret = init_port(port, baudrate, bits, parity_bit, stop_bit, flowcontrol)) != 1){
        char err[120];
        snprintf(err, 120, "Unable to initialize port: %d", ret);
        blobmsg_add_string(&buf, "ERROR", err);
        int ret = ubus_send_reply(ctx, req, buf.head);
        blob_buf_free(&buf);
        return UBUS_STATUS_UNKNOWN_ERROR;
    }

    int timeout = 2000;
    char data[32];
    if(snprintf(data, 56, "{\"action\": \"on\", \"pin\": %d}", pin) < 0){
        blobmsg_add_string(&buf, "ERROR", "Cannot format data string");
        int ret = ubus_send_reply(ctx, req, buf.head);
        blob_buf_free(&buf);
        return UBUS_STATUS_UNKNOWN_ERROR;
    }

    ret = sp_blocking_write(port, data, 32, timeout);

    if(ret != 32){
        snprintf(err, 120, "Unable to initialize port: %d", ret);
        blobmsg_add_string(&buf, "ERROR", err);
        int ret = ubus_send_reply(ctx, req, buf.head);
        blob_buf_free(&buf);
        return UBUS_STATUS_UNKNOWN_ERROR;
    }

    return UBUS_STATUS_OK;
}

int led_off(struct ubus_context *ctx,
            struct ubus_object *obj,
            struct ubus_request_data *req,
            const char *method,
            struct blob_attr *msg);

int get_sensor(struct ubus_context *ctx,
                struct ubus_object *obj,
                struct ubus_request *req,
                const char *method,
                struct blob_attr *msg);