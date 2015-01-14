// Libraries recepteur
#include "EEPROM.h"
#include "cc1101.h"
// The LED is wired to the Arduino Output 4
#define LED 4
// The connection to the hardware chip CC1101 the RF Chip
CC1101 cc1101;
// Variables
byte syncWord = 199;
long counter=0;
// a flag that a wireless packet has been received
boolean packetAvailable = false;


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//___________________________________________________________FONCTIONS_______________________________________________________________//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//___________________________________________________________________________________________________________Fonction LED pour accuser reception des données____

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

//___________________________________________________________________________________________________________Fonction Interrupt pour accuser reception des données____

void cc1101signalsInterrupt(void){
// set the flag that a package is available
packetAvailable = true;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//___________________________________________________________SETUP_______________________________________________________________//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



void setup()
{
// Initialisation
Serial.begin(9600);
Serial.println("TEST RECEPTEUR");

// Setup LED
pinMode(LED, OUTPUT);
digitalWrite(LED, LOW);
// blink once to signal the setup
blinker();

// Initialize the RF Chip
Serial.println("INITIALISATION...");
cc1101.init();
cc1101.setSyncWord(&syncWord, false);
cc1101.setCarrierFreq(CFREQ_433);
cc1101.disableAddressCheck(); //if not specified, will only display "packet received"
//cc1101.setTxPowerAmp(PA_LowPower);
attachInterrupt(0, cc1101signalsInterrupt, FALLING);
delay(1000);
Serial.println("RECEPTEUR INITIALISE");
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//___________________________________________________________LOOP____________________________________________________________________//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void loop()
{
 // Attestation de réception des données
if(packetAvailable){
Serial.println("DONNEES RECUES");
blinker();
// Détacher interruption
detachInterrupt(0);
// Clear the flag
packetAvailable = false;
// Réception des données et décriptage
CCPACKET packet;
// Vérification de la validité des données
if(cc1101.receiveData(&packet) > 0){
if(!packet.crc_ok) {
Serial.println("crc not ok");
}
// Traitement des données reçues
if(packet.length > 0)
{
// Reception Latitude
Serial.print("Latitude: ");
for(int j=0; j<4; j++)
{
Serial.print(packet.data[j]);
if(j==1){Serial.print(",");}
}
Serial.println(" ");
// Reception Longitude
Serial.print("Longitude: ");
for(int k=5; k<8; k++)
{
Serial.print(packet.data[k]);
if(k==5){Serial.print(",");}
}
Serial.println(" ");
// Reception Pression
Serial.print("Pression: ");
for(int l=9; l<12; l++)
{
Serial.print(packet.data[l]);
if(l==10){Serial.print(",");}
}
Serial.println(" HPa.");
// Reception Temperature
Serial.print("Température: ");
for(int m=13; m<16; m++)
{
Serial.print(packet.data[m]);
if(m==14){Serial.print(",");}
}
Serial.println("°C");
// Reception Acc X
Serial.print("Acc X: ");
for(int n=17; n<19; n++)
{
Serial.print(packet.data[n]);
if(n==17){Serial.print(",");}
}
Serial.println(" ");
// Reception Acc Y
Serial.print("Acc y: ");
for(int o=20; o<22; o++)
{
Serial.print(packet.data[o]);
if(o==20){Serial.print(",");}
}
Serial.println(" ");
// Reception Acc Z
Serial.print("Acc Z: ");
for(int p=23; p<25; p++)
{
Serial.print(packet.data[p]);
if(p==23){Serial.print(",");}
}
Serial.println(" ");
// Reception CSA Voltage
Serial.print("ARDUINO-Tension: ");
for(int q=26; q<28; q++)
{
Serial.print(packet.data[q]);
if(q==26){Serial.print(",");}
}
Serial.println(" V");
// Reception CSA Intensité
Serial.print("ARDUINO-Intensite: ");
for(int r=29; r<33; r++)
{
Serial.print(packet.data[r]);
if(r==31){Serial.print(",");}
}
Serial.println(" mA");
// Reception CSE Voltage
Serial.print("EMETTEUR-Tension: ");
for(int s=34; s<36; s++)
{
Serial.print(packet.data[s]);
if(s==34){Serial.print(",");}
}
Serial.println(" V");
// Reception CSE Intensité
Serial.print("EMETTEUR-Intensite: ");
for(int t=37; t<packet.length; t++)
{
Serial.print(packet.data[t]);
if(t==39){Serial.print(",");}
}
Serial.println(" mA");
}
}
}
// Enable wireless reception interrupt
attachInterrupt(0, cc1101signalsInterrupt, FALLING);
}

