// Project sponsor: N/A
// Email: 
// Creator: Cedar Lake Instruments LLC
// Date: October, 2023
//
// Description:
// Uses an M5Stack Tough unit to switch a relay on and off periodically
// 
//
// I/O Connections:
// Arduino pin   - I/O description 
// 19            - Drive relay
//
#include <M5Tough.h>
#include "time.h"

// Globals
//Track cycle time
time_t lastOnTime;
bool _started = false;
RTC_TimeTypeDef RTCtime;

// Pulse interval in seconds
const int INTERVAL_SEC = 50;

// Relay ON time in seconds
const int RELAY_ON_SEC = 2;

// IO
const int RELAY_PIN = 19;
 
// Configure start button on touchscreen
ButtonColors on_clrs = {BLUE, WHITE, WHITE};
ButtonColors off_clrs = {BLACK, WHITE, WHITE};
Button start(110, 100, 100, 80, false, "Start", off_clrs, on_clrs, MC_DATUM);

void setup() 
{ 
  M5.begin();
  M5.Lcd.setTextSize(2);
  M5.Lcd.setTextDatum(MC_DATUM);
  M5.Buttons.draw();
  //add handler for the button
  start.addHandler(start_click, E_ALL);

  pinMode(RELAY_PIN, OUTPUT);  
  digitalWrite(RELAY_PIN, LOW);

  // Initialize time
  // We don't set the time since we don't care about absolute time, just offsets
  M5.Rtc.GetTime(&RTCtime);
  struct tm timeNowSetup = {};
  timeNowSetup.tm_hour = RTCtime.Hours;
  timeNowSetup.tm_min = RTCtime.Minutes;
  timeNowSetup.tm_sec = RTCtime.Seconds;
  lastOnTime = mktime(&timeNowSetup);
  Serial.println("PeriodicRelay 1.0");
}

void loop() 
{
    M5.update();
    // Pace loop at 0.1 second interval
    delay(100);

    if (_started)
    {
        // Check Real Time Clock to see if it's time for a cycle
        M5.Rtc.GetTime(&RTCtime);
        struct tm timeNowS = {};
        timeNowS.tm_hour = RTCtime.Hours;
        timeNowS.tm_min = RTCtime.Minutes;
        timeNowS.tm_sec = RTCtime.Seconds;
        
        time_t timeNow = mktime(&timeNowS);
        if (difftime(timeNow, lastOnTime) >= INTERVAL_SEC)
        {
            doCycle();
            lastOnTime = timeNow;
        }
    }
}

// Start button handler
void start_click(Event& e) 
{
    _started = true;
    start.setLabel("Running");
    Serial.println("Started");
}

// Cycle relay on and off
void doCycle()
{
    // Turn relay on for period of time
    digitalWrite(RELAY_PIN, HIGH);
    Serial.println("Relay on");
    delay(RELAY_ON_SEC * 1000);
    digitalWrite(RELAY_PIN, LOW);
    Serial.println("Relay off");
}
