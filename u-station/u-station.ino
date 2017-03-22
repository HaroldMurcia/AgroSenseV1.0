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
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>

//SoftwareSerial Xbee(15, 14);//(RX, TX)
SI114X SI1145 = SI114X();    
SHT1x sht1x(41,39);             //----(dataPin, clockPin)
RTC_DS1307 rtc;       
TSL2561 tsl(TSL2561_ADDR_FLOAT);  
OneWire oneWire_in(A5);DallasTemperature DS18B20(&oneWire_in); 
volatile int f_wdt=1; // WathDogTImer
int Anio,Mes,Dia,Hora,Min,FV,FI,FL,SIV,SII,V1,V2,PT1,PT2,Hj,Bat,pin,md;
int Clk,Si,Fl,Sh,Pt,Ds,Bt,Lf,Lm,Sw;
float DS18,SHT,SHH,SIU,res,P2,TP1,TP2,PT,millivolts,celsius,SMP;
char  daysOfTheWeek[7][12]={"S","M","T","W","T","F","S"};
float ElapsedTime,tic,toc;
float const Ts=600000;
int Pines[]={2,3,4,5,6,7,8,9,10,23,24,25,26,27,29,30,31,32,33,34,35,37,46,50,52,53};
float X,P; // Filtro
int long SleepCycles;
// Watchdog Interrupt Service. This is executed when watchdog timed out.
ISR(WDT_vect) {
  if(f_wdt == 0) {
    // here we can implement a counter the can set the f_wdt to true if
    // the watchdog cycle needs to run longer than the maximum of eight
    // seconds.
    f_wdt=1;
  }
}

  struct DATA {
    float meassurement;
    float P;
    float Q;
    float R;
    float X;
  };
  DATA LM35={0,1e3,0.5e-3,2.5,0};
  DATA PT100_1={0,1e3,0.5e-3,0.7,0};
  DATA TAIR={0,1e3,0.5e-3,0.06,0};
  DATA HAIR={0,1e3,0.5e-3,0.3648,0};
  DATA SOILM={0,1e3,0.5e-3,0.01,0};
  DATA VIS1={0,1e3,0.5e-3,0.9818,0};
  DATA IR1={0,1e3,0.5e-3,0.9818,0};
  DATA UV1={0,1e3,0.5e-3,0.9818,0};
  DATA VIS2={0,1e3,0.5e-3,0.9818,0};
  DATA IR2={0,1e3,0.5e-3,0.9818,0};
  DATA LUX={0,1e3,0.5e-3,0.9818,0};


///////////////////////////////////////////////
// Enters the arduino into sleep mode.
void enterSleep(void)
{
  // There are five different sleep modes in order of power saving:
  // SLEEP_MODE_IDLE - the lowest power saving mode
  // SLEEP_MODE_ADC
  // SLEEP_MODE_PWR_SAVE
  // SLEEP_MODE_STANDBY
  // SLEEP_MODE_PWR_DOWN - the highest power saving mode
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();

  // Now enter sleep mode.
  sleep_mode();

  // The program will continue from here after the WDT timeout

  // First thing to do is disable sleep.
  sleep_disable();

  // Re-enable the peripherals.
  power_all_enable();
}

