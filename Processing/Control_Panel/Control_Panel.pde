/*
// ------ Faire le lien entre processing et arduino-------//
import processing.serial.*;
Serial myPort;  // Create object from Serial class
String val;     // Data received from the serial port
*/
//Traitements d'initialisation
void setup() 

{
  //Screen Size
  size(1150, 600);
  textSize(30);
/*
  println(Serial.list());    // Affichage dans la console de la liste des ports serie du PC
  myPort = new Serial(this, "/dev/tty.usbmodem1411", 9600);   //Initialisation de la communicaiton port serie
  myPort.bufferUntil('\n'); //Attend la réception d'un fin de ligne pour généer un serialEvent()
  */
}

 void draw()
{
  //importation des images 
  PImage fond;
  //fond = loadImage("fond.png");
  fond = loadImage("01.jpg");
  image(fond,0,0);
  PImage navsafe;
  navsafe = loadImage("navsafewhite.png");
  image(navsafe,900,530);
  PImage map;
  //map = loadImage("map.png");
  map = loadImage("mapbis.png");
  image(map,600,70);
  PImage solar;
  solar = loadImage("solar.png");
  PImage storm;
  storm = loadImage("storm.png");
  PImage orange;
  orange = loadImage("orange.png");
  PImage logo;
  logo = loadImage("navsafemini.png");
  PImage battery10;
  battery10 = loadImage("battery10.png");
  PImage battery20;
  battery20 = loadImage("battery20.png");
  PImage battery40;
  battery40 = loadImage("battery40.png");
  PImage battery60;
  battery60 = loadImage("battery60.png");
  PImage battery90;
  battery90 = loadImage("battery90.png");
  
  //importation de la police helvetica neue
  PFont police;
  police = loadFont("HelveticaNeue.vlw");
  
  //couleur police
  fill(255,255,255);
  
  //police du sous titre
  textFont (police, 35); 
  text("• Hour : "+hour()+" : "+minute()+" : "+second(), 20, 100);
  text("• Date : "+day()+" / "+month()+" / "+year(), 20, 50);
 
  //police du sous titre
  textFont (police, 35); 
  text("• Weather Conditions :", 20, 160);

  //trame reçue des capteurs
  String msg="1025;28.3;calm;48;10;2.5;78;6;3.1;1";
  String [] meteo=split(msg, ';');
  
  float pressure=Float.parseFloat(meteo[0]);
  float temp=Float.parseFloat(meteo[1]);
  String sea=meteo[2];
  float percent1=Float.parseFloat(meteo[3]);
  float volt1=Float.parseFloat(meteo[4]);
  float amp1=Float.parseFloat(meteo[5]);
  float percent2=Float.parseFloat(meteo[6]);
  float volt2=Float.parseFloat(meteo[7]);
  float amp2=Float.parseFloat(meteo[8]);
  int weather=Integer.parseInt(meteo[9]);
  
  //conditions météo
  if (weather == 1) {
     // beau temps
     image(solar,400,105);
  } 
  else if (weather == 2) {
     // temps moyen
     image(orange,400,105);
  }
  else if (weather == 3) {
    // tempete
    image(storm, 400, 105);
  }
  
  //police du sous-sous titre
  textFont (police, 25); 
  text("Pressure : "+pressure+" hPa", 60, 200);
  text("Temperature : "+temp+" °C", 60, 230);
  text("Sea State : "+sea, 60, 260);
  
  
  //informations batteries
  
  //police du sous titre
  textFont (police, 35); 
  text("• Battery smart :", 20, 320);
  
  //police du sous-sous titre
  textFont (police, 25); 
  text("Voltage : "+volt1+" V", 60, 360);
  text("Ampere : "+amp1+" A", 60, 390);
  textFont (police, 35);
  text(percent1+" %", 430, 385);
  if (0<=percent1 && percent1<=15) {
    image(battery10, 300, 325);
  }
  else if (15<percent1 && percent1<=35) {
    image(battery20, 300, 325);
  }
  else if (35<percent1 && percent1<50) {
    image(battery40, 300, 325);
  }
  else if (50<=percent1 && percent1<=75) {
    image(battery60, 300, 325);
  }
  else {
    image(battery90, 300, 325);
  }
  
  
  //police du sous titre
  textFont (police, 35); 
  text("• Battery emission :", 20, 450);
  
  //police du sous-sous titre
  textFont (police, 25); 
  text("Voltage : "+volt2+" V", 60, 490);
  text("Ampere : "+amp2+" A", 60, 520);
  textFont (police, 35);
  text(percent2+" %", 430, 505);
  if (0<=percent2 && percent2<=15) {
    image(battery10, 300, 450);
  }
  else if (15<percent2 && percent2<=35) {
    image(battery20, 300, 450);
  }
  else if (35<percent2 && percent2<50) {
    image(battery40, 300, 450);
  }
  else if (50<=percent2 && percent2<=75) {
    image(battery60, 300, 450);
  }
  else {
    image(battery90, 300, 450);
  }
  

  //localisation
  
  //taille de la carte
  int a=map.width;
  int b=map.height;
  //coord en bas à gauche
  //latitude (y) : entre 48.9030 et 48.8140 (0.089 de longueur)
  //longitude (x) : entre 2.2510 et 2.4170 (0.166 de longueur)
  
  //coordonnées reçues
  String gps="2.319;48.879";
  String [] coord=split(gps, ';');
 
  float x=Float.parseFloat(coord[0]);
  float y=Float.parseFloat(coord[1]);
  
  //mettre les coordonnées à l'échelle de l'image  
  x=((x-2.2510)*a)/0.166;
  y=((y-48.9030)*b)/0.0845;
  y=-y;  //inverser l'axe des ordonnées
  //déplacer par rapport à l'origine du repère
  x=x+600;
  y=y+70;
  
  //affichage des coordonnées sur la carte
  fill(0,0,0);  //couleur du rectangle
  rect(x+10, y+5, 75, 30, 10);
  fill(255,255,255);  //couleur de la police
  textFont (police, 11); 
  text("lat : "+coord[0], x+22, y+17);
  text("long : "+coord[1], x+17, y+30);
  
  //afficher les coordonnées sur la carte
  int c=logo.width;  //taille de l'image pour la centrer sur la position
  int d=logo.height;
  image(logo,x-c/2,y-d/2);

 /*
 // affichage sur le control panel les data du gps 
 if ( myPort.available() > 0) {  // If data is available,
    val = myPort.readStringUntil('\n');         // read it and store it in val
  }  
  println(val); //print it out in the console
  text("GPS Data :"+val, 30, 290);

  int [] coord = split(val, ';');

  rect(coord[0], coord[1], 10, 10, 60);
  */
  
}








