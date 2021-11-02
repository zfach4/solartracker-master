#ifndef TEVENT
#define TEVENT

#include <Arduino.h>
#include <SimpleTimer.h>
#include "src/variabel.h"
#include <RTClib.h>

//module class
RTC_DS1307 rtc;
SimpleTimer timerCommandOtomatis;
SimpleTimer timerInterval1;
SimpleTimer timerInterval2;

//jalankan timer
void timerRun()
{
    //timer untuk history
    timerCommandOtomatis.run();

    //timer untuk pengambilan data pada saat mode otomatis
    if (timerInterval1Status == true){
    timerInterval1.run();
    }

    //timer untuk pengambilan data pada saat mode manual
    if (timerInterval2Status == true){
    timerInterval2.run();
    }
}

//mengambil waktu dari rtc
void getWaktu()
{   
    DateTime now = rtc.now();
    dtjam = now.hour();
    Serial.println(dtjam);
    dtmenit = now.minute();
    dtdetik = now.second();
    dthari = now.day();
    dtbulan = now.month();
    dttahun = now.year();
}

//////////////////////

        //mencari hari dalam tahun
        int calculateDayOfYear(int day, int month, int year)
        {
            int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

            // check jika tahun ini tahun kabisat
            if (year % 4 == 0){
                if (year % 100 != 0){
                    daysInMonth[1] = 29;
                }else{
                    if (year % 400 == 0){
                        daysInMonth[1] = 29;
                    }
                }
            }

            //menghitung hari ke berapa dalam tahun ini
            int doy = 0;
            for (int i = 0; i < month - 1; i++){
                doy += daysInMonth[i];
            }
            doy += day;
            return doy;
        }

        int azimuthToDegree(int value)
        {
            int result;
            if (value <= 180)
            {
                if (value <=90){
                    result = map(value, 90, 0, 0, 90);
                }else{
                    result = map(value, 180, 90, 270, 360);
                }
            }
            else
            {
                if (value >=270){
                    result = map(value, 360, 270, 90, 180);
                }else{
                    result = map(value, 270, 180, 180, 270);
                }
            }
            return result;
        }

int getDelay(){

    int delay;
    getWaktu();
    
        //penentuan delay
        if (modePanel == '0' && dtjam >= 8 && dtjam <= 16){
            //perhitungan algoritma posisi matahari
            float LT = dtjam + (dtmenit/60) + (dtdetik/3600);
            float LSTM = 15 * GMT;
            float d = calculateDayOfYear(dthari, dtbulan, dttahun);
            float B = ((d - 81) * 360) / 365;
            float EoT = 9.87 * sin((2 * 3.14) * 2 * B / 360) - 7.52 * cos((2 * 3.14) * B / 360) 
                        - 1.5 * sin((2 * 3.14) * B / 360);
            float TC = 4 * (LONGITUDE - LSTM) + EoT;
            float LST = (LT + (TC / 60));
            int HRA = 15 * (LST - 12);

            float deklinasi = 23.45 * sin((2 * 3.14) * B / 360);

            float elevasi;
            float azimuth;
            elevasi = asin((sin((2 * 3.14) * deklinasi / 360) * sin((2 * 3.14) * LATITUDE / 360)) 
                            + (cos((2 * 3.14) * deklinasi / 360) * cos((2 * 3.14) * LATITUDE / 360) 
                            * cos((2 * 3.14) * HRA / 360))) * 180 / 3.14;

            //Azimuth
            azimuth = acos(((sin((2 * 3.14) * deklinasi / 360) * cos((2 * 3.14) * LATITUDE / 360)) 
                            - (cos((2 * 3.14) * deklinasi / 360) * sin((2 * 3.14) * LATITUDE / 360) 
                            * cos((2 * 3.14) * HRA / 360))) / cos((2 * 3.14) * elevasi / 360)) * 180 / 3.14;
            azimuth = dtjam < 12 ? azimuth : 360 - azimuth;
            azimuth = azimuthToDegree(azimuth);
            //hasil
            Serial.println("otomatis");
            Serial.print("sudut servo : ");
            Serial.println(dtSLDV);
            Serial.print("azimuth : ");
            Serial.println(azimuth);
            int a = abs((abs(dtSLDV - azimuth) * 100) + 1000);
            Serial.print("perhitungan : ");
            Serial.println(a);
            delay = a;
        }else if(modePanel == '1'){
            //manual
            Serial.print("sudut servo : ");
            Serial.println(dtSLDV);
            float cmmdManual = manualDegreeMove.toFloat();
            Serial.print("cmmdManual : ");
            Serial.println(cmmdManual);
            int c = cmmdManual - dtSLDV;
            Serial.print("c : ");
            Serial.println(c);
            int b = abs((abs(cmmdManual - dtSLDV) * 100) + 2000);
            Serial.print("perhitungan : ");
            Serial.println(b);
            delay = b;
        }else if(modePanel == '0'){
            //otomatis jam malam
            delay = 9000;
        }
Serial.print("delay : ");
Serial.println(delay);
return delay;
}

/////////////

//meminta data ke slave pada saat mode manual
void requestDataInterval2Event()
{
    getData1();
    getData2();
    timerInterval2Status = false;
    timerInterval2.deleteTimer(timerInterval2ID);
    Serial.println("delay manual off");
}

//men-set interval untuk pengambilan data ke slave saat mode manual
void requestDataInterval2()
{  
    timerInterval2ID = timerInterval2.setInterval(3000, requestDataInterval2Event);
    timerInterval2Status = true;
    Serial.println("delay manual on"); 
}

//meminta data ke slave pada saat mode otomatis
void requestDataInterval1Event()
{
    getData1();
    getData2();
    timerInterval1.deleteTimer(timerInterval1ID);
    timerInterval1Status = false;
    Serial.println("delay off"); 
}

//men-set interval untuk pengambilan data ke slave saat mode otomatis
void requestDataInterval1(int delayed)
{
    timerInterval1ID = timerInterval1.setInterval(delayed, requestDataInterval1Event);
    timerInterval1Status = true;
    Serial.println("delay on"); 
}
//6500 = 25ms
//14500 = 75ms
//18000 = 100ms

//hanya mendeklarasikan function reInitTimer agar bisa dibaca oleh function timerHistoryEvent
void reInitTimer();

//memberi perintah ke slave lalu mengambil data ke slave, pada saat mode otomatis
void timerCommandOtomatisEvent()
{
    getWaktu();
    if(modePanel == '0'){
        delayValue = getDelay();
        sendDataCommand('0', "NaN", String(dtjam), String(dtmenit), String(dtdetik), String(dthari), String(dtbulan), String(dttahun), String(ManualLampu1), String(ManualLampu2));
        timerInterval1.deleteTimer(timerInterval1ID);
        timerInterval1Status = false;
        requestDataInterval1(delayValue);
    }
    reInitTimer();
}

//men-set kembali interval untuk memanggil timerCommandOtomatisEvent() selama 40 menit
void reInitTimer()
{
    timerCommandOtomatis.deleteTimer(timerCommandOtomatisID);
    timerCommandOtomatisID = timerCommandOtomatis.setInterval(2400000, timerCommandOtomatisEvent);
}

//2400000 = 40 menit

//men-set interval awal untuk memanggil timerCommandOtomatisEvent()
void timerInit()
{
    timerCommandOtomatisID = timerCommandOtomatis.setInterval(1000, timerCommandOtomatisEvent);
}



#endif