//Libraries
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include "EEPROM.h"
#include "cc1101.h"
CC1101 cc1101;
//Library current sensor
#include <Wire.h>
#include <Adafruit_INA219.h>
// Capteur pression
#include <SPI.h>

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
// This value is often labeled "Barometer" or "Barometric Pressure" or just "Pressure"
#define NWS_BARO 29.92 

// Pin definitions
#define MPL115A1_ENABLE_PIN 9
#define MPL115A1_SELECT_PIN 7

// Masks for MPL115A1 SPI i/o
#define MPL115A1_READ_MASK  0x80
#define MPL115A1_WRITE_MASK 0x7F 

// MPL115A1 register address map
#define PRESH   0x00    // 80
#define PRESL   0x02    // 82
#define TEMPH   0x04    // 84
#define TEMPL   0x06    // 86

#define A0MSB   0x08    // 88
#define A0LSB   0x0A    // 8A
#define B1MSB   0x0C    // 8C
#define B1LSB   0x0E    // 8E
#define B2MSB   0x10    // 90
#define B2LSB   0x12    // 92
#define C12MSB  0x14    // 94
#define C12LSB  0x16    // 96
#define C11MSB  0x18    // 98
#define C11LSB  0x1A    // 9A
#define C22MSB  0x1C    // 9C
#define C22LSB  0x1E    // 9E

// Unit conversion macros
#define FT_TO_M(x) ((long)((x)*(0.3048)))
#define KPA_TO_INHG(x) ((x)*(0.295333727))
#define KPA_TO_MMHG(x) ((x)*(7.50061683))
#define KPA_TO_PSIA(x) ((x)*(0.145037738))
#define KPA_TO_KGCM2(x) ((x)*(0.0102))
#define INHG_TO_PSIA(x) ((x)*(0.49109778))
#define DEGC_TO_DEGF(x) ((x)*(9.0/5.0)+32)



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


float calculateTemperatureC() {
    
    unsigned int uiTadc;
    unsigned char uiTH, uiTL;
    
    unsigned int temperature_counts = 0;
    
    writeRegister(0x22, 0x00);  // Start temperature conversion
    delay(2);                   // Max wait time is 0.7ms, typ 0.6ms
    
    // Read pressure
    uiTH = readRegister(TEMPH);
    uiTL = readRegister(TEMPL);
    
    uiTadc = (unsigned int) uiTH << 8;
    uiTadc += (unsigned int) uiTL & 0x00FF;
    
    // Temperature is a 10bit value
    uiTadc = uiTadc >> 6;
    
    // -5.35 counts per °C, 472 counts is 25C
    return 31.5 + (uiTadc - 472) / -5.35;
}


