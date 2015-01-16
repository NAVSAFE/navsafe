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
  //Serial.println("TEST RECEPTEUR");
  // Setup LED
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
  // blink once to signal the setup
  blinker();
  // Initialize the RF Chip
  //Serial.println("INITIALISATION...");
  cc1101.init();
  cc1101.setSyncWord(&syncWord, false);
  cc1101.setCarrierFreq(CFREQ_433);
  cc1101.disableAddressCheck(); //if not specified, will only display "packet received"
  //cc1101.setTxPowerAmp(PA_LowPower);
  attachInterrupt(0, cc1101signalsInterrupt, FALLING);
  delay(1000);
  //Serial.println("RECEPTEUR INITIALISE");
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//___________________________________________________________LOOP____________________________________________________________________//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void loop()
  {
   // Attestation de réception des données
  if(packetAvailable){
    //Serial.println("DATA RECEIVED");
    blinker();
    // Détacher interruption
    detachInterrupt(0);
    // Clear the flag
    packetAvailable = false;
    // Réception des données et décriptage
    CCPACKET packet;
    // Vérification de la validité des données
    if(cc1101.receiveData(&packet) > 0){
//      if(!packet.crc_ok) {
//        Serial.println("crc not ok");
//      }
      // Traitement des données reçues
      if(packet.length > 0)
      {
        
        /*
        // Reception de toute la chaîne et conversion en chaine de caracteres
        String trame=(String)packet.data[0];
        for (int a=1;a<packet.length;a++)
        {
          trame=trame+(String)packet.data[a];
        }
        Serial.println(trame);
        */
         String latitude;
         String longitude;
         String pression;
         String temperature;
         String accx;
         String accy;
         String accz;
         String csav;
         String csai;
         String csev;
         String csei;
         
        // Reception Latitude
         //Serial.print("Latitude: ");
        for(int j=0; j<4; j++)
        {
           //Serial.print(packet.data[j]);
          latitude = latitude+packet.data[j];
          if(j==1){
             //Serial.print(",");
            latitude = latitude+".";
            }
        }
        
        // Reception Longitude
         //Serial.print("Longitude: ");
        for(int k=5; k<8; k++)
        {
           //Serial.print(packet.data[k]);
          longitude = longitude+packet.data[k];
          if(k==5){
             //Serial.print(",");
            longitude=longitude+".";
          }
        }
        
        // Reception Pression
         //Serial.print("Pressure: ");
         //Serial.print(packet.data[9]+800);
        pression = pression+(packet.data[9]+800);
        
        // Reception Temperature
         //Serial.print("Temperature: ");
        for(int m=11; m<14; m++)
        {
           //Serial.print(packet.data[m]);
          temperature = temperature+(String)packet.data[m];
          if(m==12){
             //Serial.print(",");
            temperature=temperature+".";
          }
        }
        
        // Reception Acc X
         //Serial.print("Acc X: ");
         //Serial.print((char)packet.data[15]);
         accx = (String)packet.data[15];
        for(int n=16; n<19; n++)
        {
           //Serial.print(packet.data[n]);
          accx=accx+(String)packet.data[n];
          if(n==16){
             //Serial.print(",");
            accx=accx+".";
          }
        }
        
        // Reception Acc Y
        //Serial.print("Acc y: ");
        //Serial.print((char)packet.data[20]);
        accy = (String)packet.data[20];
        for(int o=21; o<24; o++)
        {
         // Serial.print(packet.data[o]);
          accy=accy+(String)packet.data[o];
          if(o==21){
            //Serial.print(",");
            accy=accy+".";
          }
        }
        
        // Reception Acc Z
       // Serial.print("Acc Z: ");
        //Serial.print((char)packet.data[25]);
        accz = (String)packet.data[25];
        for(int p=26; p<29; p++)
        {
          //Serial.print(packet.data[p]);
          accz = (String)packet.data[p];
          if(p==26){
            //Serial.print(",");
            accz=accz+".";
          }
        }
        
        // Reception CSA Voltage
        //Serial.print("ARDUINO-Voltage: ");
        for(int q=30; q<32; q++){
          //Serial.print(packet.data[q]);
          csav = csav+(String)packet.data[q];
          if(q==30){
            //Serial.print(",");
            csav=csav+".";
          }
        }
        
        // Reception CSA Intensité
        //Serial.print("ARDUINO-Current: ");
        for(int r=33; r<37; r++)
        {
          //Serial.print(packet.data[r]);
           csai = csai+packet.data[r];
          if(r==35){
            //Serial.print(",");
            csai=csai+",";
          }
        }
        
        // Reception CSE Voltage
        //Serial.print("Transmitter-Voltage: ");
        for(int s=38; s<40; s++)
        {
          //Serial.print(packet.data[s]);
           csev = csev+(String)packet.data[s];
          if(s==38){
             //Serial.print(",");
            csev=csev+".";
          }
        }
        
        // Reception CSE Intensité
         //Serial.print("TRANMITTER-Current: ");
        for(int t=41; t<packet.length; t++)
        {
          //Serial.print(packet.data[t]);
          csei = csei+(String)packet.data[t];
          if(t==43){
            //Serial.print(",");
            csei=csei+".";
          }
        }
        
        //cocaténation de la trame à envoyer :
        String trame = latitude+";"+longitude+";"+pression+";"+temperature+";"+accx+";"+accy+";"+accz+";"+csav+";"+csai+";"+csev+";"+csei;
        Serial.println(trame);
      }
    }
    // Enable wireless reception interrupt
    attachInterrupt(0, cc1101signalsInterrupt, FALLING);
  }
}

