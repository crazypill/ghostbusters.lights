// Ghostbuster lights for proton pack

#include "light_state_machine.h"



#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif


// Which pin on the Arduino is connected to the NeoPixels?
#define PIN        6

// How many NeoPixels are attached to the Arduino?
#define kRampPixels      14
#define kCyclotronPixels 14
#define kCyclotronBase1  kRampPixels
#define NUMPIXELS        (kRampPixels + kCyclotronPixels)

// When setting up the NeoPixel library, we tell it how many pixels,
// and which pin to use to send signals. Note that for older NeoPixel
// strips you might need to change the third parameter -- see the
// strandtest example for more information on possible values.
Adafruit_NeoPixel pixels( NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800 );

#define DELAYVAL 50 // Time (in milliseconds) to pause between pixels


// ------------------------------------------
enum
{
    kRampState_Start = 0,
    kRampState_Increase
};

enum
{
    kRampIntervalTimeMS  = 50
};




// Forward declares ------------------------------------------------------

bool lights_ramp_forever( LightState* state );




void setup() 
{
    // INITIALIZE NeoPixel strip object (REQUIRED)
    pixels.begin(); 

    light_state_machine_setup( true );
    light_stack_push( lights_ramp_forever );
    light_stack_push( cyclotron_test_forever );    
}



void loop() 
{
    light_state_machine_tick();
    
    // Send the updated pixel colors to the hardware.
    pixels.show();
}



bool lights_ramp_forever( LightState* state )
{
    uint32_t current  = millis();
    uint32_t interval = current - state->start_time;
    
    // first step is to clear the lights
    if( state->step == kRampState_Start )
    {
        for( int i = 0; i < kRampPixels; i++ )
            pixels.setPixelColor( i, pixels.Color( 0, 0, 0 ) );

        state->start_time = current;
        state->step++;
        return false;
    }
        
    // next step is to look at the time, stay here till we cross the threshold
    if( state->step == kRampState_Increase )
    {
        if( interval >= kRampIntervalTimeMS )
        {
            // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
            pixels.setPixelColor( state->counter, pixels.Color( 0, 0, 10 ) );

            state->counter++;
            state->start_time = current;

            if( state->counter >= kRampPixels )
            {
                state->step = kRampState_Start;  // start back at the beginning, don't end
                state->counter = 0;
            }
        }
    }
    
    return false; // run forever
}




bool cyclotron_test_forever( LightState* state )
{
    uint32_t current  = millis();
    uint32_t interval = current - state->start_time;
    
    // first step is to clear the lights
    if( state->step == kRampState_Start )
    {
        for( int i = 0; i < kCyclotronPixels; i++ )
            pixels.setPixelColor( kCyclotronBase1 + i, pixels.Color( 0, 0, 0 ) );

        state->start_time = current;
        state->step++;
        return false;
    }
        
    // next step is to look at the time, stay here till we cross the threshold
    if( state->step == kRampState_Increase )
    {
        if( interval >= kRampIntervalTimeMS )
        {
            // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
            pixels.setPixelColor( kCyclotronBase1 + state->counter, pixels.Color( 5, 0, 0 ) );

            state->counter++;
            state->start_time = current;

            if( state->counter >= kCyclotronPixels )
            {
                state->step = kRampState_Start;  // start back at the beginning, don't end
                state->counter = 0;
            }
        }
    }
    
    return false; // run forever
}

// EOF
