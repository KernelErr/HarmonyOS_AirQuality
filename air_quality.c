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
        int result = reportMQTT(CJ702_Data);
        if (result < 0) {
            printf("[!] Failed to report data, errcode = %d.\n", result);
        } else {
            printf("[*] Successfully reported data.\n");
        }
        sleep(2);
    }
}

static void sensorReadTask(void) {
    GpioInit();
    IoSetFunc(WIFI_IOT_IO_NAME_GPIO_2, WIFI_IOT_IO_FUNC_GPIO_2_GPIO);
    GpioSetDir(WIFI_IOT_IO_NAME_GPIO_2, WIFI_IOT_GPIO_DIR_OUT);
    cj702Init();
    while(1) {
        while (cj702ReadData() != 0) {
            usleep(1500000);
            printf("[!] Retrying to read data.\n");
        }
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