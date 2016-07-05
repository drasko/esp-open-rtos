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


const char *req =
	"{\"jsonrpc\": \"2.0\", \"fnc\": 1, "
	"\"params\": [\"users\", \"wheel\", \"audio\", \"video\"], \"rto\": \"/user/1234/out\"}";

char rsp[32];


void digitalWrite(int argc, char *argv[])
{
    printf("FNC: digitalWrite()\n");
}

void digitalRead(int argc, char *argv[])
{
    printf("FNC: digitalRead()\n");
}


void electrolink(void *pvParameters)
{
    printf("Hello test!\n");

    /** ft is an array of function pointers */
    void (*ft[])(int argc, char *argv[]) = {NULL};

    /** Initialize it with our functions */
    ft[1] = digitalWrite;
    ft[2] = digitalRead;

    /** Then tell erpc library to use this lookup table */
    erpcSetFncTable(&ft);

    /** Now we can call the function */
    erpcCall(req, rsp);

    while (1)
      ;
}

void user_init(void)
{
    uart_set_baud(0, 115200);
    printf("SDK version:%s\n", sdk_system_get_sdk_version());
    xTaskCreate(electrolink, (signed char *)"elink", 1024, NULL, 2, NULL);
}
