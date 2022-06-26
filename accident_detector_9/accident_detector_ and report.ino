/* www.learningbuz.com */
/*Impport following Libraries*/
#include <SoftwareSerial.h>
#include <AltSoftSerial.h>
#include <LiquidCrystal_I2C.h>
#include <TinyGPS.h>

TinyGPS gps;
SoftwareSerial gns(3, 2); // RX, TX
AltSoftSerial gsm;        //rx@8, tx=9
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

float voltage;
int netInt;
int BtnPanic = 4;
int MPUsensor = 5;
int BatLevel = A0;
String phoneNumber = "+2348056946260";
int count;
String LAT;
String LON;
int timer;
String NetStatus;
byte batterybar0[8] = {
  0b00000,
  0b00110,
  0b01001,
  0b01001,
  0b01001,
  0b01001,
  0b01111
};

byte batterybar1[8] = {
  0b00000,
  0b00110,
  0b01001,
  0b01001,
  0b01001,
  0b01111,
  0b01111
};

byte batterybar2[8] = {
  0b00000,
  0b00110,
  0b01001,
  0b01001,
  0b01111,
  0b01111,
  0b01111
};
byte batterybar3[8] = {
  0b00000,
  0b00110,
  0b01001,
  0b01111,
  0b01111,
  0b01111,
  0b01111
};
byte batterybar4[8] = {
  0b00000,
  0b00110,
  0b01111,
  0b01111,
  0b01111,
  0b01111,
  0b01111
};


byte net4[8] = {
  0b11110,
  0b00000,
  0b11100,
  0b00000,
  0b11000,
  0b00000,
  0b10000
};


byte net2[8] = {
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b11000,
  0b00000,
  0b10000
};

byte net1[8] = {
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b10000
};

void setup() {
  Serial.begin(9600);
  gns.begin(9600);
  gsm.begin(9600);
  lcd.begin(16, 2); //Defining 16 columns and 2 rows of lcd display

  pinMode(BtnPanic, INPUT);
  pinMode(MPUsensor, INPUT);
  pinMode(A0, INPUT);

  lcd.createChar(0, batterybar1);
  lcd.createChar(1, batterybar2);
  lcd.createChar(2, batterybar3);
  lcd.createChar(3, batterybar4);
  lcd.createChar(5, net1);
  lcd.createChar(6, net2);
  lcd.createChar(7, net4);

 // writeString(0, 0, "welcome PBVARA");

 dispName();
  
  timer = millis();
}
//=========================================================loop
void loop() {
  netInt = RS();
  float flat, flon;
  unsigned long age;
  voltage = analogRead(A0) * 0.004822;
  int presTime = millis() - timer;

  if (presTime >= 6000) {

    smartdelay(200);

    gps.f_get_position(&flat, &flon, &age);

    LAT = String(flat, 6);

    LON = String(flon, 6);
    Serial.println("GPS: " + LAT + "," + LON);
    writeString(0, 1, "              ");
    writeString(0, 1, NetStatus + " PB&VA");
    clearLine(1);

    if (int(flat) != 1000) {writeString(1, 0, String(flat, 5) + "," + String(flon, 5));
    }
    else {writeString(1, 0, "GPS Connecting");}
    timer = millis();}
  
  int accidentSense = digitalRead(MPUsensor);

  if (accidentSense == 1) { message("accident");}

  
  if (digitalRead(BtnPanic) == 1) {  timer = millis();

    while (digitalRead(BtnPanic) == 1) {
      Serial.println("BTN ON");
      clearLine(1);
      writeString(1, 0, String(voltage, 2) + "volt " + String(RS()) + "dB");
      delay(1000);
    }
    int pressTime = millis() - timer;
    if (pressTime <= 3000) {message("panic"); delay(1000);}
    if (pressTime > 3000) {Serial.println("high ");}
    clearLine(1);
  }
  
  serialRead();
  delay(20);

  battBar(0, 15);
  delay(20);

  if (netInt != 0) {
    netBar(0, 0, netInt);
    count = 0;
    Netype();
  }

  if (count == 8) {
    netBar(0, 0, netInt);
  }

  count += 1;
}


