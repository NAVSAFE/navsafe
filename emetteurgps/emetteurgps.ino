//Libraries
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include "EEPROM.h"
#include "cc1101.h"
CC1101 cc1101;
//Library current sensor
#include <Wire.h>
#include <Adafruit_INA219.h>


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
//Declaration CSA
Adafruit_INA219 ina219; // Current sensor de la batterie Arduino
int batterieArduino = 0;
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
  ina219.begin();
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
data.length=11;
//Variables
int unit; int unite; char sep=';'; int unita;
//Delimitations des coordonnes
int j=3; int h=7; int n=10;
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
float busvoltageA = 0;
  busvoltageA = ina219.getBusVoltage_V();
  Serial.println(busvoltageA);
  busvoltageA=busvoltageA*100;
  Serial.println(busvoltageA);
while(busvoltageA>100)
{unita=(int)busvoltageA%100;
data.data[n]=unita;
busvoltageA=busvoltageA/100;
n--;
if(busvoltageA<100){data.data[n]=busvoltageA;}
}


data.data[9]=(int)busvoltageA;
//Traitement Current Sensor Emission
//Traitement des séparateurs
data.data[4]=sep;
data.data[8]=sep;
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
Serial.print("Voltage: ");
Serial.print(data.data[9]);
Serial.print(data.data[10]);
Serial.println(" V");

Serial.print((char)data.data[4]);


blinker();
}
else{
Serial.println("sent failed !");
}
}

//Loop
void loop()
{
  
// This sketch displays information every time a new sentence is correctly encoded.
 /* while (ss.available() > 0)
  if (gps.encode(ss.read()))
  {displayInfo();}

  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    while(true);
  }  
*/

 send_data();
 delay(2000);

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

