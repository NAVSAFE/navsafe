//Library accelerometre
#include <Wire.h>
//Library GPS
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
//Library current sensor
#include <Wire.h>
#include <Adafruit_INA219.h>
//Library Sleep Mode
#include <avr/sleep.h>
#include <avr/power.h>
//Library emetteur
#include "EEPROM.h"
#include "cc1101.h"
//Library pression
#include <SPI.h>


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//___________________________________________________________DECLARATION_______________________________________________________________
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

float ratio = 0.0;
float temps = 180.0;
int niveauAlerte = 0;
int batterie = -1;
int estimationSurvie = -1;
const int r1=3000;
const int r2=1000;
const int refVolt =5;
float referenceVoltBatterie = 1023.0;
const int vSolaire_pin=0;
float referenceTensionSolaire = 4.0;

//_______________________________________________________________________Declaration Induction____
int inductionState = 9;

//_______________________________________________________________________Declaration Bouton Navsafe____
int bouton = 2;

//_______________________________________________________________________Declaration Sleep Mode____
  int sleepmode=0;
  int pulse = 3;


//_____________________________________________________________________Declaration capteur Accelerometre_____
#define MMA8452_ADDRESS 0x1D // 0x1D if SA0 is high, 0x1C if low

//Define a few of the registers that we will be accessing on the MMA8452
#define OUT_X_MSB 0x01
#define XYZ_DATA_CFG 0x0E
#define WHO_AM_I 0x0D
#define CTRL_REG1 0x2A
#define GSCALE 2 // Sets full-scale range to +/-2, 4, or 8g. Used to calc real g values.
float compteurVague=0.0;
float compteurTour=0.0;
int accelCount[3]; // Stores the 12-bit signed value
float accelG[3]; // Stores the real accel value in g's
float seuilVague=0.6;

//_________________________________________________________________________Declaration LEDs_____

int leda = 5;
int ledb = 6;
int ledc = 7;
int ledBouton = 8;

//______________________________________________________________________Declaration capteur Pression_____
#define NWS_BARO 29.92 

// Pin definitions
#define MPL115A1_ENABLE_PIN 9
#define MPL115A1_SELECT_PIN 10

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

float pressure_pKa = 0;
float temperature_c= 0;
long altitude_ft = 0;
    

//_______________________________________________________________________Declaration current sensor + batteries__
Adafruit_INA219 ina219_A; // Current sensor de la batterie Arduino
Adafruit_INA219 ina219_E(0x41); // Current sensor de la batterie Emission
int batterieArduino = 0;
int batterieEmission = 0;

//______________________________________________________________________Declaration GPS__
static const int RXPin = 4, TXPin = 3;
static const uint32_t GPSBaud = 9600;
// The TinyGPS++ object
TinyGPSPlus gps;
// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);

//______________________________________________________________________Declaration Emetteur____
CC1101 cc1101;
int counter;
byte syncWord = 199;




///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//___________________________________________________________FONCTIONS_______________________________________________________________
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//____________________________________________________________________Fonction pour le Sleep Mode____
void Light(void)
{
  //Allumer la LED du bouton
  digitalWrite(ledBouton, HIGH);
  
}


void enterSleep(void)
{
  /* Setup pin2 as an interrupt and attach handler. */
  attachInterrupt(0, Light, FALLING);
  attachInterrupt(1, Light, RISING);
  delay(100);
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  sleep_mode();
  /* The program will continue from here.
  First thing to do is disable sleep. */
  detachInterrupt(0);
  detachInterrupt(1);
  sleep_disable();}


//___________________________________________________________________Fonction LEDs____
void blinkLed(int i, int intensite)
{
  int j=0;
  while(j<i)
  {
    analogWrite(leda, intensite);
    analogWrite(ledb, 0);
    analogWrite(ledc, 0);
    delay(temps/i);
    analogWrite(leda, 0);
    analogWrite(ledb, intensite);
    analogWrite(ledc, 0);
    delay(temps/i);
    analogWrite(leda, 0);
    analogWrite(ledb, 0);
    analogWrite(ledc, intensite);
    delay(temps/i);
    j++;
  }
    digitalWrite(leda, 0);
    digitalWrite(ledb, 0);
    digitalWrite(ledc, 0);
    
}

