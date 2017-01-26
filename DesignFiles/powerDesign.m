% Universidad de Ibagu?
% Research group: SI2C
% January of 2017
% Calc: Solar Panel

clear,clc,close all

Vdc=5;
Ih=0.35;
iL=0.08;
T=10*60;
Th=5;
Tl=T-Th;
idc=(Ih*Th+iL*Tl)/T;
P=idc*Vdc;
n=0.85;
Ec=P*24/n;  % Energy consumption

Hs=6;       % Sun hours
Hb=24-Hs;    
securityNum=4;
Ppanel=(Ec/Hs)*securityNum
Pbatt=Ec/(Hb)
