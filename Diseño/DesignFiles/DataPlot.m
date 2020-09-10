
% Universidad de Ibagu?
% Research group: SI2C
% January of 2017
% Data plot

clear,clc,close all

Data=load('test.txt');


% Light 
    lightVis1=Data(:,6);
    lightVis2=Data(:,9);
    lightIr1=Data(:,7);
    lightIr2=Data(:,10);
    lightUv=Data(:,8);
    Lux=Data(:,11);
% Temperature
    Tair=Data(:,12);
    PT100_1=Data(:,14)+5.0923;
    Ds18=Data(:,16);
    Lm35=Data(:,20)-1.2750;
    
% Humidity
    Hair=Data(:,13);
    SoilMoisture=Data(:,17);
    
figure(1)
    subplot(2,3,1)
    plot(lightVis1)
    grid on
    legend('Visible Si1145')
    subplot(2,3,2)
    plot(lightIr1)
    grid on
    legend('IR Si1145')
    subplot(2,3,3)
    plot(Lux)
    grid on
    legend('Uv Si1145')
    subplot(2,3,4)
    plot(lightVis2)
    grid on
    legend('Visible Flora')
    subplot(2,3,5)
    plot(lightIr2)
    grid on
    legend('IR Flora')
    subplot(2,3,6)
    plot(lightUv)
    grid on
    legend('Lux Flora')

figure(2)
    plot(Tair)
    grid on
    hold on
    plot(PT100_1)
    plot(Ds18)
    plot(Lm35)
    ylabel('Temperature [?C]')
    plot(Data(:,1)-1.2750)
    legend('Air','PT100_{1}','Ds18','Lm35','filter')
    
    
figure(3)
    subplot(211)
    plot(Hair)
    grid on
    legend('RH [%]')
    subplot(212)
    plot(SoilMoisture)
    grid on
    legend('SoilMoisture')
    
