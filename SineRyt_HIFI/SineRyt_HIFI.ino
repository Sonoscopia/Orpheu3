/*  
 *  Rhythmic sinewaves 
    Using Mozzi library in HIFI mode by Tim Barrass 2012-13, CC by-nc-sa
    Made for an Arduino MEGA 2560
*/
 
#include <MozziGuts.h>
#include <Oscil.h> // oscillator template
#include <tables/sin2048_int8.h> // sine table for oscillator
#include <Ead.h> // exponential attack decay
#include <EventDelay.h>
#include <mozzi_rand.h>

#define CONTROL_RATE 256 // powers of 2 please
#define FREQ 5 // connect frequency knob to analog 0
#define DUR 4 // connect duration knob to analog 1
#define DURVAR 3 // connect duration variation knob to analog 2
#define ENV 2 // connect duration variation knob to analog 3

// use: Oscil <table_size, update_rate> oscilName (wavetable), look in .h file of table #included above
Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> aSin(SIN2048_DATA);
EventDelay kDelay; // for triggering envelope start
Ead kEnvelope(CONTROL_RATE); // resolution will be CONTROL_RATE
int amplitude; 
unsigned int duration, decay, attack; 

void setup(){
  startMozzi(CONTROL_RATE); // uses the default control rate of 64, defined in mozzi_config.h
  aSin.setFreq(440); // set the frequency
}


void updateControl(){
  aSin.setFreq(mozziAnalogRead(FREQ)*8+100); // TA: set sinewave frequency
  duration = mozziAnalogRead(DUR) + 10 + rand(mozziAnalogRead(DURVAR)); // set duration
  decay = (duration-1)/(mozziAnalogRead(ENV)>>2);
  attack = decay;   
  if(kDelay.ready()){
    // set random parameters
    //unsigned int duration = rand(500u)+200;
    //unsigned int attack = rand(75)+5; // +5 so the internal step size is more likely to be >0
    //unsigned int decay = duration - attack;
    kEnvelope.start(attack,decay);
    kDelay.start(duration);
  }
  amplitude = (int) kEnvelope.next();     
}


int updateAudio(){
  // this would make more sense with a higher resolution signal
  // but still benefits from using HIFI to avoid the 16kHz pwm noise
  return (amplitude*aSin.next())<<8; // 8 bits scaled up to 14 bits
}


void loop(){
  audioHook(); // required here
}
