
/*
  WiFiAccessPoint.ino creates a WiFi access point and provides a web server on it.
  Steps:
  1. Connect to the access point "yourAp"
  2. Point your web browser to http://192.168.4.1/H to turn the LED on or http://192.168.4.1/L to turn it off
     OR
     Run raw TCP "GET /H" and "GET /L" on PuTTY terminal with 192.168.4.1 as IP address and 80 as port
  Created for arduino-esp32 on 04 July, 2018
  by Elochukwu Ifediora (fedy0)
  All_Free_Fonts_Demo
  Example for TFT_eSPI library
  Created by Bodmer 31/12/16
  WiFiSwitch Mashup 
  ShotokuTech 2 August 2020

  try: Host: 192.168.14.14
  if not, open serial monitor and look at IP. 
  Instructions: On TTGO,  Connect RED wire from servo to 5v, BLACK to GND and WHITE to servo1Pin(27).

*/

#include <WiFi.h>
//#include <WiFiClient.h>
#include <WiFiAP.h>
#include "Free_Fonts.h"
#include "SPI.h"
#include "TFT_eSPI.h"
#include <ESP32Servo.h> 


#define BUTTON_R_PIN 35
#define BUTTON_L_PIN 0

Servo myservo;
unsigned long current_time = 0;
int servo1Pin = 27;
long clockwise_speed = 110;
long STOP_SPEED = 95;
long counter_clock_speed = 80;
long TURN_MS = 3000;
long BETWEEN_TURNS_SHORT = 5 * 1000L;
long MINUTE = 60 * 1000L;
unsigned long HOUR = 60 * MINUTE;
long HALF_HOUR = 30 * MINUTE;
unsigned long BETWEEN_TURNS_LONG = 4 * MINUTE;
unsigned long SCREEN_COUNTDOWN = 5 * MINUTE;


long sleep_time = (HOUR * 7) + HALF_HOUR;
int hours = 0;
int minutes = 0;
unsigned long last_action = 0;


//display Use hardware SPI
TFT_eSPI tft = TFT_eSPI();
unsigned long drawTime = 0;

// Set these to your desired credentials.
const char *ssid = "Bennet";
const char *password = "1besalel";

WiFiServer server(80);
void setup() {

  Serial.begin(115200);
  Serial.println();
  Serial.println("starting:..");
  
  myservo.attach(servo1Pin); //attaches the servo on assigned pin. 
  // Set up Buttons:
  pinMode(BUTTON_R_PIN, INPUT_PULLUP);
  pinMode(BUTTON_L_PIN, INPUT_PULLUP);
  
  // initialization check:
  for (int x=0; x < 2; x++){
     myservo.write(clockwise_speed);
     delay(TURN_MS);
     myservo.write(STOP_SPEED);
     delay(TURN_MS);
     myservo.write(counter_clock_speed);
      delay(TURN_MS);
     myservo.write(STOP_SPEED);
     delay(5 * 1000L); 
  }


  
  //  Uncomment to start access-point or server:
   // start_wifi_access_point();
  start_server();
  
  Serial.println("Server started");

    //display
    tft.begin();
    tft.setRotation(1);
    tft.fillScreen(TFT_PURPLE);
    tft.drawString("Testing :)", 1, 43, GFXFF);
    delay(3000);
    tft.fillScreen(TFT_GOLD);
    tft.drawString("Testing :)", 1, 43, GFXFF);
    delay(3000);
    tft.fillScreen(TFT_BLUE);
    tft.drawString("Testing :)", 1, 43, GFXFF);
    delay(3000);
    print_screen("WAITING");            
}

void loop() {
   // if in wakeup time:
  if (current_time > sleep_time){ // && current_time < (sleep_time + (MINUTE*30))){
      sleep_time = current_time + 24 * HOUR;
      wakeup();
    }
  if (last_action + SCREEN_COUNTDOWN < current_time){
    // Shut-off screen:
      tft.fillScreen(TFT_BLACK);
  }
  handle_server();
  handle_buttons();
  current_time = millis();
}


