#ifndef CONFIG
#define CONFIG

#include <Arduino.h>
//Define Adress
const int I2CSLAVE = 2;

//Define i2c PIN
struct pinArd
{
    byte SDA = 4;
    byte SCL = 5;
} pinArduino;

//alamat host database firebase
#define HOST "smartcity-gbaz-ae18-default-rtdb.firebaseio.com"
//token authentication database firebase
#define AUTH "E3BuJo21J7gSnrWX7X1k5OhK46YEeG87k05rwnQo"
//SSID WiFi
#define SSID "AP-Sekre" //"AP-PLC"  "AP-REL" "AP_REL"
//Password WiFi 
#define PASS "tebaklah" // "ProloG19" "NamloPREL" "AENamloP2019"

#endif
