// Copyright 2020 Azusa Pacific Univeristy


// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon), and you can
// e-mail it to yourself
char auth[] = ""; // put your auth token here


// see the README.md file for more info

// Virtual Pin assignments
// V0 Red Light from app to device
// V1 Green "
// V2 Blue "
// V3 Light intensity           from device to app
// V4 Percent Relative Humidity "
// V5 Temperature in Fahrenheit "
// V6 Temperature in Celsisus   "


/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial


#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <Wire.h>

// This code is designed to work with the SI7006-A20_I2CS I2C Mini Module available from ControlEverything.com.
// https://www.controleverything.com/content/Humidity?sku=SI7006-A20_I2CS#tabs-0-product_tabset-2
// SI7006-A20 I2C address is 0x40(64)
#define Addr 0x40



// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "APU-MYDEVICES";
char pass[] = "";



// the number of the LED pin
const int ledPin = 25;

// setting PWM properties
const int freq = 5000;
const int resolution = 10;
float humidity = -1;
float ctemp = -1;
float ftemp = -1;

// see https://github.com/blynkkk/blynk-library/blob/master/examples/GettingStarted/PushData/PushData.ino
BlynkTimer timer;

void myTimerEvent()
{
  //
  // these are communicating data on virtual pins from the ESP device to the Blynk app
  //
  int raw_light = analogRead(34);

  // a voltage of 0-3.3V on the pin turns into a 12-bit value from 0-4095
  // https://randomnerdtutorials.com/esp32-adc-analog-read-arduino-ide/
  // the map function lets 
  // https://www.arduino.cc/reference/tr/language/functions/math/map/
  uint8_t percent_light = map(raw_light, 0, 4095, 0, 100);
  Blynk.virtualWrite(V3, percent_light);
  Serial.print("raw light value is ");
  Serial.print(raw_light);
  Serial.print(", percentage light is ");
  Serial.println(percent_light);
  
  //humidity and temperature
  Blynk.virtualWrite(V4, humidity);
  Blynk.virtualWrite(V5, ftemp);
  Blynk.virtualWrite(V6, ctemp);
}


// these are data coming on virtual pins from the Blynk app to the ESP device
//
BLYNK_WRITE(V0)
{
  int pinData = param.asInt(); 
  pinData=map(pinData, 0, 1023, 1023, 0);
  ledcWrite(0, pinData);
  Serial.print("V0 Slider value is: ");
  Serial.println(pinData);
}

BLYNK_WRITE(V1)
{
  int pinData = param.asInt(); 
  pinData=map(pinData, 0, 1023, 1023, 0);
  ledcWrite(1, pinData);
  Serial.print("V1 Slider value is: ");
  Serial.println(pinData);
}


BLYNK_WRITE(V2)
{
  int pinData = param.asInt(); 
  pinData=map(pinData, 0, 1023, 1023, 0);
  ledcWrite(2, pinData);
  Serial.print("V2 Slider value is: ");
  Serial.println(pinData);
}

void setup()
{
  // Debug console
  Serial.begin(115200);

  // https://randomnerdtutorials.com/esp32-pwm-arduino-ide/
  ledcSetup(0, freq, resolution);
  ledcSetup(1, freq, resolution);
  ledcSetup(2, freq, resolution);
  
  ledcAttachPin(25, 0);
  ledcAttachPin(26, 1);
  ledcAttachPin(27, 2);

  Blynk.begin(auth, ssid, pass);

  timer.setInterval(1 * 1000L, myTimerEvent);

/********Humidity Code Starts************/
  pinMode(21, OUTPUT);
  pinMode(22,  OUTPUT);

  digitalWrite(21, HIGH);  // Power
  digitalWrite(22,  LOW); // Ground
  
  Wire.begin(23, 19);

  
  Serial.begin(115200);

  // Start I2C transmission
  Wire.beginTransmission(Addr);
  // Stop I2C transmission
  Wire.endTransmission();
  delay(300);
/********Humidity Code Ends ************/
}

void loop()
{
  Blynk.run();
  timer.run();
  int light = analogRead(4);

/********Humidity Code Starts************/
    uint8_t data[2] = {0};
  
    // Start I2C transmission
    Wire.beginTransmission(Addr);
    // Send humidity measurement command, NO HOLD MASTER
    Wire.write(0xF5);
    // Stop I2C transmission
    Wire.endTransmission();
    delay(500);
    
    // Request 2 bytes of data
    Wire.requestFrom(Addr, 2);

    // Read 2 bytes of data
    // humidity msb, humidity lsb 
    if(Wire.available() == 2)
    {
      data[0] = Wire.read();
      data[1] = Wire.read();
    }
    
    // Convert the data
    humidity  = ((data[0] * 256.0) + data[1]);
    humidity = ((125 * humidity) / 65536.0) - 6;

    // Start I2C transmission
    Wire.beginTransmission(Addr);
    // Send temperature measurement command, NO HOLD MASTER
    Wire.write(0xF3);
    // Stop I2C transmission
    Wire.endTransmission();
    delay(500);
    
    // Request 2 bytes of data
    Wire.requestFrom(Addr, 2);

    // Read 2 bytes of data
    // temp msb, temp lsb
    if(Wire.available() == 2)
    {
      data[0] = Wire.read();
      data[1] = Wire.read();
    }

    // Convert the data
    float rawtemp  = ((data[0] * 256.0) + data[1]);
    ctemp = ((175.72 * rawtemp) / 65536.0) - 46.85;
    ftemp = ctemp * 1.8 + 32;
    
    // Output data to serial monitor
    Serial.print("Relative humidity : ");
    Serial.print(humidity);
    Serial.println(" % RH");
    Serial.print("Temperature in Celsius : ");
    Serial.print(ctemp);
    Serial.println(" C");
    Serial.print("Temperature in Fahrenheit : ");
    Serial.print(ftemp);
    Serial.println(" F");
    delay(500);
/********Humidity Code Ends************/
}






// from Blynk Blink sketch:
/*************************************************************
  Download latest Blynk library here:
    https://github.com/blynkkk/blynk-library/releases/latest

  Blynk is a platform with iOS and Android apps to control
  Arduino, Raspberry Pi and the likes over the Internet.
  You can easily build graphic interfaces for all your
  projects by simply dragging and dropping widgets.

    Downloads, docs, tutorials: http://www.blynk.cc
    Sketch generator:           http://examples.blynk.cc
    Blynk community:            http://community.blynk.cc
    Follow us:                  http://www.fb.com/blynkapp
                                http://twitter.com/blynk_app

  Blynk library is licensed under MIT license
  This example code is in public domain.

 *************************************************************

  You’ll need:
   - Blynk App (download from AppStore or Google Play)
   - ESP32 board
   - Decide how to connect to Blynk
     (USB, Ethernet, Wi-Fi, Bluetooth, ...)

  There is a bunch of great example sketches included to show you how to get
  started. Think of them as LEGO bricks  and combine them as you wish.
  For example, take the Ethernet Shield sketch and combine it with the
  Servo example, or choose a USB sketch and add a code from SendData
  example.
 *************************************************************/
