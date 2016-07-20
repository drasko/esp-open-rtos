/**
 * Copyright (c) Project Iota
 *
 * uCbor is licensed under an Apache license, version 2.0 license.
 * All rights not explicitly granted in the Apache license, version 2.0 are reserved.
 * See the included LICENSE file for more details.
 */
#include "espressif/esp_common.h"
#include "esp/uart.h"
#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "erpc.h"

#include <ssid_config.h>

#include <espressif/esp_sta.h>
#include <espressif/esp_wifi.h>

#include <paho_mqtt_c/MQTTESP8266.h>
#include <paho_mqtt_c/MQTTClient.h>

#include <semphr.h>


/* You can use http://test.mosquitto.org/ to test mqtt_client instead
 * of setting up your own MQTT server */
#define MQTT_HOST ("test.mosquitto.org")
#define MQTT_PORT 1883

#define MQTT_USER NULL
#define MQTT_PASS NULL

xSemaphoreHandle wifi_alive;
xQueueHandle publish_queue;
#define PUB_MSG_LEN 16

static int elSend(void *msg)
{
    int rc = 0;
    char msg[PUB_MSG_LEN];

    printf("beat\r\n");
    if (xQueueSend(publish_queue, (void *)msg, 0) == pdFALSE) {
        printf("Publish queue overflow.\r\n");
        rc = -1;
    }

    return rc;
}


const char *req =
	"{\"jsonrpc\": \"2.0\", \"fnc\": 1, "
	"\"params\": [\"users\", \"wheel\", \"audio\", \"video\"], \"rto\": \"/user/1234/out\"}";

char rsp[32];


void digitalWrite(int argc, char *argv[])
{
    printf("FNC: digitalWrite()\n");

    int gpio = argv[0];
    int val = argv[1];

    gpio_enable(gpio, GPIO_OUTPUT);

    gpio_write(gpio, val);
}

void digitalRead(int argc, char *argv[])
{
    printf("FNC: digitalRead()\n");

    int val = -1;
    gpio_enable(gpio, GPIO_INPUT);

    val = gpio_read(argv[0]);

    elSend(val);

}


void elInit(void)
{
    printf("Hello test!\n");

    /** ft is an array of function pointers */
    void (*ft[])(int argc, char *argv[]) = {NULL};

    /** Initialize it with our functions */
    ft[1] = digitalWrite;
    ft[2] = digitalRead;

    /** Then tell erpc library to use this lookup table */
    erpcSetFncTable(&ft);

    elInitialized = true;
}

/**
 * Message Received
 */
static void messageReceived(MessageData *md)
{
    int i;
    MQTTMessage *message = md->message;
    printf("Received: ");
    for( i = 0; i < md->topic->lenstring.len; ++i)
        printf("%c", md->topic->lenstring.data[i]);

    printf(" = ");
    for( i = 0; i < (int)message->payloadlen; ++i)
        printf("%c", ((char *)(message->payload))[i]);


    /** Now we can call the function */
    erpcCall(req, rsp);

    printf("\r\n");
}

/**
 * MQTT
 */
