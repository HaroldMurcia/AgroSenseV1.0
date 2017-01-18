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


int Anio,Mes,Dia,Hora,Min,FV,FI,FL,SIV,SII,V1,V2,PT1,Hj,Bat;
float DS18,SHT,SHH,SIU,res,P1,P2,TP1,TP2,PT,millivolts,celsius,SMP;
char  daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
const int chipSelect = 10;
float ElapsedTime,tic,toc;
float const Ts=10000;
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
  pinMode(24, OUTPUT);//digitalWrite(24, HIGH);//LED 1
  pinMode(25, OUTPUT);//digitalWrite(25, HIGH);
  pinMode(26, OUTPUT);//digitalWrite(26, HIGH);//LED 2 
  pinMode(27, OUTPUT);//digitalWrite(27, HIGH);
  pinMode(29, OUTPUT);//digitalWrite(29, HIGH); // LM35
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
  
  SwON();
  Serial.begin(57600);                     //--->Inicia Com Serial 9600 
  DS18B20.begin();                        //--->Inicia Sonda DS18B20 
  SI1145.Begin();                         //--->Inicia Sensor IF,UV,Vis
  while (!SI1145.Begin()) {//Serial.println("Si1145 is not ready!");
  delay(1000);}
  rtc.begin();                            //--->Inicia reloj
  tsl.begin();                            //--->Inicia Flora light
  tsl.setGain(TSL2561_GAIN_16X);                  //--->Conf Flora light
  tsl.setTiming(TSL2561_INTEGRATIONTIME_101MS);   //--->Conf Flora light
  SwOFF();
  delay(100);
  SwON();
  delay(300);
  SwOFF();
  delay(200);
  SwON();
  delay(500);
}

struct Data{
  float Val[3]={0,0,0};
};

///////////////////////////////////////////////////////////////////////////////////////// Main Loop
void loop(){
  tic=millis();

  
    
  SwON();
  delay(500);
  Clock();          //--->Reloj
  Si1145();         //--->Sensor IF,UV,VIS  
  Flora();          //--->Sensor Flora  
  Sht10();          //--->Sensor Tem,Hum
  Pt100();          //--->Sonda Pt100
  SondaDS18B20();   //--->Sonda Ds18B20  
  Battery();
  Leaf(); 
  lm35(); 
  SoilWater(celsius);
  SwOFF();

  Filtro(0,celsius);
  PrintData(0);

  // SampleTime measurement
  toc=millis();
  ElapsedTime=(toc-tic);
  while(ElapsedTime<Ts){
    toc=millis();
    ElapsedTime=(toc-tic);
  }
  Serial.print("dTime:");
  Serial.println(ElapsedTime/1000.0);
  Xbee.print("dTime:");
  Xbee.println(ElapsedTime/1000.0);
}
/////////////////////////////////////////////////////////////////////////////////////////

/// Functions

void Clock(){
  rtc.adjust(DateTime(2017, 1, 13, 12, 40, 0)); // Ajustar Fecha
  DateTime now = rtc.now();
  Anio=now.year();  Mes=now.month();  Dia=now.day();  Hora=now.hour();  Min=now.minute();
}

void Si1145(){
  SIV = SI1145.ReadVisible();       
  delay(1);
  SII = SI1145.ReadIR();            
  delay(1);
  SIU = (float)SI1145.ReadUV()/100; 
  delay(1);
}

void Flora(){
  uint16_t x = tsl.getLuminosity(TSL2561_VISIBLE);//Serial.println(x, DEC);
  uint32_t lum = tsl.getFullLuminosity();
  uint16_t ir, full;ir = lum >> 16;full = lum & 0xFFFF;
  FV =(full - ir);                  
  FI =(ir);                                
  FL =(tsl.calculateLux(full, ir)); 
}

void Sht10(){
  SHT = sht1x.readTemperatureC(); 
  delay(1);
  SHH = sht1x.readHumidity();     
  delay(1);
//SHF =  sht1x.readTemperatureF();  
}

void Pt100(){
  int k=10;
  PT1=AverageMeasurement(1,k) ;
  TP1 = ((PT1*4.6)/1024)*100; 
//P2 = analogRead (A2); TP2 = ((P2*4.6)/1024)*124.136 -15;
//     Serial.print(TP2);Serial.print(" "); 
}

void SondaDS18B20(){
    DS18B20.requestTemperatures();
    delay(1);
    DS18=(DS18B20.getTempCByIndex(0));
    delay(1);
}

void SoilWater(float T){
  float Vin,Vo; int k=10;
  
  digitalWrite(52,HIGH);
  digitalWrite(53, LOW);  
  delay(1); 
  Vin=AverageMeasurement(10,k) ;
  Vo=AverageMeasurement(11,k) ;
  res=1500*(Vin/Vo-1);

  digitalWrite(52,LOW);
  digitalWrite(53, HIGH); 
  delay(1);
  Vin=AverageMeasurement(11,k) ;
  Vo=AverageMeasurement(10,k) ;
  res=(res+1500*(Vin/Vo-1))/2.0;  
  SMP=(-0.0032*res-4.093)/(1-9.7330e-06*res-0.01205*T);

  digitalWrite(52,LOW);
  digitalWrite(53, LOW);    
}

void Battery(){
  float B_val=0; int k=10;
  Bat=AverageMeasurement(9,k) ;
  Bat = Bat/237.0*100;
}

void Leaf(){
  float H_val=0; int k=10;
  H_val=AverageMeasurement(15,k) ;
  Hj = H_val;
}

