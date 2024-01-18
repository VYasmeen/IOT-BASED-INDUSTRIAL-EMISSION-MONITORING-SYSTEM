#include <TinyGPS++.h>
#include <WiFi.h>
#include "secrets.h"
#include "ThingSpeak.h" // always include thingspeak header file after other header files and custom macros
TinyGPSPlus gps;      // The TinyGPS++ object
float latitude;     //Storing the Latitude
float longitude;    //Storing the Longitude
float velocity;     //Variable  to store the velocity
float sats;         //Variable to store no. of satellites response
String bearing;     //Variable to store orientation or direction of GPS
unsigned int move_index = 1;       // fixed location for now
float lat1 =  15.1092751;
float long1 =  77.6368365;
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
char ssid[] = "project";   // your network SSID (name) 
char pass[] = "project1234";   // your network password
int keyIndex = 0;            // your network key Index number (needed only for WEP)
WiFiClient  client;
LiquidCrystal_I2C lcd(0x27, 16, 2);
unsigned long myChannelNumber = 1;
const char * myWriteAPIKey = "1JZ3X7O75KNKYU8U";
String myStatus = "";
int mq4 = 39;
int mq6 = 36;
int mq2 = 34;
int bz = 2;
int led = 4;
int q4,q2,q6,cl;
void checkGPS()
{
  if (gps.charsProcessed() < 10)
  {
   // Serial.println(F("No GPS detected: check wiring."));
  }
}
void setup() {
  Serial.begin(9600);
  Serial2.begin(9600);
  pinMode(mq4,INPUT);
  pinMode(mq2,INPUT);
  pinMode(mq6,INPUT);
  pinMode(bz,OUTPUT);
  pinMode(led,OUTPUT);
  lcd.begin();         // initialize the lcd
  lcd.backlight();    // open the backlight
  lcd.setCursor(0, 0);
  lcd.print("WELCOME TO INDUSTRIAL");
  lcd.setCursor(0, 1);
  lcd.print("MONITORING SYS...");
  delay(1000);
  delay(1000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("MQ2:    MQ4:   ");
  lcd.setCursor(0, 1);
  lcd.print("MQ6");
  Serial.println("AT");
 delay(500);
 Serial.println("AT+CNMI=2,2,0,0,0");
 delay(500);
 Serial.println("AT+CMGF=1");
 delay(1000);
 Serial.println("AT+CMGF=1");
 delay(500);
 WiFi.mode(WIFI_STA);   
 ThingSpeak.begin(client);
}
void loop() {
  q2 = analogRead(mq2);
  q4 = analogRead(mq4);
  q6 = analogRead(mq6);
 q2 = map(q2,0,4095,0,500);
 q4 = map(q4,0,4095,0,500);
  q6 = map(q6,0,4095,0,500);
  lcd.setCursor(0, 0);
  lcd.print("MQ2:");
  lcd.print(q2);
  lcd.print(" MQ4:");
  lcd.print(q4);
  lcd.print(" PPM");
  lcd.setCursor(0, 1);
  lcd.print(" MQ6:");
  lcd.print(q6);
  lcd.print(" PPM");
 cl = cl + 1;
 //Serial.print(cl);
 while (Serial.available() > 0)
  {
    // sketch displays information every time a new sentence is correctly encoded.
    if (gps.encode(Serial.read()))
     displayInfo();
  }
 if(cl > 20)
 {
 cl=0;
 send_sms();
}
  if(WiFi.status() != WL_CONNECTED){
    Serial.print("Attempting to connect to SSID: ");
    //Serial.println(SECRET_SSID);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);     
    } 
    Serial.println("\nConnected.");
  }

  // set the fields with the values
  ThingSpeak.setField(1, q2);//solar
  ThingSpeak.setField(2, q4);// battery
  ThingSpeak.setField(3, q6);//current
  int k = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if(k == 200){
    Serial.println("Channel update successful.");
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(q2));//   15.1092751, 77.6368365//14.6776° N, 77.6025° E
  }  
}
void displayInfo()
{
  if (gps.location.isValid() )
  {
    sats = gps.satellites.value();       //get number of satellites
    latitude = (gps.location.lat());     //Storing the Lat. and Lon.
    longitude = (gps.location.lng());
    velocity = gps.speed.kmph();         //get velocity
    bearing = TinyGPSPlus::cardinal(gps.course.value());     // get the direction
 
    /*Serial.print("SATS:  ");
    Serial.println(sats);  // float to x decimal places
    Serial.print("LATITUDE:  ");
    Serial.println(latitude, 6);  // float to x decimal places
    Serial.print("LONGITUDE: ");
    Serial.println(longitude, 6);
    Serial.print("SPEED: ");
    Serial.print(velocity);
    Serial.println("kmph");
    Serial.print("DIRECTION: ");
    Serial.println(bearing);*/
   }
 // Serial.println();
}
void send_sms()
{
    Serial.println("AT+CMGF=1");
    delay(500);
    Serial.println("AT+CMGS=\"+916281144853\"\r"); // Replace x with mobile number917892606752
    delay(1000);
    //Serial.println("TROUBLE AT https://www.google.com/maps/place/17.3951,78.4735"); // STALEY COLLEGE
    //Serial.println("TROUBLE AT https://www.google.com/maps/place/17.4598,78.5569"); // HOME
    Serial.print("MQ2 =");
    Serial.print(q2);
    Serial.print(", MQ4 =");
    Serial.print(q4);
    Serial.print(", MQ6 =");
    Serial.print(q6);
    Serial.print(" AT https://www.google.com/maps/place/");
    Serial.print(lat1);  
    Serial.print(",");
    Serial.println(long1);
    delay(1000);
    delay(1000);
    Serial.println(" ");
    delay(100);
    Serial.println((char)26); // ASCII code of CTRL+Z
    delay(1000);
}