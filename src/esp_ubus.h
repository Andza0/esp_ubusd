#include <libubus.h>
#include <libubox/blobmsg.h>
#include <libubox/blobmsg_json.h>
#include <libubox/uloop.h>
#include <stdio.h>
#include "usb_comm.h"

enum
{
    PORT,
    PIN,
    __LED_MAX
};

enum
{
    PORT,
    PIN,
    MODEL,
    SENSOR,
    __SENSOR_MAX
};

const struct blobmsg_policy led_policy[] = {
    [PORT] = {
        .name = "port",
        .type = BLOBMSG_TYPE_STRING
    },
    [PIN] = {
        .name = "pin",
        .type = BLOBMSG_TYPE_INT16
    }
};

const struct blobmsg_policy sensor_policy[] = {
    [PORT] = {
        .name = "port",
        .type = BLOBMSG_TYPE_STRING
    },
    [PIN] = {
        .name = "pin",
        .type = BLOBMSG_TYPE_INT16
    },
    [MODEL] = {
        .name = "model",
        .type = BLOBMSG_TYPE_STRING
    },
    [SENSOR] = {
        .name = "sensor",
        .type = BLOBMSG_TYPE_STRING
    }
};

int led_on(struct ubus_context *ctx,
            struct ubus_object *obj,
            struct ubus_request_data *req,
            const char *method,
            struct blob_attr *msg);

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


int get_devices(struct ubus_context *ctx,
                struct ubus_object *obj,
                struct ubus_request_data *req,
                const char *method,
                struct blob_attr *msg);

static const struct ubus_method methods[] = {
    UBUS_METHOD_NOARG("device", get_devices)
};

static struct ubus_object_type object_type = UBUS_OBJECT_TYPE("esp", methods);

static struct ubus_object object = {
    .name = "esp",
    .type = &object_type,
    .methods = methods,
    .n_methods = 1,
};
