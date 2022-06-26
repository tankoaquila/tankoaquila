#include <SoftwareSerial.h>
#include <Wire.h>
#include <Adafruit_Fingerprint.h>
SoftwareSerial mySerial(12, 13);    //RX,TX, fingerprint

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
int relay1 = 9;           //-----OUTPUT-------//output to Relay pin to kick-start
int relay2 = 10;         //------OUTPUT------//output to Relay pin to Fuel-pump
int motionPin = 11;     //-------INPUT------//motion detection pin
int n, p, t, timer, prevTime;
uint8_t id = 1;
String message2M = "";
String Report = "";
char A[]={'0','0','0','0'};
bool newMessage= false;
char bufReport[20];

void setup() {
  finger.begin(57600);                 //HW
  Serial.begin(9600);                 // start serial for output
  Wire.begin(8);
  pinMode(motionPin, INPUT);
  pinMode(relay1, OUTPUT);      //to hw
  pinMode(relay2, OUTPUT);     //to hw
  pinMode(4, OUTPUT);         //to hw
  Wire.onRequest(ReturnRequest);
  Wire.onReceive(receiveEvents);
  
  if (finger.verifyPassword()) {} 
  else {while (1) {digitalWrite(4,HIGH);}}
  finger.getTemplateCount();
    Serial.print("Sensor contains "); 
    Serial.print(finger.templateCount); 
    Serial.println(" plates");
    digitalWrite(relay1,HIGH); 
    digitalWrite(relay2,HIGH);
    }

void loop() {
  
if (newMessage == true){ 
    while (Wire.available()){ 
      message2M += char(Wire.read());
    }
    delay(500);
      A[0]='1'; A[1]='1'; A[2]='1'; A[3]='1';
      newMessage = false;
        Serial.print("newMessage: "); Serial.println(newMessage);
        Report="";
      goto rain;
      }
  Serial.print("newMessage: "); Serial.println(newMessage);

  /*-----------------A----------------- 
   motionSensor|template,enroll,empty|getfingerprint info|relay controller
   report:     00|00|00|00
   receive:    00|00|00|00
   */
 rain: 
if(digitalRead(motionPin) == 1 && finger.verifyPassword() && A[0]==1){
  //sensing finger and motion sensing
  Report += "M1 ";
  Serial.println(Report);

  while(1){getFingerprintIDez(); 
    if(finger.confidence >= 127){  
      digitalWrite(relay1, LOW); 
      digitalWrite(relay2, LOW);
      Report += "Fid"+String(finger.confidence) + " ";//Figer Print Id
      Report += "FCid"+ String(finger.fingerID )+" "; //Finger Confidence id
      break;}}
      delay(500);
      }
      
String v = message2M.substring(1,3);
int V = v.toInt();
Serial.print("V: "); Serial.println(V);
if((V >= 0 && V <= 14) && A[0]== '1'){ //empty detal base|gettemplate|enrolling
    if(v == "13" && A == 1){
       finger.emptyDatabase(); 
       A[0]= '0';  
       Report += "NFP:0 ";}//empty finger print eeprom
        
    else if (v == "14" && A[0]== '1'){
      finger.getTemplateCount();
      A[0]= '0'; 
      Report += "NFP:"+String(finger.templateCount)+ " ";}
    
    else {
       String small = message2M.substring(1,3);
       id = small.toInt();
       getFingerprintEnroll(); A[0]= '0'; 
       Report += "Rg"+String(id)+" ";}//registered fingerprint id
       digitalWrite(4, LOW); 
  } else {A[0]= '0';}
  delay(500);
//-----------------B(delete a model)-----------------
String w = message2M.substring(3,5);
            Serial.print("w: "); Serial.println(w);

if (w != "00" && A[1]== '1'){
  String small = message2M.substring(3, 5);
  id = small.toInt();
  finger.deleteModel(id); 
  Report += "DFP:"+String(id)+" ";//deleted Finger id Report
  A[1] = '0';
  }
  
 //-----------------C-----------------
 /*String y = message2M.substring(5,7);
             Serial.print("y: "); Serial.println(y);

 if(y != "00" && A[2]== '1'){getFingerprintID(); A[2] = '0';}*/


 //-----------------D-----------------
String z = message2M.substring(7,9);
            Serial.print("z: "); Serial.println(z);

if((z=="00" ||z=="01" ||z=="10" ||z=="11") && A[3]== '1'){ //control relay
  if(z == "00" && A[3]== '1'){
    digitalWrite(relay1,HIGH); 
    digitalWrite(relay2,HIGH); A[3] = '0';}
  else if(z == "11" && A[3]== '1'){
    digitalWrite(relay1,LOW); digitalWrite(relay2,LOW); A[3] = '0';}
  else if(z == "01" && A[3]== '1'){
    digitalWrite(relay1,HIGH); digitalWrite(relay2,LOW); A[3] = '0';}
  else if(z == "10" && A[3]== '1'){
    digitalWrite(relay1,LOW); digitalWrite(relay2,HIGH);A[3] = '0';}
    Report += " RLS:"+z;//relay state Report
    A[3] = '0';}else{A[3] = '0';}
  
delay(500);
Serial.print("Report: "); Serial.println(Report);

 if(A[5]==1){ Report.toCharArray(bufReport, 32); A[5]=0;}
  
 Serial.print("MSG: "); Serial.println(message2M);

}
    
//--------------------i2c communication-----------------

void ReturnRequest() {//wrie
Wire.write(bufReport);} 
void receiveEvents(){
  message2M="";
  newMessage = true; }
      
uint8_t getFingerprintEnroll() {
  int p = -1;
  while (p != FINGERPRINT_OK) { p = finger.getImage();
    switch (p) {
      case FINGERPRINT_NOFINGER:
        blinkLed(300, 300, 4); break;
      default: break;}}
      
  p = finger.image2Tz(1);
//  imageConvertCondition();

  p = 0;
  while (p != FINGERPRINT_NOFINGER) {p = finger.getImage();}  
  p = -1;
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_NOFINGER:
        blinkLed(90, 90, 4); break;
      default:
        break;}}

  p = finger.image2Tz(2);
  imageConvertCondition();
  p = finger.createModel();
  if (p == FINGERPRINT_OK) {Report +="";}//"matched!
  else {Report +="Unknown error "; return p;}

  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {Report += ""; }//Stored!
  else if (p == FINGERPRINT_PACKETRECIEVEERR) {Report += "Com error "; return p; }
  else { Report += "Unknown error "; return p;}
}


//========================getFingerprintID() =================================
uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
     // Report += "taken "; 
     break;
    default:
      Report += "Unknown error "; return p;}

  p = finger.image2Tz();
 // imageConvertCondition();
  p = finger.fingerFastSearch();
  if (p == FINGERPRINT_OK) {Report += "FP ";} //found Print
  else { Report += "UE "; return p;}//("Unknown error")
  return finger.fingerID;}

int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;
  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;
  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;
  return finger.fingerID;}

//--------------------i2c communication--------0---------
void blinkLed(int onTime, int offTime, int Led) {
  if (t == 1 && timer >= onTime) {
    digitalWrite(Led, HIGH); 
    t = 2; prevTime = millis(); 
    timer = 0;}
  else if (t == 2 && timer >= offTime  ) { 
    digitalWrite(Led, LOW); 
    t = 1;
    prevTime = millis(); timer = 0; }
  if (t == 0) { 
    t = 1; 
    pinMode(Led, OUTPUT); 
    prevTime = millis();}
    timer = millis() - prevTime;
}
