#include <SoftwareSerial.h>
#include <EEPROM.h>

int voltmeter = A1;
int valve = 2;
int LEDfront = 5;
int LEDsanitiser = 6;
int infrared = 7;
int chargingControl = 9;
int dispenseTime;
int timer;
int chargeState = 1;
int SensorValue;
float voltagelevel;
int count;

void setup() {

  Serial.begin(9600);
  pinMode(voltmeter, INPUT);//ok
  pinMode(infrared, INPUT);//ok
  pinMode(valve, OUTPUT);//ok
  pinMode(LEDsanitiser, OUTPUT);//ok
  pinMode(LEDfront, OUTPUT);//ok
  pinMode(chargingControl, OUTPUT);//
  digitalWrite(LEDsanitiser, 1);
  digitalWrite(LEDfront, 1);
  settingDispenceTime();
  digitalWrite(LEDsanitiser, 0);
  digitalWrite(LEDfront, 0);
  digitalWrite(chargingControl, 1);
}

void loop() {
  batteryMonitor();
  //dispenser
  timer = millis();
  if (digitalRead(infrared) == 0) {
    digitalWrite(valve, 1);
    digitalWrite(LEDsanitiser, 1);
    delay(dispenseTime);
    analogWrite(valve, 0);
    digitalWrite(LEDsanitiser, 0);
    digitalWrite(chargingControl, 1);
    while (digitalRead(infrared) == 0) {
      batteryMonitor();
      if (digitalRead(infrared) == 1) break;
    }
  }
}



void settingDispenceTime() {
  timer = millis();
  Serial.println(timer);

  while (1) {

    delay(1000);

    //capture dispence time=================
    if (digitalRead(infrared) == 0) {
      timer = millis();

      while (digitalRead(infrared) == 0) {
        blinkLED(LEDfront, 500);
        dispenseTime = millis() - timer;
        Serial.println("while infrared ==0");
        Serial.println(dispenseTime);
        if (dispenseTime >= 5000)break;
      }
      EEPROM.put(0, dispenseTime);
    }
    //wait time
    if (millis() - timer >= 5000)break;
  }

  EEPROM.get(0, dispenseTime);
  if (dispenseTime < 1000) {
    dispenseTime = 1200;
  }
  if (dispenseTime >= 5000) {
    dispenseTime = 5000;
  }
}

void blinkLED(int pin, int blinkTime) {
  digitalWrite(pin, HIGH);
  delay(blinkTime);
  digitalWrite(pin, LOW);
  delay(blinkTime);
}

void batteryMonitor() {
  SensorValue = digitalRead(infrared);
  voltagelevel = analogRead(voltmeter) * (3.968 / 880)+0.39;
  delay(250);
  Serial.print("SensorValue:"); Serial.println(SensorValue); Serial.print("voltagelevel: "); Serial.println(voltagelevel);
  //charging
  if (voltagelevel <= 3.70) {
    digitalWrite(chargingControl, 1);
    //chargeState=1
    digitalWrite(LEDfront, 1);
    delay(1000);
    digitalWrite(LEDfront, 0);
    Serial.println("1");
  }

  else if (voltagelevel > 3.70 && voltagelevel <= 4.15) {
    digitalWrite(chargingControl, 1);
    digitalWrite(LEDfront, 0);
    delay(1000);
    Serial.println("2");
  }

  else if (voltagelevel >= 4.15) {

    digitalWrite(chargingControl, 1);
    digitalWrite(LEDfront, 1);
    delay(1000);
    Serial.println("3");
  }
}
