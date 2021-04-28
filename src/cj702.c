#include <stdio.h>
#include <unistd.h>
#include "cmsis_os2.h"
#include "wifiiot_uart.h"
#include "wifiiot_errno.h"
#include "cj702.h"
#include "misc.h"

void cj720PhaseData(byte* data);
void cj720Print(byte* data);

void cj702Init(void) {
    uint32_t ret;

    WifiIotUartAttribute uart_attr = {
        .baudRate = 9600,
        .dataBits = 8,
        .stopBits = 1,
        .parity = 0,
    };

    ret = UartInit(WIFI_IOT_UART_IDX_1, &uart_attr, NULL);
    if (ret != WIFI_IOT_SUCCESS) {
        printf("[!] Failed to init uart, errcode = %d.\n", ret);
        return;
    }
    printf("[*] UART sucessfully initailized.\n");
}

void cj702ReadData(void) {
    byte buff[17];
    UartRead(WIFI_IOT_UART_IDX_1, buff, 17);
    cj720PhaseData(buff);
    if (CJ702_Data.valid != 0) {
        cj720Print(buff);
    } else {
        printf("[!] Found invalid data.\n");
    }
}

void cj720PhaseData(byte* data) {
    if (data[0] != 60 || data[1] != 2) {
        printf("[!] Invalid header data.\n");
        CJ702_Data.valid = 0;
        return;
    }
    CJ702_Data.co2 = byteToOct(data[2], data[3]);
    CJ702_Data.hcho = byteToOct(data[4], data[5]);
    CJ702_Data.tvoc = byteToOct(data[6], data[7]);
    CJ702_Data.pm25 = byteToOct(data[8], data[9]);
    CJ702_Data.pm10 = byteToOct(data[10], data[11]);
    CJ702_Data.temp = byteToFloat(data[12], data[13]);
    CJ702_Data.hum = byteToFloat(data[14], data[15]);
    byte checkSum = 0;
    for (int i = 0; i <= 15; i++) {
        checkSum += data[i];
    }
    if (checkSum != data[16]) {
        printf("[!] Invalid checksum.\n");
        CJ702_Data.valid = 0;
        return;
    }
    CJ702_Data.valid = 1;
}

void cj720Print(byte* data) {
    printf("[*] Data from CJ702:\n");
    printf("eCO2: %d ppm\n", CJ702_Data.co2);
    printf("eCH2O: %d ug/m3\n", CJ702_Data.hcho);
    printf("TVOC: %d ug/m3\n", CJ702_Data.tvoc);
    printf("PM2.5: %d ug/m3\n", CJ702_Data.pm25);
    printf("PM10: %d ug/m3\n", CJ702_Data.pm10);
    printf("Temp: %f C\n", CJ702_Data.temp);
    printf("Hum: %f %%\n", CJ702_Data.hum);
    printf("Original: ");
    for (int i = 0; i < 17; i++) {
        printf("%d ", data[i]);
    }
    printf("\n\n");
}