# Agrosensor

## Description
The present work describes the design and implementation of an on-line sensor system to collect environmental data into crops performing real-time acquisition of relevant variables using an Internet of Things IoT platform based on low cost electronics. The system consists of some slave stations powered by solar panels, which acquire the agrometeorological data from the crop and send it via radio frequency (RF using ZigBee) to a master station with electricity and internet connection through 3G communications, publishing the evolution of variables on an open internet website with public access. Hardware and software design, including electronic signals conditioning, software filters for data, early warning protocol and battery protections are presented and described in detail. Preliminary experimental results obtained in laboratory are compared with data generated by meteorological stations providing discussion elements for future developments.

Colombia is at a point where decisions must be made that define the type of development that its society follows in the future and the quality of life of its population. In particular, in the field of agriculture, Colombia faces a series of important challenges that must be faced appropriately. The agricultural production of Tolima region is not different to this situation, since for some years this climatic phenomenon has had repercussions on several crops such as rice, one of the most consumed products in the country and of great importance in the region. As a first step in favor of this initiative and under the initiative of the Ministry of Agriculture, formal studies have been developed on the effects of climate variability on rice production, generating a rather daunting scenario for producers in the region. Given the great participation that the Department of Tolima has in the arrogant market of the country, this situation presents a great concern for multiple points of view, affecting factors such as: the participation of rice in Colombian exports, the number of people and families benefited directly from the rice economy, and the importance of rice in the diet of Colombians. In this way, is necessary to generate solutions that reduce the impact of climate changes on rice production as little as possible. However, in order to implement strategies to mitigate these effects, is vital to have the appropriate information and tools to study them. The increase in temperatures recorded in the last 25 years has produced a decline in rice production in various parts of the world, between 10% and 20%, according to a study published in 'Proceedings of the National Academy of Sciences' (Órtiz, 2012). In this way, researchers have stated that as minimum daytime temperatures rise, or that the nights are hotter, rice production will continue to fall. In Colombia, the "Institute of Hydrology, Meteorology and Environmental Studies" IDEAM and the Ministry of Agriculture, have taken measures in this regard through agroclimatological study projects in the region with the main result being alerts towards the communities of farmers and researchers in order to establish deep field studies that generate particular solutions and strategies to mitigate the climatological effect on rice production in the region (Fernández, 2013) (Cortés & Alarcón, 2016). One of these worrying warning signs corresponds to the prediction of potential yield in irrigated rice crops for the region, specifically for the municipalities of “Espinal” and “Purificación”, for which a decrease of 60% would be expected. 90%, taking into account that the Tolima region has a very low water availability index (Dry) from October to January according to the IDEAM. Given the aforementioned climate change scenarios, a negative water balance could be expected, especially in the second half of the year, associated with few precipitation contributions in Tolima, an increase in reference evapotranspiration rates and an increase in maximum and minimum air temperatures.

For this reason, it is advisable to make a thorough analysis of crops, irrigation systems and water storage systems in order to improve the sowing of the rice crop, the management of irrigation and finally study strategies to mitigate the negative impact in the crops. The main concern of this application is that in order to make an accurate diagnosis it is necessary to have reliable online measurements of the main parameters of the crop area in real time as well as a history that records their corresponding trends, too many precision agriculture applications have emerged from this kind of systems: (Guo & Zhong, 2015), Machine Learning Support (Murcia, Gonzalez, & López, 2017) and connections with other systems (Polo, Duijneveld, Garcia, & Casas, 2015). If there is no certainty and total reliability in the measurement, it is not possible to execute actions on the crop, since there is a risk of affecting it negatively. As an initial response to this situation, the present work seeks to make a contribution to the technology of agricultural practices, creating precision farming tools that allow knowing the specific needs of crops for optimum performance.

## System architecture

