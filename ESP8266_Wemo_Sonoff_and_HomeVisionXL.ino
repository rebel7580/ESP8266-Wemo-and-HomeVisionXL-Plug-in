/*
 * ESP8266 Wemo and HomeVisionXL
 * Ron Boston 
 * 20 Aug 2017
 * 
 * Code based on the work of Nassir Malik. See his Youtube tutorials at
 *  https://www.youtube.com/channel/UCn8v7OzXk7IqRdKZdf14yjA
 *  and code at https://github.com/nassir-malik
 */

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <functional>

void prepareIds();
boolean connectWifi();
boolean connectUDP();
void startHttpServer();
void debounce();

/*********************************/
// You only need to modify following settings to get it to work with Alexa and your network
const char* ssid = "xxxxxxxx";
const char* password = "xxxxxxxx";
String device_name= "TestRelay"; // you can change the device name from "TestRelay" to anything
/*********************************/

// set pin numbers:
const int relayPin = 12;    // the number of the relay pin
const int ledPin = 13;      // the number of the LED pin
const int buttonPin = 0;    // the number of the pushbutton input pin

// Debounce 
int buttonState;              // the current reading from the input pin
int lastButtonState = HIGH;   // the previous reading from the input pin

// the following variables are unsigned long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers

boolean wifiConnected = false;       // flag to show WIFI connection

WiFiUDP UDP;
boolean udpConnected = false;       // flag to show WIFI connection
IPAddress ipMulti(239, 255, 255, 250);
unsigned int portMulti = 1900;      // local port to listen on

char packetBuffer[UDP_TX_PACKET_MAX_SIZE]; //buffer to hold incoming packet,

String serial;
String persistent_uuid;

ESP8266WebServer HTTP(80);

String switch1 = "Off";


void setup() {
  Serial.begin(115200);

  // button
  pinMode(buttonPin, INPUT);

  // Setup Relay & LED
  pinMode(relayPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);  // Turn on LED during initial WiFi & UDP attempts
  
  prepareIds();
  
  // Initialise WiFi connection
  wifiConnected = connectWifi();

  // only proceed if WiFi connection successful
  if(wifiConnected){
    udpConnected = connectUDP();
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());
    if (udpConnected){
      // initialise pins if needed 
      startHttpServer();
    }
  }  
  digitalWrite(ledPin, HIGH);  // Turn off LED after initial WiFi & UDP attempts
}

void loop() {

  HTTP.handleClient();
  delay(1);

  // if there's data available, read a packet
  // check if the WiFi and UDP connections were successful
  if(wifiConnected){
    if(udpConnected){    
      // if there's data available, read a packet
      int packetSize = UDP.parsePacket();
      if(packetSize) {
        int len = UDP.read(packetBuffer, 255);
        if (len > 0) {
          packetBuffer[len] = 0;
        }
        String request = packetBuffer;
         
        if(request.indexOf('M-SEARCH') > 0) {
          if(request.indexOf("urn:Belkin:device:**") > 0) {                 
            Serial.println("");
            Serial.print("Received packet of size ");
            Serial.println(packetSize);
            Serial.print("From ");
            IPAddress remote = UDP.remoteIP();
            for (int i =0; i < 4; i++) {
              Serial.print(remote[i], DEC);
              if (i < 3) {
                Serial.print(".");
              }
            }
            Serial.print(", port ");
            Serial.println(UDP.remotePort());
            Serial.println("Request:");
            Serial.println(request);
            Serial.println("Responding to search request ...");
            
            respondToSearch();
          }
        }
      }       
      delay(10);
    }
  } else {
      // Turn on/off to indicate cannot connect ..      
  }
  debounce();
}

void prepareIds() {
  uint32_t chipId = ESP.getChipId();
  Serial.println("ChipId");
  Serial.println(chipId);
  char uuid[64];
  sprintf_P(uuid, PSTR("38323636-4558-4dda-9188-cda0e6%02x%02x%02x"),
        (uint16_t) ((chipId >> 16) & 0xff),
        (uint16_t) ((chipId >>  8) & 0xff),
        (uint16_t)   chipId        & 0xff);

  serial = String(uuid);
  persistent_uuid = "Socket-1_0-" + serial;
}
  
// connect to WiFi - returns true if successful or false if not
boolean connectWifi() {
  boolean state = true;
  int i = 0;
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  Serial.println("Connecting to WiFi");
  Serial.println(ssid);
    
  // Wait for connection
  Serial.print("Connecting ...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    Serial.print(WiFi.status());
    if (i > 15){
      state = false;
      break;
    }
    i++;
  }
  
  if (state){
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }
  else {
    Serial.println("");
    Serial.println("Connection failed.");
  }
  
  return state;
}

