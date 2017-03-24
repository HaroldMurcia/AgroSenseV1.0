# Universidad de Ibague
# Research Group: SI2C
# AgroSense

import serial, time, datetime, requests,os,sys
import urllib, urllib2, httplib, smtplib

# MAIL DATA
from email.MIMEMultipart import MIMEMultipart
from email.MIMEText import MIMEText

# Program Variables
Ts=60;       #Sample Time in Secs

# internet test
urlTest='http://www.google.com'
timeoutTest=5

# Xbee Config
Xbee=serial.Serial("/dev/ttyUSB0",baudrate=9600,timeout=700)
Xbee.flushInput()
dataXbee=''

# Varaibles for ThingSpeak
THINGSPEAKURL ='https://api.thingspeak.com/channels/105645'
THINGSPEAKKEY ='VTI226PU2IPWWL30' 

# Variables
XbeeFail=0;dataXbee='0';ErrorMode=0;InternetFail=0;DataReport=0

def sendEmail(toaddr,subject,body,fileData):
	msg = MIMEMultipart()
	if(fileData!=''):
		msg.attach(MIMEText(file(fileData).read()))
	fromaddr='agrosense.info@gmail.com'
	msg['From'] = fromaddr
	msg['To'] = toaddr
	msg['Subject'] = subject
	msg.attach(MIMEText(body, 'plain'))
	mailer = smtplib.SMTP('smtp.gmail.com', 587)
	mailer.starttls()
	mailer.login(fromaddr, "semilleroSI2C")
	text = msg.as_string()
	mailer.sendmail(fromaddr, toaddr, text)
	mailer.quit()

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

def InternetTest():
    try:
        _ = requests.get(urlTest, timeout=timeoutTest)
        print("Internet ok")
        InternetFail=0
    except requests.ConnectionError:
        print("No internet")
        print("-------------------------")
        InternetFail=InternetFail+1
        if(IntenernetFail>120):
            ErrorMode=2
            InternetFail=0
            print "Reboot"
            time.sleep(1)
            os.system("sudo reboot")
#-------------------------------------------------------------------- MAIN
if __name__ == "__main__":
    print('Iniciando... ')
    os.system("sudo wvdial &")
    time.sleep(20)
    fileText=time.strftime("%d-%m-%y")+'-'+time.strftime("%I-%M-%S")+".txt"
    print("Fichero: "+fileText)
    f = open(fileText,'w')
    f.close()
    InternetTest()
    if(InternetFail==0):
        sendEmail('harold.murcia@unibague.edu.co','System State','System OK','')
    while True:
        f = open(fileText,"a") #opens file with name of "test.txt"
        Tic=time.time()
        try:
            dataXbee=Xbee.readline()
            dataFile=dataXbee
            dataFile=dataFile.strip()
            Times='\t'+time.strftime("%y")+'\t'+time.strftime("%m")+'\t'+time.strftime("%d")+'\t'+time.strftime("%I")+'\t'+time.strftime("%M")+'\t'+time.strftime("%S")+'\r\n'
            dataFile=dataFile+Times
            f.write(dataFile)
            if(dataXbee==''):
                print("Xbee Fail")
                XbeeFail=XbeeFail+1
                if(XbeeFail>10):
                    XbeeFail=0
                    sendEmail('harold.murcia@unibague.edu.co','Fail','Xbee Fail','')
                    ErrorMode=1
                    print "Reboot"
                    time.sleep(1)
                    #os.system("sudo reboot")
            else:
                print "Xbee Ok"
                XbeeFail=0
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
        Toc=time.strftime("%I-%M-%S")
        Toc=Toc.split('-')
        Hour=float(Toc[0])
        Minutes=float(Toc[1])
        if(Hour!=0):
            DataReport=0
        if(Hour==0 and Minutes>0 and Minutes<30 and DataReport==0):
            DataReport=1
            f.close()
            sendEmail('harold.murcia@unibague.edu.co','Report','This is the data report',fileText)
            fileText=time.strftime("%d-%m-%y")+'-'+time.strftime("%I-%M-%S")+".txt"
            print("Fichero: "+fileText)
            f = open(fileText,'w')
            f.close()
        Toc=time.time()
        dT=Toc-Tic
        f.close()
        print('Dt: '+ str(dT))