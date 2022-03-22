#include "DHT.h"

#include <LiquidCrystal.h>

#include <WiFi.h>

#include "ThingSpeak.h"

#include <Adafruit_Sensor.h>


const char * ssid = "AndroidAPC816"; // your network SSID (name) 
const char * password = "sbvt4022"; // your network password

WiFiClient client;

unsigned long myChannelNumber = 1661110;
const char * myWriteAPIKey = "Y5VNFLCEAJL1I0LB";

int counter = 0;
int flag = 0;

int maxd = 50; 
int mind = 0;

float Temperature;
float Humidity;

unsigned long lastTime = 0;
unsigned long timerDelay = 30000;

int LED1 = 5;
int LED2 = 18;
int LED3 = 19;

int echoPin= 22; 
int trigPin = 21; 

int Button = 23;
int dhtPin = 4;

DHT dht(dhtPin, DHT11);

void setup() {
  Serial.begin(9600);

  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);

  pinMode(echoPin, INPUT);
  pinMode(trigPin, OUTPUT);
  
  pinMode(Button, INPUT);

  pinMode(dhtPin, INPUT);

  WiFi.mode(WIFI_STA);

  ThingSpeak.begin(client); // Initialize ThingSpeak

  dht.begin();
}

void loop() {
  int button_situation = digitalRead(Button);
  
  if (button_situation == 1 && flag ==0){
  counter = counter + 1 ; 
  flag = 1 ; 
  }
  else if (button_situation == 0) {
    flag = 0 ; 
  }

  if (counter == 1){
    resetLed();
    digitalWrite(LED1,HIGH);
    digitalWrite(LED2,LOW);
    digitalWrite(LED3,LOW);
  }
  else if (counter == 2 ){
    resetLed();
    digitalWrite(LED1,HIGH);
    digitalWrite(LED2,HIGH);
    digitalWrite(LED3,LOW);
  }
  else if (counter == 3 ){
    resetLed();
    digitalWrite(LED1,HIGH);
    digitalWrite(LED2,HIGH);
    digitalWrite(LED3,HIGH);
  }
  else {
    resetLed();
    counter = 0 ; 
  }
  
    if ((millis() - lastTime) > timerDelay) {
    
    if(WiFi.status() != WL_CONNECTED){
        Serial.print("Attempting to connect");
        while(WiFi.status() != WL_CONNECTED){
        WiFi.begin(ssid, password); 
        delay(5000);     
      } 
      
    Serial.println("\nConnected.");
    
    }

    if (counter == 1 ){
      int distance = caldistance(maxd,mind);
      ThingSpeak.setField(1, distance);
      ThingSpeak.setField(2, -1);
      ThingSpeak.setField(3, -1);
    }
    else if (counter == 2 ){
      int distance = caldistance(maxd,mind);
      Temperature = dht.readTemperature(); 
      ThingSpeak.setField(1, distance);
      ThingSpeak.setField(2, Humidity);
      ThingSpeak.setField(3, -1);
    }
    else if (counter == 3 ){
      int distance = caldistance(maxd,mind);
      Humidity = dht.readHumidity();
      Temperature = dht.readTemperature(); 
      ThingSpeak.setField(1, distance);
      ThingSpeak.setField(2, Humidity);
      ThingSpeak.setField(3, Temperature);   
    }
    else {
       ThingSpeak.setField(1, -1);
       ThingSpeak.setField(2, -1);
       ThingSpeak.setField(3, -1); 
    }
    
    int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
    
    if(x == 200){
      Serial.println("Channel update successful.");
    }
    else{
      Serial.println("Problem updating channel. HTTP error code " + String(x));
    }
    
    lastTime = millis();
 
    }
}

int caldistance(int maxr, int minr) {
  
  long duration, distance;

  digitalWrite(trigPin,LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin,HIGH);
  delayMicroseconds(2);
  digitalWrite(trigPin,LOW);

  duration = pulseIn(echoPin,HIGH);
  distance = duration / 58.2; 
  delay(50);

  if(distance >= maxr || distance <= minr)
  return 0;
  return distance;
  
}
void resetLed() {
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, LOW);
}