void serialRead() {
  if (gsm.available()) {
    //Serial.println(gsm.readString());
      writeString(1, 0,gsm.readString());

  }
  if (Serial.available()) {
    gsm.println(Serial.readString());
  }
  delay(1000);
}

static void smartdelay(unsigned long ms)
{
  unsigned long start = millis();
  do
  {
    while (gns.available())
      gps.encode(gns.read());
  } while (millis() - start < ms);
}



void writeString(int line, int pixel, String Value) {
  lcd.setCursor(pixel, line);
  lcd.print(Value);
}

void writeByte(int line, int pixel, int Value) {
  lcd.setCursor(pixel, line);
  lcd.write(byte(Value));
}

void battBar(int line, int pixel) {

  if (voltage <= 3.65) {
    writeByte(line, pixel, 0);
  }
  if (voltage > 3.65 && voltage <= 3.80) {
    writeByte(line, pixel, 1);
  }
  if (voltage > 3.80 && voltage <= 4.00) {
    writeByte(line, pixel, 2);
  }
  if (voltage > 4.00) {
    writeByte(line, pixel, 3);
  }
}

void netBar(int line, int pixel, int netInt) {
  if (netInt <= 10) {
    writeByte(line, pixel, 5);
  }
  if (netInt > 10 && voltage <= 15) {
    writeByte(line, pixel, 6);
  }
  if (netInt > 15) {
    writeByte(line, pixel, 7);
  }
}

int RS() {
  gsm.write("AT+CSQ\r");
  String raw = gsm.readString();
  //Serial.println(raw);
  raw = raw.substring(raw.indexOf(' ') + 1, raw.indexOf(','));
  int rssi = raw.toInt();
  return rssi;
}

void Netype() {
  
  gsm.write("AT+COPS?\r");
  
  delay(200);
  
  String raw = gsm.readString();
  
  Serial.println(raw);
  
  raw.toUpperCase();
  
  if (raw.indexOf("MTN") > 1) {
  
    NetStatus = "MTN";
  }
  else if (raw.indexOf("GLO") > 1) {
    writeString(0, 1, " Glo");
    NetStatus = "Glo";
  }
  
  else if (raw.indexOf("AIRTEL") > 1) {
    NetStatus = "Airtel";
  }
  
  else  if (raw.indexOf("62160") > 1) {
    NetStatus = "9Mob.";
  }
  
  else {
    NetStatus = "No Net.";
  }

}

void message(String SMS) {
  clearLine(1);
  writeString(1, 0, "Sending Message");
  String loc = "\nview http://maps.google.com/maps?q=loc:" + LAT + "," + LON;
  gsm.println("AT+CMGF=1");
  delay(500);
  gsm.println("AT+CMGS=\"" + phoneNumber + "\"");
  delay(1000);

  if (SMS.indexOf("fire") >= 0) {
    gsm.println("my car is on fire" + loc);
  }

  else if (SMS.indexOf("panic") >= 0) {
    gsm.println("i am under distress." + loc);
    Serial.println("i am under distress." + loc);
  }
  else if (SMS.indexOf("accident") >= 0) {
    gsm.println("I just had an accident." + loc);
  }

  delay(100);
  gsm.write(0x1A); //Equivalent to sending Ctrl+Z.
  writeString(1, 0, "Message Sent!");
  delay(1000);
  clearLine(1);
}

void clearLine(int line) {
  writeString(line, 0, "                ");
}

void dispName() {
  writeString(0, 0, "this project is");
  writeString(1, 0, "presented by");
  delay(3000);
  clearLine(0); clearLine(1);
  writeString(0, 0, "Tanko Aquila &");
  writeString(1, 0, "Segun Ariyo");
  delay(3000);
}