void lm35(){ 
  int k=10; float T_val=0;
  T_val=AverageMeasurement(4,k) ;
  millivolts = (T_val / 1023.0) * 5000.0;
  celsius = millivolts / 10.0 -3.5; 
  
  Data T_LM35; 
  T_LM35.Val[2]=T_LM35.Val[1];
  T_LM35.Val[1]=T_LM35.Val[0];
  T_LM35.Val[0]=celsius;
  
}

void PrintData(int mode){
  // mode=1 to print labels via USB
  // Clock
//    Serial.print(Anio); Serial.print('\t');
//    Serial.print(Mes);  Serial.print('\t');
//    Serial.print(Dia);  Serial.print('\t');
//    Serial.print(Hora); Serial.print('\t');
//    Serial.print(Min);  Serial.print('\t');
    Xbee.print("year="); Xbee.print(Anio); Xbee.print('\t');
    Xbee.print("month=");Xbee.print(Mes);  Xbee.print('\t');
    Xbee.print("day=");Xbee.print(Dia);  Xbee.print('\t');
    Xbee.print("hour=");Xbee.print(Hora); Xbee.print('\t');
    Xbee.print("minute=");Xbee.print(Min);  Xbee.print('\t');

  // Light Si1145
    if(mode==1){Serial.print("SIV:");}
    Serial.print(SIV);Serial.print('\t');
    if(mode==1){Serial.print("SII:");}
    Serial.print(SII);Serial.print('\t');
    if(mode==1){Serial.print("SIU:");}
    Serial.print(SIU);Serial.print('\t');
    Xbee.print("si1145-visible=");   Xbee.print(SIV);Xbee.print('\t');
    Xbee.print("si1145-infrared=");  Xbee.print(SII);Xbee.print('\t');
    Xbee.print("si1145-uv=");        Xbee.print(SIU);Xbee.print('\t'); 

  // Light Flora
    if(mode==1){Serial.print("FV:");}
    Serial.print(FV);Serial.print('\t'); 
    if(mode==1){Serial.print("FI:");}
    Serial.print(FI);Serial.print('\t');
    if(mode==1){Serial.print("FL:");}
    Serial.print(FL);Serial.print('\t');
    Xbee.print("flora-visible=");  Xbee.print(FV);Xbee.print('\t'); 
    Xbee.print("flora-infrared=");  Xbee.print(FI);Xbee.print('\t');        
    Xbee.print("flora-lux=");  Xbee.print(FL);Xbee.print('\t');

  // SHT10
    if(mode==1){Serial.print("SHT:");}
    Serial.print(SHT);Serial.print('\t');
    if(mode==1){Serial.print("SHH:");}
    Serial.print(SHH);Serial.print('\t');
    Xbee.print("sht10-temp=");Xbee.print(SHT);Xbee.print(";");
    Xbee.print("sht10-humidity="); Xbee.print(SHH);Xbee.print(";"); 

  // PT100 1
    if(mode==1){Serial.print("PT1:"); }
    Serial.print(TP1); Serial.print('\t');  
    Xbee.print("pt100-temp="); Xbee.print(TP1);Xbee.print('\t'); 

  // SondaDS18B20
    if(mode==1){Serial.print("T-DS18:");}
    Serial.print(DS18);Serial.print('\t');
    Xbee.print("ds18b20-temp=");Xbee.print(DS18);Xbee.print('\t');

  // SoilWater
    if(mode==1){Serial.print("200ss:");}
    Serial.print(SMP);Serial.print('\t');
    Xbee.print("200ss-humidity="); Xbee.print(SMP);Xbee.print('\t'); 

  // Battery
    if(mode==1){Serial.print("Batt:");}
    Serial.print(Bat);Serial.print('\t');
    Xbee.print("battery");Xbee.print(Bat);Xbee.print('\t');

 // Leaf
    if(mode==1){Serial.print("LH:");}
    Serial.print(Hj);Serial.print('\t');
    Xbee.print("leaf-humidity=");Xbee.print(Hj);Xbee.print('\t');

 // LM35 - Internal Temperature
    if(mode==1){Serial.print("LM35:");}
    Serial.print(celsius);Serial.print('\t');
    Xbee.print("lm35-temp=");Xbee.print(celsius);Xbee.print('\t'); 
}

float AverageMeasurement(int Port,int K){
  // This function read a analog port "K" times and then return de average value.
  float Result=0,Val=0;
  for(int i=1; i <= K; i++){
    Val = Val+analogRead(Port);
    delay(10);   
  }
  Result=Val/K;  
  return Result;
}

void Filtro(float u, float meassurement){
  float X,X_1,B,F,Q,P,K,H,R;
  u=0; B=0;F=1;Q=1e-9;H=1;R=0.2090;
  X=X*F+B*u;
  P=F*P*F+Q;
  K=P*H/(H*P*H+R);
  X=X+K*(meassurement-H*X);
  P=(1-K*H)*P;
  Serial.print(X), Serial.print('\t');
}


void SwON(){
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
  digitalWrite(24, HIGH);//LED 1
  digitalWrite(25, HIGH);
  digitalWrite(26, HIGH);//LED 2
  digitalWrite(27, HIGH);
  digitalWrite(29, HIGH); // LM35
  digitalWrite(30, HIGH);   
  digitalWrite(31, HIGH);
  digitalWrite(33, HIGH);
  digitalWrite(35, HIGH);
  digitalWrite(37, HIGH);
  digitalWrite(46, HIGH);
  digitalWrite(50, HIGH);
}

void SwOFF(){
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
  digitalWrite(29, LOW);// LM35
  digitalWrite(30, LOW);   
  digitalWrite(31, LOW);
  digitalWrite(33, LOW);
  digitalWrite(35, LOW);
  digitalWrite(37, LOW);
  //digitalWrite(46, LOW);
  digitalWrite(50, LOW);
}