void Led ()
{
 estimationVie(temperature_c, pressure_pKa, 1.0, niveauAlerte);
  Serial.print("Estimation du temps de survie: ");
  Serial.print(estimationVie(temperature_c, pressure_pKa, 1.0, niveauAlerte));
  Serial.println(" H");
  
  // Si batterie Arduino non en état critique
  if (batterieArduino == 0)
  {
        if(estimationSurvie<=1)
      {
        Serial.println("Mode performant");
        blinkLed(4, 255);
        
      }
      else if(estimationSurvie>=4)
      {
        Serial.println("Mode economie energie");
        blinkLed(1, 127);
      }
      else
      {
        Serial.println("Mode modere");
        blinkLed(2, 85);
      }
  }   
  // Si batterie Arduino en état critique
  if (batterieArduino == 1)
    {
        Serial.println("Batterie Arduino critique : Mode economie energie");
        blinkLed(1, 127);
      } 
  
}

//_________________________________________________________________________Fonction Etat de la batterie Arduino____
void StatebatterieArduino ()
{
  float busvoltageA = 0;
  busvoltageA = ina219_A.getBusVoltage_V();
  // Détermination du niveau de charge de la batterie Arduino
  if (busvoltageA<=3,30) {Serial.print("Batterie Arduino restante: <5%");}
  if (3.30<busvoltageA<=3,60) {Serial.print("Batterie Arduino restante: 10%");}
  if (3.60<busvoltageA<=3,70) {Serial.print("Batterie Arduino restante: 20%");}
  if (3.70<busvoltageA<=3,75) {Serial.print("Batterie Arduino restante: 30%");}
  if (3.75<busvoltageA<=3,79) {Serial.print("Batterie Arduino restante: 40%");}
  if (3.79<busvoltageA<=3,83) {Serial.print("Batterie Arduino restante: 50%");}
  if (3.83<busvoltageA<=3,87) {Serial.print("Batterie Arduino restante: 60%");}
  if (3.87<busvoltageA<=3,92) {Serial.print("Batterie Arduino restante: 70%");}
  if (3.92<busvoltageA<=3,97) {Serial.print("Batterie Arduino restante: 80%");}
  if (3.97<busvoltageA<=4,10) {Serial.print("Batterie Arduino restante: 90%");}
  if (busvoltageA>4.10) {Serial.print("Batterie Arduino restante: 100%");}
  // Détermination du niveau critique de la batterie (<20%)
  if (busvoltageA <= 3.70) {batterieArduino == 1;}
  if (busvoltageA > 3.70) {batterieArduino == 0;}
}

//___________________________________________________________________Fonction Etat de la batterie Emission____
void StatebatterieEmission ()
{
  float busvoltageE = 0;
  busvoltageE = ina219_E.getBusVoltage_V();
  // Détermination du niveau de charge de la batterie Arduino
  if (busvoltageE<=3,30) {Serial.print("Batterie Emission restante: <5%");}
  if (3.30<busvoltageE<=3,60) {Serial.print("Batterie Emission restante: 10%");}
  if (3.60<busvoltageE<=3,70) {Serial.print("Batterie Emission restante: 20%");}
  if (3.70<busvoltageE<=3,75) {Serial.print("Batterie Emission restante: 30%");}
  if (3.75<busvoltageE<=3,79) {Serial.print("Batterie Emission restante: 40%");}
  if (3.79<busvoltageE<=3,83) {Serial.print("Batterie Emission restante: 50%");}
  if (3.83<busvoltageE<=3,87) {Serial.print("Batterie Emission restante: 60%");}
  if (3.87<busvoltageE<=3,92) {Serial.print("Batterie Emission restante: 70%");}
  if (3.92<busvoltageE<=3,97) {Serial.print("Batterie Emission restante: 80%");}
  if (3.97<busvoltageE<=4,10) {Serial.print("Batterie Emission restante: 90%");}
  if (busvoltageE>4.10) {Serial.print("Batterie Emission restante: 100%");}
  // Détermination du niveau critique de la batterie (<20%)
  if (busvoltageE <= 3.70) {batterieEmission == 1;}
  if (busvoltageE > 3.70) {batterieEmission == 0;}
}


