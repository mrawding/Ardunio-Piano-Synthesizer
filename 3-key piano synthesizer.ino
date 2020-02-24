/*  Example applying an ADSR envelope to an audio signal
    with Mozzi sonification library.  This shows 
    internal updates at CONTROL_RATE, using update() in updateControl(),
    with interpolation and output using next() at AUDIO_RATE in updateAudio().
    This is the "ordinary" way to use ADSR for smooth amplitude transitions while
    maintaining reasonable efficiency by updating internal states in updateControl().
    
    Demonstrates a simple ADSR object being controlled with
    noteOn() and noteOff() instructions.
  
    Tim Barrass 2013, CC by-nc-sa.
*/

#include <MozziGuts.h>
#include <Oscil.h>
#include <EventDelay.h>
#include <ADSR.h>
#include <tables/sin8192_int8.h> 
#include <mozzi_rand.h>
#include <mozzi_midi.h>

#define CONTROL_RATE 64

Oscil <8192, AUDIO_RATE> aOscil(SIN8192_DATA);; 

// for triggering the envelope
EventDelay noteDelay;

ADSR <CONTROL_RATE, AUDIO_RATE> envelope;

boolean note_is_on = true;

void setup(){
  //Serial.begin(9600); // for Teensy 3.1, beware printout can cause glitches
  Serial.begin(115200);
  randSeed(); // fresh random
  noteDelay.set(2000); // 2 second countdown
  startMozzi(CONTROL_RATE);
}


unsigned int duration, attack, decay, sustain, release_ms;

void updateControl(){

  byte attack_level = 255;
  byte decay_level = 50;
  envelope.setReleaseLevel(0);
  envelope.setADLevels(attack_level,decay_level);
  attack = decay = release_ms = 25; // times for the attack, decay, and release of envelope
  sustain  = 0; //sustain time set to zero so notes can be played as long as key is presssed without being effect by previus touch
  envelope.setTimes(attack,decay,sustain,release_ms);    
  int key0 = mozziAnalogRead(A0); //reads value 0-1023 of analog pin A0 (1st key) (reading the Force resistor voltage in 4-bit level)
  int key1 = mozziAnalogRead(A1); //reads value 0-1023 of analog pin A1 (2nd key) (reading the Force resistor voltage in 4-bit level)
  int key2 = mozziAnalogRead(A2); //reads value 0-1023 of analog pin A2 (3rd key) (reading the Force resistor voltage in 4-bit level)
  int freq1;
  
  if(key0 > 400 and key2 < 400 and key1 < 400){ // first key is pressed
     envelope.noteOn();
     freq1 = 261; // C note on 4th musical octave
     envelope.update(); // updates freuqncy level
     
     
     }
    
     else if(key1 > 400 and key2 < 400 and key0 < 400){ // second key is pressed
     envelope.noteOn();
     freq1 = 293; // D note on 4th musical octave
     envelope.update();
     }
 
     else if(key2 > 400 and key1 < 400 and key0 < 400){ // third key is pressed
      envelope.noteOn();
      freq1  = 329; // E note on 4th musical octave
      envelope.update();
      
     }
     else if(key2 < 400 and key1 > 400 and key0 > 400){ // first and second key is pressed
      envelope.noteOn();
      freq1  = 349; // F note on 4th musical octave
      envelope.update();
      
     }
     else if(key2  >400 and key1 > 400 and key0 < 400){ // third and second key are pressed
      envelope.noteOn();
      freq1  = 392; // G note on 4th musical octave
      envelope.update(); 
      
     }
     else if(key2 > 400 and key1 < 400 and key0 > 400){ //third and first key are pressed
      envelope.noteOn();
      freq1  = 440; // A note on 4th musical octave
      envelope.update();
      
     }
     else if(key2 > 400 and key1 > 400 and key0 > 400){ // all keys are pressed
      envelope.noteOn();
      freq1  = 494; // B note on 4th musical octave
      envelope.update();
      
     }
     
  
     else{
      envelope.noteOff();
      envelope.update();
     }
    aOscil.setFreq(freq1); // sets freuquency function based on which key-state is triggered
 
  envelope.update();
} 


int updateAudio(){
  return (int) (envelope.next() * aOscil.next())>>8;
}


void loop(){
  audioHook(); // mozzi pwm function, controls timer modulation and such based on variables set at the envelop level.






