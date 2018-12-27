
// Based on code from https://github.com/umarsear/Esp8266_NeoPixel_clock
// as described in https://www.instructables.com/id/NeoPixel-NTP-Clock-Using-ESP8266/

#include <NtpClientLib.h>
#include <ESP8266WiFi.h>          
#include <DNSServer.h>            
#include <ESP8266WebServer.h>     
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic
#include <Adafruit_NeoPixel.h>

#define NUMPIXELS      60       // number of NeoPixel LEDs
#define PIN            2        // digital pin on ESP8266 for the NeoPixel data line
#define mirror_hands   true     // In case the NeoPixel ring is wired anti-clockwise. 

#define minFrameMax 63
#define hourFrameMax 60
#define DELAY 50

#define DAYBRIGHT 100
#define DAYMID 20
#define DAYLO 3

#define NIGHTBRIGHT 20
#define NIGHTMID 5
#define NIGHTLO 2

int bright = DAYBRIGHT;
int mid = DAYMID;
int lo = DAYLO;

byte hour_hand, minute_hand, second_hand, previous_second;
int minFrame=0;
int hourFrame=0;
byte hourPattern=1;
byte minPattern=1;  

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

// clear all the leds to off
void clearHands() {
  for (byte i=0; i<=NUMPIXELS;i++) {
    pixels.setPixelColor(i,pixels.Color(0,0,0));
  }

  pixels.setPixelColor(0,pixels.Color(0,0,bright));
  pixels.setPixelColor(30,pixels.Color(0,0,mid));
  pixels.setPixelColor(15,pixels.Color(0,0,mid));
  pixels.setPixelColor(45,pixels.Color(0,0,mid));
  pixels.setPixelColor(5,pixels.Color(0,0,lo));
  pixels.setPixelColor(10,pixels.Color(0,0,lo));
  pixels.setPixelColor(20,pixels.Color(0,0,lo));
  pixels.setPixelColor(25,pixels.Color(0,0,lo));
  pixels.setPixelColor(35,pixels.Color(0,0,lo));
  pixels.setPixelColor(40,pixels.Color(0,0,lo));
  pixels.setPixelColor(50,pixels.Color(0,0,lo));
  pixels.setPixelColor(55,pixels.Color(0,0,lo));

}

void drawHands(){
  calcTime();
  // show all the LED's, only the ones we have set with a color will be visible.
  pixels.show();

  // just a debug string, can be removed
  Serial.printf("hour:%d (%d), minute:%d (%d) second:%d (%d)\n",hour(),hour_hand,minute(),minute_hand,second(),second_hand);
}

void calcTime() {
   clearHands();
   calcHour();
   calcMinute();
   calcSeconds();
}


void calcHour() {
  pixels.setPixelColor(hour_hand,pixels.Color(bright,0,0));
  pixels.setPixelColor((hour_hand+59)%60,pixels.Color(mid,0,0));
  pixels.setPixelColor((hour_hand+1)%60,pixels.Color(mid,0,0));
}

void calcMinute() {   
  // if hour and minute are the same led, use a different color to show that
  pixels.setPixelColor(minute_hand,pixels.getPixelColor(minute_hand)+pixels.Color(0,bright,0));
}

void calcSeconds() {
  // draw the second LED, using medium brightness white
  pixels.setPixelColor(second_hand,pixels.Color(bright,bright,bright));
  pixels.setPixelColor((second_hand+1)%60,pixels.getPixelColor((second_hand+1)%60)+pixels.Color(mid,mid,mid));
  pixels.setPixelColor((second_hand+2)%60,pixels.getPixelColor((second_hand+2)%60)+pixels.Color(lo,lo,lo)); 
}