//_____________________________________________________________________Fonction estimation de survie____
int estimationVie(float temperature, float pression, float soleil, int alerteVague) //estimer le temps de survie de l utilisateur
{
  int estimation=0;
  int estimationPression=0;
  int estimationTemperature=0;
  float estimationSoleil=0;
  int tempsSurvie=0;
   
  estimationSoleil = voltageSolaire()/referenceTensionSolaire;
  
  if(temperature<20)
  {
    estimationTemperature=1;
    if(temperature<15)
    {
      estimationTemperature=2;
      if(temperature<10)
      {
        estimationTemperature=3;
        if(temperature<5)
        {
           estimationTemperature=4; 
        }
      } 
    }
  }
  
  if(pression<1013)
  {
    estimationPression=1;
    if(pression<1001)
    {
      estimationPression=2;
      if(pression<989)
      {
        estimationPression=3;
        if(pression<965)
        {
           estimationPression=4; 
        }
      } 
    }
  }
  
  
  estimationSurvie = max(max(estimationPression, estimationTemperature), alerteVague);
  estimationSurvie = estimationSurvie*estimationSoleil;
  
  if(estimationSurvie==0){tempsSurvie=72;}
  else if(estimationSurvie==1){tempsSurvie=48;}
  else if(estimationSurvie==2){tempsSurvie=24;}
  else if(estimationSurvie==3){tempsSurvie=8;}
  else if(estimationSurvie==4){tempsSurvie=3;}
  else if(estimationSurvie==5){tempsSurvie=1;}
  
  return tempsSurvie;
}



//_____________________________________________________________________Fonction panneau solaire____
float voltageSolaire()
{
  
  float voltage = analogRead(vSolaire_pin);
  Serial.print("Tension panneau solaire: ");
  Serial.print(voltage);
  Serial.print(" V");
  return voltage;
}



 //________________________________________________________________Fonctions capteur Pression_____
 long calculateAltitudeFt(float pressure_kPa)
{    
    float delta;
    long altitude_ft;
    
    delta = KPA_TO_PSIA(pressure_kPa) / INHG_TO_PSIA( NWS_BARO );
    altitude_ft = (1 - pow(delta, (1 / 5.25587611))) / 0.0000068756;
    
    return altitude_ft;
}


float calculateTemperatureC()
{    
    unsigned int uiTadc;
    unsigned char uiTH, uiTL;
    
    unsigned int temperature_counts = 0;
    
    writeRegisterPression(0x22, 0x00);  // Start temperature conversion
    delay(2);                   // Max wait time is 0.7ms, typ 0.6ms
    
    // Read pressure
    uiTH = readRegisterPression(TEMPH);
    uiTL = readRegisterPression(TEMPL);
    
    uiTadc = (unsigned int) uiTH << 8;
    uiTadc += (unsigned int) uiTL & 0x00FF;
    
    // Temperature is a 10bit value
    uiTadc = uiTadc >> 6;
    
    // -5.35 counts per Â°C, 472 counts is 25Â°C
    return 25 + (uiTadc - 472) / -5.35;
}


