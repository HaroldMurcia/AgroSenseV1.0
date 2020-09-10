clc; clear all; %close all;
DatosC = xlsread('Datos AgroSense Modificada')
DatosD = xlsread('Datos Davis Instruments Modificada')
TemAireC = DatosC(:,6)*(0.185)+18.2;
TemAireD = DatosD(:,8);
HumAireC = DatosC(:,7);
HumAireD = DatosD(:,9);
TemIntC = DatosC(:,12);
TemAireX = DatosC(86:184,6)*(0.16)+19.1;
TemAireXD = DatosD(86:184,8);
Size = 0:1:375;
SizeX = 0:1:98;

% figure(1)
% plot(TemIntC)
% axis ([ 0 376 15 50])
% 
% figure(2)
% plot(Size,HumAireD,Size,HumAireC)
% axis ([ 0 376 20 100])
% 
figure(3)
plot(Size,TemAireD,Size,TemAireC)
axis ([ 0 376 20 28])

% figure(4)
% plot(SizeX,TemAireX,SizeX,TemAireXD)
% axis ([ 0 98 20 28])
