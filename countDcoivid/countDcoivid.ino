// i have not developed the gsm part fully

#include <SoftwareSerial.h>

SoftwareSerial internetConnection(6, 7); //connect TX to pin 6, RX to pin 7
const int trigPin = 9;
const int echoPin = 10;
int pV=0;
int count;
long duration;
int changeInHeight;
int detectThreshold=125; // minimum Height for detection

void setup() {
  Serial.begin(9600);
  internetConnection.begin(9600);
}
void loop() {
   getHeight();
   pV = changeInHeight;
   
while (changeInHeight >= detectThreshold){    
      int npV = changeInHeight - pV;
      int vector = vector + npV;
      Serial.print("-/-vector : ");
      Serial.print(vector);
      delay(200);
      pV = changeInHeight;
      getHeight();
      if (changeInHeight < detectThreshold){
        vector=vector/(abs(vector));
        count=count + vector; 
            vector=0;
            npV=0;
            loop(); }}
  Serial.print("changeInHeight: ");
  Serial.print(changeInHeight);
  Serial.print("--> count: ");
  Serial.println(count);
  delay(200);       
  }

int getHeight(){
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  changeInHeight = duration*0.034/2;
    }
  
