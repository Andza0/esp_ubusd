#ifndef ESP_UBUS_H_
#define ESP_UBUS_H_

#include <libubus.h>
#include <libubox/blobmsg.h>
#include <libubox/blobmsg_json.h>
#include <libubox/uloop.h>
#include <stdio.h>
#include "usb_comm.h"

enum
{
    LED_PORT,
    LED_PIN,
    __LED_MAX
};

enum
{
    SENSOR_PORT,
    SENSOR_PIN,
    SENSOR,
    MODEL,
    __SENSOR_MAX
};

int init_objects(struct ubus_context *ctx);

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
                struct ubus_request_data *req,
                const char *method,
                struct blob_attr *msg);


int get_devices(struct ubus_context *ctx,
                struct ubus_object *obj,
                struct ubus_request_data *req,
                const char *method,
                struct blob_attr *msg);

static int set_led(char *port_name, int pin, struct blob_buf *buf, struct ubus_context *ctx, struct ubus_request_data *req, char *set);

int read_sensor(char *port_name, int pin, char *model, char *sensor, struct blob_buf *buf, struct ubus_context *ctx, struct ubus_request_data *req);

#endif