void setup() {
  Serial.begin(115200);
  WiFiManager wifiManager; // wifi configuration wizard
  wifiManager.autoConnect("NeoPixel_Clock", "secret"); // configuration for the access point, set your own secret. 
  Serial.println("WiFi Client connected!)");
  NTP.begin("uk.pool.ntp.org", 0, true); // get time from NTP server pool.
  NTP.setInterval(63);
  pixels.begin();
  pixels.setBrightness(254);
}

void animateHour(int frame, byte pattern) {
   Serial.printf("Animating for the hour change %d frame %d\n",hour(),frame);
   if (frame <= 30) {
      int i=frame;
      calcHands();
      clearHands();
      calcMinute();
      calcSeconds();
      switch (pattern) {
      case 1:
         for (int j=0; j<i; j++) {
            pixels.setPixelColor((hour_hand+j)%60,pixels.Color(bright,0,0));
            pixels.setPixelColor((hour_hand+60-j)%60,pixels.Color(bright,0,0));
         }
         break;
      case 2:
         pixels.setPixelColor((hour_hand+i)%60,pixels.Color(bright,0,0));
         pixels.setPixelColor((hour_hand+60-i)%60,pixels.Color(bright,0,0));
         break;
      case 3:
         pixels.setPixelColor((hour_hand+i)%60,pixels.Color(bright,0,0));
         pixels.setPixelColor((hour_hand+i+59)%60,pixels.Color(mid,0,0));
         pixels.setPixelColor((hour_hand+i+1)%60,pixels.Color(mid,0,0));
         break;
      }
   } else if (frame >30 && frame<=60) {
      int i=minFrameMax-frame-3;
      calcHands();
      clearHands();
      calcMinute();
      calcSeconds();
      switch (pattern) {
      case 1:  
         for (int j=0; j<i; j++) {
            pixels.setPixelColor((hour_hand+j)%60,pixels.Color(bright,0,0));
            pixels.setPixelColor((hour_hand+60-j)%60,pixels.Color(bright,0,0));
         }
         break;
      case 2:
         pixels.setPixelColor((hour_hand+i)%60,pixels.Color(bright,0,0));
         pixels.setPixelColor((hour_hand+60-i)%60,pixels.Color(bright,0,0));
         break;
      case 3:
         pixels.setPixelColor((hour_hand+i)%60,pixels.Color(bright,0,0));
         pixels.setPixelColor((hour_hand+i+59)%60,pixels.Color(mid,0,0));
         pixels.setPixelColor((hour_hand+i+1)%60,pixels.Color(mid,0,0));
         break;
      }
   }
   pixels.show();
}

