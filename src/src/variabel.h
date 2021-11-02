#ifndef VARIABEL
#define VARIABEL

#include <FirebaseESP8266.h>

//konstanta global
const float GMT = 7;
const float LONGITUDE = 107.6211;
const float LATITUDE = -6.8775;

int delayValue;

//Variabel Global
char modePanel = '0';
String dtmodePanel;
int timerCommandOtomatisID = 0;
int timerInterval1ID = 0;
int timerInterval2ID = 0;
bool timerInterval1Status = false;
bool timerInterval2Status = false;
bool flagWrite = false;
String manualDegreeMove;
FirebaseData firebaseData;
float dtvoltageBaterai;
float dtvoltagePanelSurya;
float dtcurrentPanelSurya;
float dtpzemEnergy;
int dtjam;
int dtmenit;
int dtdetik;
int dthari;
int dtbulan;
int dttahun;
String dtRelay;
String dtLampu1;
String dtLampu2;
String ManualLampu1 = "0";
String ManualLampu2 = "0";
String dtSumberAC;
float dtKoordinat;
float dtADV;
float dtSLDV;
String mode;

#endif