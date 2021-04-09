// 
// Robot with ESP8266 WiFI
//
#include <Arduino.h>
#include <ESP8266WiFi.h>

#ifndef STASSID
#define STASSID "PLUGNS" // your network
#define STAPSK  "18997707422" // your password
#endif
//L298n motor driver 
#define PIN_PWN_MOTOR1 D0 //D5
#define PIN_MOTOR1_IN1 D1 //D8
#define PIN_MOTOR1_IN2 D2 //D7
#define PIN_PWN_MOTOR2 D3 //D6
#define PIN_MOTOR2_IN3 D4 //D4
#define PIN_MOTOR2_IN4 D5 //D3

const char* ssid     = STASSID;
const char* password = STAPSK;

const char* host = "192.168.0.102";
const uint16_t port = 5000;

void goBack();
void goForward();
void setSpeed(int,int);
void connected();
void stop();

WiFiClient client;

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

uint16_t send_array[3];
uint16_t receive_array[3];

void setup() {
  Serial.begin(9600);
  // sets the pins as outputs:
  pinMode(PIN_MOTOR1_IN1, OUTPUT);
  pinMode(PIN_MOTOR1_IN2, OUTPUT);
  pinMode(PIN_PWN_MOTOR1, OUTPUT);
  pinMode(PIN_MOTOR2_IN3, OUTPUT);
  pinMode(PIN_MOTOR2_IN4, OUTPUT);
  pinMode(PIN_PWN_MOTOR2, OUTPUT);
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.print("connecting to ");
  Serial.print(host);
  Serial.print(':');
  Serial.println(port);

  connected();
}

void connected() { 
  // Use WiFiClient class to create TCP connections
  if (!client.connect(host, port)) {
    Serial.println("Connection failed");
    delay(5000);
    return;
  }  
  // This will send a string to the server
  Serial.println("Sending Robot name to server");
  if (client.connected()) {
    client.println("Robot01");
  }  
}

void loop() {
  // Read all the lines of the reply from server and print them to Serial
  Serial.println("Receiving from remote server");
  //String s_rep = join(send_array, ' ', 5);
  //Serial.print("SENDING >> ");
  //Serial.println(s_rep);
  // not testing 'client.connected()' since we do not need to send data here
  while (client.connected()) {
      if (client.available())
      {
        String buffer = client.readStringUntil('\n');
        client.println("Ok - Command Receive");
        split(buffer, ';', receive_array, 3);
        Serial.print("RECEIVING << ");
        Serial.println(buffer);
        for (int i=0; i<3; i++) {
          Serial.println(receive_array[i]);
        }
        if (receive_array[2]==1)
          goForward();
        else
          if (receive_array[2]==2)
            goBack();     
          else
            stop();
        setSpeed(receive_array[0],receive_array[1]);
      }   
  }  

  stop();

  if (!client.connected()) {
    connected();
  }

  //  Close the connection
  //  Serial.println("closing connection");
  //  client.stop();
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