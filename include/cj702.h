#ifndef _CJ702_H_
#define _CJ702_H_

typedef unsigned char byte;

typedef struct {
    int valid;
    int hcho;
    int co2;
    int tvoc;
    int pm25;
    int pm10;
    float temp;
    float hum;
} CJ702_Data_Typedef;

extern CJ702_Data_Typedef CJ702_Data;

void cj702Init(void);
void cj702ReadData(void);

#endif