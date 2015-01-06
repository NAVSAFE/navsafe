#include "EEPROM.h"
#include "cc1101.h"

CC1101 cc1101;

// The LED is wired to the Arduino Output 4 (physical panStamp pin 19)
#define LED 4

// counter to get increment in each loop
int counter;
byte b;
byte syncWord = 199;

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

void setup()
{
Serial.begin(38400);
Serial.println("Test emetteur");

// setup the blinker output
pinMode(LED, OUTPUT);
digitalWrite(LED, LOW);

// blink once to signal the setup
blinker();

// reset the counter
counter=32767;

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
data.length=10;
int blinkCount=counter++;
data.data[0]=5;
data.data[1]=blinkCount;data.data[2]=0;
data.data[3]=1;data.data[4]=0;
//cc1101.flushTxFifo ();
if(cc1101.sendData(data)){
Serial.print(blinkCount,DEC);
Serial.println(" sent ok !!");
Serial.print(blinkCount,HEX);
Serial.println(" sent ok !!");
blinker();
}else{
Serial.println("sent failed !");
}
}

void loop()
{
send_data();
delay(4000);
}