static void mqtt_task(void *pvParameters)
{
    int ret = 0;
    struct Network network;
    MQTTClient client = DefaultClient;
    char mqtt_client_id[20];
    uint8_t mqtt_buf[100];
    uint8_t mqtt_readbuf[100];
    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;

    /** Initialize Electrolink Fuction Table */
    elInit();

    NewNetwork(&network);
    memset(mqtt_client_id, 0, sizeof(mqtt_client_id));
    strcpy(mqtt_client_id, "ESP-");
    strcat(mqtt_client_id, get_my_id());

    while (1) {
        xSemaphoreTake(wifi_alive, portMAX_DELAY);
        printf("%s: started\n\r", __func__);
        printf("%s: (Re)connecting to MQTT server %s ... ",__func__,
               MQTT_HOST);
        ret = ConnectNetwork(&network, MQTT_HOST, MQTT_PORT);
        if (ret) {
            printf("error: %d\n\r", ret);
            taskYIELD();
            continue;
        }
        printf("done\n\r");
        NewMQTTClient(&client, &network, 5000, mqtt_buf, 100,
                      mqtt_readbuf, 100);

        data.willFlag = 0;
        data.MQTTVersion = 3;
        data.clientID.cstring = mqtt_client_id;
        data.username.cstring = MQTT_USER;
        data.password.cstring = MQTT_PASS;
        data.keepAliveInterval = 10;
        data.cleansession = 0;
        printf("Send MQTT connect ... ");
        ret = MQTTConnect(&client, &data);
        if (ret) {
            printf("error: %d\n\r", ret);
            DisconnectNetwork(&network);
            taskYIELD();
            continue;
        }
        printf("done\r\n");
        MQTTSubscribe(&client, "/electrolib", QOS1, messageReceived);
        xQueueReset(publish_queue);

        while (1) {
            char msg[PUB_MSG_LEN - 1] = "\0";
            while(xQueueReceive(publish_queue, (void *)msg, 0) ==
                  pdTRUE){
                printf("got message to publish\r\n");
                MQTTMessage message;
                message.payload = msg;
                message.payloadlen = PUB_MSG_LEN;
                message.dup = 0;
                message.qos = QOS1;
                message.retained = 0;
                ret = MQTTPublish(&client, "/beat", &message);
                if (ret != SUCCESS) {
                    printf("error while publishing message: %d\n", ret );
                    break;
                }
            }

            ret = MQTTYield(&client, 1000);
            if (ret == DISCONNECTED)
                break;
        }
        printf("Connection dropped, request restart\n\r");
        taskYIELD();
    }
}


/**
 * WiFi task
 */
static void  wifi_task(void *pvParameters)
{
    uint8_t status  = 0;
    uint8_t retries = 30;
    struct sdk_station_config config = {
        .ssid = WIFI_SSID,
        .password = WIFI_PASS,
    };

    printf("WiFi: connecting to WiFi\n\r");
    sdk_wifi_set_opmode(STATION_MODE);
    sdk_wifi_station_set_config(&config);

    while(1)
    {
        while ((status != STATION_GOT_IP) && (retries)){
            status = sdk_wifi_station_get_connect_status();
            printf("%s: status = %d\n\r", __func__, status );
            if( status == STATION_WRONG_PASSWORD ){
                printf("WiFi: wrong password\n\r");
                break;
            } else if( status == STATION_NO_AP_FOUND ) {
                printf("WiFi: AP not found\n\r");
                break;
            } else if( status == STATION_CONNECT_FAIL ) {
                printf("WiFi: connection failed\r\n");
                break;
            }
            vTaskDelay(1000 / portTICK_RATE_MS);
            --retries;
        }
        if (status == STATION_GOT_IP) {
            printf("WiFi: Connected\n\r");
            xSemaphoreGive(wifi_alive);
            taskYIELD();
        }

        while ((status = sdk_wifi_station_get_connect_status()) == STATION_GOT_IP) {
            xSemaphoreGive(wifi_alive);
            taskYIELD();
        }
        printf("WiFi: disconnected\n\r");
        sdk_wifi_station_disconnect();
        vTaskDelay(1000 / portTICK_RATE_MS);
    }
}

void user_init(void)
{
    uart_set_baud(0, 115200);
    printf("SDK version:%s\n", sdk_system_get_sdk_version());

    vSemaphoreCreateBinary(wifi_alive);
    publish_queue = xQueueCreate(3, PUB_MSG_LEN);
    xTaskCreate(&wifi_task, (int8_t *)"wifi_task",  256, NULL, 2, NULL);
    xTaskCreate(&mqtt_task, (int8_t *)"mqtt_task", 1024, NULL, 4, NULL);
}
