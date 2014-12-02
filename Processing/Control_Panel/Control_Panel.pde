
// ------ Faire le lien entre processing et arduino-------//

import processing.serial.*;
Serial myPort;  // Create object from Serial class
String val;     // Data received from the serial port

//Traitements d'initialisation
void setup() 

{
  //Screen Size
  size(2000, 1200);
  textSize(30);

  println(Serial.list());    // Affichage dans la console de la liste des ports serie du PC
  myPort = new Serial(this, "/dev/tty.usbmodem1411", 9600);   //Initialisation de la communicaiton port serie
  myPort.bufferUntil('\n'); //Attend la réception d'un fin de ligne pour généer un serialEvent()
}

 void draw()
{
  //importation de la police helvetica neue
PFont police;
police = loadFont("HelveticaNeue.vlw");

//police du titre
textFont (police, 48); 
text("SEARCH & RESCUE CONTROL PANEL", 350, 75);
//police du sous titre
textFont (police, 35); 
text("• Location :", 20, 250);

//police du sous-sous titre
textFont (police, 25); 
text("Latitude :", 60, 325);
text("Longitude :", 60, 400);

//police du sous titre
textFont (police, 35); 
text("• Weather Conditions:", 20, 475);

//police du sous-sous titre
textFont (police, 25); 
text("Pressure :", 60, 550);
text("Temperature :", 60, 625);
text("Sea State :", 60, 700);

textFont (police, 35); 
text("• Hour : 18:35:23", 800, 250);

//importation des images
PImage solar;
solar = loadImage("solar.png");
image(solar,550,400);

PImage navsafe;
navsafe = loadImage("navsafe.png");
image(navsafe,1133,750);

PImage storm;
storm = loadImage("storm.png");
image(storm,550,600);


PImage map;
map = loadImage("map.png");
image(map,800,280);

 // affichage sur le control panel les data du gps 
 
if ( myPort.available() > 0) 
  {  // If data is available,
  val = myPort.readStringUntil('\n');         // read it and store it in val
  }   
println(val); //print it out in the console
text("GPS Data :"+val, 30, 290);
rect(30, 290, 400, 60, 20);

}