The proposed system consists of different measurement stations located on different points into the crop as shown in the Figure 1a and a collector node which receive information from the stations to process it and send it to an internet server via 3G communication, this structure has been successfully tested in similar applications (Peña, Murcia, Londoño, & Botina, 2017). Each station was optimized for low energy consumption and is powered by a solar panel with a LiPo Battery to guarantee their operation 24 hours a day. The soil temperature, soil water potential, air temperature, air relative- humidity, visual solar intensity, ultraviolet light intensity, infrared light intensity, internal system temperature and geoposition (GPS) are acquired every ten minutes in each station. Each one of the measured variables has an associate sensor on board of the “AgroSensor” shield (see Figure 1b), an electronic board designed to be compatible with a popular open source and low-cost development system: The Arduino Mega 2560, a microcontroller board based on the ATmega2560 with 54 digital input/output lines (of which 15 can be used as PWM outputs), 16 analog inputs, 4 UARTs (hardware serial ports), a 16 MHz crystal oscillator, a USB connection, a power jack, an ICSP header, and a reset button (Arduino, 2018). Besides the instruments, the “AgroSensor” has the conditioning circuits, protections and soft filters to improve the information extraction from the signals in each case.

The information package is labeled according to the station and variable, then is sent to the collector node by using Xbee devices (Communication modules which can be configured in a network with multiple connections on a collector node) in a Wireless Network (Digi, 2018). On the other side, the collector node receives the data from all the stations located into the crop. It consists in an embedded system Raspberry Pi with 1.4GHz 64-bit quad-core processor, dual-band wireless LAN, Bluetooth 4.2/BLE, faster Ethernet, and Power-over-Ethernet support (Raspberry, 2018) with internet connection by using a cellular modem GSM/GPRS which is used to upload the crop data to a web site through the “ThingSpeak”, a web server with an open access IoT source information platform that comprehensive in storing the sensor data of varied ‘IoT applications’ and conspire the sensed data output in graphical form at the web level (Maurerira, Oldenhof, & Teerntra, 2011), (Jaishetty & Patil, 2016), (ThingSpeak, 2018) ; and to communicate the system with the user or developer in a remote Secure Shell "SSH" session by using Logmein Hamachi software, a hosted Virtual Private Network VPN service that lets you securely extend LAN-like networks to distributed teams, mobile workers (LogmeIn Hamachi, 2018) . The station executes a control algorithm based on C++ programed in Arduino, the Raspberry Pi executes a Raspbian operative system based on Linux Debian and a control algorithm for the system programed in Python, both available on (Murcia, AgroSense GitHub, 2018) . The Figure 2, illustrates the main components of the system and its relation.