float calculatePressurekPa()
{    
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
    
    writeRegisterPression(0x24, 0x00);      // Start Both Conversions
    //writeRegisterPression(0x20, 0x00);    // Start Pressure Conversion
    //writeRegisterPression(0x22, 0x00);    // Start temperature conversion
    delay(2);                       // Max wait time is 1ms, typ 0.8ms
    
    // Read pressure
    uiPH = readRegisterPression(PRESH);
    uiPL = readRegisterPression(PRESL);
    uiTH = readRegisterPression(TEMPH);
    uiTL = readRegisterPression(TEMPL);
    
    uiPadc = (unsigned int) uiPH << 8;
    uiPadc += (unsigned int) uiPL & 0x00FF;
    uiTadc = (unsigned int) uiTH << 8;
    uiTadc += (unsigned int) uiTL & 0x00FF;
    
    // Placing Coefficients into 16-bit Variables
    // a0
    sia0MSB = readRegisterPression(A0MSB);
    sia0LSB = readRegisterPression(A0LSB);
    sia0 = (signed int) sia0MSB << 8;
    sia0 += (signed int) sia0LSB & 0x00FF;
    
    // b1
    sib1MSB = readRegisterPression(B1MSB);
    sib1LSB = readRegisterPression(B1LSB);
    sib1 = (signed int) sib1MSB << 8;
    sib1 += (signed int) sib1LSB & 0x00FF;
    
    // b2
    sib2MSB = readRegisterPression(B2MSB);
    sib2LSB = readRegisterPression(B2LSB);
    sib2 = (signed int) sib2MSB << 8;
    sib2 += (signed int) sib2LSB & 0x00FF;
    
    // c12
    sic12MSB = readRegisterPression(C12MSB);
    sic12LSB = readRegisterPression(C12LSB);
    sic12 = (signed int) sic12MSB << 8;
    sic12 += (signed int) sic12LSB & 0x00FF;
    
    // c11
    sic11MSB = readRegisterPression(C11MSB);
    sic11LSB = readRegisterPression(C11LSB);
    sic11 = (signed int) sic11MSB << 8;
    sic11 += (signed int) sic11LSB & 0x00FF;
    
    // c22
    sic22MSB = readRegisterPression(C22MSB);
    sic22LSB = readRegisterPression(C22LSB);
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

    decPcomp = ((65.0/1023.0)*siPcomp)+50;
    
    return decPcomp;
}


