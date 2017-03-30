# Universidad de Ibague# Research Group: SI2C
# AgroSense

import serial, time, datetime, requests,os,sys
import urllib, httplib, smtplib, socket, fcntl, struct

# MAIL DATA
from email.MIMEMultipart import MIMEMultipart
from email.MIMEText import MIMEText

# Program Variables
Ts=60;       #Sample Time in Secs

# internet test
urlTest='https://thingspeak.com'
timeoutTest=5

# Xbee Config
Xbee=serial.Serial("/dev/ttyACM0",baudrate=9600,timeout=30)
Xbee.flushInput()
dataXbee=''
Ruta='/home/pi/Desktop/AgroSense/Data/'

time.sleep(10)

#Modem Config
userPhone="3118993288"
Modem=serial.Serial("/dev/ttyUSB2",baudrate=115200,timeout=5)
Modem.flushInput()

# Varaibles for ThingSpeak
THINGSPEAKURL  ='https://api.thingspeak.com/channels/105645'
THINGSPEAKURL2 ='https://api.thingspeak.com/channels/249055'
THINGSPEAKKEY  ='VTI226PU2IPWWL30' 
THINGSPEAKKEY2 ='TRKYEPE7132940MJ'

#Variables for Data
varNames=['airHum','airT','batt', 'FI', 'FV', 'internalT', 'lux', 'PT100', 'SII', 'SIV', 'soilMoisture', 'soilT', 'valveState', 'waterT' ]
airHum=''; airT=''; FI=''; FV=''; internalT=''; PT100=''; soilT=''; waterT=''; soilMoisture='';  SII=''; SIV=''; lux=''; batt=''; valveState=''
myiP3G=''; myiPeth=''; warningReport=0; warningReport=0

# Variables System
XbeeFail=0;dataXbee='0';ErrorMode=0; InternetFail=0; DataReport=0; StartReport=0; myiP='0.0.0.0'

def createFile():
    global fileText
    fileText=Ruta+time.strftime("%d-%m-%y")+'-'+time.strftime("%I-%M-%S")+".txt"
    print("Fichero: "+fileText+"\n")
    f = open(fileText,'w')
    f.close()

def getIpAddress(ifname):
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    return socket.inet_ntoa(fcntl.ioctl(
        s.fileno(),
        0x8915,  # SIOCGIFADDR
        struct.pack('256s', ifname[:15])
    )[20:24])

    
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

def sendSMS(recipient,message):
    Modem.write('ATZ\r')
    time.sleep(1)
    Modem.write('AT+CMGF=1\r')
    time.sleep(1)
    Modem.write('''AT+CMGS="'''+recipient+'''"\r''')
    time.sleep(1)
    Modem.write(message+ "\r")
    time.sleep(1)
    Modem.write(chr(26))
    Modem.write(chr(29))
    Modem.write(chr(10))
    time.sleep(1)
    #Modem.close()
 
def ReadData(dataRaw): 
    global varNames, airHum, airT, FI, FV, internalT, PT100, soilT, waterT, soilMoisture,  SII, SIV, lux, batt, valveState
    L=len(dataRaw)
    for k in range (0,L-1):
        try:
            var,aux= dataRaw[k].split('=')
            float(aux)
        except:
            aux=''
        print(aux)
        print(dataRaw[k])
        # Variable Cases
        if varNames[0] in dataRaw[k]:
            if(aux!=''):
                airHum=float(aux)
            else:
                airHum=''
        elif varNames[1] in dataRaw[k]:
            if(aux!=''):
                airT=float(aux)
            else:
                airT=''
        elif varNames[2] in dataRaw[k]:
            if(aux!=''):
                batt=float(aux)
            else:
                batt=''
        elif varNames[3] in dataRaw[k]:
            if(aux!=''):
                FI=float(aux)
            else:
                FI=''
        elif varNames[4] in dataRaw[k]:
            if(aux!=''):
                FV=float(aux)
            else:
                FV=''
        elif varNames[5] in dataRaw[k]:
            if(aux!=''):
                internalT=float(aux)
            else:
                internalT=''
        elif varNames[6] in dataRaw[k]:
            if(aux!=''):
                lux=float(aux)
            else:
                lux=''
        elif varNames[7] in dataRaw[k]:
            if(aux!=''):
                PT100=float(aux)
            else:
                PT100=''
        elif varNames[8] in dataRaw[k]:
            if(aux!=''):
                SII=float(aux)
            else:
                SII=''
        elif varNames[9] in dataRaw[k]:
            if(aux!=''):
                SIV=float(aux)
            else:
                SIV=''
        elif varNames[10] in dataRaw[k]:
            if(aux!=''):
                soilMoisture=float(aux)
            else:
                soilMoisture=''
        elif varNames[11] in dataRaw[k]:
            if(aux!=''):
                soilT=float(aux)
            else:
                soilT=''
        elif varNames[12] in dataRaw[k]:
            if(aux!=''):
                valveState=float(aux)
            else:
                valveState=''
        elif varNames[13] in dataRaw[k]:
            if(aux!=''):
                waterT=float(aux)
            else:
                waterT=''
        else:
            print ('No Data')
          