// Setup the Watch Dog Timer (WDT)
void setupWatchDogTimer() {
  // The MCU Status Register (MCUSR) is used to tell the cause of the last
  // reset, such as brown-out reset, watchdog reset, etc.
  // NOTE: for security reasons, there is a timed sequence for clearing the
  // WDE and changing the time-out configuration. If you don't use this
  // sequence properly, you'll get unexpected results.

  // Clear the reset flag on the MCUSR, the WDRF bit (bit 3).
  MCUSR &= ~(1<<WDRF);

  // Configure the Watchdog timer Control Register (WDTCSR)
  // The WDTCSR is used for configuring the time-out, mode of operation, etc

  // In order to change WDE or the pre-scaler, we need to set WDCE (This will
  // allow updates for 4 clock cycles).

  // Set the WDCE bit (bit 4) and the WDE bit (bit 3) of the WDTCSR. The WDCE
  // bit must be set in order to change WDE or the watchdog pre-scalers.
  // Setting the WDCE bit will allow updates to the pre-scalers and WDE for 4
  // clock cycles then it will be reset by hardware.
  WDTCSR |= (1<<WDCE) | (1<<WDE);

  /**
   *  Setting the watchdog pre-scaler value with VCC = 5.0V and 16mHZ
   *  WDP3 WDP2 WDP1 WDP0 | Number of WDT | Typical Time-out at Oscillator Cycles
   *  0    0    0    0    |   2K cycles   | 16 ms
   *  0    0    0    1    |   4K cycles   | 32 ms
   *  0    0    1    0    |   8K cycles   | 64 ms
   *  0    0    1    1    |  16K cycles   | 0.125 s
   *  0    1    0    0    |  32K cycles   | 0.25 s
   *  0    1    0    1    |  64K cycles   | 0.5 s
   *  0    1    1    0    |  128K cycles  | 1.0 s
   *  0    1    1    1    |  256K cycles  | 2.0 s
   *  1    0    0    0    |  512K cycles  | 4.0 s
   *  1    0    0    1    | 1024K cycles  | 8.0 s
  */
  WDTCSR  = (1<<WDP3) | (0<<WDP2) | (0<<WDP1) | (1<<WDP0);
  // Enable the WD interrupt (note: no reset).
  WDTCSR |= _BV(WDIE);
}

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
  setupWatchDogTimer();  
}



  

void loop(){
  tic=millis();
  
  // Wait until the watchdog have triggered a wake up.
  if(f_wdt != 1) {
    return;
  }
  
 // Boton();
  SwON();
  delay(1000);
//  Clock();        //--->Reloj
//  Si1145();       //--->Sensor IF,UV,VIS  
//  Flora();          //--->Sensor Flora  
  Sht10();          //--->Sensor Tem,Hum
  Pt100();          //--->Sonda Pt100
//  SondaDS18B20();   //--->Sonda Ds18B20  
  Battery();
//  Leaf(); 
  lm35(); 
  SoilWater(celsius);
  SwOFF();
  
  // Filtrado
  Filtro(celsius,LM35.P,LM35.Q,LM35.R,LM35.X);
  LM35.P=P;LM35.X=X;
  
  Filtro(TP1,PT100_1.P,PT100_1.Q,PT100_1.R,PT100_1.X);
  PT100_1.P=P;PT100_1.X=X;

  Filtro(SHT,TAIR.P,TAIR.Q,TAIR.R,TAIR.X);
  TAIR.P=P;TAIR.X=X;

  Filtro(SHH,HAIR.P,HAIR.Q,HAIR.R,HAIR.X);
  HAIR.P=P;HAIR.X=X;

  Filtro(SMP,SOILM.P,SOILM.Q,SOILM.R,SOILM.X);
  SOILM.P=P;SOILM.X=X;

  Filtro(SIV,VIS1.P,VIS1.Q,VIS1.R,VIS1.X);
  VIS1.P=P;VIS1.X=X;

  Filtro(SII,IR1.P,IR1.Q,IR1.R,IR1.X);
  IR1.P=P;IR1.X=X;

  Filtro(SIU,UV1.P,UV1.Q,UV1.R,UV1.X);
  UV1.P=P;UV1.X=X;

  Filtro(FV,VIS2.P,VIS2.Q,VIS2.R,VIS2.X);
  VIS2.P=P;VIS2.X=X;

  Filtro(FI,IR2.P,IR2.Q,IR2.R,IR2.X);
  IR2.P=P;IR2.X=X;

  Filtro(FL,LUX.P,LUX.Q,LUX.R,LUX.X);
  LUX.P=P;LUX.X=X;

  pinMode(11,OUTPUT);
  digitalWrite(11,LOW);
  delay(500);
  PrintData(1);
  delay(1000);
  pinMode(11,INPUT);
  digitalWrite(11,HIGH);
  
  //SampleTime measurement
 
  toc=millis();
  //ElapsedTime=(toc-tic);
  //SleepCycles=74; //para 10 min
  SleepCycles=8;
  
  // clear the flag so we can run above code again after the MCU wake up
  f_wdt = 0;
  for (int i=0; i <= SleepCycles; i++){
    enterSleep();
  }
  
  //while(ElapsedTime<Ts){
  //  toc=millis();
  //  ElapsedTime=(toc-tic);
  //  Serial.println("HOLA");
  //  enterSleep();
  //}
  //Serial.print("dTime:");
 // Serial.println('\t');
 // Serial.println(ElapsedTime/1000.0);
 // Serial3.print('dTime:');
 // Serial3.println(ElapsedTime/1000.0);
}



