//Libraries
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include "EEPROM.h"
#include "cc1101.h"
CC1101 cc1101;

//Declaration emetteur
// The LED is wired to the Arduino Output 4 (physical panStamp pin 19)
#define LED 5
// counter to get increment in each loop
byte syncWord = 199;
//Declaration GPS
static const int RXPin = 4, TXPin = 3;
static const uint32_t GPSBaud = 9600;
// The TinyGPS++ object
TinyGPSPlus gps;
// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);
//Declaration variables
float longi;
float lati;



//Functions
void blinker(){
digitalWrite(LED, HIGH);
delay(500);
digitalWrite(LED, LOW);
delay(500);
digitalWrite(LED, HIGH);
delay(500);
digitalWrite(LED, LOW);
delay(500);
}

//Set up
void setup()
{
  Serial.begin(9600);
  ss.begin(GPSBaud);
  Serial.println("Test emetteur");
// setup the blinker output
pinMode(LED, OUTPUT);
digitalWrite(LED, LOW);
// blink once to signal the setup
blinker();
lati=0;
longi=0;
// initialize the RF Chip
Serial.println("initializing...");
cc1101.init();
cc1101.setSyncWord(&syncWord, false);
cc1101.setCarrierFreq(CFREQ_433);
cc1101.disableAddressCheck();
//cc1101.setTxPowerAmp(PA_LowPower);
delay(1000);
Serial.println("device initialized");
}

void send_data() {
CCPACKET data;
data.length=60;
//Variables
int unit; int unite; char sep=';';
//Delimitations des coordonnes
int j=3; int h=7;
//Recupérations des coordonnees
float latitude=lati;
float longitude=longi;
for(int l=0;l<data.length;l++)
{data.data[l]=0;}
//Traitement latitude
while(latitude>100)
{unit=(int)latitude%100;
data.data[j]=unit;
latitude=latitude/100;
j--;
if(latitude<100){data.data[j]=latitude;}
}
//Traitement longitude
while(longitude>100)
{unite=(int)longitude%100;
data.data[h]=unite;
longitude=longitude/100;
h--;
if(longitude<100){data.data[h]=longitude;}
}
data.data[4]=sep;
//Traitement Pression
//Traitement Temperature
//Traitement Accelerometre
//Traitement Current Sensor Arduino
//Traitement Current Sensor Emission
//Traitement des séparateurs
/*data.data[7]=data.data[14]=data.data[20]=data.data[24]=sep;
data.data[28]=data.data[32]=data.data[36]=data.data[40]=sep;
data.data[45]=data.data[48]=data.data[52]=data.data[57]=sep;
*/
//cc1101.flushTxFifo ();
if(cc1101.sendData(data)){
Serial.print("latitude ");
for(int i=0; i<4; i++){
Serial.print(data.data[i]);
if(i==1){Serial.print(",");}
}
Serial.println(" sent ok !!");
Serial.print("longitude ");
for(int k=5; k<8; k++){
Serial.print(data.data[k]);
if(k==5){Serial.print(",");}
}
Serial.println(" sent ok !!");

blinker();
}else{
Serial.println("sent failed !");
}
}

//Loop
void loop()
{
  
// This sketch displays information every time a new sentence is correctly encoded.
  while (ss.available() > 0)
  if (gps.encode(ss.read()))
  {displayInfo();}

  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    while(true);
  }  

}

void displayInfo()
{
  if (gps.location.isValid())
  {
    Serial.println("GPS LOCATION AVAILABLE !");
    lati=gps.location.lat()*10000;
    longi=gps.location.lng()*10000;
    send_data();
  }
  else
  {
    Serial.println("GPS Out of Service, please wait...");
  }

}

