//Universidad de Ibagué
//Semillero de Investigación SI2C - Grupo D+TEC
//Proyecto: AgroSensor
//
//This file corresponds to the software of an agro-meteorological 
//station which sense some variables in a crop and transmit 
//the information via Xbee to a collector node.


#include <SoftwareSerial.h>
SoftwareSerial Xbee(15, 14);//(RX, TX)
#include <OneWire.h>                                 //--->Sonda DS18B20
#include <DallasTemperature.h>                       //--->Sonda DS18B20
OneWire oneWire_in(A5);                              //--->Sonda DS18B20
DallasTemperature DS18B20(&oneWire_in);              //--->Sonda DS18B20
#include <SHT1x.h>          //--->Sensor Tem,Hum
#include <Wire.h>           //--->Com I2C
#include "Arduino.h"
#include "SI114X.h"         //--->Sensor IF,UV,VIS
#include "RTClib.h"         //--->Reloj
#include "TSL2561.h"        //--->Flora Light

SI114X SI1145 = SI114X();    
SHT1x sht1x(41,39); //------(dataPin, clockPin)
RTC_DS1307 rtc;       
TSL2561 tsl(TSL2561_ADDR_FLOAT);  


int Anio,Mes,Dia,Hora,Min,FV,FI,FL,SIV,SII,V1,V2,PT1,Hj,Bat,value;
float DS18,SHT,SHH,SIU,res,P1,P2,TP1,TP2,PT,millivolts,celsius;
char  daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
const int chipSelect = 10;
//------------Main-----------------//
void setup(){
  Xbee.begin(9600);
  pinMode(2, OUTPUT);digitalWrite(2, HIGH);
  pinMode(3, OUTPUT);//digitalWrite(3, HIGH);
  pinMode(4, OUTPUT);//digitalWrite(4, HIGH);
  pinMode(5, OUTPUT);//digitalWrite(5, HIGH);
  pinMode(6, OUTPUT);//digitalWrite(6, HIGH);
  pinMode(7, OUTPUT);//digitalWrite(7, HIGH);
  pinMode(8, OUTPUT);//digitalWrite(8, HIGH);
  pinMode(9, OUTPUT);//digitalWrite(9, HIGH);
  pinMode(10, OUTPUT);//digitalWrite(10, HIGH);
  pinMode(23, OUTPUT);//digitalWrite(23, HIGH);
  pinMode(24, OUTPUT);//digitalWrite(24, HIGH);//LED
  pinMode(25, OUTPUT);//digitalWrite(25, HIGH);
  pinMode(26, OUTPUT);//digitalWrite(26, HIGH);//LED
  pinMode(27, OUTPUT);//digitalWrite(27, HIGH);
  pinMode(29, OUTPUT);//digitalWrite(29, HIGH);
  pinMode(30, OUTPUT);//digitalWrite(30, HIGH);   
  pinMode(31, OUTPUT);//digitalWrite(31, HIGH);
  pinMode(32, OUTPUT);//BOTON
  pinMode(33, OUTPUT);//digitalWrite(33, HIGH);
  pinMode(34, OUTPUT);//BOTON
  pinMode(35, OUTPUT);//digitalWrite(35, HIGH);
  pinMode(37, OUTPUT);//digitalWrite(37, HIGH);
  pinMode(46, OUTPUT);digitalWrite(46, HIGH);
  pinMode(50, OUTPUT);//digitalWrite(50, HIGH);
  pinMode(53, OUTPUT);//200SS
  pinMode(52, OUTPUT);//200ss
  ledsON();
  Serial.begin(9600);                     //--->Inicia Com Serial 9600 
  DS18B20.begin();                        //--->Inicia Sonda DS18B20 
  SI1145.Begin();                         //--->Inicia Sensor IF,UV,Vis
  while (!SI1145.Begin()) {//Serial.println("Si1145 is not ready!");
  delay(1000);}
  rtc.begin();                            //--->Inicia reloj
  tsl.begin();                            //--->Inicia Flora light
  tsl.setGain(TSL2561_GAIN_16X);                  //--->Conf Flora light
  tsl.setTiming(TSL2561_INTEGRATIONTIME_101MS);   //--->Conf Flora light
  ledsOFF();
}
void loop(){
  ledsON();
  delay(1000);
  Reloj();          //--->Reloj
  Si1145();         //--->Sensor IF,UV,VIS  
  Flora();          //--->Sensor Flora  
  Sht10();          //--->Sensor Tem,Hum
  Pt100();          //--->Sonda Pt100
  SondaDS18B20();   //--->Sonda Ds18B20  
  granada();
  Bateria();
  Hoja(); 
  lm35(); 
  ledsOFF();
  delay(10000);
}
void Reloj(){
 rtc.adjust(DateTime(2017, 1, 13, 12, 40, 0)); // Ajustar Fecha
 DateTime now = rtc.now();
   Anio=now.year();  Mes=now.month();  Dia=now.day();  Hora=now.hour();  Min=now.minute();
    Serial.print(Anio); Serial.print(";");
    Serial.print(Mes);  Serial.print(";");
    Serial.print(Dia);  Serial.print(";");
    Serial.print(Hora); Serial.print(";");
    Serial.print(Min);  Serial.print(";");

    Xbee.print("year="); Xbee.print(Anio); Xbee.print(";");
    Xbee.print("month=");Xbee.print(Mes);  Xbee.print(";");
    Xbee.print("day=");Xbee.print(Dia);  Xbee.print(";");
    Xbee.print("hour=");Xbee.print(Hora); Xbee.print(";");
    Xbee.print("minute=");Xbee.print(Min);  Xbee.print(";");
    delay(500);
}