## Related documents
* [Murcia-Moreno, H., González-Quintero, B., & López-Gaona, J. (2017, November). An online learning method for embedded decision support in agriculture irrigation. In International Conference of ICT for Adapting Agriculture to Climate Change (pp. 234-249). Springer, Cham.](https://link.springer.com/chapter/10.1007/978-3-319-70187-5_18)
* [Towards the connection of agriculture with the internet of  things in Colombian farms](https://github.com/HaroldMurcia/AgroSense/blob/master/Eventos/Murcia_TOWARDS%20THE%20CONNECTION%20OF%20AGRICULTURE%20WITH%20THE%20INTERNET%20OF%20THINGS%20IN%20COLOMBIAN%20FARMS%20.pdf)
* [Jiménez Tovar, D.J. (2019). Aprendizaje automático para toma decisiones en aplicaciones de riego inteligente. Tesis de pregrado en Ingeniería Electrónica. Universidad de Ibagué, Tolima, Colombia](https://repositorio.unibague.edu.co/jspui/bitstream/20.500.12313/1308/1/Trabajo%20de%20grado.pdf)

## Related videos
[![Agrosensor](https://sites.google.com/a/unibague.edu.co/si2c/_/rsrc/1498445005827/species-description/AgroS.004.jpg)](https://www.youtube.com/watch?v=UN_PwREh9Nc&feature=emb_logo)


[![AgroSense](https://sites.google.com/a/unibague.edu.co/si2c/_/rsrc/1498445094765/species-description/Screen%20Shot%202017-06-25%20at%209.43.59%20PM.png?height=266&width=400)](https://www.youtube.com/watch?v=Ysgb1RF3knw&feature=emb_logo)

## Other repositories
* [Smart AgroSensor for Irrigation Applications](https://github.com/HaroldMurcia/smartAgroS)

## Referencess
* Adafruit datasheet. (2018). Obtenido de https://cdn-shop.adafruit.com/datasheets/TSL2561.pdf
* Allen, R. (2000). Calibration for the watermark 200ss soil water potential sensor to fit the 7-19-96 “calibration# 3” table from
* Irrometer. University of Idaho, Kimberley, Idaho, USA.
* Arduino. (2018). Obtenido de Technical Information Arduino MEGA: https://store.arduino.cc/usa/arduino-mega-2560-rev3
* Cortés, Y., & Alarcón, J. (2016). Impactos del cambio climático sobre las áreas óptimas de nueve cultivos en Cundinamarca-
Colombia. Revista Temas Agrarios 21(2), 51-64.
* Digi. (2018). Xbee Modules. Obtenido de [https://www.digi.com/xbee
* DS18B20 datasheet. (2018). Obtenido de https://datasheets.maximintegrated.com/en/ds/DS18B20.pdf
* Fernández, M. E. (2013). Efectos del cambio climático en el rendimiento de tres cultivos mediante el uso del Modelo AquaCrop. Bogotá: FONADE, IDEAM y BID.
* Guo, T., & Zhong, W. (2015). Design and implementation of the span greenhouse agriculture Internet of Things system. IEEE
* International Conference on Fluid Power and Mechatronics (FPM) 2015 (págs. 398-401). IEEE.
* Irrometer datasheet. (2018). Obtenido de https://www.kimberly.uidaho.edu/water/swm/Calibration_Watermark2.htm
* Jaishetty, A., & Patil, R. (2016). IoT Sensor Network Based Approach for Agricultural Field Monitoring and Control.
* IJRET:International Journal of Research in Engineering and Technology vol (5), 06.
* LogmeIn Hamachi. (2018). Obtenido de http://vpn.net
* Maurerira, M., Oldenhof, D., & Teerntra, L. (2011). ThingSpeak–an API and Web Service for the Internet of Things. Obtenido de https://staas.home.xs4all.nl/t/swtr/documents/wt2014_thingspeak.pdf
* Murcia, H. (2018). AgroSense GitHub. Obtenido de GitHub: https://github.com/HaroldMurcia/AgroSense
* Murcia, H., Gonzalez, T., & López, J. (2017). An Online Learning Method for Embedded Decision Support in Agriculture Irrigation. En Advances in Information and Communication Technologies for Adapting Agriculture to Climate Change: Proceedings of the International Conference of ICT for Adapting Agriculture to Climate Change (AACC'17) Vol 687 (pág. 234). Popayán: Springer.
* Órtiz, R. (2012). El cambio climático y la producción agrícola. Banco Interamericano de desarrollo. Banco Interamericano de desarrollo BID.
* Paayero, J., Khalilian, A., Mirzakhani-Nafchi, A., & Davis, R. (2017). Evaluating the Effect of Soil Texture on the Response of
Three Types of Sensors Used to Monitor Soil Water Status. Journal of Water Resource and Protection, vol 9(06), 566.
* Peña, L., Murcia, H., Londoño, W., & Botina, H. (2017). Low-Cost Alternative for the Measurement of Water Levels in Surface Water Streams. Sensors & Transducers, 217(11), 36-44.
* Polo, J., Duijneveld, C., Garcia, A., & Casas, O. (2015). Design of a low-cost wireless sensor network with UAV mobile node for agricultural application. Computers and electronics in agriculture, 19-32.
PT100 datasheet. (2018).Obtenido de http://www2.schneider-electric.com/resources/sites/SCHNEIDER_ELECTRIC/content/live/FAQS/239000/FA239836/ru_RU/5pt100sensoren_e.pdf
* Raspberry. (2018). Obtenido de Techincal Information Raspberry Pi 3+: https://www.raspberrypi.org/products/raspberry-pi-3- model-b-plus
* Sparkfun air T/H datadheet. (2018). Obtenido de https://www.sparkfun.com/datasheets/Sensors/SHT1x_datasheet.pdf
* Sparkfun GPS. (2018). Obtenido de Sparkfun: https://cdn.sparkfun.com/datasheets/GPS/GP-20U7.pdf
* Sparkfun ML-8511. (2018). Obtenido de https://learn.sparkfun.com/tutorials/ml8511-uv-sensor-hookup-guide
* Sparkfun MPPT. (2018). Obtenido de https://www.sparkfun.com/products/12885]
* ThingSpeak. (2018). Obtenido de [https://thingspeak.com
* TI datasheet. (2018). Obtenido de http://www.ti.com/lit/ds/symlink/lm335.pdf
