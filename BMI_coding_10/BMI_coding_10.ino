#include <SoftwareSerial.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "HX711.h"

#define trigPin 9 //for ultrasonic
#define echoPin 10 //for ultrasonic

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
HX711 scale(6, 7);
bool setting = false;
int Btn = 8;
int BatLevel = A0;
int t;
int s;
int checker;
int timerChart;
int pTS;
//float hc;
float Hc;
float h1;
float w1;
float bmi;

void setup() {

  lcd.begin(16, 2);
  delay(500);
  Serial.begin(9600);
  scale.set_scale(2280.f);                      // this value is obtained by calibrating the scale with known weights; see the README for details
  scale.tare();               // reset the               // print a raw reading from the ADC
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(BatLevel, INPUT);
  pinMode(Btn, INPUT);

  t = millis();
  s = 0;
  pTS = 5000;
}

void loop() {
  voltmeter();
  if (setting == false)
  { setDW();
    setting = true;
  }

  while (checker == 0) { // checker 0 //for display owner //==============0======================
    dispName("BMI Project", "Body Mass Index ", "Measuring Device");
    h1 = 0;
    w1 = 0;

    voltmeter();
    delay(500);
    float w = scale.get_units(10) * 0.1040;

    if (w > 3) {
      checker = 1;
      clearLCD();
      break;
    }
    resetBtn();
  }

  while (checker == 1) {//===============1========================
    float h = Hc - height();
    float w = scale.get_units(10) * 0.1040;
    Serial.println("Hc updated =" + String(Hc));
    voltmeter();

    if (h > h1) {
      h1 = h;
    }
    if (w > w1) {
      w1 = w;
    }
    bmi = w1 / (h1 * h1);

    writeLCD(0, 5, "        ");
    writeLCD(0, 0, "BMI: " + String(bmi, 1));
    writeLCD(1, 0, String(h1, 2) + "m");
    writeLCD(1, 7, "|");
    writeLCD(1, 9, String(w1, 1) + "kg");

    delay(200);

    if (w <= 3) {
      checker = 2;
      timerChart = millis();
      writeLCD(1, 0, "                ");
      pTS = 0;
      break;
    }
    resetBtn();
  }

  while (checker == 2) {//=============2===============
    voltmeter();
    if (bmi < 18.5) {
      BMI("Under weight!");
    }
    else  if (bmi >= 18.5 && bmi <= 24.9) {
      BMI("Normal Weight!");
    }
    else  if (bmi > 24.9 && bmi <= 29.9) {
      BMI("over weight!");
    }
    else  if (bmi > 29.9 && bmi <= 34.9) {
      BMI("obesed!");
    }

    else  if (bmi > 34.9) {
      BMI("Too Obesed!");
    }
    delay(200);

    pTS = millis() - timerChart;
    if (pTS >= 5000) {
      clearLCD();
      checker = 0;
      t = millis();
      break;
    }
    resetBtn();
  }
  resetBtn();
}

float height() {
  float duration, distance;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = (duration / 200) * 0.038383;
  Serial.println(distance);

  return distance;
}

void voltmeter() {

  float readVolt = analogRead(BatLevel) * 4.02 / 818;
  Serial.println("voltage : " + String(readVolt));
  if (readVolt <= 3.60) {
    writeLCD(0, 14, "B1");
  }

  else if (readVolt > 3.60 && readVolt <= 3.75) {
    writeLCD(0, 14, "B2");
  }

  else if (readVolt > 3.75 && readVolt <= 4.00) {
    writeLCD(0, 14, "B3");
  }

  else if (readVolt > 4.00) {
    writeLCD(0, 14, "B4");
  }
}

void BMI(String val) {
  writeLCD(1, 0, val);
}

void clearLCD() {
  writeLCD(0, 0, "              ");
  writeLCD(1, 0, "                ");
}

void dispName(String Title, String Name1, String Name2)
{
  writeLCD(0, 0, Title);
  if (pTS >= 5000 && s == 0) {
    writeLCD(1, 0, Name1);
    t = millis();
    pTS = 0;
    s = 1;
  }

  pTS = millis() - t;
  if (pTS >= 5000 && s == 1) {
    writeLCD(1, 0, "                ");

    writeLCD(1, 0, Name2);
    t = millis();
    s = 0;
    pTS = 0;
  }
}

void resetBtn() {
  
  if (digitalRead(Btn) == 1) {
    int ct = millis();
    while (digitalRead(Btn) == 1) {
      clearLCD();

      writeLCD(0, 0, "CalH =" + String(Hc) + "m");

      writeLCD(1, 0, "Bat Level = " + String(analogRead(BatLevel) * 4.02 / 818));
      delay(500);
      clearLCD();
      int rt = millis() - ct;
      if (rt >= 6000) {
        break;
      }
      clearLCD();
    }
    while (digitalRead(Btn) == 1) {
      delay(500);
      float hc = height();
      writeLCD(0, 0, "CalinH =" + String(hc) + "m");
      writeLCD(1, 0, "Bat Level = " + String(analogRead(BatLevel) * 4.02 / 818));
      if (hc > Hc) {
        Hc = hc;
      }
    }
  }
}

void setDW() {
  clearLCD();
  String loading;
  for (int i = 0; i <= 5; i++) {
    float hc = height();
    if (hc > Hc) {
      Hc = hc;
      Serial.println("Hc updated =" + String(Hc));
    }
    delay(1000);
    loading  = loading + ".";
    Serial.println("count=" + String(i));
    writeLCD(1, 0, "Setting" + loading);
  }
  clearLCD();
}

void writeLCD(int line, int pixel, String msg) {
  lcd.setCursor(pixel, line);
  lcd.print(msg);
}
