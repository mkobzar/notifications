#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager
#include <WiFiClient.h>
#include <ESP8266mDNS.h>
#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>

int buttonApin = 0;
LiquidCrystal_PCF8574 lcd(0x3F); // use i2cScanner to get i2c address
String form = "<form action='led'><input type='radio' name='state' value='1' checked>On<input type='radio' name='state' value='0'>Off<p><input type='submit' value='Submit'></p></form>";

ESP8266WebServer server ( 80 );
const int led = 16;
void setup() {
  WiFiManager wifiManager;
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(buttonApin, INPUT); // GRN->D0 connected = ON, =LOW
  if (digitalRead(buttonApin) == LOW) {
    Serial.println("force reset");
    wifiManager.resetSettings();
  }
  wifiManager.autoConnect("AutoConnectAP");
  //if you get here you have connected to the WiFi
  Serial.println("wifiManager.getConfigPortalSSID()-");
  Serial.println(wifiManager.getConfigPortalSSID());

  pinMode ( led, OUTPUT );
  digitalWrite ( led, 0 );

  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
  }

  if ( MDNS.begin ( "esp8266" ) ) {
    Serial.println ( "MDNS responder started" );
  }
  Serial.println ( WiFi.localIP() );

//  server.on ( "/", handleRoot );
//  server.on ( "/test.svg", drawGraph );
//  server.on ( "/inline", []() {
//    server.send ( 200, "text/plain", "this works as well" );
//  } );
//  server.onNotFound ( handleNotFound );
//  server.begin();
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
    lcd.print("Hello LCD");
    delay(1000);

    lcd.setBacklight(0);
    delay(400);
    lcd.setBacklight(255);
    lcd.clear();
    lcd.print(WiFi.localIP());

  }
  server.on("/", []() {
    server.send(200, "text/html", form);
  });
  server.on("/adc", handle_adc);
  server.on("/led", handle_led);
    server.begin();
  Serial.println("HTTP server started");

}

void loop() {
  server.handleClient();
}



void handleNotFound() {
  digitalWrite ( led, 1 );
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += ( server.method() == HTTP_GET ) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for ( uint8_t i = 0; i < server.args(); i++ ) {
    message += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";
  }

  server.send ( 404, "text/plain", message );
  digitalWrite ( led, 0 );
}

void handle_adc() {
  float val = analogRead(0);
  server.send(200, "text/plain", String(val));
}

void handle_led() {
  // get the value of request argument "state" and convert it to an int
  int state = server.arg("state").toInt();
  digitalWrite(LED_BUILTIN, state);
  //  digitalWrite(led, state);
 String msg =  String("LED is now ") + ((state) ? "off" : "on");
  server.send(200, "text/plain", msg);
//  server.send(200, "text/plain", String("LED is now ") + ((state) ? "on" : "off"));
     lcd.clear();
   lcd.print(msg);
      Serial.println(msg);
  // Serial.println(String("LED is now ") + ((state) ? "on" : "off"));
}

 

