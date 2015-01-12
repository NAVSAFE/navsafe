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
float longitude=0;
float latitude=0;



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

void send_data() {
CCPACKET data;
data.length=10;
data.data[0]=5;
data.data[1]=latitude;data.data[2]=5;
data.data[3]=5;data.data[4]=longitude;
data.data[5]=5;data.data[6]=5;
data.data[7]=5;data.data[8]=5;
data.data[9]=5;
//cc1101.flushTxFifo ();
if(cc1101.sendData(data)){
Serial.print(latitude,DEC);
Serial.println(" sent ok !!");
Serial.print(longitude,DEC);
Serial.println(" sent ok !!");

blinker();
}else{
Serial.println("sent failed !");
}
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


void displayInfo()
{
  if (gps.location.isValid())
  {
    latitude=gps.location.lat();
    longitude=gps.location.lng();
    Serial.print("Donnees recuperees");
  }
  else
  {
    Serial.print(F("******, ******"));
  }

}

//Loop
void loop()
{
  
// This sketch displays information every time a new sentence is correctly encoded.
  while (ss.available() > 0)
  if (gps.encode(ss.read()))
  displayInfo();

  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    while(true);
  }  
  
send_data();
delay(4000);
}


