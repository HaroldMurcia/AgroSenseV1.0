# Universidad de Ibague
# Research Group: SI2C
# AgroSense

import serial, time, datetime
import urllib, urllib2, httplib

# Program Variables
Ts=60;       #Sample Time in Secs

Xbee=serial.Serial('/dev/ttyACM0',baudrate=9600)
dataXbee=''

# Varaibles for ThingSpeak
THINGSPEAKURL ='https://api.thingspeak.com/channels/105645'
THINGSPEAKKEY ='VTI226PU2IPWWL30' 

def ThingSpeak(KEY,d1,d2):
        params = urllib.urlencode( {'field1':d1,'field2':d2,'api_key':'VTI226PU2IPWWL30'})
        headers = {"Content-typZZe": "application/x-www-form-urlencoded","Accept": "text/plain"}
        conn = httplib.HTTPConnection("api.thingspeak.com:80")
        try:
            conn.request("POST", "/update", params, headers)
            response=conn.getresponse()
            print response.status, response.reason
            data=response.read()
            conn.close()
        except:
            print "Conection Failed"



if __name__ == "__main__":
    while True:
        Tic=time.time()
        dataXbee=""
        dataXbee=Xbee.readline()
        #print (dataXbee)
        try:
                dato1,dato2=dataXbee.split('\t')
                #ThingSpeak(THINGSPEAKKEY,dato1,dato2)
                nows = datetime.datetime.now()
                stime=nows.strftime("%H:%M")
                H,M=stime.split(':')
                ThingSpeak(THINGSPEAKKEY,H,M)
        except:
                print(dataXbee)
        # Sample Time
        Toc=time.time()
        dT=Toc-Tic
        print(dT)
        #while(Ts>dT):
        #    Toc=time.time()
        #    dT=Toc-Tic