boolean connectUDP() {
  boolean state = false;
  
  Serial.println("");
  Serial.println("Connecting to UDP");
  
  if(UDP.beginMulticast(WiFi.localIP(), ipMulti, portMulti)) {
    Serial.println("Connection successful");
    state = true;
  }
  else{
    Serial.println("Connection failed");
  }
  
  return state;
}

void respondToSearch() {
    Serial.println("");
    Serial.print("Sending response to ");
    Serial.println(UDP.remoteIP());
    Serial.print("Port : ");
    Serial.println(UDP.remotePort());

    IPAddress localIP = WiFi.localIP();
    char s[16];
    sprintf(s, "%d.%d.%d.%d", localIP[0], localIP[1], localIP[2], localIP[3]);

    String response = 
         "HTTP/1.1 200 OK\r\n"
         "CACHE-CONTROL: max-age=86400\r\n"
         "DATE: Fri, 15 Apr 2016 04:56:29 GMT\r\n"
         "EXT:\r\n"
         "LOCATION: http://" + String(s) + ":80/setup.xml\r\n"
         "OPT: \"http://schemas.upnp.org/upnp/1/0/\"; ns=01\r\n"
         "01-NLS: b9200ebb-736d-4b93-bf03-835149d13983\r\n"
         "SERVER: Unspecified, UPnP/1.0, Unspecified\r\n"
         "ST: urn:Belkin:device:**\r\n"
         "USN: uuid:" + persistent_uuid + "::urn:Belkin:device:**\r\n"
         "X-User-Agent: redsonic\r\n\r\n";

    UDP.beginPacket(UDP.remoteIP(), UDP.remotePort());
    UDP.write(response.c_str());
    UDP.endPacket();                    

     Serial.println("Response sent !");
}

