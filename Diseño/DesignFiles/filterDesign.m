% Universidad de Ibagu?
% Research group: SI2C
% January of 2017
% Filter Design

clc,close all

F=1;
B=0;
H=1;
I=1;
Q=0.5e-3;
R=2.5;   %Lm35
%R=0.7;    %Pt100_1
%R=0.06;   %Tair
%R=0.3648;   %Humedad aire
%R=0.01; %Soil moisture
R=0.9818;%Visible Si1145
R=0.9818;%Visible lightIr1
R=0.9818; %Lux
R=0.9818; %lightVis2
R=0.9818; %ightIr2
R=0.9818; %lightUv
y=lightUv;
x=y*0;
P=y*0;P(1)=1e3;
for(k=2:length(y))
    x(k)=x(k-1);
    P(k)=P(k-1)+Q;
    K=P(k)/(P(k)+R);
    x(k)=x(k)+K*(y(k)-x(k));
    P(k)=(1-K)*P(k);
end

plot(y)
hold on
grid on
plot(x)