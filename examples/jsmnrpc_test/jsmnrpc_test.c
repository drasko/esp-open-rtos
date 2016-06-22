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
#include <sys/time.h>
#include "jsmnrpc.h"


const char *req =
	"{\"jsonrpc\": \"2.0\", \"method\": \"digitalWrite\", "
	"\"params\": [\"users\", \"wheel\", \"audio\", \"video\"], \"id\": 1}";

char rsp[32];

void jsmnrpc_test(void *pvParameters)
{
    printf("Hello test!\n");

    jrpcCall(req, rsp);

    while(1)
        ;
}

void user_init(void)
{
    uart_set_baud(0, 115200);
    printf("SDK version:%s\n", sdk_system_get_sdk_version());
    xTaskCreate(jsmnrpc_test, (signed char *)"jsmnrpct", 1024, NULL, 2, NULL);
}
