#include <stdio.h>

int byteToOct(unsigned char h, unsigned char l) {
    int t = h << 8;
    t += l;
    return t;
}

float byteToFloat(unsigned char h, unsigned char l) {
    float t;
    char s[20];
    sprintf(s, "%d.%d", h, l);
    sscanf(s, "%f", &t);
    return t;
}