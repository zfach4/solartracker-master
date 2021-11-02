#ifndef KOMDAT
#define KOMDAT

#include <Arduino.h>
#include "variabel.h"
#include <Wire.h>

//memulai komunikasi i2c
void komdatInit(byte SDA, byte SCL)
{
    Wire.begin(SDA, SCL);
}

//mengambil data sensor dan relay
void getData1()
{
    Wire.requestFrom(I2CSLAVE, 32);
    while (Wire.available())
    {         
        if (char(Wire.read()) == '*')
        {
            String pData[5];
            int sIndex = 0;
            boolean flagFinish = false;
            while (Wire.available()){
                char sRead = Wire.read();
                if (sRead == '/'){
                    flagFinish = sIndex == 4 ? true : false;
                    break;
                }else if (sRead == ':'){
                    sIndex++;
                    if (sIndex >= 5){
                        break;
                    }
                }else{
                    pData[sIndex].concat(sRead);
                }           
            }
              if (flagFinish == true){
                  //tegangan baterai
                  dtvoltageBaterai = pData[0].toFloat();
                  //tegangan panel surya
                  dtvoltagePanelSurya = pData[1].toFloat();
                  //arus panel surya
                  dtcurrentPanelSurya = pData[2].toFloat();
                  //nilai Energy (Kwh) dari sensor pzem
                  dtpzemEnergy = pData[3].toFloat();
                  //data pin relay
                  dtRelay = pData[4];
              }
        }
    }
}

//mengambil data servo dan azimuth matahari
void getData2()
{
    Wire.requestFrom(I2CSLAVE, 32);
    while (Wire.available())
    {         
        if (char(Wire.read()) == '*')
        {
            String pData[3];
            int sIndex = 0;
            boolean flagFinish = false;
            while (Wire.available())
            {
                char sRead = Wire.read();
                if (sRead == '/'){
                    flagFinish = sIndex == 2 ? true : false;
                    break;
                }else if (sRead == ':'){
                    sIndex++;
                    if (sIndex >= 3){
                        break;
                    }
                }else{
                    pData[sIndex].concat(sRead);
                }           
            }
              if (flagFinish == true){
                  //Nilai Azimuth Matahari
                  dtKoordinat = pData[0].toFloat();
                  //Azimuth Matahari to Degree value
                  dtADV = pData[1].toFloat();
                  //Servo Last Degree Value
                  dtSLDV = pData[2].toFloat();
              }
        }
        //ketika flagWrite true maka nodeMCU akan menulis ke firebase
        flagWrite = true;
    }
}

//mengirim data perintah ke slave
void sendDataCommand(char mode, String commandMove, String jam, String menit, String detik, 
                     String hari, String bulan, String tahun, String lampu1, String lampu2)
{
    Wire.beginTransmission(I2CSLAVE);
    Wire.write('*');
    Wire.write(mode);
    Wire.write(':');
    Wire.write(commandMove.c_str());
    Wire.write(':');
    Wire.write(jam.c_str());
    Wire.write(':');
    Wire.write(menit.c_str());
    Wire.write(':');
    Wire.write(detik.c_str());
    Wire.write(':');
    Wire.write(hari.c_str());
    Wire.write(':');
    Wire.write(bulan.c_str());
    Wire.write(':');
    Wire.write(tahun.c_str());
    Wire.write(':');
    Wire.write(lampu1.c_str());
    Wire.write(':');
    Wire.write(lampu2.c_str());
    Wire.write('/');
    Wire.endTransmission();
}

#endif