float calculatePressurekPa() {
    
    // See Freescale document AN3785 for detailed explanation
    // of this implementation.
    
    signed char sia0MSB, sia0LSB;
    signed char sib1MSB, sib1LSB;
    signed char sib2MSB, sib2LSB;
    signed char sic12MSB, sic12LSB;
    signed char sic11MSB, sic11LSB;
    signed char sic22MSB, sic22LSB;
    signed int sia0, sib1, sib2, sic12, sic11, sic22, siPcomp;
    float decPcomp;
    signed long lt1, lt2, lt3, si_c11x1, si_a11, si_c12x2;
    signed long si_a1, si_c22x2, si_a2, si_a1x1, si_y1, si_a2x2;
    unsigned int uiPadc, uiTadc;
    unsigned char uiPH, uiPL, uiTH, uiTL;
    
    writeRegister(0x24, 0x00);      // Start Both Conversions
    //writeRegister(0x20, 0x00);    // Start Pressure Conversion
    //writeRegister(0x22, 0x00);    // Start temperature conversion
    delay(2);                       // Max wait time is 1ms, typ 0.8ms
    
    // Read pressure
    uiPH = readRegister(PRESH);
    uiPL = readRegister(PRESL);
    uiTH = readRegister(TEMPH);
    uiTL = readRegister(TEMPL);
    
    uiPadc = (unsigned int) uiPH << 8;
    uiPadc += (unsigned int) uiPL & 0x00FF;
    uiTadc = (unsigned int) uiTH << 8;
    uiTadc += (unsigned int) uiTL & 0x00FF;
    
    // Placing Coefficients into 16-bit Variables
    // a0
    sia0MSB = readRegister(A0MSB);
    sia0LSB = readRegister(A0LSB);
    sia0 = (signed int) sia0MSB << 8;
    sia0 += (signed int) sia0LSB & 0x00FF;
    
    // b1
    sib1MSB = readRegister(B1MSB);
    sib1LSB = readRegister(B1LSB);
    sib1 = (signed int) sib1MSB << 8;
    sib1 += (signed int) sib1LSB & 0x00FF;
    
    // b2
    sib2MSB = readRegister(B2MSB);
    sib2LSB = readRegister(B2LSB);
    sib2 = (signed int) sib2MSB << 8;
    sib2 += (signed int) sib2LSB & 0x00FF;
    
    // c12
    sic12MSB = readRegister(C12MSB);
    sic12LSB = readRegister(C12LSB);
    sic12 = (signed int) sic12MSB << 8;
    sic12 += (signed int) sic12LSB & 0x00FF;
    
    // c11
    sic11MSB = readRegister(C11MSB);
    sic11LSB = readRegister(C11LSB);
    sic11 = (signed int) sic11MSB << 8;
    sic11 += (signed int) sic11LSB & 0x00FF;
    
    // c22
    sic22MSB = readRegister(C22MSB);
    sic22LSB = readRegister(C22LSB);
    sic22 = (signed int) sic22MSB << 8;
    sic22 += (signed int) sic22LSB & 0x00FF;
    
    // Coefficient 9 equation compensation
    uiPadc = uiPadc >> 6;
    uiTadc = uiTadc >> 6;
    
    // Step 1 c11x1 = c11 * Padc
    lt1 = (signed long) sic11;
    lt2 = (signed long) uiPadc;
    lt3 = lt1*lt2;
    si_c11x1 = (signed long) lt3;
    
    // Step 2 a11 = b1 + c11x1
    lt1 = ((signed long)sib1)<<14;
    lt2 = (signed long) si_c11x1;
    lt3 = lt1 + lt2;
    si_a11 = (signed long)(lt3>>14);
    
    // Step 3 c12x2 = c12 * Tadc
    lt1 = (signed long) sic12;
    lt2 = (signed long) uiTadc;
    lt3 = lt1*lt2;
    si_c12x2 = (signed long)lt3;
    
    // Step 4 a1 = a11 + c12x2
    lt1 = ((signed long)si_a11<<11);
    lt2 = (signed long)si_c12x2;
    lt3 = lt1 + lt2;
    si_a1 = (signed long) lt3>>11;
    
    // Step 5 c22x2 = c22*Tadc
    lt1 = (signed long)sic22;
    lt2 = (signed long)uiTadc;
    lt3 = lt1 * lt2;
    si_c22x2 = (signed long)(lt3);
    
    // Step 6 a2 = b2 + c22x2
    lt1 = ((signed long)sib2<<15);
    lt2 = ((signed long)si_c22x2>1);
    lt3 = lt1+lt2;
    si_a2 = ((signed long)lt3>>16);
    
    // Step 7 a1x1 = a1 * Padc
    lt1 = (signed long)si_a1;
    lt2 = (signed long)uiPadc;
    lt3 = lt1*lt2;
    si_a1x1 = (signed long)(lt3);
    
    // Step 8 y1 = a0 + a1x1
    lt1 = ((signed long)sia0<<10);
    lt2 = (signed long)si_a1x1;
    lt3 = lt1+lt2;
    si_y1 = ((signed long)lt3>>10);
    
    // Step 9 a2x2 = a2 * Tadc
    lt1 = (signed long)si_a2;
    lt2 = (signed long)uiTadc;
    lt3 = lt1*lt2;
    si_a2x2 = (signed long)(lt3);
    
    // Step 10 pComp = y1 + a2x2
    lt1 = ((signed long)si_y1<<10);
    lt2 = (signed long)si_a2x2;
    lt3 = lt1+lt2;
    
    // Fixed point result with rounding
    //siPcomp = ((signed int)lt3>>13);
    siPcomp = lt3/8192;
    
    // decPcomp is defined as a floating point number
    // Conversion to decimal value from 1023 ADC count value
    // ADC counts are 0 to 1023, pressure is 50 to 115kPa respectively
    decPcomp = ((65.0/1023.0)*siPcomp)+50;
    
    return decPcomp;
}


unsigned int readRegister(byte thisRegister) {
    
    byte result = 0;
    
    // select the MPL115A1
    digitalWrite(MPL115A1_SELECT_PIN, LOW);
    
    // send the request
    SPI.transfer(thisRegister | MPL115A1_READ_MASK);
    result = SPI.transfer(0x00);
    
    // deselect the MPL115A1
    digitalWrite(MPL115A1_SELECT_PIN, HIGH);
    
    return result;  
}


void writeRegister(byte thisRegister, byte thisValue) {
    
    // select the MPL115A1
    digitalWrite(MPL115A1_SELECT_PIN, LOW);
    
    // send the request
    SPI.transfer(thisRegister & MPL115A1_WRITE_MASK);
    SPI.transfer(thisValue);
    
    // deselect the MPL115A1
    digitalWrite(MPL115A1_SELECT_PIN, HIGH);
}

//Set up
void setup()
{
// Initialisation
Serial.begin(9600);
ss.begin(GPSBaud);
SPI.begin();

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
// Initialisation capteur pression
// initialize the chip select and enable pins
    pinMode(MPL115A1_SELECT_PIN, OUTPUT);
    pinMode(MPL115A1_ENABLE_PIN, OUTPUT);
    
    // sleep the MPL115A1
    digitalWrite(MPL115A1_ENABLE_PIN, LOW);
    
    // set the chip select inactive, select signal is CS LOW
    digitalWrite(MPL115A1_SELECT_PIN, HIGH);

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
// Préparation à l'envoi
for (int j=3; j>0; j--)
{unit=(int)latitude%100;
data.data[j]=unit;
latitude=latitude/100;
if(latitude<100){data.data[j-1]=latitude;}
}
//Traitement longitude
float longitude=longi;
while(longitude>100)
{unite=(int)longitude%100;
data.data[h]=unite;
longitude=longitude/100;
h--;
if(longitude<100){data.data[h]=longitude;}
}
//Traitement Pression
float pressure_pKa = 0;
// wake the MPL115A1
digitalWrite(MPL115A1_ENABLE_PIN, HIGH);
delay(20);  // give the chip a few ms to wake up
pressure_pKa = calculatePressurekPa();
Serial.print("Pression: ");
Serial.println(pressure_pKa);
   
//Traitement Temperature
float temperature_c= 0;
temperature_c = calculateTemperatureC();
Serial.print("Temperature: ");
Serial.println(temperature_c);

// put the MPL115A1 to sleep, it has this feature why not use it
// while in shutdown the part draws ~1uA
digitalWrite(MPL115A1_ENABLE_PIN, LOW);

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
send_data();
delay(2000);

}
}

