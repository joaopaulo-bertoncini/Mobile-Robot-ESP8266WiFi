// 
// Robot with ESP8266 WiFI
//
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266Ping.h>
#include <SPI.h>

//L298n motor driver 
#define PIN_PWN_MOTOR1 D0 //D5
#define PIN_MOTOR1_IN1 D1 //D8
#define PIN_MOTOR1_IN2 D2 //D7
#define PIN_PWN_MOTOR2 D3 //D6
#define PIN_MOTOR2_IN3 D4 //D4
#define PIN_MOTOR2_IN4 D5 //D3

const char* host = "192.168.0.102";
const uint16_t port = 5000;

WiFiClient client;
uint16_t send_array[3];
uint16_t receive_array[3];

// DEFINE HERE THE KNOWN NETWORKS
const char* KNOWN_SSID[] = {"PLUGNS", "JPSB"};
const char* KNOWN_PASSWORD[] = {"18997707422", "1833235315"};
const int   KNOWN_SSID_COUNT = sizeof(KNOWN_SSID) / sizeof(KNOWN_SSID[0]); // number of known networks


String join(uint16_t vals[], char sep, int items) {
  String out = "";
  
  for (int i=0; i<items; i++) {
    out = out + String(vals[i]);
    if ((i + 1) < items) {
      out = out + sep;
    }
  }

  return out;
}

void split(String value, char sep, uint16_t vals[], int items) {
  int count = 0;
  char *val = strtok((char *)value.c_str(), &sep);
  
  while (val != NULL && count < items) {
    vals[count++] = (uint16_t)atoi(val);
    val = strtok(NULL, &sep);
  }
}

void setSpeed(int value_engine1, int value_engine2) {
    analogWrite(PIN_PWN_MOTOR1, value_engine1);
    analogWrite(PIN_PWN_MOTOR2, value_engine2);  
}

void goBack() {
     digitalWrite(PIN_MOTOR1_IN1,HIGH);
     digitalWrite(PIN_MOTOR1_IN2,LOW);
     digitalWrite(PIN_MOTOR2_IN3,HIGH);
     digitalWrite(PIN_MOTOR2_IN4,LOW);     
}

void goLeft() {
      digitalWrite(PIN_MOTOR1_IN1,HIGH);
      digitalWrite(PIN_MOTOR1_IN2,LOW);
      digitalWrite(PIN_MOTOR2_IN3,LOW);
      digitalWrite(PIN_MOTOR2_IN4,HIGH); 
}  

void goRight() {
      digitalWrite(PIN_MOTOR1_IN1,LOW);
      digitalWrite(PIN_MOTOR1_IN2,HIGH);
      digitalWrite(PIN_MOTOR2_IN3,HIGH);
      digitalWrite(PIN_MOTOR2_IN4,LOW); 
} 

void goForward() {
      digitalWrite(PIN_MOTOR1_IN1,LOW);
      digitalWrite(PIN_MOTOR1_IN2,HIGH);
      digitalWrite(PIN_MOTOR2_IN3,LOW);
      digitalWrite(PIN_MOTOR2_IN4,HIGH); 
} 

void stop() {
      digitalWrite(PIN_MOTOR1_IN1,LOW);
      digitalWrite(PIN_MOTOR1_IN2,LOW);
      digitalWrite(PIN_MOTOR2_IN3,LOW);
      digitalWrite(PIN_MOTOR2_IN4,LOW); 
}  

void connected() { 
  Serial.print(">>CONNECTING TO ");
  Serial.print(host);
  Serial.print(':');
  Serial.println(port);
  // Use WiFiClient class to create TCP connections
  if (!client.connect(host, port)) {
    Serial.println(">>CONNECTION FAILED");
    delay(5000);
    return;
  }  
  // This will send a string to the server
  Serial.println(">>SENDING ROBOT NAME TO SERVER ");
  if (client.connected()) {
    client.println("Robot01");
  }  
}

void printMacAddress(byte mac[]) {
  for (int i = 5; i >= 0; i--) {
    if (mac[i] < 16) {
      Serial.print("0");
    }
    Serial.print(mac[i], HEX);
    if (i > 0) {
      Serial.print(":");
    }
  }
  Serial.println();
}

void printWiFiData() {
  // print your WiFi 101 Shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP address : ");
  Serial.println(ip);

  Serial.print("Subnet mask: ");
  Serial.println((IPAddress)WiFi.subnetMask());

  Serial.print("Gateway IP : ");
  Serial.println((IPAddress)WiFi.gatewayIP());

  // print your MAC address:
  byte mac[6];
  WiFi.macAddress(mac);
  Serial.print("MAC address: ");
  printMacAddress(mac);
}

