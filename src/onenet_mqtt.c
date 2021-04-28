#include "MQTTClient.h"
#include "cmsis_os2.h"
#include "cj702.h"
#include "cJSON.h"
#include <stdio.h>
#include <string.h>

#define ONENET_HOST "183.230.102.116"
#define ONENET_PORT 1883
#define ONENET_CID "***"
#define ONENET_USERNAME "***"
#define ONENET_PASSWORD "***"
#define ONENET_REPORT_TOPIC "***"


static int initLock = 0;
static MQTTClient mqttClient;

int messageID = -1;
int buffSize;
unsigned char *mqttBuf;
unsigned char *mqttReadBuf;
Network n;
MQTTPacket_connectData data = MQTTPacket_connectData_initializer;

static int mqttEntry(void) {
    int rc = 0;

    NetworkInit(&n);
    NetworkConnect(&n, ONENET_HOST, ONENET_PORT);
    buffSize = 2048;
    mqttBuf = (unsigned char *) malloc(buffSize);
    mqttReadBuf = (unsigned char *) malloc(buffSize);
    if (!(mqttBuf && mqttReadBuf)) {
        printf("[!] Can't calloc MQTT client buffer.\n");
        return -2;
    }

    MQTTClientInit(&mqttClient, &n, 1000, mqttBuf, buffSize, mqttReadBuf, buffSize);
    MQTTStartTask(&mqttClient);

    data.keepAliveInterval = 30;
    data.cleansession = 1;
    data.clientID.cstring = ONENET_CID;
    data.username.cstring = ONENET_USERNAME;
    data.password.cstring = ONENET_PASSWORD;

    data.keepAliveInterval = 10;
    data.cleansession = 1;

    rc = MQTTConnect(&mqttClient, &data);
    return rc;
}

int mqttInit(void) {
    int result = 0;
    if (initLock) {
        return 0;
    }

    if (mqttEntry() < 0) {
        printf("[!] Failed to initialize MQTT clent.\n");
        return result;
    }
    return result;
}

int reportMQTT(CJ702_Data_Typedef data) {
    if (data.valid == 0) {
        return 0;
    }
    messageID++;
    /*
        {
            "id": "123",
            "params": {
                "Power": {
                    "value": "12345",
                },
                "temp": {
                    "value": 23.6,
                }   
            }
        }
    */
    char s[30];
    cJSON *payload = cJSON_CreateObject();
    cJSON *id = NULL;
    cJSON *params = NULL;
    cJSON *param = NULL;
    cJSON *value = NULL;

    if (payload == NULL) {
        return -1;
    }
    
    sprintf(s, "%d", messageID);
    id = cJSON_CreateString(s);
    if (id == NULL) {
        return -1;
    }
    cJSON_AddItemToObject(payload, "id", id);
    
    params = cJSON_CreateObject();
    if (params == NULL) {
        return -1;
    }

    param = cJSON_CreateObject();
    if (param == NULL) {
        return -1;
    }
    value = cJSON_CreateNumber(data.co2);
    if (value == NULL) {
        return -1;
    }
    cJSON_AddItemToObject(param, "value", value);
    cJSON_AddItemToObject(params, "eCO2", param);

    param = cJSON_CreateObject();
    if (param == NULL) {
        return -1;
    }
    value = cJSON_CreateNumber(data.hcho);
    if (value == NULL) {
        return -1;
    }
    cJSON_AddItemToObject(param, "value", value);
    cJSON_AddItemToObject(params, "eCH2O", param);

    param = cJSON_CreateObject();
    if (param == NULL) {
        return -1;
    }
    value = cJSON_CreateNumber(data.tvoc);
    if (value == NULL) {
        return -1;
    }
    cJSON_AddItemToObject(param, "value", value);
    cJSON_AddItemToObject(params, "TVOC", param);

    param = cJSON_CreateObject();
    if (param == NULL) {
        return -1;
    }
    value = cJSON_CreateNumber(data.pm25);
    if (value == NULL) {
        return -1;
    }
    cJSON_AddItemToObject(param, "value", value);
    cJSON_AddItemToObject(params, "PM2_5", param);

    param = cJSON_CreateObject();
    if (param == NULL) {
        return -1;
    }
    value = cJSON_CreateNumber(data.pm10);
    if (value == NULL) {
        return -1;
    }
    cJSON_AddItemToObject(param, "value", value);
    cJSON_AddItemToObject(params, "PM10", param);

    param = cJSON_CreateObject();
    if (param == NULL) {
        return -1;
    }
    value = cJSON_CreateNumber(data.temp);
    if (value == NULL) {
        return -1;
    }
    cJSON_AddItemToObject(param, "value", value);
    cJSON_AddItemToObject(params, "Temperature", param);

    param = cJSON_CreateObject();
    if (param == NULL) {
        return -1;
    }
    value = cJSON_CreateNumber(data.hum);
    if (value == NULL) {
        return -1;
    }
    cJSON_AddItemToObject(param, "value", value);
    cJSON_AddItemToObject(params, "Humidity", param);

    cJSON_AddItemToObject(payload, "params", params);
    char *string = NULL;
    string = cJSON_PrintUnformatted(payload);
    
    MQTTMessage mqttMessage;
    mqttMessage.qos = QOS1;
    mqttMessage.retained = 0;
    mqttMessage.payload = (void *) string;
    mqttMessage.payloadlen = strlen(string);

    if (MQTTPublish(&mqttClient, ONENET_REPORT_TOPIC, &mqttMessage) < 0) {
        return -2;
    }

    cJSON_free(string);
    cJSON_Delete(payload);
    return 0;
}