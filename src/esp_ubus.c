#include "esp_ubus.h"


static const struct blobmsg_policy led_policy[] = {
    [LED_PORT] = {
        .name = "port",
        .type = BLOBMSG_TYPE_STRING
    },
    [LED_PIN] = {
        .name = "pin",
        .type = BLOBMSG_TYPE_INT32
    }
};

static const struct blobmsg_policy sensor_policy[] = {
    [SENSOR_PORT] = {
        .name = "port",
        .type = BLOBMSG_TYPE_STRING
    },
    [SENSOR_PIN] = {
        .name = "pin",
        .type = BLOBMSG_TYPE_INT32
    },
    [SENSOR] = {
        .name = "sensor",
        .type = BLOBMSG_TYPE_STRING
    },
    [MODEL] = {
        .name = "model",
        .type = BLOBMSG_TYPE_STRING
    }
};

static const struct ubus_method methods[] = {
    UBUS_METHOD_NOARG("device", get_devices),
    UBUS_METHOD("on", led_on, led_policy),
    UBUS_METHOD("off", led_off, led_policy),
    UBUS_METHOD("get", get_sensor, sensor_policy),
};

static struct ubus_object_type object_type = UBUS_OBJECT_TYPE("esp", methods);

static struct ubus_object object = {
    .name = "esp",
    .type = &object_type,
    .methods = methods,
    .n_methods = ARRAY_SIZE(methods),
};

int init_objects(struct ubus_context *ctx)
{
    return ubus_add_object(ctx, &object);
}


int get_devices(struct ubus_context *ctx,
                      struct ubus_object *obj,
                      struct ubus_request_data *req,
                      const char *method,
                      struct blob_attr *msg)
{
    struct blob_buf buf = {0};
    blob_buf_init(&buf, 0);

    iterate_port_list(&buf, VID, PID);

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

    if (!tb[LED_PORT] || !tb[LED_PIN])
        return UBUS_STATUS_INVALID_ARGUMENT;

    char *port_name = blobmsg_get_string(tb[LED_PORT]);
    int pin = blobmsg_get_u32(tb[LED_PIN]);
    
    struct blob_buf buf = {0};
    blob_buf_init(&buf, 0);
    int ret = set_led(port_name, pin, &buf, ctx, req, "on");
    blob_buf_free(&buf);

    return ret;
}

int led_off(struct ubus_context *ctx,
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

    if (!tb[LED_PORT] || !tb[LED_PIN])
        return UBUS_STATUS_INVALID_ARGUMENT;

    char *port_name = blobmsg_get_string(tb[LED_PORT]);
    int pin = blobmsg_get_u32(tb[LED_PIN]);
    
    struct blob_buf buf = {0};
    blob_buf_init(&buf, 0);
    int ret = set_led(port_name, pin, &buf, ctx, req, "off");
    blob_buf_free(&buf);

    return ret;
}

int get_sensor(struct ubus_context *ctx,
                struct ubus_object *obj,
                struct ubus_request_data *req,
                const char *method,
                struct blob_attr *msg)
{
	struct blob_attr *tb[__SENSOR_MAX] = {0};
	if(!msg){
		return UBUS_STATUS_INVALID_ARGUMENT;
	}
	blobmsg_parse(sensor_policy, __SENSOR_MAX, tb, blob_data(msg), blob_len(msg));
	if (!tb[SENSOR_PORT] || !tb[SENSOR_PIN] || !tb[SENSOR] || !tb[MODEL]){
		return UBUS_STATUS_INVALID_ARGUMENT;
	}

	char *port_name = blobmsg_get_string(tb[SENSOR_PORT]);
	int pin = blobmsg_get_u32(tb[SENSOR_PIN]);
	char *model = blobmsg_get_string(tb[SENSOR]);
	char *sensor = blobmsg_get_string(tb[MODEL]);

	struct blob_buf buf = {0};
	blob_buf_init(&buf, 0);
	int ret = read_sensor(port_name, pin, sensor, model, &buf, ctx, req);
	blob_buf_free(&buf);
	return ret;
}

