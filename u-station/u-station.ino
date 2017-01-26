//Universidad de Ibagué
//Semillero de Investigación SI2C - Grupo D+TEC
//Proyecto: AgroSensor
//
//This file corresponds to the software of an agro-meteorological 
//station which sense some variables in a crop and transmit 
//the information via Xbee to a collector node.

//#include <SoftwareSerial.h>     //--->Serial Virtual
#include <OneWire.h>            //--->Sonda DS18B20
#include <DallasTemperature.h>  //--->Sonda DS18B20
#include <SHT1x.h>              //--->Sensor Tem,Hum
#include <Wire.h>               //--->Com I2C
#include "Arduino.h"
#include "SI114X.h"             //--->Sensor IF,UV,VIS
#include "RTClib.h"             //--->Reloj
#include "TSL2561.h"            //--->Flora Light

//SoftwareSerial Xbee(15, 14);//(RX, TX)
SI114X SI1145 = SI114X();    
SHT1x sht1x(41,39);             //----(dataPin, clockPin)
RTC_DS1307 rtc;       
TSL2561 tsl(TSL2561_ADDR_FLOAT);  
OneWire oneWire_in(A5);DallasTemperature DS18B20(&oneWire_in); 

int Anio,Mes,Dia,Hora,Min,FV,FI,FL,SIV,SII,V1,V2,PT1,PT2,Hj,Bat,pin,md;
int Clk,Si,Fl,Sh,Pt,Ds,Bt,Lf,Lm,Sw;
float DS18,SHT,SHH,SIU,res,P,P2,TP1,TP2,PT,millivolts,celsius,SMP;
char  daysOfTheWeek[7][12]={"S","M","T","W","T","F","S"};
float ElapsedTime,tic,toc;
float const Ts=5000;
int Pines[]={2,3,4,5,6,7,8,9,10,23,24,25,26,27,29,30,31,32,33,34,35,37,46,50,52,53};
// Boton 32,34 - Leds 24,26
//------------Main-----------------//
void setup(){
  for (pin=0; pin<28; pin=pin+1){pinMode(Pines[pin],OUTPUT);}
  Serial.begin(9600); 
  Serial3.begin(9600);
  pinMode(32,INPUT);
  pinMode(11,INPUT);
  digitalWrite(11,HIGH); // Para ruido de Xbee primera vez
  md=0;
}

void loop(){
  tic=millis();
 // Boton();
  SwON();
  delay(1000);
  Clock();        //--->Reloj
  Si1145();       //--->Sensor IF,UV,VIS  
  Flora();          //--->Sensor Flora  
  Sht10();          //--->Sensor Tem,Hum
  Pt100();          //--->Sonda Pt100
  SondaDS18B20();   //--->Sonda Ds18B20  
  Battery();
  Leaf(); 
  lm35(); 
  SoilWater(celsius);
  SwOFF();

  pinMode(11,OUTPUT);
  digitalWrite(11,LOW);
  delay(60000);
  //Filtro(0,celsius);
  PrintData(1);
  pinMode(11,INPUT);
  digitalWrite(11,HIGH);

//SampleTime measurement
  toc=millis();
  ElapsedTime=(toc-tic);
  while(ElapsedTime<Ts){
    toc=millis();
    ElapsedTime=(toc-tic);
  }
  //Serial.print("dTime:");
  //Serial.println(ElapsedTime/1000.0);
 // Serial3.print('dTime:');
  //Serial3.println(ElapsedTime/1000.0);
}



