#include "EEPROM.h"
#include "cc1101.h"

// The LED is wired to the Arduino Output 4 (physical panStamp pin 19)
#define LED 4

// The connection to the hardware chip CC1101 the RF Chip
CC1101 cc1101;

byte syncWord = 199;
long counter=0;


// a flag that a wireless packet has been received
boolean packetAvailable = false;

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
/* Handle interrupt from CC1101 (INT0) gdo0 on pin2 */
void cc1101signalsInterrupt(void){
// set the flag that a package is available
packetAvailable = true;
}

void setup()
{
Serial.begin(9600);
Serial.println("Test recepteur");

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
cc1101.disableAddressCheck(); //if not specified, will only display "packet received"
//cc1101.setTxPowerAmp(PA_LowPower);
attachInterrupt(0, cc1101signalsInterrupt, FALLING);
delay(1000);
Serial.println("device initialized");
}

void loop()
{
if(packetAvailable){
Serial.println("packet received");
blinker();
// Disable wireless reception interrupt
detachInterrupt(0);

// clear the flag
packetAvailable = false;

CCPACKET packet;

if(cc1101.receiveData(&packet) > 0){
if(!packet.crc_ok) {
Serial.println("crc not ok");
}

if(packet.length > 0){
Serial.print("latitude: ");
if(packet.data[0]==0)
{for(int j=1; j<7; j++){
Serial.println(packet.data[j]);
if(j==2){Serial.print(",");}
}
}
else
{for(int j=0; j<7; j++){
Serial.println(packet.data[j]);
if(j==2){Serial.print(",");}
}
}
Serial.print("longitude: ");
if(packet.data[7]==0)
{for(int k=8; k<packet.length; k++){
Serial.println(packet.data[k]);
if(k==8){Serial.print(",");}
}
}
else
{for(int k=7; k<packet.length; k++){
Serial.println(packet.data[k]);
if(k==8){Serial.print(",");}
}
}
}
}
// Enable wireless reception interrupt
attachInterrupt(0, cc1101signalsInterrupt, FALLING);
}
}