static int set_led(char *port_name, int pin, struct blob_buf *buf, struct ubus_context *ctx, struct ubus_request_data *req, char *set)
{
    size_t err_sz = 120;
    char err[err_sz];

    int ret;
    int baudrate = 9600, bits = 8, parity_bit = SP_PARITY_NONE;
    int stop_bit = 1, flowcontrol = SP_FLOWCONTROL_NONE;

    int timeout = 2000;

    struct sp_port *port = {0};

    size_t data_sz = 32;
    char data[data_sz];

    size_t resp_sz = 128;
	char response[resp_sz];

    void *table = blobmsg_open_table(buf, "RESPONSE");
    if((ret = sp_get_port_by_name(port_name, &port)) != SP_OK){
        blobmsg_add_string(buf, "ERROR", "Cannot find a port by given name");
        goto end;
    }

    if((ret = init_port(port, baudrate, bits, parity_bit, stop_bit, flowcontrol)) != 1){
        snprintf(err, err_sz, "Unable to initialize port: %d", ret);
        blobmsg_add_string(buf, "ERROR", err);
        goto end;
    }

    if(snprintf(data, data_sz, "{\"action\": \"%s\", \"pin\": %d}", set, pin) < 0){
        blobmsg_add_string(buf, "ERROR", "Cannot format data string");
        goto end;
    }

    ret = sp_blocking_write(port, data, data_sz, timeout);
    if(ret != data_sz){
        snprintf(err, err_sz, "Unable to send command: %d", ret);
        blobmsg_add_string(buf, "ERROR", err);
        goto end;
    }

    ret = sp_blocking_read(port, response, resp_sz, timeout);
	
	if (ret > 0){		
        //response[ret] = '\0';
        if (!blobmsg_add_json_from_string(buf, response)) {
            blobmsg_add_string(buf, "ERROR", "Invalid json returned");
        }
	}
	end:
	sp_close(port);
	sp_free_port(port);
	
    blobmsg_close_table(buf, table);
	ubus_send_reply(ctx, req, buf->head);
	return UBUS_STATUS_OK;

}

int read_sensor(char *port_name, int pin, char *sensor, char *model, struct blob_buf *buf, struct ubus_context *ctx, struct ubus_request_data *req)
{			
	size_t err_sz = 120;
	char err[err_sz];

	int ret;
	int baudrate = 9600, bits = 8, parity_bit = SP_PARITY_NONE;
	int stop_bit = 1, flowcontrol = SP_FLOWCONTROL_NONE;
	int timeout = 3000;
	struct sp_port *port = {0};
	size_t data_sz = 64;
	char data[data_sz];

	size_t resp_sz = 128;
	char response[resp_sz];

    void *table = blobmsg_open_table(buf, "RESPONSE");
	if((ret = sp_get_port_by_name(port_name, &port)) != SP_OK){
		blobmsg_add_string(buf, "ERROR", "Cannot find a port by given name");
		goto end;
	}

    int port_vid, port_pid;
    if(sp_get_port_usb_vid_pid(port, &port_vid, &port_pid) != SP_OK){
        blobmsg_add_string(buf, "ERROR", "Unable to check port vid:pid");
        goto end;
    }

    if(port_vid != VID || port_pid != PID){
        blobmsg_add_string(buf, "ERROR", "Wrong device type");
        goto end;
    }


	if((ret = init_port(port, baudrate, bits, parity_bit, stop_bit, flowcontrol)) != 1){
		snprintf(err, err_sz, "Unable to initialize port: %d", ret);
		blobmsg_add_string(buf, "ERROR", err);
		goto end;
	}

	if(snprintf(data, data_sz, "{\"action\": \"get\", \"pin\": %d, \"sensor\":\"%s\", \"model\":\"%s\"}", pin, sensor, model) < 0){
		blobmsg_add_string(buf, "ERROR", "Cannot format data string");
		goto end;
	}
    	
	ret = sp_blocking_write(port, data, data_sz, timeout);
	if(ret != data_sz){
		blobmsg_add_string(buf, "ERROR", "Unable to send get command");
		goto end;
	}

	ret = sp_blocking_read(port, response, resp_sz, timeout);
	
	if (ret > 0){		
//		    response[ret] = '\0';
    		if (!blobmsg_add_json_from_string(buf, response)) {
        	    blobmsg_add_string(buf, "ERROR", "Invalid json returned");
    		}
	}
    blobmsg_close_table(buf, table);
	ubus_send_reply(ctx, req, buf->head);

	end:
	sp_close(port);
	sp_free_port(port);
	return UBUS_STATUS_OK;
}