// Set up HTTP request handlers and start HTTP server
void startHttpServer() {
    String result;
    
    HTTP.on("/index.html", HTTP_GET, [](){
      Serial.println("Got Request index.html ...\n");
      HTTP.send(200, "text/plain", "Hello");
    });

    // On/Off requests from Alexa
    HTTP.on("/upnp/control/basicevent1", HTTP_POST, []() {
      Serial.println("########## Responding to  /upnp/control/basicevent1 ... ##########");      

      /*for (int x=0; x <= HTTP.args(); x++) {
        Serial.println(HTTP.arg(x));
      }*/
  
      String request = HTTP.arg(0);      
      //Serial.print("request:");
      //Serial.println(request);
 
      if(request.indexOf("<BinaryState>1</BinaryState>") > 0) {
          Serial.println("Got Turn on request");
          digitalWrite(relayPin, HIGH);
          digitalWrite(ledPin, LOW);
      }

      if(request.indexOf("<BinaryState>0</BinaryState>") > 0) {
          Serial.println("Got Turn off request");
          digitalWrite(relayPin, LOW);
          digitalWrite(ledPin, HIGH);
      }
      
      HTTP.send(200, "text/plain", "");
    });

    // HTTP "Tasmota" type commands
    HTTP.on("/cm", HTTP_GET, []() {
      Serial.println("########## Responding to switch on/off get request ... ##########");      

      for (int x=0; x < HTTP.args(); x++) {
        Serial.print(HTTP.argName(x) + ": ");
        Serial.println(HTTP.arg(x));
      }
  
      String request = HTTP.arg(0);
      request.toLowerCase();
       
      if(request == "power") {
          Serial.println("Got switch state request");
          if (digitalRead(relayPin) == HIGH) {
              switch1 = "On";
          } else {
              switch1 = "Off";    
          }
      }
      
      if(request == "power on") {
          Serial.println("Got switch Turn on request");
          digitalWrite(relayPin, HIGH);
          digitalWrite(ledPin, LOW);
          switch1 = "On";
      }
      
      if(request == "power off") {
          Serial.println("Got switch Turn off request");
          digitalWrite(relayPin, LOW);
          digitalWrite(ledPin, HIGH);
          switch1 = "Off";
      }

      if(request == "power toggle") {
          Serial.println("Got switch Toggle request");
          if(switch1 == "On") {
              digitalWrite(relayPin, LOW);
              digitalWrite(ledPin, HIGH);
              switch1 = "Off";
          } else {
              digitalWrite(relayPin, HIGH);
              digitalWrite(ledPin, LOW);
              switch1 = "On";   
          }
      }
     
      HTTP.send(200, "text/plain", device_name + " " + switch1);
    });

    // Wemo emulation service request response
    HTTP.on("/eventservice.xml", HTTP_GET, [](){
      Serial.println(" ########## Responding to eventservice.xml ... ########\n");
      String eventservice_xml = "<?scpd xmlns=\"urn:Belkin:service-1-0\"?>"
            "<actionList>"
              "<action>"
                "<name>SetBinaryState</name>"
                "<argumentList>"
                  "<argument>"
                    "<retval/>"
                    "<name>BinaryState</name>"
                    "<relatedStateVariable>BinaryState</relatedStateVariable>"
                    "<direction>in</direction>"
                  "</argument>"
                "</argumentList>"
                 "<serviceStateTable>"
                  "<stateVariable sendEvents=\"yes\">"
                    "<name>BinaryState</name>"
                    "<dataType>Boolean</dataType>"
                    "<defaultValue>0</defaultValue>"
                  "</stateVariable>"
                  "<stateVariable sendEvents=\"yes\">"
                    "<name>level</name>"
                    "<dataType>string</dataType>"
                    "<defaultValue>0</defaultValue>"
                  "</stateVariable>"
                "</serviceStateTable>"
              "</action>"
            "</scpd>\r\n"
            "\r\n";
            
      HTTP.send(200, "text/plain", eventservice_xml.c_str());
    });

    // Wemo Emulation setup request response
    HTTP.on("/setup.xml", HTTP_GET, [](){
      Serial.println(" ########## Responding to setup.xml ... ########\n");

      IPAddress localIP = WiFi.localIP();
      char s[16];
      sprintf(s, "%d.%d.%d.%d", localIP[0], localIP[1], localIP[2], localIP[3]);
    
      String setup_xml = "<?xml version=\"1.0\"?>"
            "<root>"
             "<device>"
                "<deviceType>urn:Belkin:device:controllee:1</deviceType>"
                "<friendlyName>"+ device_name +"</friendlyName>"
                "<manufacturer>Belkin International Inc.</manufacturer>"
                "<modelName>Emulated Socket</modelName>"
                "<modelNumber>3.1415</modelNumber>"
                "<UDN>uuid:"+ persistent_uuid +"</UDN>"
                "<serialNumber>221517K0101769</serialNumber>"
                "<binaryState>0</binaryState>"
                "<serviceList>"
                  "<service>"
                      "<serviceType>urn:Belkin:service:basicevent:1</serviceType>"
                      "<serviceId>urn:Belkin:serviceId:basicevent1</serviceId>"
                      "<controlURL>/upnp/control/basicevent1</controlURL>"
                      "<eventSubURL>/upnp/event/basicevent1</eventSubURL>"
                      "<SCPDURL>/eventservice.xml</SCPDURL>"
                  "</service>"
              "</serviceList>" 
              "</device>"
            "</root>\r\n"
            "\r\n";
            
        HTTP.send(200, "text/xml", setup_xml.c_str());
        
        Serial.print("Sending :");
        Serial.println(setup_xml);
    });
    
    // Start HTTP server
    HTTP.begin();  
    Serial.println("HTTP Server started ..");
}

/*
  Debounce
  
  Debounce built-in Sonoff button and toggle relay
  
  This function created from debounce code from:
  
  created 21 Nov 2006
  by David A. Mellis
  modified 30 Aug 2011
  by Limor Fried
  modified 28 Dec 2012
  by Mike Walters
  modified 30 Aug 2016
  by Arturo Guadalupi

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/Debounce
  
  modified 17 Sep 2017
  by Ron Boston
 */
void debounce() {
  // read the state of the button
  int reading = digitalRead(buttonPin);

  // check to see if you just pressed the button
  // (i.e. the input went from HIGH to LOW),  and you've waited
  // long enough since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;

      // only toggle the LED if the new button state is LOW
      if (buttonState == LOW) {
        if (digitalRead(relayPin) == HIGH) {
          digitalWrite(relayPin, LOW); 
          digitalWrite(ledPin, HIGH);
          switch1 = "Off";
        } else {
          digitalWrite(relayPin, HIGH);
          digitalWrite(ledPin, LOW);
          switch1 = "On";   
        }
      }
      Serial.println("Button Press!!");
      Serial.print("switch1:");
      Serial.print(switch1);
      Serial.print("; reading:");
      Serial.print(reading);
      Serial.print("; last:");
      Serial.println(lastButtonState);
    }
  }
  // save the reading.  Next time through the loop,
  // it'll be the lastButtonState
  lastButtonState = reading;
}