def ThingSpeak(KEY,d1,d2,d3,d4,d5,d6,d7,d8):
        params = urllib.urlencode( {'field1':d1,'field2':d2,'field3':d3,'field4':d4,'field5':d5,'field6':d6,'field7':d7,'field8':d8,'api_key':KEY})
        headers = {"Content-typZZe": "application/x-www-form-urlencoded","Accept": "text/plain"}
        conn = httplib.HTTPConnection("api.thingspeak.com:80")
        try:
            conn.request("POST", "/update", params, headers)
            response=conn.getresponse()
            print (response.status, response.reason)
            #data=response.read()
            conn.close()
        except:
            print ("Conection Failed")

def InternetTest():
    global InternetFail, StartReport, myiP, userPhone
    try:
        r = requests.get(urlTest, timeout=timeoutTest)
        print("Internet ok. \n")
        InternetFail=0
        #s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        #s.connect(("gmail.com",80))
        #myiP=s.getsockname()[0]
        try:
            myiPeth=getIpAddress('eth0')
        except:
            print('No Ehternet Conection\n')
        try:
            myiP3G=getIpAddress('ppp0')
        except:
            print('No 3G Conection\n')
        print(Modem)
        if(StartReport==0):
            body='AgroSense OK. \n'+'Eth ip Address: '+myiPeth+'\n'+'3G ip Address: '+myiP3G+'\n'+'Date:'+time.strftime("%d-%m-%y")+'\n'+'Time: '+time.strftime("%I-%M-%S")
            sendEmail('harold.murcia@unibague.edu.co','System State',body,'')
            time.sleep(5)
            #sendSMS(userPhone,body)
            StartReport=1
            print('Initial info Sended')
    except requests.ConnectionError:
        print("No internet. \n")
        print("-------------------------------")
        InternetFail=InternetFail+1
        if(InternetFail>12):
            sendSMS(userPhone,'AgroSense.info \n Internet Fail')
            ErrorMode=2
            InternetFail=0
            print ("Reboot")
            time.sleep(1)
            os.system("sudo reboot")

def warningTest():
    global warningReport, userPhone, varNames, airHum, airT, FI, FV, internalT, PT100, soilT, waterT, soilMoisture,  SII, SIV, lux, batt, valveState
    msg=' >> Out of Range\n'+'Date:'+time.strftime("%d-%m-%y")+'\n'+'Time: '+time.strftime("%I-%M-%S")
    HinternalT=30; HsoilT=28; HairT=28
    body=''
    warning=0
    if(warningReport>21):
        warningReport=0
    if(airT>=HairT):
        body=body+'Air Temperature, '
        warningReport=warningReport+1
        warning=1
    if(internalT>=HinternalT):
        body=body+'Internal Temperature, '
        warningReport=warningReport+1
        warning=1
    if(soilT>=HsoilT):
        body=body+'Soil Temperature, '
        warningReport=warningReport+1
        warning=1
    if(warning==1 and warningReport<4):
        #sendSMS(userPhone,body+msg)
        print('---------------------------- enviando mensaje')
        sendEmail('harold.murcia@unibague.edu.co','Out of Range',body+msg,'')
    else:
        print("Normal Data Ranges\n")
    print('warningReport: ')
    print(warningReport)         
#-------------------------------------------------------------------- MAIN
if __name__ == "__main__":
    print('Inicializando Sistema...\n ')
    time.sleep(10)
    os.system("sudo wvdial &")
    time.sleep(10)
    createFile()
    InternetTest()
    while True:
        print("Working \n")
        InternetTest()
        f = open(fileText,"a") #opens file with name of "test.txt"
        Tic=time.time()
        try:
            dataXbee=Xbee.readline()
            dataFile=dataXbee
            print('dataFile: '+dataFile)
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
                    print ("Restarting System")
                    time.sleep(1)
                    #os.system("sudo reboot")
            else:
                print ("Xbee Ok")
                XbeeFail=0
        except KeyboardInterrupt:
            break
        data=dataXbee.split('\t')
        N=len(data)
        print('N: '+str(N))
        print ('Data: '+str(data))
        ReadData(data)
        warningTest()
        try:
            if(N>2):
                ThingSpeak(THINGSPEAKKEY,airT,soilT,waterT,airHum,soilMoisture,lux,FI,FV)
                ThingSpeak(THINGSPEAKKEY2,PT100,SII,SIV,'0',dT,internalT,batt,valveState)
        except:
            print('Error uploading Data')
        Toc=time.strftime("%H-%M-%S")
        Toc=Toc.split('-')
        Hour=float(Toc[0])
        Minutes=float(Toc[1])
        if(Hour!=0):
            DataReport=0
        if(Hour==0 and Minutes>0 and Minutes<20 and DataReport==0):
            f.close()
            if(InternetFail==0):
                filetextR=fileText
                sendEmail('harold.murcia@unibague.edu.co','Report','This is the data report. ',fileTextR)
                DataReport=1
            createFile()
        Toc=time.time()
        dT=Toc-Tic
        f.close()
        print('Dt: '+ str(dT))