void animateMinute(int frame, byte pattern) {
   Serial.printf("Animating for the minute change %d Frame %d pattern %d\n",minute(),frame, pattern);
   if (frame <= 30) {
      int i=frame;
      calcHands();
      clearHands();
      calcHour();
      calcSeconds();
      if (pattern == 1 || pattern == 3) {
         pixels.setPixelColor((minute_hand+i)%60,pixels.getPixelColor((minute_hand+i)%60)+pixels.Color(0,bright,0));
      }
      if (pattern < 3) {
         pixels.setPixelColor((minute_hand+60-i)%60,pixels.getPixelColor((minute_hand+60-i)%60)+pixels.Color(0,bright,0));
      }
      if (i>0) {
         if (pattern == 1 || pattern == 3) {
            pixels.setPixelColor((minute_hand+i+59)%60,pixels.getPixelColor((minute_hand+i+59)%60)+pixels.Color(0,mid,0));
         }
         if (pattern < 3) {
            pixels.setPixelColor((minute_hand+61-i)%60,pixels.getPixelColor((minute_hand+61-i)%60)+pixels.Color(0,mid,0));   
         }
      }
      if (i>1) {
         if (pattern == 1 || pattern == 3) {
            pixels.setPixelColor((minute_hand+i+58)%60,pixels.getPixelColor((minute_hand+i+58)%60)+pixels.Color(0,lo,0));
         }
         if (pattern < 3) {
            pixels.setPixelColor((minute_hand+62-i)%60,pixels.getPixelColor((minute_hand+62-i)%60)+pixels.Color(0,lo,0)); 
         }   
      }
   } else if (frame >30 && frame<=60) {
      int i=minFrameMax-frame-3;
      calcHands();
      clearHands();
      calcHour();
      calcSeconds();
      if (pattern == 1 || pattern == 3) {
         pixels.setPixelColor((minute_hand+60-i)%60,pixels.getPixelColor((minute_hand+60-i)%60)+pixels.Color(0,bright,0));
         pixels.setPixelColor((minute_hand+59-i)%60,pixels.getPixelColor((minute_hand+59-i)%60)+pixels.Color(0,mid,0));   
         pixels.setPixelColor((minute_hand+58-i)%60,pixels.getPixelColor((minute_hand+58-i)%60)+pixels.Color(0,lo,0));   
      }
      if (pattern < 3) {
         pixels.setPixelColor((minute_hand+i)%60,pixels.getPixelColor((minute_hand+i)%60)+pixels.Color(0,bright,0));
         pixels.setPixelColor((minute_hand+i+1)%60,pixels.getPixelColor((minute_hand+i+1)%60)+pixels.Color(0,mid,0));
         pixels.setPixelColor((minute_hand+i+2)%60,pixels.getPixelColor((minute_hand+i+2)%60)+pixels.Color(0,lo,0));
      }
   } else {
      calcHands();
      clearHands();
      calcHour();
      calcSeconds();
      pixels.setPixelColor(minute_hand,pixels.getPixelColor(minute_hand)+pixels.Color(0,bright,0));
      if (pattern == 1 || pattern == 3) {
         pixels.setPixelColor((minute_hand+1)%60,pixels.getPixelColor((minute_hand+1)%60)+pixels.Color(0,lo,0));
      }
      if (pattern < 3) {
         pixels.setPixelColor((minute_hand+59)%60,pixels.getPixelColor((minute_hand+59)%60)+pixels.Color(0,lo,0));   
      }
   }
   pixels.show();
}

void calcHands() {
  
  byte hour_offset;
  
  minute_hand=minute();
  if (minute_hand>=10) {
    hour_offset=(minute_hand / 10)-1;
  }else
  {
    hour_offset=0;
  }
  
  if (hour() >= 12) {
    hour_hand = ((hour() - 12) * 5) + hour_offset;
  }
  else {
    hour_hand = (hour() * 5) + hour_offset;
  }
   
  if (mirror_hands) {
    hour_hand=60-hour_hand;
    minute_hand=60-minute_hand;
    second_hand=(60-second());
    if (second_hand==60) {
      second_hand=0;
    }
    if (minute_hand==60) {
      minute_hand=0;
    }
    if (hour_hand==60) {
      hour_hand=0;
    }
  } else {
    second_hand=second();
  }

}

void loop() {
  calcHands();

  if (minute()==0 && second()<3 && hourFrame==0 ) {
    hourFrame=hourFrameMax;
    hourPattern=random(1,4);
  } else if (second()==0 && minFrame==0) {
    minFrame=minFrameMax;
    minPattern=random(1,4);
  }
  if (hour()>=23 || hour()<=7) {
     if (bright != NIGHTBRIGHT) {
        bright = NIGHTBRIGHT;
        mid = NIGHTMID;
        lo = NIGHTLO;
     }
  } else if (bright == NIGHTBRIGHT) {
     bright = DAYBRIGHT;
     mid = DAYMID;
     lo = DAYLO;    
  }
  delay(DELAY);
  
  if (hourFrame>0) {
     animateHour(hourFrameMax-hourFrame, hourPattern);
     hourFrame--;  
  } else if (minFrame>0) {
     animateMinute(minFrameMax-minFrame, minPattern);
     minFrame--;  
  } else if (second_hand!=previous_second) {
     previous_second=second_hand;
     drawHands();
  }
}