/// Functions
//void Boton(){
//  B1 = digitalRead(32);
//  if(B1==1 && md==0){
//    md=1;SwON();delay(2000);SwOFF();}
//  if(B1==1 && md==1){
//    md=0;SwON();delay(2000);SwOFF();}
//}


void Clock(){
  Clk=1;
  rtc.begin();delay(100);
  //rtc.adjust(DateTime(2017, 1, 13, 12, 40, 0));
  DateTime now = rtc.now();delay(1);
  Anio=now.year(); Mes=now.month(); Dia=now.day(); Hora=now.hour(); Min=now.minute();
}
void Si1145(){
  Si=1;
  SI1145.Begin();
  SIV = SI1145.ReadVisible();       delay(1);
  SII = SI1145.ReadIR();            delay(1);
  SIU = (float)SI1145.ReadUV()/100; delay(1);
}
void Flora(){
  Fl=1;
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
void Sht10(){
  Sh=1;
  SHT = sht1x.readTemperatureC(); delay(1);
  SHH = sht1x.readHumidity();     delay(1);
//SHF = sht1x.readTemperatureF();  
}
void Pt100(){
  Pt=1;
  int k = 10;
  //TP1 = analogRead(A1);
  //TP2 = analogRead(A2);
  PT1 = AverageMeasurement(1,k) ;
  TP1 = PT1*0.4473-3.1594; 
  PT2 = AverageMeasurement(2,k) ;
  TP2 = PT2*0.4374   -3.1594; 
}
void SondaDS18B20(){
  Ds=1;
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
  //SMP=(-0.0032*res-4.093)/(1-9.7330e-06*res-0.01205*T);
  SMP=res;
  digitalWrite(52,LOW);
  digitalWrite(53, LOW);    
}

void Battery(){
  Bt=1;
  float B_val=0; int k=10;
  Bat=AverageMeasurement(14,k) ;
  Bat = Bat/757.0*100;
}
void Leaf(){
  Lf=1;
  float H_val=0; int k=10;
  H_val=AverageMeasurement(15,k) ;
  Hj = H_val;
}

void lm35(){
  Lm=1;
  float X,P,P_1,Q,K,R; //FILTRO
  int k=10; float T_val=0;
  
  T_val=AverageMeasurement(4,k) ;
  millivolts = (T_val / 1023.0) * 5000.0;
  celsius = millivolts / 10.0 -3.575;
}


void Filtro(float meassurement, float p, float Q, float R, float x){
  p=p+Q;
  float K=p/(p+R);
  x=x+K*(meassurement-x);
  p=(1-K)*p;
  //P_1=P;
  X=x; P=p; 
}


void PrintData(int mode){
//CLK
  if(Clk==0){Anio=0000;Mes=00;Dia=00;Hora=00;Min=00;}
  switch(mode){
    case 0:
    Serial.print(Anio); Serial.print('\t'); Serial.print(Mes);  Serial.print('\t');
    Serial.print(Dia);  Serial.print('\t'); Serial.print(Hora); Serial.print('\t');
    Serial.print(Min);  Serial.print('\t'); 
    Serial3.print("year=");  Serial3.print(Anio); Serial3.print('\t');
    Serial3.print("month="); Serial3.print(Mes);  Serial3.print('\t');
    Serial3.print("day=");   Serial3.print(Dia);  Serial3.print('\t');
    Serial3.print("hour=");  Serial3.print(Hora); Serial3.print('\t');
    Serial3.print("minute=");Serial3.print(Min);  Serial3.print('\t');
    break;
    case 1:
    Serial.print(Anio); Serial.print('\t'); Serial.print(Mes);  Serial.print('\t');
    Serial.print(Dia);  Serial.print('\t'); Serial.print(Hora); Serial.print('\t');
    Serial.print(Min);  Serial.print('\t'); 
    Serial3.print(Anio); Serial3.print('\t'); Serial3.print(Mes);  Serial3.print('\t');
    Serial3.print(Dia);  Serial3.print('\t'); Serial3.print(Hora); Serial3.print('\t');
    Serial3.print(Min);  Serial3.print('\t');
    break;}
// Light Si1145
  if(Si==0){SIV=000;SII=000;SIU=0.00;}
  switch(mode){
    case 0:
    Serial.print("SIV:");Serial.print(VIS1.X);Serial.print('\t');
    Serial.print("SII:");Serial.print(IR1.X);Serial.print('\t');
    Serial.print("SIU:");Serial.print(UV1.X);Serial.print('\t');
    Serial3.print("si1145-visible=");   Serial3.print(VIS1.X);Serial3.print('\t');
    Serial3.print("si1145-infrared=");  Serial3.print(IR1.X);Serial3.print('\t');
    Serial3.print("si1145-uv=");        Serial3.print(UV1.X);Serial3.print('\t'); 
    break;
    case 1:
    Serial.print(VIS1.X);Serial.print('\t');
    Serial.print(IR1.X);Serial.print('\t');
    Serial.print(SIU);Serial.print('\t');
    Serial3.print(VIS1.X);Serial3.print('\t');
    Serial3.print(IR1.X);Serial3.print('\t');
    Serial3.print(UV1.X);Serial3.print('\t');
    break;
    }
// Light Flora
  if(Fl==0){FV=000;FI=000;FL=0.00;}
  switch(mode){
    case 0:
    Serial.print("FV:");Serial.print(VIS2.X);Serial.print('\t'); 
    Serial.print("FI:");Serial.print(IR2.X);Serial.print('\t');
    Serial.print("Lux:");Serial.print(LUX.X);Serial.print('\t');
    Serial3.print("flora-visible=");   Serial3.print(VIS2.X);Serial3.print('\t'); 
    Serial3.print("flora-infrared=");  Serial3.print(IR2.X);Serial3.print('\t');        
    Serial3.print("flora-lux=");       Serial3.print(LUX.X);Serial3.print('\t');
    break;
    case 1:
    Serial.print(VIS2.X);Serial.print('\t');
    Serial.print(IR2.X);Serial.print('\t');
    Serial.print(LUX.X);Serial.print('\t');
    Serial3.print(VIS2.X);Serial3.print('\t'); 
    Serial3.print(IR2.X);Serial3.print('\t');        
    Serial3.print(LUX.X);Serial3.print('\t');
    break;       
    }
// SHT10
  if(Sh==0){SHT=000;SHH=000;}
  switch(mode){
    case 0:
    Serial.print("SHT:");Serial.print(TAIR.X);Serial.print('\t');
    Serial.print("SHH:");Serial.print(HAIR.X);Serial.print('\t');
    Serial3.print("sht10-temp=");      Serial3.print(TAIR.X);Serial3.print('\t');
    Serial3.print("sht10-humidity=");  Serial3.print(HAIR.X);Serial3.print('\t'); 
    break;
    case 1:
    Serial.print(TAIR.X);Serial.print('\t');
    Serial.print(HAIR.X);Serial.print('\t');
    Serial3.print(TAIR.X);Serial3.print('\t');
    Serial3.print(HAIR.X);Serial3.print('\t');
    break; 
   }    
// PT100
  if(Pt==0){TP1=00;TP2=00;}
  switch(mode){
  case 0:
  Serial.print("TP1:");Serial.print(PT100_1.X); Serial.print('\t');
  Serial.print("TP2:");Serial.print(TP2); Serial.print('\t');
  Serial3.print("pt100-temp="); Serial3.print(PT100_1.X);Serial3.print('\t');
  Serial3.print("pt100-temp="); Serial3.print(TP2);Serial3.print('\t');
  break;
  case 1:
  Serial.print(PT100_1.X); Serial.print('\t');
  Serial.print(TP2); Serial.print('\t');
  Serial3.print(PT100_1.X);Serial3.print('\t');
  Serial3.print(TP2);Serial3.print('\t');
  break;  
  }
// SondaDS18B20
  if(Ds==0){DS18=00;}
  switch(mode){
  case 0:
    Serial.print("T-DS18:");Serial.print(DS18);Serial.print('\t');
    Serial3.print("ds18b20-temp=");Serial3.print(DS18);Serial3.print('\t');
    break;
  case 1:
    Serial.print(DS18);Serial.print('\t');
    Serial3.print(DS18);Serial3.print('\t');
    break;    
    }
// SoilWater
  if(Sw==0){SMP=00;}
  switch(mode){
  case 0:
    Serial.print("200ss:");Serial.print(SOILM.X);Serial.print('\t');
    Serial3.print("200ss-SoilMoisture="); Serial3.print(SOILM.X);Serial3.print('\t'); 
    break;
  case 1:
    Serial.print(SOILM.X);Serial.print('\t');
    Serial3.print(SOILM.X);Serial3.print('\t'); 
    break;    
    }
// Battery
  if(Bt==0){Bat=000;}
  switch(mode){
  case 0:
    Serial.print("Batt:");Serial.print(Bat);Serial.print('\t');
    Serial3.print("battery");Serial3.print(Bat);Serial3.print('\t');
    break; 
  case 1:
    Serial.print(Bat);Serial.print('\t');
    Serial3.print(Bat);Serial3.print('\t');
    break; 
    }
// Leaf
  if(Lf==0){Hj=000;}
  switch(mode){
  case 0:
    Serial.print("LH:");Serial.print(Hj);Serial.print('\t');
    Serial3.print("leaf-humidity=");Serial3.print(Hj);Serial3.print('\t');
    break; 
  case 1:
    Serial.print(Hj);Serial.print('\t');
    Serial3.print(Hj);Serial3.print('\t');
    break; 
  }
// LM35 - Internal Temperature
  if(Lm==0){celsius=00.00;}
  switch(mode){
  case 0:
    Serial.print("LM35:");Serial.print('\t'); Serial.println(LM35.X);
    Serial3.print("lm35-temp=");Serial3.println(LM35.X);
    break; 
  case 1:
    Serial.print(LM35.X);
    Serial3.println(LM35.X);   
    Serial.print('\t');
    Serial.println(0);
    break;    
    }  
}

float AverageMeasurement(int Port,int k){
  float Result=0,Val=0;
  for(int i=1; i <= k; i++){
    Val = Val+analogRead(Port);
    delay(50);   
  }
  Result=Val/k;return Result;
}


void SwON(){
  for (pin=0; pin<27; pin=pin+1){digitalWrite(Pines[pin],HIGH);}
}
void SwOFF(){  
  for (pin=0; pin<27; pin=pin+1){digitalWrite(Pines[pin],LOW);}
}
