% University of Ibagué
% Temperature sensors - Calibration

clear,clc,close all

RealT=[23.73 26.40 29.50 32.57 37.20 43.70 47.30]; % Termmometrer data
PT100_1=[65 76 84 93 102 112 119];                  % Raw Data 1
PT100_2=[65 73 77 86 97 111 119];                   % Raw Data 2
DS18B20=[22.94 25.24 27.94 31.37 36.13 46.19 48.88];     % Sensor DS18B20

P1=polyfit(PT100_1,RealT,1);
P2=polyfit(PT100_2,RealT,1);
P3=polyfit(DS18B20,RealT,1);

plot(PT100_1,RealT,'o')
hold on
plot(PT100_2,RealT,'*')
plot(DS18B20,RealT,'+')

plot(PT100_1,PT100_1*P1(1)+P1(2))
plot(PT100_2,PT100_2*P2(1)+P2(2))
plot(DS18B20,DS18B20*P3(1)+P3(2))

ylabel('Temperature [ºC]')
xlabel('Raw Data')

legend('Real data PT100 1', 'Real Data Pt100 2','Real Data DS18B20','Pt100 1 linear aprox','PT100 2 linear aprox','DS18B20 linear aprox')
grid on