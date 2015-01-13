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
data.length=13;
//Variables
int unit; int unite;
//Delimitations des coordonnes
int j=6; int h=12;
//Recupérations des coordonnees
float latitude=lati;
float longitude=longi;
Serial.println(latitude);
Serial.println(longitude);
for(int l=0;l<data.length;l++)
{data.data[l]=0;}
//Traitement latitude
while(latitude>10)
{unit=(int)latitude%10;
data.data[j]=unit;
latitude=latitude/10;
j--;
if(latitude<10){data.data[j]=latitude;}
}
//Traitement longitude
while(longitude>10)
{unite=(int)longitude%10;
data.data[h]=unite;
longitude=longitude/10;
h--;
if(longitude<10){data.data[h]=longitude;}
}
//cc1101.flushTxFifo ();
if(cc1101.sendData(data)){
Serial.print("latitude ");
for(int i=0; i<7; i++){
Serial.print(data.data[i]);
Serial.print(" ");
}
Serial.println(" sent ok !!");
Serial.print("longitude ");
for(int k=7; k<13; k++){
Serial.print(data.data[k]);
Serial.print(" ");
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
Serial.println("coucou");

}

void displayInfo()
{
Serial.println("hello");
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