void Si1145(){
  SIV = SI1145.ReadVisible();       Serial.print(SIV);Serial.print(";");
  SII = SI1145.ReadIR();            Serial.print(SII);Serial.print(";");
  SIU = (float)SI1145.ReadUV()/100; Serial.print(SIU);Serial.print(";"); 
  Xbee.print("si1145-visible=");   Xbee.print(SIV);Xbee.print(";");
  Xbee.print("si1145-infrared=");  Xbee.print(SII);Xbee.print(";");
  Xbee.print("si1145-uv=");        Xbee.print(SIU);Xbee.print(";"); 

  
  delay(500);   
}

void Flora(){
  uint16_t x = tsl.getLuminosity(TSL2561_VISIBLE);//Serial.println(x, DEC);
  uint32_t lum = tsl.getFullLuminosity();
  uint16_t ir, full;ir = lum >> 16;full = lum & 0xFFFF;
  FV =(full - ir);                  Serial.print(FV);Serial.print(";"); 
  FI =(ir);                         Serial.print(FI);Serial.print(";");        
  FL =(tsl.calculateLux(full, ir)); Serial.print(FL);Serial.print(";");
  Xbee.print("flora-visible=");  Xbee.print(FV);Xbee.print(";"); 
  Xbee.print("flora-infrared=");  Xbee.print(FI);Xbee.print(";");        
  Xbee.print("flora-lux=");  Xbee.print(FL);Xbee.print(";");
  delay(500);
}

void Sht10(){
  SHT = sht1x.readTemperatureC(); Serial.print(SHT);Serial.print(" ");
  SHH = sht1x.readHumidity();     Serial.print(SHH);Serial.print(" ");
//SHF =  sht1x.readTemperatureF(); 
  Xbee.print("sht10-temp=");Xbee.print(SHT);Xbee.print(";");
  Xbee.print("sht10-humidity="); Xbee.print(SHH);Xbee.print(";");  
  delay(500);
}