/// Functions
//void Boton(){
//  B1 = digitalRead(32);
//  if(B1==1 && md==0){
//    md=1;SwON();delay(2000);SwOFF();}
//  if(B1==1 && md==1){
//    md=0;SwON();delay(2000);SwOFF();}
//}
void Clock(){Clk=1;
  rtc.begin();delay(100);
  //rtc.adjust(DateTime(2017, 1, 13, 12, 40, 0));
  DateTime now = rtc.now();delay(1);
  Anio=now.year(); Mes=now.month(); Dia=now.day(); Hora=now.hour(); Min=now.minute();
}
void Si1145(){Si=1;
  SI1145.Begin();
  SIV = SI1145.ReadVisible();       delay(1);
  SII = SI1145.ReadIR();            delay(1);
  SIU = (float)SI1145.ReadUV()/100; delay(1);
}
void Flora(){Fl=1;
  tsl.begin();                                    //--->Inicia Flora light
  tsl.setGain(TSL2561_GAIN_16X);                  //--->Conf Flora light
  tsl.setTiming(TSL2561_INTEGRATIONTIME_101MS);   //--->Conf Flora light
  uint16_t x = tsl.getLuminosity(TSL2561_VISIBLE);//Serial.println(x, DEC);
  uint32_t lum = tsl.getFullLuminosity();
  uint16_t ir, full;
  ir = lum >> 16;
  full = lum & 0xFFFF;
  FV =(full - ir);                  
  FI =(ir);                                
  FL =(tsl.calculateLux(full, ir)); 
}
void Sht10(){Sh=1;
  SHT = sht1x.readTemperatureC(); delay(1);
  SHH = sht1x.readHumidity();     delay(1);
//SHF = sht1x.readTemperatureF();  
}
void Pt100(){Pt=1;
  int k = 10;
  //TP1 = analogRead(A1);
  //TP2 = analogRead(A2);
  PT1 = AverageMeasurement(1,k) ;
  TP1 = PT1*0.4473-7.2517; 
  PT2 = AverageMeasurement(2,k) ;
  TP2 = PT2*0.4374   -4.8990; 
}
void SondaDS18B20(){Ds=1;
    DS18B20.begin();                    delay(10);
    DS18B20.requestTemperatures();      delay(1);
    DS18=(DS18B20.getTempCByIndex(0));  delay(1);
    DS18=DS18* 0.8651    +4.8444;
}
void SoilWater(float T){
  Sw=1;   // print Flag
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
void Battery(){Bt=1;
  float B_val=0; int k=10;
  Bat=AverageMeasurement(9,k) ;
  Bat = Bat/237.0*100;
}
void Leaf(){Lf=1;
  float H_val=0; int k=10;
  H_val=AverageMeasurement(15,k) ;
  Hj = H_val;
}
void lm35(){Lm=1;
  int k=10; float T_val=0;
  T_val=AverageMeasurement(4,k) ;
  millivolts = (T_val / 1023.0) * 5000.0;
  celsius = millivolts / 10.0 -3.5; 
}
void PrintData(int mode){
//CLK
  if(Clk==0){Anio=0000;Mes=00;Dia=00;Hora=00;Min=00;}
  switch(mode){
    case 0:
    Serial.print(Anio); Serial.print(" "); Serial.print(Mes);  Serial.print(" ");
    Serial.print(Dia);  Serial.print(" "); Serial.print(Hora); Serial.print(" ");
    Serial.print(Min);  Serial.print(" "); 
    Serial3.print("year=");  Serial3.print(Anio); Serial3.print('\t');
    Serial3.print("month="); Serial3.print(Mes);  Serial3.print('\t');
    Serial3.print("day=");   Serial3.print(Dia);  Serial3.print('\t');
    Serial3.print("hour=");  Serial3.print(Hora); Serial3.print('\t');
    Serial3.print("minute=");Serial3.print(Min);  Serial3.print('\t');
    break;
    case 1:
    Serial.print(Anio); Serial.print(" "); Serial.print(Mes);  Serial.print(" ");
    Serial.print(Dia);  Serial.print(" "); Serial.print(Hora); Serial.print(" ");
    Serial.print(Min);  Serial.print(" "); 
    Serial3.print(Anio); Serial3.print('\t'); Serial3.print(Mes);  Serial3.print('\t');
    Serial3.print(Dia);  Serial3.print('\t'); Serial3.print(Hora); Serial3.print('\t');
    Serial3.print(Min);  Serial3.print('\t');
    break;}
// Light Si1145
  if(Si==0){SIV=000;SII=000;SIU=0.00;}
  switch(mode){
    case 0:
    Serial.print("SIV:");Serial.print(SIV);Serial.print(" ");
    Serial.print("SII:");Serial.print(SII);Serial.print(" ");
    Serial.print("SIU:");Serial.print(SIU);Serial.print(" ");
    Serial3.print("si1145-visible=");   Serial3.print(SIV);Serial3.print('\t');
    Serial3.print("si1145-infrared=");  Serial3.print(SII);Serial3.print('\t');
    Serial3.print("si1145-uv=");        Serial3.print(SIU);Serial3.print('\t'); 
    break;
    case 1:
    Serial.print(SIV);Serial.print(" ");
    Serial.print(SII);Serial.print(" ");
    Serial.print(SIU);Serial.print(" ");
    Serial3.print(SIV);Serial3.print('\t');
    Serial3.print(SII);Serial3.print('\t');
    Serial3.print(SIU);Serial3.print('\t');
    break;
    }
// Light Flora
  if(Fl==0){FV=000;FI=000;FL=0.00;}
  switch(mode){
    case 0:
    Serial.print("FV:");Serial.print(FV);Serial.print(" "); 
    Serial.print("FI:");Serial.print(FI);Serial.print(" ");
    Serial.print("FL:");Serial.print(FL);Serial.print(" ");
    Serial3.print("flora-visible=");   Serial3.print(FV);Serial3.print('\t'); 
    Serial3.print("flora-infrared=");  Serial3.print(FI);Serial3.print('\t');        
    Serial3.print("flora-lux=");       Serial3.print(FL);Serial3.print('\t');
    break;
    case 1:
    Serial.print(FV);Serial.print(" ");
    Serial.print(FI);Serial.print(" ");
    Serial.print(FL);Serial.print(" ");
    Serial3.print(FV);Serial3.print('\t'); 
    Serial3.print(FI);Serial3.print('\t');        
    Serial3.print(FL);Serial3.print('\t');
    break;       
    }
// SHT10
  if(Sh==0){SHT=000;SHH=000;}
  switch(mode){
    case 0:
    Serial.print("SHT:");Serial.print(SHT);Serial.print(" ");
    Serial.print("SHH:");Serial.print(SHH);Serial.print(" ");
    Serial3.print("sht10-temp=");      Serial3.print(SHT);Serial3.print('\t');
    Serial3.print("sht10-humidity=");  Serial3.print(SHH);Serial3.print('\t'); 
    break;
    case 1:
    Serial.print(SHT);Serial.print(" ");
    Serial.print(SHH);Serial.print(" ");
    Serial3.print(SHT);Serial3.print('\t');
    Serial3.print(SHH);Serial3.print('\t');
    break; 
   }    
// PT100
  if(Pt==0){TP1=00;TP2=00;}
  switch(mode){
  case 0:
  Serial.print("TP1:");Serial.print(TP1); Serial.print(" ");
  Serial.print("TP2:");Serial.print(TP2); Serial.print(" ");
  Serial3.print("pt100-temp="); Serial3.print(TP1);Serial3.print('\t');
  Serial3.print("pt100-temp="); Serial3.print(TP2);Serial3.print('\t');
  break;
  case 1:
  Serial.print(TP1); Serial.print(" ");
  Serial.print(TP2); Serial.print(" ");
  Serial3.print(TP1);Serial3.print('\t');
  Serial3.print(TP2);Serial3.print('\t');
  break;  
  }
// SondaDS18B20
  if(Ds==0){DS18=00;}
  switch(mode){
  case 0:
    Serial.print("T-DS18:");Serial.print(DS18);Serial.print(" ");
    Serial3.print("ds18b20-temp=");Serial3.print(DS18);Serial3.print('\t');
    break;
  case 1:
    Serial.print(DS18);Serial.print(" ");
    Serial3.print(DS18);Serial3.print('\t');
    break;    
    }
// SoilWater
  if(Sw==0){SMP=00;}
  switch(mode){
  case 0:
    Serial.print("200ss:");Serial.print(SMP);Serial.print(" ");
    Serial3.print("200ss-humidity="); Serial3.print(SMP);Serial3.print('\t'); 
    break;
  case 1:
    Serial.print(SMP);Serial.print(" ");
    Serial3.print(SMP);Serial3.print('\t'); 
    break;    
    }
// Battery
  if(Bt==0){Bat=000;}
  switch(mode){
  case 0:
    Serial.print("Batt:");Serial.print(Bat);Serial.print(" ");
    Serial3.print("battery");Serial3.print(Bat);Serial3.print('\t');
    break; 
  case 1:
    Serial.print(Bat);Serial.print(" ");
    Serial3.print(Bat);Serial3.print('\t');
    break; 
    }
// Leaf
  if(Lf==0){Hj=000;}
  switch(mode){
  case 0:
    Serial.print("LH:");Serial.print(Hj);Serial.print(" ");
    Serial3.print("leaf-humidity=");Serial3.print(Hj);Serial3.print('\t');
    break; 
  case 1:
    Serial.print(Hj);Serial.print(" ");
    Serial3.print(Hj);Serial3.print('\t');
    break; 
  }
// LM35 - Internal Temperature
  if(Lm==0){celsius=00.00;}
  switch(mode){
  case 0:
    Serial.print("LM35:");Serial.println(celsius);
    Serial3.print("lm35-temp=");Serial3.println(celsius);
    break; 
  case 1:
    Serial.println(celsius);
    Serial3.println(celsius);   
    break;    
    }  
}

float AverageMeasurement(int Port,int K){
  float Result=0,Val=0;
  for(int i=1; i <= K; i++){
    Val = Val+analogRead(Port);
    delay(10);   
  }
  Result=Val/K;return Result;
}
void Filtro(float u, float meassurement){
  float X,X_1,B,F,Q,P,K,H,R;
  u=0; B=0;F=1;Q=1e-9;H=1;R=0.2090;
  X=X*F+B*u;
  P=F*P*F+Q;
  K=P*H/(H*P*H+R);
  X=X+K*(meassurement-H*X);
  P=(1-K*H)*P;
  //Serial.print(X), Serial.print(" ");
}
void SwON(){
  for (pin=0; pin<27; pin=pin+1){digitalWrite(Pines[pin],HIGH);}
}
void SwOFF(){  
  for (pin=0; pin<27; pin=pin+1){digitalWrite(Pines[pin],LOW);}
}