void printCurrentNet() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI): ");
  Serial.println(rssi);
}

void pingNetwork() {
  const IPAddress remote_ip(192, 168, 15, 1);
  Serial.print("Pinging host ");
  Serial.println(remote_ip);

  if(Ping.ping(remote_ip)) {
    Serial.println("Success!!");
  } else {
    Serial.println("Error :(");
  }
}

void setup() {
  boolean wifiFound = false;
  int i, n;
  Serial.begin(9600);
  // sets the pins as outputs:
  pinMode(PIN_MOTOR1_IN1, OUTPUT);
  pinMode(PIN_MOTOR1_IN2, OUTPUT);
  pinMode(PIN_PWN_MOTOR1, OUTPUT);
  pinMode(PIN_MOTOR2_IN3, OUTPUT);
  pinMode(PIN_MOTOR2_IN4, OUTPUT);
  pinMode(PIN_PWN_MOTOR2, OUTPUT);

  // ----------------------------------------------------------------
  // Set WiFi to station mode and disconnect from an AP if it was previously connected
  // ----------------------------------------------------------------
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  Serial.println(">>SETUP DONE");

  // ----------------------------------------------------------------
  // WiFi.scanNetworks will return the number of networks found
  // ----------------------------------------------------------------
  Serial.println(F(">>SCAN START"));
  int nbVisibleNetworks = WiFi.scanNetworks();
  Serial.println(F(">>SCAN DONE"));
  if (nbVisibleNetworks == 0) {
    Serial.println(F(">>NO NETWORKS FOUND. RESET TO TRY AGAIN"));
    while (true); // no need to go further, hang in there, will auto launch the Soft WDT reset
  }

  // ----------------------------------------------------------------
  // if you arrive here at least some networks are visible
  // ----------------------------------------------------------------
  Serial.print(nbVisibleNetworks);
  Serial.println(" NETWORK(S) FOUND");

  // ----------------------------------------------------------------
  // check if we recognize one by comparing the visible networks
  // one by one with our list of known networks
  // ----------------------------------------------------------------
  for (i = 0; i < nbVisibleNetworks; ++i) {
    Serial.println(WiFi.SSID(i)); // Print current SSID
    for (n = 0; n < KNOWN_SSID_COUNT; n++) { // walk through the list of known SSID and check for a match
      if (strcmp(KNOWN_SSID[n], WiFi.SSID(i).c_str())) {
        Serial.print(F("\tNot matching "));
        Serial.println(KNOWN_SSID[n]);
      } else { // we got a match
        wifiFound = true;
        break; // n is the network index we found
      }
    } // end for each known wifi SSID
    if (wifiFound) break; // break from the "for each visible network" loop
  } // end for each visible network

  if (!wifiFound) {
    Serial.println(F(">>NO KNOWN NETWORK IDENTIFIED. RESET TO TRY AGAIN"));
    while (true); // no need to go further, hang in there, will auto launch the Soft WDT reset
  }

  // ----------------------------------------------------------------
  // if you arrive here you found 1 known SSID
  // ----------------------------------------------------------------
  Serial.print(F("\nCONNECTING TO "));
  Serial.println(KNOWN_SSID[n]);

  // ----------------------------------------------------------------
  // We try to connect to the WiFi network we found
  // ----------------------------------------------------------------
  WiFi.begin(KNOWN_SSID[n], KNOWN_PASSWORD[n]);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");

  // ----------------------------------------------------------------
  // SUCCESS, you are connected to the known WiFi network
  // ----------------------------------------------------------------
  printWiFiData();

  connected();
}

void loop() {
  // Read all the lines of the reply from server and print them to Serial
  Serial.println(">>RECEIVING FROM REMOTE SERVER");
  //String s_rep = join(send_array, ' ', 5);
  //Serial.print("SENDING >> ");
  //Serial.println(s_rep);
  // not testing 'client.connected()' since we do not need to send data here
  while (client.connected()) {
      if (client.available())
      {
        String buffer = client.readStringUntil('\n');
        client.println("OK - COMMAND RECEIVE");
        split(buffer, ';', receive_array, 3);
        Serial.print("RECEIVING << ");
        Serial.println(buffer);
        for (int i=0; i<3; i++) {
          Serial.println(receive_array[i]);
        }
        switch (receive_array[2])
        {
        case 1:
          goForward();
          break;
        case 2:
          goBack();
          break;
        case 3:
          goRight();
          break;
        case 4:
          goLeft();
          break;
        default:
          stop();
          break;
        }
        setSpeed(receive_array[0],receive_array[1]);
      }   
  }  

  stop();

  if (!client.connected()) {
    connected();
  }
}