void Pt100(){
  PT1 = analogRead (A1); TP1 = ((PT1*4.6)/1024)*100;
      Serial.print(TP1);Serial.print(" ");   
      Xbee.print("pt100-temp="); Xbee.print(TP1);Xbee.print(";"); 
//P2 = analogRead (A2); TP2 = ((P2*4.6)/1024)*124.136 -15;
//     Serial.print(TP2);Serial.print(" "); 
  delay(500);
}

void SondaDS18B20(){
    DS18B20.requestTemperatures();
    DS18=(DS18B20.getTempCByIndex(0));
    Serial.print(DS18);Serial.print(" ");
    Xbee.print("ds18b20-temp=");Xbee.print(DS18);Xbee.print(";");
    delay(500);
}

void granada(){
  float Vin,Vo;
  digitalWrite(52,HIGH);
  digitalWrite(53, LOW);  
  delay(1); 
  Vin = analogRead(10);
  Vo = analogRead(11);
  res=4700*(Vin/Vo-1);

  digitalWrite(52,LOW);
  digitalWrite(53, HIGH); 
  delay(1);
  Vin = analogRead(11);
  Vo = analogRead(10);
  res=(res+4700*(Vin/Vo-1))/2.0;  

  digitalWrite(52,LOW);
  digitalWrite(53, LOW); 
  
  Serial.print(res);Serial.print(" ");
  Xbee.print("200ss-humidity="); Xbee.print(res);Xbee.print(";");    
}

void Bateria(){
  Bat = analogRead(A9);
  Serial.print(Bat);Serial.print(" ");
  Xbee.print("battery");Xbee.print(Bat);Xbee.print(";");
  delay(500);
}

void Hoja(){
  Hj = analogRead(A15);
  Serial.print(Hj);Serial.print(" ");
  Xbee.print("leaf-humidity=");Xbee.print(Hj);Xbee.print(";");
  delay(500);
}

void lm35(){
  digitalWrite(29, HIGH);
  delay(1000);  
  value = analogRead(A4);
  Serial.print(value);Serial.print(" ");  
  millivolts = (value / 1023.0) * 5000.0;
  celsius = millivolts / 10.0;
  Serial.print(celsius);Serial.println(" ");
  Xbee.print("lm35-temp=");Xbee.print(celsius);Xbee.println(";");   
  delay(500);
  digitalWrite(29, LOW);
}

void ledsON(){
digitalWrite(2, HIGH);
digitalWrite(3, HIGH);
digitalWrite(4, HIGH);
digitalWrite(5, HIGH);
digitalWrite(6, HIGH);
digitalWrite(7, HIGH);
digitalWrite(8, HIGH);
digitalWrite(9, HIGH);
digitalWrite(10, HIGH);
digitalWrite(23, HIGH);
digitalWrite(24, HIGH);//LED
digitalWrite(25, HIGH);
digitalWrite(26, HIGH);//LED
digitalWrite(27, HIGH);
digitalWrite(29, HIGH);
digitalWrite(30, HIGH);   
digitalWrite(31, HIGH);
digitalWrite(33, HIGH);
digitalWrite(35, HIGH);
digitalWrite(37, HIGH);
digitalWrite(46, HIGH);
digitalWrite(50, HIGH);
}
void ledsOFF(){
//digitalWrite(2, LOW);
digitalWrite(3, LOW);
digitalWrite(4, LOW);
digitalWrite(5, LOW);
digitalWrite(6, LOW);
digitalWrite(7, LOW);
digitalWrite(8, LOW);
digitalWrite(9, LOW);
digitalWrite(10, LOW);
digitalWrite(23, LOW);
digitalWrite(24, LOW);//LED
digitalWrite(25, LOW);
digitalWrite(26, LOW);//LED
digitalWrite(27, LOW);
digitalWrite(29, LOW);
digitalWrite(30, LOW);   
digitalWrite(31, LOW);
digitalWrite(33, LOW);
digitalWrite(35, LOW);
digitalWrite(37, LOW);
//digitalWrite(46, LOW);
digitalWrite(50, LOW);
}
