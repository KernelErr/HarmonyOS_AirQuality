#include <stdio.h>
#include <unistd.h>
#include "cj702.h"
#include "ohos_init.h"
#include "cmsis_os2.h"
#include "wifi_connect.h"
#include "onenet_mqtt.h"

#define WiFi_SSID "***"
#define WiFi_PASSWORD "***"

CJ702_Data_Typedef CJ702_Data;

static void mqttReportTask(void) {
    WifiConnect(WiFi_SSID, WiFi_PASSWORD);
    mqttInit();
    while (1) {
        if (reportMQTT(CJ702_Data) < 0) {
            printf("[!] Failed to report data.\n");
        } else {
            printf("[*] Successfully reported data\n");
        }
        sleep(2);
    }
}

static void sensorReadTask(void) {
    cj702Init();
    while(1) {
        cj702ReadData();
        sleep(2);
    }
}

static void entry(void) {
    osThreadAttr_t attr;
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 10240;
    attr.priority = osPriorityNormal;

    attr.name = "MQTT_Report_Task";
    if (osThreadNew((osThreadFunc_t) mqttReportTask, NULL, &attr) == NULL) {
        printf("[!] Failed to create MQTT_Report_Task\n");
    }

    attr.name = "Sensor_Read_Task";
    if (osThreadNew((osThreadFunc_t) sensorReadTask, NULL, &attr) == NULL) {
        printf("[!] Failed to create Sensor_Read_Task\n");
    }
}

SYS_RUN(entry);