unsigned int readRegisterPression(byte thisRegister)
{    
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


void writeRegisterPression(byte thisRegister, byte thisValue)
{    
    // select the MPL115A1
    digitalWrite(MPL115A1_SELECT_PIN, LOW);
    
    // send the request
    SPI.transfer(thisRegister & MPL115A1_WRITE_MASK);
    SPI.transfer(thisValue);
    
    // deselect the MPL115A1
    digitalWrite(MPL115A1_SELECT_PIN, HIGH);
}

void Pression ()
{

    // wake the MPL115A1
    digitalWrite(MPL115A1_ENABLE_PIN, HIGH);
    delay(20);  // give the chip a few ms to wake up
    
    pressure_pKa = calculatePressurekPa();
    pressure_pKa = pressure_pKa*10;//convertir en hPa
    temperature_c = calculateTemperatureC();
    altitude_ft = calculateAltitudeFt(pressure_pKa);
    
    // put the MPL115A1 to sleep, it has this feature why not use it
    // while in shutdown the part draws ~1uA
    digitalWrite(MPL115A1_ENABLE_PIN, LOW);
  
    // print table of altitude, pressures, and temperatures to console
/*  Serial.print("Pression: ");
    Serial.print(altitude_ft);
    Serial.print(" ft | ");
    Serial.print(FT_TO_M(altitude_ft));
    Serial.print(" m | ");
    Serial.print(KPA_TO_INHG(pressure_pKa), 2);
    Serial.print(" in Hg | ");
    Serial.print(KPA_TO_MMHG(pressure_pKa), 0);
    Serial.print(" mm Hg | ");
    Serial.print(KPA_TO_PSIA(pressure_pKa), 2);
    Serial.print(" psia | ");
    Serial.print(KPA_TO_KGCM2(pressure_pKa), 3);
    Serial.print(" kg/cm2 | ");*/
    Serial.print("Pression: ");
    Serial.print(pressure_pKa, 1);
    Serial.println("   hPa");
    
    // At a res of -5.35 counts/Â°C, digits lower than 0.1Â°C are not significant
    Serial.print("Temperature: ");
    Serial.print(temperature_c, 1);
    Serial.println("   Celcius");
//  Serial.print(DEGC_TO_DEGF(temperature_c), 1);
//  Serial.print(" F\n");    
    Serial.println("");
}


//________________________________________________________________________Fonctions capteur Accelerometre___
void readAccelData(int *destination)
{
  byte rawData[6]; // x/y/z accel register data stored here

  readRegisters(OUT_X_MSB, 6, rawData); // Read the six raw data registers into data array

  // Loop to calculate 12-bit ADC and g value for each axis
  for(int i = 0; i < 3 ; i++)
  {
    int gCount = (rawData[i*2] << 8) | rawData[(i*2)+1]; //Combine the two 8 bit registers into one 12-bit number
    gCount >>= 4; //The registers are left align, here we right align the 12-bit integer

    // If the number is negative, we have to make it so manually (no 12-bit data type)
    if (rawData[i*2] > 0x7F)
    {
      gCount = ~gCount + 1;
      gCount *= -1; // Transform into negative 2's complement #
    }

    destination[i] = gCount; //Record this gCount into the 3 int array
  }
}

// Initialize the MMA8452 registers
// See the many application notes for more info on setting all of these registers:
// http://www.freescale.com/webapp/sps/site/prod_summary.jsp?code=MMA8452Q
void initMMA8452()
{
  byte c = readRegister(WHO_AM_I); // Read WHO_AM_I 
  //byte c = 0x2A;
  if (c == 0x2A) // WHO_AM_I should always be 0x2A
  {
    Serial.println("MMA8452Q is online...");
  }
  else
  {
    Serial.print("Could not connect to MMA8452Q: 0x");
    Serial.println(c, HEX);
    while(1) ; // Loop forever if communication doesn't happen
  }

  MMA8452Standby(); // Must be in standby to change registers

  // Set up the full scale range to 2, 4, or 8g.
  byte fsr = GSCALE;
  if(fsr > 8) fsr = 8; //Easy error check
  fsr >>= 2; // Neat trick, see page 22. 00 = 2G, 01 = 4A, 10 = 8G
  writeRegister(XYZ_DATA_CFG, fsr);

  //The default data rate is 800Hz and we don't modify it in this example code

  MMA8452Active(); // Set to active to start reading
}

// Sets the MMA8452 to standby mode. It must be in standby to change most register settings
void MMA8452Standby()
{
  byte c = readRegister(CTRL_REG1);
  writeRegister(CTRL_REG1, c & ~(0x01)); //Clear the active bit to go into standby
}

// Sets the MMA8452 to active mode. Needs to be in this mode to output data
void MMA8452Active()
{
  byte c = readRegister(CTRL_REG1);
  writeRegister(CTRL_REG1, c | 0x01); //Set the active bit to begin detection
}

// Read bytesToRead sequentially, starting at addressToRead into the dest byte array
void readRegisters(byte addressToRead, int bytesToRead, byte * dest)
{
  Wire.beginTransmission(MMA8452_ADDRESS);
  Wire.write(addressToRead);
  Wire.endTransmission(false); //endTransmission but keep the connection active

  Wire.requestFrom(MMA8452_ADDRESS, bytesToRead); //Ask for bytes, once done, bus is released by default

  while(Wire.available() < bytesToRead); //Hang out until we get the # of bytes we expect

  for(int x = 0 ; x < bytesToRead ; x++)
    dest[x] = Wire.read();
}

// Read a single byte from addressToRead and return it as a byte
byte readRegister(byte addressToRead)
{
  Wire.beginTransmission(MMA8452_ADDRESS);
  Wire.write(addressToRead);
  Wire.endTransmission(false); //endTransmission but keep the connection active

  Wire.requestFrom(MMA8452_ADDRESS, 1); //Ask for 1 byte, once done, bus is released by default

  while(!Wire.available()) ; //Wait for the data to come back
  return Wire.read(); //Return this one byte
}

// Writes a single byte (dataToWrite) into addressToWrite
void writeRegister(byte addressToWrite, byte dataToWrite)
{
  Wire.beginTransmission(MMA8452_ADDRESS);
  Wire.write(addressToWrite);
  Wire.write(dataToWrite);
  Wire.endTransmission(); //Stop transmitting
}
void Accelerometre ()
{

  readAccelData(accelCount); // Read the x/y/z adc values

  // Now we'll calculate the accleration value into actual g's
  for (int i = 0 ; i < 3 ; i++)
  {
    accelG[i] = (float) accelCount[i] / ((1<<12)/(2*GSCALE)); // get actual g value, this depends on scale being set
  }

  // Print out values
  Serial.println("Accelerometre: ");
  for (int i = 0 ; i < 3 ; i++)
  {
    Serial.print(accelG[i], 4); // Print g values
    Serial.print("\t"); // tabs in between axes
  }
  
  if(abs(accelG[0])>seuilVague||abs(accelG[2])>seuilVague)
  {
    Serial.println(" ");
    Serial.println(" ");
    Serial.println("Vagues!");
    compteurVague = compteurVague+1;
  }
  
  compteurTour = compteurTour+1;
  ratio = compteurVague/compteurTour;
  
  Serial.print("Nombre de tours: ");
  Serial.println(compteurTour);
  Serial.print("Nombre de vagues: ");
  Serial.println(compteurVague);
  Serial.print("Ratio: ");
  Serial.println(ratio);
  
  niveauAlerte=0;
  if(compteurTour>10)//si plus de 10 tours
  {
    if(ratio>=0.5)//si rapport vague/tour superieur a  0.8
      {
        Serial.println("/////////////////////////////");
        Serial.println("/// Alerte Vagues force 3 ///");
        Serial.println("/////////////////////////////");
blinkLed(4,255);
        niveauAlerte=3;
      }
      
      else if((ratio>=0.3)&&(ratio<0.5))//si rapport vague/tour superieur a  0.6
      {
        Serial.println("/////////////////////////////");
        Serial.println("/// Alerte Vagues force 2 ///");
        Serial.println("/////////////////////////////");
  blinkLed(2,150);
        niveauAlerte=2;
      }
      
      else if((ratio>=0.1)&&(ratio<0.3))//si rapport vague/tour superieur a  0.4
      {
        Serial.println("/////////////////////////////");
        Serial.println("/// Alerte Vagues force 1 ///");
        Serial.println("//////////////////////////////");
blinkLed(1,50);
        niveauAlerte=1;
      }
    
      else if((compteurVague/compteurTour)<0.1)//si rapport vague/tour inferieur a  0.2
      {
        compteurVague=0;//reinitialiser compteurs
        compteurTour=0;
        niveauAlerte=0;
      }
  }
  Serial.println();
}



//________________________________________________________Fonctions GPS___ 
 void displayInfo()
{
  if (gps.location.isValid())
  {
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
  }
  else
  {
    Serial.print(F("******, ******"));
  }
Serial.print(F(", "));
  if (gps.date.isValid())
  {
    Serial.print(gps.date.month());
    Serial.print(F("/"));
    Serial.print(gps.date.day());
    Serial.print(F("/"));
    Serial.print(gps.date.year());
  }
  else
  {
    Serial.print(F("**/**/****"));
  }
Serial.print(F(", "));
  
  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10) Serial.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(F(":"));
    if (gps.time.minute() < 10) Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(F(":"));
    if (gps.time.second() < 10) Serial.print(F("0"));
    Serial.print(gps.time.second());
 
  }
  else
  {
    Serial.print(F("**:**:**"));
  }

  Serial.println();
}
 
