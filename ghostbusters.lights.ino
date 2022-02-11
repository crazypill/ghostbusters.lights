// Ghostbuster lights for proton pack

#include "light_state_machine.h"



#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif


// Which pin on the Arduino is connected to the NeoPixels?
#define PIN        6

// How many NeoPixels are attached to the Arduino?
#define kRampPixels       14
#define kCyclotron0Pixels 4
#define kCyclotron1Pixels 4
#define kCyclotron2Pixels 4
#define kCyclotron3Pixels 4
#define kCyclontronPixelCount (kCyclotron0Pixels + kCyclotron1Pixels + kCyclotron2Pixels + kCyclotron3Pixels)
#define kCyclotronBase0       kRampPixels
#define kCyclotronBase1       (kCyclotronBase0 + kCyclotron0Pixels)
#define kCyclotronBase2       (kCyclotronBase1 + kCyclotron1Pixels)
#define kCyclotronBase3       (kCyclotronBase2 + kCyclotron2Pixels)

#define NUMPIXELS  (kRampPixels + kCyclontronPixelCount)

// When setting up the NeoPixel library, we tell it how many pixels,
// and which pin to use to send signals. Note that for older NeoPixel
// strips you might need to change the third parameter -- see the
// strandtest example for more information on possible values.
Adafruit_NeoPixel pixels( NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800 );



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


// ------------------------------------------
enum
{
    kSpinState_Startup = 0,
    kSpinState_Light0,
    kSpinState_Light1,
    kSpinState_Light2,
    kSpinState_Light3
};

enum
{
    kSpinSlowIntervalTimeMS = 200,
    kSpinFastIntervalTimeMS = 15,
    kSpinStartupIntervalTimeMS = 1000,
    kSpinRampUpIntervalTimeMS = 4000
};



// Forward declares ------------------------------------------------------

bool lights_ramp_forever( LightState* state );
bool cyclotron_spin_forever( LightState* state );




void setup() 
{
     randomSeed( analogRead( 0 ) );
 
    // INITIALIZE NeoPixel strip object (REQUIRED)
    pixels.begin(); 

    light_state_machine_setup( true );
    light_stack_push( lights_ramp_forever );
    light_stack_push( cyclotron_spin_forever );    
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


bool cyclontron_clear( int lightBaseIndex, size_t pixelCount )
{
    for( int i = 0; i < pixelCount; i++ )
      pixels.setPixelColor( lightBaseIndex + i, pixels.Color( 0, 0, 0 ) );

    return false;
}



bool cyclontron_ramp( int lightBaseIndex, LightState* state )
{
    // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
    pixels.setPixelColor( lightBaseIndex + state->counter, pixels.Color( 5, 0, 0 ) );
    
    state->counter++;
    state->start_time = millis();
    return false;
}


bool cyclontron_twinkle( int lightBaseIndex, size_t pixelCount, LightState* state )
{
    cyclontron_clear( lightBaseIndex, pixelCount );
    int32_t randomPixel = random( 0, pixelCount );
    pixels.setPixelColor( lightBaseIndex + randomPixel, pixels.Color( 5, 0, 0 ) );
    
    state->counter++;
    state->start_time = millis();
    return false;
}


bool cyclotron_spin_forever( LightState* state )
{
    uint32_t        current  = millis();
    uint32_t        interval = current - state->start_time;
    static int32_t  speed    = kSpinSlowIntervalTimeMS;
    static bool     ramp     = false;
    static uint32_t startupCount = 0;

    // first step is to clear the lights
    if( state->step == kSpinState_Startup )
    {
        for( int i = 0; i < kCyclontronPixelCount; i++ )
            pixels.setPixelColor( kCyclotronBase0 + i, pixels.Color( 0, 0, 0 ) );

        state->start_time = current;
        state->step++;
        speed = kSpinSlowIntervalTimeMS; // slowest speed
        return false;
    }
        
    // next step is to look at the time, stay here till we cross the threshold
    if( state->step == kSpinState_Light0 )
    {
        if( interval >= speed )
        {
            if( ramp )
                cyclontron_ramp( kCyclotronBase0, state );
            else
                cyclontron_twinkle( kCyclotronBase0, kCyclotron0Pixels, state );
            if( state->counter > kCyclotron0Pixels )
            {
                cyclontron_clear( kCyclotronBase0, kCyclotron0Pixels );
                state->step = kSpinState_Light1;  // go to next cyclotron window
                state->counter = 0;
            }
        }
    }
    

    if( state->step == kSpinState_Light1 )
    {
        if( interval >= speed )
        {
            if( ramp )
                cyclontron_ramp( kCyclotronBase1, state );
            else
                cyclontron_twinkle( kCyclotronBase1, kCyclotron1Pixels, state );
            if( state->counter > kCyclotron1Pixels )
            {
                cyclontron_clear( kCyclotronBase1, kCyclotron1Pixels );
                state->step = kSpinState_Light2;  // go to next cyclotron window
                state->counter = 0;
            }
        }
    }

    if( state->step == kSpinState_Light2 )
    {
        if( interval >= speed )
        {
            if( ramp )
                cyclontron_ramp( kCyclotronBase2, state );
            else
                cyclontron_twinkle( kCyclotronBase2, kCyclotron2Pixels, state );
            if( state->counter > kCyclotron2Pixels )
            {
                cyclontron_clear( kCyclotronBase2, kCyclotron2Pixels );
                state->step = kSpinState_Light3; // go to next cyclotron window
                state->counter = 0;
            }
        }
    }

    if( state->step == kSpinState_Light3 )
    {
        if( interval >= speed )
        {
            if( ramp )
                cyclontron_ramp( kCyclotronBase3, state );
            else
                cyclontron_twinkle( kCyclotronBase3, kCyclotron3Pixels, state );
                
            if( state->counter > kCyclotron3Pixels )
            {
                cyclontron_clear( kCyclotronBase3, kCyclotron3Pixels );
                state->step = kSpinState_Light0;  // start back at the beginning, don't end
                state->counter = 0;

                // each time around, make the speed faster until we hit the fastest speed
                if( speed > kSpinFastIntervalTimeMS )
                {
                    float fspeed = speed / 1.2f;
                    speed = (int32_t)(fspeed + 0.5f); // round before truncating...
                }
                // after four go-arounds of startup, switch to ramping...
                if( startupCount++ > 2 )
                    ramp = true;
            }
        }
    }

 
    return false; // run forever
}

// EOF
