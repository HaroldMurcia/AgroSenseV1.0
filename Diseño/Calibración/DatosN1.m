clc,  %close all %,clear all
%Datos = xlsread('DatosSondas')
%Datos(:,1)
NM = 0:1:34;

figure (1)
plot(NM,Datos(:,1),'r--',NM,Datos(:,2),'b')
axis([-5 40 30 55])
grid on

figure (2)
plot(NM,Datos(:,1),'r--',NM,Datos(:,3),'b')
axis([-5 40 30 55])
grid on

figure (3)
plot(NM,Datos(:,1),'r--',NM,(Datos(:,2)*0.92+3.8),'b')
axis([-5 40 30 55])
grid on

figure (4)
plot(NM,Datos(:,1),'r--',NM,(Datos(:,3)*0.98+2.9),'b')
axis([-5 40 30 55])
grid on

figure (5)
plot(NM,Datos(:,1),'r--',NM,(Datos(:,3)*0.98+2.9),'b',NM,(Datos(:,3)),'g')
axis([-5 40 30 55])
title('PT-100 Temperature Calibration')
xlabel('Samples')
ylabel('Measured Temperature') 
grid on

figure (6)
plot(NM,Datos(:,1),'r--',NM,(Datos(:,2)*0.92+3.8),'b',NM,(Datos(:,2)),'g')
axis([-5 40 30 55])
title('DS18B20 Soil Temperature')
xlabel('Samples')
ylabel('Temperature in ?C') 
grid on
