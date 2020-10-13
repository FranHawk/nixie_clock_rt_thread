/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-10-02     Alpha       the first version
 */

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <rtthread.h>


#define DBG_ENABLE
#define DBG_SECTION_NAME    "mqtt.sample"
#define DBG_LEVEL           DBG_LOG
#define DBG_COLOR
#include <rtdbg.h>

#include "paho_mqtt.h"

#include "display_thread.h"


#include "cJSON.h"

#define MQTT_URI                "tcp://a1w0XJbXwh0.iot-as-mqtt.cn-shanghai.aliyuncs.com:1883"
#define MQTT_CLIENTID           "12345|securemode=3,signmethod=hmacsha1|"
#define MQTT_USERNAME           "nixie_clock&a1w0XJbXwh0"
#define MQTT_PASSWORD           "8D01913086A8D950D89DED4273D1EBF769B6EF3C"
#define MQTT_SUBTOPIC           "/sys/a1w0XJbXwh0/nixie_clock/thing/service/property/set"
#define MQTT_PUBTOPIC           "/sys/a1w0XJbXwh0/nixie_clock/thing/event/property/post"
#define MQTT_WILLMSG            "Goodbye!"

/* define MQTT client context */
static MQTTClient client;
static int is_started = 0;


static void mqtt_connect_callback(MQTTClient *c)
{
    LOG_D("inter mqtt_connect_callback!");
}

static void mqtt_online_callback(MQTTClient *c)
{
    LOG_D("inter mqtt_online_callback!");
}

static void mqtt_offline_callback(MQTTClient *c)
{
    LOG_D("inter mqtt_offline_callback!");
}

static void mqtt_sub_callback(MQTTClient *c, MessageData *msg_data)
{
    //uint8_t display_num;
    *((char *)msg_data->message->payload + msg_data->message->payloadlen) = '\0';
    LOG_D("mqtt sub callback:%s",(char *)msg_data->message->payload);

    cJSON* cjson_test = NULL;
    cJSON* cjson_items = NULL;
    cJSON* cjson_display = NULL;
    cJSON* cjson_display_value = NULL;
    cjson_test = cJSON_Parse((char *)msg_data->message->payload);
        if(cjson_test == NULL)
        {
            LOG_D("parse fail.\n");
            return;
        }

        cjson_items = cJSON_GetObjectItem(cjson_test,"items");
        cjson_display = cJSON_GetObjectItem(cjson_items,"Display");
        cjson_display_value = cJSON_GetObjectItem(cjson_display,"value");
        display_num = cjson_display_value->valueint;

        cJSON_Delete(cjson_test);

        LOG_D("display number:%d",display_num);


}

static void mqtt_sub_default_callback(MQTTClient *c, MessageData *msg_data)
{

}


int mqtt_start()
{
    /* init condata param by using MQTTPacket_connectData_initializer */
    MQTTPacket_connectData condata = MQTTPacket_connectData_initializer;
    static char cid[20] = { 0 };



    if (is_started)
    {
        LOG_E("mqtt client is already connected.");
        return -1;
    }
    /* config MQTT context param */
    {
        client.isconnected = 0;
        client.uri = MQTT_URI;


        rt_snprintf(cid, sizeof(cid), "rtthread%d", rt_tick_get());
                /* config connect param */
        memcpy(&client.condata, &condata, sizeof(condata));


        /* generate the random client ID */
        client.condata.MQTTVersion = 3;
        client.condata.clientID.cstring = MQTT_CLIENTID;
        client.condata.keepAliveInterval = 30;
        client.condata.cleansession = 1;
        client.condata.username.cstring = MQTT_USERNAME;
        client.condata.password.cstring = MQTT_PASSWORD;

        /* config MQTT will param. */
        client.condata.willFlag = 1;
        client.condata.will.qos = 1;
        client.condata.will.retained = 0;
        client.condata.will.topicName.cstring = MQTT_PUBTOPIC;
        client.condata.will.message.cstring = MQTT_WILLMSG;

        /* malloc buffer. */
        client.buf_size = client.readbuf_size = 2048;
        client.buf = rt_calloc(1, client.buf_size);
        client.readbuf = rt_calloc(1, client.readbuf_size);
        if (!(client.buf && client.readbuf))
        {
            LOG_E("no memory for MQTT client buffer!");
            return -1;
        }

        /* set event callback function */
        client.connect_callback = mqtt_connect_callback;
        client.online_callback = mqtt_online_callback;
        client.offline_callback = mqtt_offline_callback;

        /* set subscribe table and event callback */
        client.messageHandlers[0].topicFilter = rt_strdup(MQTT_SUBTOPIC);
        client.messageHandlers[0].callback = mqtt_sub_callback;
        client.messageHandlers[0].qos = QOS1;

        /* set default subscribe event callback */
        client.defaultMessageHandler = mqtt_sub_default_callback;
    }

    /* run mqtt client */
    paho_mqtt_start(&client);
    is_started = 1;
    LOG_D("mqtt_thread init succeed!");
    return 0;
}




