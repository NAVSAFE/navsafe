#include <avr/sleep.h>
#include <avr/power.h>


int trigger = 2;
int ledBouton=8;
int seconds=0;

void Light(void)
{
  //Allumer la LED du bouton
  digitalWrite(ledBouton, HIGH);
  
}


void enterSleep(void)
{
  /* Setup pin2 as an interrupt and attach handler. */
  attachInterrupt(0, Light, LOW);
  delay(100);
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  sleep_mode();
  /* The program will continue from here.
  First thing to do is disable sleep. */
  detachInterrupt(0);
  sleep_disable(); 
}

void setup()
{
  Serial.begin(9600);
  /* Setup the pin direction. */
  pinMode(trigger, INPUT);
  pinMode(ledBouton, OUTPUT);
}

void loop()
{
  delay(1000);
  seconds++;
  
  Serial.print("Awake for ");
  Serial.print(seconds, DEC);
  Serial.println(" seconds");
  
  if(seconds == 3)
  {
    Serial.println("Entering sleep");
    delay(200);
    seconds = 0;
    digitalWrite(ledBouton,LOW);
    enterSleep();
  }
  
}
