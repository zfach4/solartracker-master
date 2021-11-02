#include <Arduino.h>
#include <FirebaseESP8266.h>
#include "src/variabel.h"
#include "src/config.h"
#include "src/komdat.h"
#include "src/tEvent.h"

void setup()
{
    //memulai komunikasi i2c
    komdatInit(pinArduino.SDA, pinArduino.SCL);
    //mengatur kecepatan komunikasi serial dengan baudrate 9600
    Serial.begin(9600);
    //memulai koneksi ke jaringan WiFi
    WiFi.begin(SSID, PASS);
    //koneksikan nodeMCU ke firebase
    Firebase.begin(HOST, AUTH);
    //reconnect ke WiFi jika koneksi terputus
    Firebase.reconnectWiFi(true);
    //atur mode awal perangkat menjadi mode otomatis
    char modePanel = '0';
    //aktifkan timerCommandOtomatis
    timerInit();
    
}

void loop()
{
//jalankan timer-timer
timerRun();

//ketika flagWrite true maka nodeMCUakan menulis ke firebase
if(flagWrite == true){
  
  Serial.println("uploaded");

  //deklarasi status lampu 1
  if (dtRelay[0] == '1'){
    dtLampu1 = "ON";
  }else if (dtRelay[0] == '0'){
    dtLampu1 = "OFF";
  }
  //deklarasi status lampu 1
  if (dtRelay[1] == '1'){
    dtLampu2 = "ON";
  }else if (dtRelay[0] == '0'){
    dtLampu2 = "OFF";
  }
  //deklarasi status sumber AC
  if (dtRelay[2] == '1'){
    dtSumberAC = "Inverter";
  }else if (dtRelay[0] == '0'){
    dtSumberAC = "Polman";
  }
  //deklarasi status mode panel
  if (modePanel == '1'){
    dtmodePanel = "Manual";
  }else if (modePanel == '0'){
    dtmodePanel = "Otomatis";
  }
  
       //update data terakhir ke fierbase
       Firebase.setString(firebaseData,"/DataUpdateTerakhir/Jam",String(dtjam));
       Firebase.setString(firebaseData,"/DataUpdateTerakhir/Menit",String(dtmenit));
       Firebase.setString(firebaseData,"/DataUpdateTerakhir/Hari",String(dthari));
       Firebase.setString(firebaseData,"/DataUpdateTerakhir/Mode",String(dtmodePanel));
       Firebase.setString(firebaseData,"/DataUpdateTerakhir/NilaiSudutAzimuthMathari",String(dtKoordinat));
       Firebase.setString(firebaseData,"/DataUpdateTerakhir/NilaiSudutServo",String(dtSLDV));
       Firebase.setString(firebaseData,"/DataUpdateTerakhir/Lampu1",String(dtLampu1));
       Firebase.setString(firebaseData,"/DataUpdateTerakhir/Lampu2",String(dtLampu2));
       Firebase.setString(firebaseData,"/DataUpdateTerakhir/pzemValue",String(dtpzemEnergy));
       Firebase.setString(firebaseData,"/DataUpdateTerakhir/SumberListrikAC",String(dtSumberAC));
       Firebase.setString(firebaseData,"/DataUpdateTerakhir/VoltagePanelSUrya",String(dtvoltagePanelSurya));
       Firebase.setString(firebaseData,"/DataUpdateTerakhir/CurrentPanelSurya",String(dtcurrentPanelSurya));
       Firebase.setString(firebaseData,"/DataUpdateTerakhir/VoltageBaterai",String(dtvoltageBaterai));
       
       //tulis ke history jika mode otomatis
       if (modePanel == '0'){
       String History = "/History/"+String(dthari)+"-"+String(dtbulan)+"-"+String(dttahun)+"|"+ String(dtjam)+" : "+String(dtmenit);
       Firebase.setString(firebaseData,History + "/Jam", String(dtjam));
       Firebase.setString(firebaseData,History + "/Menit", String(dtmenit));
       Firebase.setString(firebaseData,History + "/Hari", String(dthari));
       Firebase.setString(firebaseData,History + "/Bulan", String(dtbulan));
       Firebase.setString(firebaseData,History + "/Tahun", String(dttahun));
       Firebase.setString(firebaseData,History + "/NilaiSudutAzimuthMathari",String(dtKoordinat));
       Firebase.setString(firebaseData,History + "/NilaiSudutAzimuthMathariYangDiKonversiKeRefrensiSudutServo",String(dtADV));
       Firebase.setString(firebaseData,History + "/NilaiSudutAkhirServo",String(dtSLDV));
       Firebase.setString(firebaseData,History + "/Lampu1",String(dtLampu1));
       Firebase.setString(firebaseData,History + "/Lampu2",String(dtLampu2));
       Firebase.setString(firebaseData,History + "/pzemValue",String(dtpzemEnergy,3));
       Firebase.setString(firebaseData,History + "/SumberListrikAC",String(dtSumberAC));
       Firebase.setString(firebaseData,History + "/VoltagePanelSUrya",String(dtvoltagePanelSurya));
       Firebase.setString(firebaseData,History + "/CurrentPanelSurya",String(dtcurrentPanelSurya));
       Firebase.setString(firebaseData,History + "/VoltageBaterai",String(dtvoltageBaterai));
       }
       //non aktifkan kembali flagwrite sehingga nodeMCU tidak menulis ke firebase kembali
       flagWrite = false;
   }

 
 //pengontrolan lampu 1 berdasarkan inputan dari firebase
 if (Firebase.getString(firebaseData, "Control/Lampu1")){
     if (firebaseData.dataType() == "string"){
         //jika nilai sama dengan 1 (on)
         if(firebaseData.stringData() == "1" && modePanel == '1'){
           Firebase.setString(firebaseData,"/Control/Confirm", "Data telah terkirim");
           Firebase.setString(firebaseData,"/Control/Lampu1",String(-1));
           ManualLampu1 = "1";
           getWaktu();
           delayValue = getDelay();
           sendDataCommand('1', String(manualDegreeMove), String(dtjam), String(dtmenit), String(dtdetik), String(dthari), 
                                String(dtbulan), String(dttahun), String(ManualLampu1), String(ManualLampu2));
           timerInterval2Status = false;
           timerInterval2.deleteTimer(timerInterval2ID); 
           requestDataInterval2();
         //jika nilai sama dengan 0 (off)
        }else if(firebaseData.stringData() == "0" && modePanel == '1'){
            Firebase.setString(firebaseData,"/Control/Confirm", "Data telah terkirim");
            Firebase.setString(firebaseData,"/Control/Lampu1",String(-1));
            ManualLampu1 = "0";
            getWaktu();
            delayValue = getDelay();
            sendDataCommand('1', String(manualDegreeMove), String(dtjam), String(dtmenit), String(dtdetik), String(dthari), 
                                 String(dtbulan), String(dttahun), String(ManualLampu1), String(ManualLampu2));
            timerInterval2Status = false;
            timerInterval2.deleteTimer(timerInterval2ID);
            requestDataInterval2(); 
        }
      }
    }
 
 //pengontrolan lampu 2 berdasarkan inputan dari firebase
 if (Firebase.getString(firebaseData, "Control/Lampu2")){
     if (firebaseData.dataType() == "string"){
         //jika nilai sama dengan 1 (on)
         if(firebaseData.stringData() == "1" && modePanel == '1'){
           Firebase.setString(firebaseData,"/Control/Lampu2",String(-1));
           Firebase.setString(firebaseData,"/Control/Confirm", "Data telah terkirim");
           ManualLampu2 = "1";
           delayValue = getDelay();
           sendDataCommand('1', String(manualDegreeMove), String(dtjam), String(dtmenit), String(dtdetik), String(dthari), String(dtbulan), String(dttahun), String(ManualLampu1), String(ManualLampu2));
           timerInterval2Status = false;
           timerInterval2.deleteTimer(timerInterval2ID); 
           requestDataInterval2();
        }else if(firebaseData.stringData() == "0" && modePanel == '1'){
          //jika nilai sama dengan 0 (off)
            Firebase.setString(firebaseData,"/Control/Lampu2",String(-1));
            Firebase.setString(firebaseData,"/Control/Confirm", "Data telah terkirim");
            ManualLampu2 = "0";
            delayValue = getDelay();
            sendDataCommand('1', String(manualDegreeMove), String(dtjam), String(dtmenit), String(dtdetik), String(dthari), String(dtbulan), String(dttahun), String(ManualLampu1), String(ManualLampu2));
            timerInterval2Status = false;
            timerInterval2.deleteTimer(timerInterval2ID); 
            requestDataInterval2();
        }
      }
    }

//pengontrolan motor servo berdasarkan inputan dari firebase
if (Firebase.getString(firebaseData, "Control/Servo")){
    if (firebaseData.dataType() == "string"){
        if(firebaseData.stringData() != "-1" && modePanel == '1'){
          manualDegreeMove = firebaseData.stringData();
          Firebase.setString(firebaseData,"/Control/Servo",String(-1));
          Firebase.setString(firebaseData,"/Control/Confirm", "Data telah terkirim");
          delayValue = getDelay();
          sendDataCommand('1', String(manualDegreeMove), String(dtjam), String(dtmenit), String(dtdetik), String(dthari), String(dtbulan), String(dttahun), String(ManualLampu1), String(ManualLampu2));       
          manualDegreeMove = "NaN";
          timerInterval1.deleteTimer(timerInterval1ID);
          timerInterval1Status = false;
          Serial.println("request");
          requestDataInterval1(delayValue);
        }
      }
    }
     
     //pengontrollan mode berdasarkan inputan dari firebase
  if (Firebase.getString(firebaseData, "Control/Mode")){
     if (firebaseData.dataType() == "string"){
       mode = firebaseData.stringData();
        //jika mode manual
        if(firebaseData.stringData() == "0"){
           Firebase.setString(firebaseData,"/Control/Mode",String(-1));
           Firebase.setString(firebaseData,"/Control/Confirm", "Data telah terkirim");
           modePanel = '1';
           Serial.println("manual");
           delayValue = getDelay();
           sendDataCommand('1', "NaN", String(dtjam), String(dtmenit), String(dtdetik), String(dthari), String(dtbulan), String(dttahun), String(ManualLampu1), String(ManualLampu2));
           timerInterval2Status = false;
           timerInterval2.deleteTimer(timerInterval2ID);
          requestDataInterval2();
        }else if(firebaseData.stringData() == "1"){
           Firebase.setString(firebaseData,"/Control/Mode",String(-1));
           Firebase.setString(firebaseData,"/Control/Confirm", "Data telah terkirim");
           ManualLampu1 = "0";
           ManualLampu2 = "0";
           modePanel = '0';
           Serial.println("delay");
           delayValue = getDelay();
           sendDataCommand('0', "NaN", String(dtjam), String(dtmenit), String(dtdetik), String(dthari), String(dtbulan), String(dttahun), String(ManualLampu1), String(ManualLampu2));
           timerInterval1.deleteTimer(timerInterval1ID);
           timerInterval1Status = false;
           requestDataInterval1(delayValue);
     }
    }
  }

//jika nodeMCU tidak konek ke WiFi
if(WiFi.status() != WL_CONNECTED)
  {
    modePanel = '0';
  }
}