
/*
* Copyright (c) 2014 FirstBuild
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:

* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.

* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/


#include <FiniteStateMachine.h>
//#include <Adafruit_NeoPixel.h>

#define WAIT_FOR_SECOND_TOUCH 4700
#define WAIT_FOR_TIMEOUT 20000
#define WAIT_COMPLETED_INDICATOR 5000

//state machine setup 
State stateOn = State(stateOnEnter, stateOnUpdate, stateOnExit);
State stateOffCompleted = State(stateOffCompletedEnter, stateOffCompletedUpdate,stateOffCompletedExit);
State stateOff = State(stateOffEnter, stateOffUpdate, stateOffExit);
State stateOffTapped = State(stateOffTappedEnter, stateOffTappedUpdate, stateOffTappedExit);
FSM bpStateMachine = FSM(stateOff);     

static const uint8_t DISPOSER_RELAY = 0;
static const uint8_t VALVE_RELAY = 2;
static const uint8_t GREEN_LED = 5;
static const uint8_t BLUE_LED = 4;
static const uint8_t BUTTON = 1;
static const uint8_t WATCH_DOG = 3;

unsigned long start_completed_indicator_timer_millis = 0;
unsigned long start_completed_timer_millis = 0;
unsigned long start_single_tap_millis = 0;
int lastButtonState = LOW;
int blueValue = 0;
boolean blueDirectionForward = true;
boolean dogbark = true;

//Adafruit_NeoPixel strip = Adafruit_NeoPixel(1, PIXEL_CONTROL, NEO_GRB + NEO_KHZ800);

void setup() { 
  pinMode(BUTTON, INPUT);
  pinMode(VALVE_RELAY, OUTPUT);
//  pinMode(PIXEL_CONTROL, OUTPUT);
  pinMode(DISPOSER_RELAY, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(WATCH_DOG, OUTPUT);
  
  bpStateMachine.transitionTo(stateOff);
//  strip.begin();
//  strip.show();         
}

void loop(){
  bpStateMachine.update();
}
 
//------------------------------------------
// stateOn
//------------------------------------------
void stateOnEnter()
{ 
  digitalWrite(DISPOSER_RELAY, HIGH);
  digitalWrite(VALVE_RELAY, HIGH);
  analogWrite(BLUE_LED, 255);
  digitalWrite(WATCH_DOG, HIGH);
  
//  strip.setPixelColor(0, strip.Color(  0,   0, 255));
//  strip.show();
  
  start_completed_timer_millis = millis();
}

void stateOnUpdate()
{
  unsigned long current_millis = millis();
  int currentButtonState = digitalRead(BUTTON);
  
  if (dogbark == true)
  {
    dogbark = false;
    digitalWrite(WATCH_DOG, LOW);
  }
  else 
  {
    dogbark = true;
    digitalWrite(WATCH_DOG, HIGH);
  }
  
  if (currentButtonState != lastButtonState)
  {
    bpStateMachine.transitionTo(stateOff);
  }
  else if ( current_millis - start_completed_timer_millis > WAIT_FOR_TIMEOUT )
  {
    bpStateMachine.transitionTo(stateOffCompleted);
  }  
  
}

void stateOnExit()
{ 
}

//------------------------------------------
// stateOffCompleted
//------------------------------------------
void stateOffCompletedEnter()
{ 
  start_completed_indicator_timer_millis = millis();
  
  digitalWrite(DISPOSER_RELAY, LOW);
  digitalWrite(VALVE_RELAY, LOW);
  digitalWrite(GREEN_LED, HIGH);
  analogWrite(BLUE_LED, 0);
  digitalWrite(WATCH_DOG, LOW);
  
//  strip.setPixelColor(0, strip.Color(  0,   255, 0));
//  strip.show();
}

void stateOffCompletedUpdate()
{
  unsigned long current_millis = millis();
  if ( current_millis - start_completed_indicator_timer_millis > WAIT_COMPLETED_INDICATOR )
  {
    bpStateMachine.transitionTo(stateOff);
  } 
}

void stateOffCompletedExit()
{ 
}


//------------------------------------------
// stateOff
//------------------------------------------
void stateOffEnter()
{
  digitalWrite(DISPOSER_RELAY, LOW);
  digitalWrite(VALVE_RELAY, LOW);
  
  // get the current state of the button to detect changes
  lastButtonState = digitalRead(BUTTON);
  
  digitalWrite(GREEN_LED, LOW);
  analogWrite(BLUE_LED, 0);
  
//  strip.setPixelColor(0, 0);
//  strip.show();
}

void stateOffUpdate()
{ 
  int currentButtonState = digitalRead(BUTTON);
  if (currentButtonState != lastButtonState)
  {
    //tapped lets see if we get another tap
    lastButtonState = currentButtonState;
    bpStateMachine.transitionTo(stateOffTapped);
  }
}  

void stateOffExit()
{ 
}

//------------------------------------------
// stateOffTapped
//------------------------------------------
void stateOffTappedEnter()
{
  start_single_tap_millis = millis();
}

void stateOffTappedUpdate()
{ 
  unsigned long current_millis = millis();
  int currentButtonState = digitalRead(BUTTON);
  
  if (current_millis - start_single_tap_millis > WAIT_FOR_SECOND_TOUCH)
  {
    //timeout
    bpStateMachine.transitionTo(stateOff); 
  }
  else if (currentButtonState != lastButtonState)
  {
    //we got another tap, lets dispose!
    lastButtonState = currentButtonState;
    bpStateMachine.transitionTo(stateOn);
  }
  else
  {
     if(blueValue==255)
     {
       blueDirectionForward=false;
     } 
     else if (blueValue==0)
     {
       blueDirectionForward=true;
     }
     
     if (blueDirectionForward==true)
     {
       blueValue++;
     }
     else
     {
       blueValue--;
     }
     
     analogWrite(BLUE_LED, blueValue);
//     strip.setPixelColor(0, strip.Color(  0,   0, blueValue));
//     strip.show();
     delay(3);
  }
}  

void stateOffTappedExit()
{ 
   
}


