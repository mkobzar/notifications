#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager
#include <WiFiClient.h>
#include <ESP8266mDNS.h>
#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>

int buttonApin = D0;
LiquidCrystal_PCF8574 lcd(0x3F); // use i2cScanner to get i2c address
//String form = "<form action='led'><input type='radio' name='state' value='1' checked>On<input type='radio' name='state' value='0'>Off<p><input type='submit' value='Submit'></p></form>";

ESP8266WebServer server ( 80 );
void setup() {
  WiFiManager wifiManager;
  Serial.begin(115200);
  pinMode(buttonApin, INPUT); // GRN->D0 connected = ON, =LOW
  if (digitalRead(buttonApin) == LOW) {
    Serial.println("force reset");
    wifiManager.resetSettings();
  }
  wifiManager.autoConnect("AutoConnectAP");
  //if you get here you have connected to the WiFi

  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
  }

  if ( MDNS.begin ( "esp8266" ) ) {
    Serial.println ( "MDNS responder started" );
  }
  Serial.println ( WiFi.localIP() );

  Serial.println ( "HTTP server started" );
  Serial.println ( WiFi.localIP() );
  lcd.begin(16, 2); // initialize the lcd

  Wire.begin();
  Wire.beginTransmission(0x3F);
  int error = Wire.endTransmission();
  Serial.print("Error: ");
  Serial.print(error);

  if (error == 0) {
    lcd.setBacklight(255);
    lcd.home(); lcd.clear();
//    lcd.print("Hello LCD");
//    delay(1000);

//    lcd.setBacklight(0);
//    delay(400);
//    lcd.setBacklight(255);
//    lcd.clear();
    lcd.print(WiFi.localIP());

  }
//  server.on("/", []() {    server.send(200, "text/html", form);  });
  server.on("/calls", handle_calls);
  server.on("/led", handle_led);
    server.begin();
  Serial.println("HTTP server started");
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  server.handleClient();
}



void handle_led() {
  // get the value of request argument "state" and convert it to an int
  int state = server.arg("state").toInt();
  digitalWrite(LED_BUILTIN, state);
  //  digitalWrite(led, state);
 String msg =  String("LED is now ") + ((state) ? "off" : "on");
  server.send(200, "text/plain", msg);
     lcd.clear();
   lcd.print(msg);
      Serial.println(msg);
}

void handle_calls() {
  int calls = server.arg("calls").toInt();
  digitalWrite(LED_BUILTIN, calls);
 String msg =  String(calls) + " missed calls";
  server.send(200, "text/plain", msg);
     lcd.clear();
     if(calls>0){
       lcd.print(calls); 
      lcd.setCursor(0, 1); 
      lcd.print("missed calls"); 
     }
      Serial.println(msg);
}