//___________________________________________________________________Fonction Induction___
void CheckInduction ()
{
    if (inductionState == HIGH)
  {
    Serial.println("Recharge par induction détectée.");
    delay(1000);
    Serial.println("La balise est sur son socle, préparation du Sleep Mode...");
    delay(1000);
    Serial.println("Activation du Sleep Mode, au revoir.");
    delay(1000);
    enterSleep();
  }
}

//_________________________________________________________________Fonction Emetteur___
 void emetteur() {
CCPACKET data;
data.length=10;
int blinkCount=counter++;
data.data[0]=5;
data.data[1]=blinkCount;data.data[2]=5;
data.data[3]=5;data.data[4]=5;
data.data[5]=5;data.data[6]=5;
data.data[7]=5;data.data[8]=5;
data.data[9]=5;
//cc1101.flushTxFifo ();
if(cc1101.sendData(data)){
Serial.print(blinkCount,DEC);
Serial.println(" sent ok !!");
blinkLed(4, 255);
}else{
Serial.println("sent failed !");
}
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//___________________________________________________________SETUP_______________________________________________________________
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void setup()
{
  Serial.begin(38400);
  
//_______________________________________________Setup Induction____
pinMode(inductionState, INPUT);

//_______________________________________________Setup Sleep Mode____  
  pinMode(ledBouton, OUTPUT);
  digitalWrite (ledBouton, HIGH);
//______________________________________________Setup Bouton____  
    pinMode(pulse, INPUT);
    pinMode(bouton, INPUT);
    pinMode(bouton, HIGH);
//________________ _____________________________Setup LEDs____

    pinMode(leda, OUTPUT);  
    pinMode(ledb, OUTPUT);
    pinMode(ledc, OUTPUT);
    pinMode(ledBouton, OUTPUT);

//_________________________________________Setup capteur Pression____
   // initialize SPI interface
    SPI.begin();
    
    // initialize the chip select and enable pins
    pinMode(MPL115A1_SELECT_PIN, OUTPUT);
    pinMode(MPL115A1_ENABLE_PIN, OUTPUT);
    
    
    
    // sleep the MPL115A1
    digitalWrite(MPL115A1_ENABLE_PIN, LOW);
    
    // set the chip select inactive, select signal is CS LOW
    digitalWrite(MPL115A1_SELECT_PIN, HIGH);

//______________________    _________________________Setup capteur Accelerometre____
    Wire.begin(); //Join the bus as a master

  initMMA8452(); //Test and intialize the MMA8452

//___________________________________________________Setup current sensor____
  uint32_t currentFrequency;
  ina219_A.begin(); // Current sensor de la batterie Arduino
  ina219_E.begin(); // Current sensor de la batterie Emission
  

//_____________________________________________________Setup GPS____
  ss.begin(GPSBaud);
//_____________________________________________________Setup Emetteur__
Serial.println("Test emetteur");
// blink once to signal the setup
blinkLed(4, 255);

// reset the counter
counter=0;

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



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//___________________________________________________________LOOP____________________________________________________________________
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void loop()
{ 
  // Sleep Mode 
  if(sleepmode==0)
  {
    delay(1000);
    Serial.print("Entering Sleep mode in: 3 seconds");
    delay(1000);
    Serial.print("Entering Sleep mode in: 2 seconds");
    delay(1000);
    Serial.print("Entering Sleep mode in: 1 second");
    delay(1000);
    Serial.print("Entering Sleep mode...");
    delay(200);
    sleepmode++;
    digitalWrite (ledBouton, LOW);
    enterSleep();
  }  
// Etat de la batterie Arduino 
  StatebatterieArduino();
// Etat de la batterie Emission 
  StatebatterieEmission();
// Etat panneau solaire
voltageSolaire();
// Etat de la pression
  Pression ();
// Etat Accelerometre
  Accelerometre ();
// Localisation GPS
// This sketch displays information every time a new sentence is correctly encoded.
  while (ss.available() > 0)
    if (gps.encode(ss.read()))
      displayInfo();

  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    while(true);
  }
// Gérer l'état des LEDs
  Led();
// Emettre le signal
emetteur ();
// Vérifier si Induction active
CheckInduction ();
// Attendre avant nouvel affichage
delay (3000);

  
}


