
float temps = 180.0;
// Conexion of the 3 LEDS to 3 digital pins
int leda = 11;
int ledb = 12;
int ledc = 13;

void setup() {                
  // initialize the digital pins as an output.
  pinMode(leda, OUTPUT);
pinMode(ledb, OUTPUT);
pinMode(ledc, OUTPUT);
}


void blinkLed(int i, int intensite)//faire clignoter les LEDs
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



// the loop routine runs over and over again forever:
void loop() {
  blinkLed(4,255);
}


