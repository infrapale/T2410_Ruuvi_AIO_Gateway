
#include "Arduino.h"
#include "String.h"

char u4hex[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

// Converts hexadecimal values to decimal values
uint16_t hexadecimalToDecimal(String hexVal)
{
    uint8_t len = hexVal.length();
    uint16_t base = 1;
    int dec_val = 0;

    //Serial.print("hexadecimalToDecimal(");  Serial.print(hexVal);
    for (int i = len - 1; i >= 0; i--)
    {
        if (hexVal[i] >= '0' && hexVal[i] <= '9')
        {
            dec_val += (hexVal[i] - 48) * base;
            base = base * 16;
        }
        else if (hexVal[i] >= 'A' && hexVal[i] <= 'F')
        {
            dec_val += (hexVal[i] - 55) * base;
            base = base * 16;
        }
    }
    return dec_val;
};


String u8_to_hex(uint8_t u8)
{
    String res = "";
    res = String(u4hex[u8 >> 4]);
    res = res + String(u4hex[u8 & 0x0F]);
    return res;
}