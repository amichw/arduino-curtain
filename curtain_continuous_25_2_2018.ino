#include <Servo.h>
Servo myservo;
int i = 0;
unsigned long time = 0;

unsigned long HOURS_7 = 7 * 60 * 60 * 1000L;
unsigned long HALF_HOUR = 30 * 60 * 1000L;

unsigned long sleep = 7 * 60 * 60 * 1000L;
long turn_MS = 3000;
unsigned long wait_next_step = 4 * 60 * 1000L;
long turn_speed = 110;
long stop_speed = 95;

long D_turn_MS = 1000;
unsigned long D_wait_next_step = 5 * 1000L;
unsigned long sleep_65 = HOURS_7 - HALF_HOUR;
unsigned long sleep_7 = HOURS_7;
unsigned long sleep_75 = HOURS_7 + HALF_HOUR;
unsigned long sleep_8 = HOURS_7 + HALF_HOUR + HALF_HOUR;
unsigned long five_min = 5 * 60 * 1000L;


void setup() {
  myservo.attach(9); //attaches the servo on pin 9 
}

void loop() {
   myservo.attach(10);
  if ( i == 0 ){
    // initialization check:
      for (int x=0; x <= 7; x++){
         myservo.write(turn_speed);
         delay(turn_MS);
         myservo.write(stop_speed);
         delay(5 * 1000L); 
      }
      //wait 7 hours
//    delay(sleep);
  i++;    
  }
//  if (i < 7) {
//   open_a_little();
//   i++;
//  }
//  if ( i==7){
//    myservo.detach();
//  }

//open_curtain(11, 1);
delay(sleep_75);
open_curtain(10, sleep_75);

 
  }

  
void open_curtain(int pin, long wakeUpTime){
  time = millis();
  if (time>wakeUpTime && time<(wakeUpTime + five_min)){
    myservo.attach(pin); 
      for (int x=0; x <= 7; x++){ open_a_little(); }
    myservo.detach();
  }
}

 void open_a_little(){
  myservo.write(turn_speed);
  delay(turn_MS);
  myservo.write(stop_speed);
  delay(wait_next_step); 
 }

