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
// LED
#define LED 5
byte syncWord = 199;
//Declaration GPS
static const int RXPin = 4, TXPin = 3;
static const uint32_t GPSBaud = 9600;
// The TinyGPS++ object
TinyGPSPlus gps;
// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);
//Declaration Current Sensor
Adafruit_INA219 ina219_A; // Current sensor de la batterie Arduino
Adafruit_INA219 ina219_E(0x41); // Current sensor de la batterie Emission
int batterieArduino = 0;
int batterieEmission = 0;
//Declaration variables
float longi;
float lati;



//Functions (à adapter dans Navsafe)
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
// Initialisation
Serial.begin(9600);
ss.begin(GPSBaud);
// Current Sensors
uint32_t currentFrequency;
ina219_A.begin(); // Current sensor de la batterie Arduino
ina219_E.begin(); // Current sensor de la batterie Emission
// Depart
Serial.println("Test emetteur");
// setup the blinker output
pinMode(LED, OUTPUT);
digitalWrite(LED, LOW);
// Signal LED du setup
blinker();
lati=0;
longi=0;
// Initialisation du RF Chip
Serial.println("INITIALISATION...");
cc1101.init();
cc1101.setSyncWord(&syncWord, false);
cc1101.setCarrierFreq(CFREQ_433);
cc1101.disableAddressCheck();
//cc1101.setTxPowerAmp(PA_LowPower);
delay(1000);
Serial.println("EMETTEUR INITIALISE");
}

void send_data() 
{
// Initialisation de l'émission  
CCPACKET data;
data.length=41;
//Variables
int unit; int unite; int unita;
char sep=';'; 
//Delimitations des coordonnees
int h=7; int n=27;
data.data[4]=sep;
// Initialisation de la chaîne à une chaîne de 0
for(int u=0;u<data.length;u++)
{data.data[u]=0;}
//Traitement des séparateurs
data.data[4]=sep;
data.data[8]=sep;
//Traitement latitude
//Recupérations des coordonnees
float latitude=lati;
<<<<<<< Updated upstream
// Préparation à l'envoi
for (int j=3; j>0; j--)
{unit=(int)latitude%100;
data.data[j]=unit;
latitude=latitude/100;
if(latitude<100){data.data[j-1]=latitude;}
=======
float longitude=longi;
Serial.println(longitude);
Serial.println(latitude);
for(int l=0;l<data.length;l++)
{data.data[l]=0;}
//Traitement latitude
while(latitude>100)
{unit=(int)latitude%100;
data.data[j]=unit;
latitude=latitude/100;
Serial.println(latitude);
j--;
if(latitude<100){data.data[j]=latitude;}
>>>>>>> Stashed changes
}
//Traitement longitude
float longitude=longi;
while(longitude>100)
{unite=(int)longitude%100;
data.data[h]=unite;
longitude=longitude/100;
Serial.println(longitude);
h--;
if(longitude<100){data.data[h]=longitude;}
}
//Traitement Pression
//Traitement Temperature
//Traitement Accelerometre
//Traitement Current Sensor Arduino
float busvoltageA = 0;
busvoltageA = ina219_A.getBusVoltage_V();
busvoltageA=busvoltageA*100;
while(busvoltageA>100)
{unita=(int)busvoltageA%100;
data.data[n]=unita;
busvoltageA=busvoltageA/100;
n--;
if(busvoltageA<100){data.data[n]=busvoltageA;}
}
//Traitement Current Sensor Emission
// Envoi des données
if(cc1101.sendData(data))
{
// Afficher Latitude
Serial.print("Latitude: ");
for(int i=0; i<4; i++){
Serial.print(data.data[i]);
if(i==1){Serial.print(",");}
}
Serial.println(" sent ok !!");
Serial.print("Longitude: ");
for(int k=5; k<8; k++){
Serial.print(data.data[k]);
if(k==5){Serial.print(",");}
}
Serial.println(" sent ok !!");
Serial.print("ARDUINO-Tension: ");
Serial.print(data.data[9]);
Serial.print(",");
Serial.print(data.data[10]);
Serial.println(" sent ok !!");

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
    delay(2000);
  }
  else
  {
    Serial.println("GPS Out of Service, please wait...");
  }

}