void handle_server(){
  WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    Serial.println("New Client.");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character
          current_time = millis();
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            print_time();
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();
            // HTML Header
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            
            // CSS - Modify as desired
            client.println("<style>body { text-align: center; font-family: \"Trebuchet MS\", Arial; margin-left:auto; margin-right:auto; }</style>");
                    
            // Page title
            client.println("</head><body style=\"background-color:#70cfff;\"><h1 style=\"color:#ff3410;\"> <a href=\"/\">Curtain Server</a></h1>");
           
            // the content of the HTTP response follows the header:
            client.print("<br><br>");       
            client.print("<h2 style=\"color:#ffffff;\">Time for sleep:</h2>");
            client.print(hours); 
//            client.print(int((sleep_time-current_time)/HOUR));
            client.print("<h2 style=\"color:#ffffff;\"> Hours, :</h2>"); 
            client.print(minutes);
//            client.print(int(((sleep_time-current_time) - (long((sleep_time-current_time)/HOUR)*HOUR))/MINUTE));
            client.print("<h2 style=\"color:#ffffff;\"> Minutes:</h2>"); 

            client.print("<br><br>");            
            client.print("Click <a href=\"/1\">here</a> to ADD half-hour to sleep time.<br>");

            client.print("<br><br>");
            client.print("Click <a href=\"/2\">here</a> to DECREASE half-hour from sleep time.<br>");

            client.print("<br><br>");
            client.print("Click <a href=\"/3\">here</a> to OPEN NOW.<br>"); 
            client.print("<br><br>");
            client.print("Click <a href=\"/4\">here</a> to CLOSE NOW.<br>");             
            // End page
            client.println("</body></html>");     

            // The HTTP response ends with another blank line:
            client.println();

            // break out of the while loop:
            break;
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // Check to see if the client request was "GET /H" or "GET /R":
        if (currentLine.endsWith("GET /1")) {
                print_screen("ADDING");
                sleep_time = sleep_time + HALF_HOUR;          
        }
       
      if (currentLine.endsWith("GET /2")) {
                print_screen("DECREASING");
                sleep_time = sleep_time - HALF_HOUR;
//                if (sleep_time < HALF_HOUR){ sleep_time = 0; }
//                else{ sleep_time = sleep_time - HALF_HOUR;}
        }
     
        if (currentLine.endsWith("GET /3")) {
                client.println();
                print_screen("opening");
                delay(1000);
                open_now();
                // ESP.restart();                
        }
        if (currentLine.endsWith("GET /4")) {
                client.println();
                print_screen("CLOSING!");
                close_now();
                          
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
//    print_screen("I'm still WAITING...   :) whatever");
//    delay(3000);
    print_time();

  }
}

void start_wifi_access_point(){
  Serial.println("Configuring access point...");
  // You can remove the password parameter if you want the AP to be open.
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.begin();
}
void start_server(){
    Serial.println("Starting server:  ");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    server.begin();
}

void move_curtain(long speed, long between_turns){
   // opening:
  for (int x=0; x < 5; x++){
    Serial.print("Moving!!  ");
    Serial.println(x);
    myservo.write(speed);
    delay(TURN_MS);
    myservo.write(STOP_SPEED);
    delay(between_turns);
  }
}

void close_now(){
  move_curtain(counter_clock_speed, BETWEEN_TURNS_SHORT);
}
void open_now(){
  move_curtain(clockwise_speed, BETWEEN_TURNS_SHORT);
}
void wakeup(){
  print_screen("Waky Waky!!");
  move_curtain(clockwise_speed, BETWEEN_TURNS_LONG);
}

void print_screen(String payload){          
  tft.fillScreen(TFT_BLACK);
  tft.setFreeFont(FF23);
  tft.drawString(payload, 1, 43, GFXFF);
  last_action = millis();
}

void print_time(){
  if (sleep_time < current_time){ hours = 0; minutes = 0;
    sleep_time = current_time + 24 * HOUR;
    wakeup();
  }
  else {
    hours = int((sleep_time-current_time)/HOUR);
    minutes = int(((sleep_time-current_time) - (long((sleep_time-current_time)/HOUR)*HOUR))/MINUTE);
  }
  print_screen("CD:\n "+String(hours)+":" + String(minutes));
}



void toggleButtonRight() {
  Serial.println("Right. Button 1 Pressed!");
  sleep_time = sleep_time + HALF_HOUR;
  print_time();
  delay(750);
}

void toggleButtonLeft() {
  Serial.println("Left.  Button 2 Pressed!");
  sleep_time = sleep_time - HALF_HOUR;
//  if (sleep_time < HALF_HOUR){ sleep_time = 0; }
//  else{ sleep_time = sleep_time - HALF_HOUR;}
  print_time();
  delay(750);
}
void handle_buttons(){
  if(digitalRead(BUTTON_L_PIN)==0 && digitalRead(BUTTON_R_PIN)==1){ toggleButtonLeft(); }
  if(digitalRead(BUTTON_R_PIN)==0 && digitalRead(BUTTON_L_PIN)==1){ toggleButtonRight(); }
}


