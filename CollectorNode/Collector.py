# Universidad de Ibague
# Research Group: SI2C
# AgroSense

import serial, time, datetime
import urllib, urllib2, httplib

# Program Variables
Ts=60;       #Sample Time in Secs

Xbee=serial.Serial("/dev/ttyUSB0",baudrate=9600,timeout=700)
Xbee.flushInput()
dataXbee=''

# Varaibles for ThingSpeak
THINGSPEAKURL ='https://api.thingspeak.com/channels/105645'
THINGSPEAKKEY ='VTI226PU2IPWWL30' 

def ThingSpeak(KEY,d1,d2,d3,d4,d5,d6,d7,d8):
        params = urllib.urlencode( {'field1':d1,'field2':d2,'field3':d3,'field4':d4,'field5':d5,'field6':d6,'field7':d7,'field8':d8,'api_key':'VTI226PU2IPWWL30'})
        headers = {"Content-typZZe": "application/x-www-form-urlencoded","Accept": "text/plain"}
        conn = httplib.HTTPConnection("api.thingspeak.com:80")
        try:
            conn.request("POST", "/update", params, headers)
            response=conn.getresponse()
            print response.status, response.reason
            #data=response.read()
            conn.close()
        except:
            print "Conection Failed"



if __name__ == "__main__":
        print('Iniciando... ')
        file=time.strftime("%d-%m-%y")+'-'+time.strftime("%I-%M-%S")+".txt"
	print("Fichero: "+file)
	f = open(file,'w')
        f.close()
        while True:
                f = open(file,"a") #opens file with name of "test.txt"
                Tic=time.time()
		dataXbee='0'
		try:
			dataXbee=Xbee.readline()
			dataFile=dataXbee
			dataFile=dataFile.strip()
			Times='\t'+time.strftime("%y")+'\t'+time.strftime("%m")+'\t'+time.strftime("%d")+'\t'+time.strftime("%I")+'\t'+time.strftime("%M")+'\t'+time.strftime("%S")+'\r\n'
			dataFile=dataFile+Times
			f.write(dataFile)
		except KeyboardInterrupt:
			break
                data=dataXbee.split('\t')
		N=len(data)
		print('N: '+str(N))
                print ('Data: '+str(data))
                try:
               		if(N>19):
                		ThingSpeak(THINGSPEAKKEY,data[17],data[9],data[10],data[11],data[12],data[13],data[19],data[16])
                except:
                	print('Error uploading Data')
                # Sample Time
                Toc=time.time()
                dT=Toc-Tic
                f.close()
                print('Dt: '+ str(dT))
                #while(Ts>dT):
                #    Toc=time.time()
                #    dT=Toc-Tic
