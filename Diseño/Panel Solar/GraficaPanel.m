clc,  %close all %,clear all
Datos = xlsread('DatosPanel');
Voltaje = Datos(:,1);
Corriente = Datos(:,2);

figure (1)
plot(Corriente,Voltaje,'b*',Corriente,Voltaje,'b')
title('Panel Solar - Curvas I-V')
xlabel('Voltaje (V)')
ylabel('Corriente (A)') 
grid on