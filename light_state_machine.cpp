//
//  light_state_machine.cpp
//  
//
//  Created by Alex Lelievre on 2/9/22.
//

#include "light_state_machine.h"


// Defines -----------------------------------------------------------------

#define STACK_MAX        50




// Constants and static data ---------------------------------------------

static const uint16_t kFlashDelayMS = 100;

static int32_t      s_counter       = 0;
static bool         s_toggle_state  = false;
static bool         s_run_parallel  = false;
static LightState   s_light_state   = {0};
static LightFunc    s_light_func    = NULL;

// this is the function stack
static LightFunc    s_func_stack[STACK_MAX]        = { NULL };
static LightState   s_light_state_stack[STACK_MAX] = { NULL };
static int32_t      s_func_stack_index             = 0;

#define countof( a ) (sizeof( a ) / sizeof( a[0] ))


// Code -----------------------------------------------------------------

#pragma mark -

void light_state_machine_setup( bool run_parallel )
{
    memset( &s_light_state, 0, sizeof( s_light_state ) );
    memset( &s_light_state_stack, 0, sizeof( s_light_state_stack ) );
    
    s_run_parallel = run_parallel;
}

 
void light_state_machine_tick()
{
//    if( !s_light_func )
//        randomly_fill_stack();
    
    if( s_run_parallel )
    {
        // run thru the stack if there's one and execute each function in turn
        // we do not pop any elements when they are complete, they must be manually removed
        for( int i = 0; i < s_func_stack_index; i++ )
        {
            LightFunc func = s_func_stack[i];

            // just keep calling the routines over and over until they are done
            if( func && func( &s_light_state_stack[i] ) )
            {
                // add code to remove this routine from the stack !!@
              
                // clear this for next reuse
                memset( &s_light_state_stack[i], 0, sizeof( s_light_state_stack[i] ) );
            }
        }
    }
    else
    {
        // just keep calling the routine over and over until it is done
        if( s_light_func && s_light_func( &s_light_state ) )
        {
            s_light_func = light_stack_pop();
            memset( &s_light_state, 0, sizeof( s_light_state ) );   // clear this for next func
        }
    }
}



#pragma mark -


void flash_led( uint8_t num_pulses )
{
  for( int i = 0; i < num_pulses; i++ )
  {
    digitalWrite( LED_BUILTIN, HIGH );
    delay( kFlashDelayMS );
    digitalWrite( LED_BUILTIN, LOW );
    delay( kFlashDelayMS );
  }
}



#pragma mark -


/*
bool flicker_ramp_on( LightState* state )
{
    analogWrite( LED_FLICKER_PIN, state->step );
    state->step += 5;
    
    // brown-out flicker
    if( state->step > 220 )
        analogWrite( LED_FLICKER_PIN, random( kFlickerBrownoutMinIntensity, kFlickerBrownoutMaxIntensity ) );

    if( state->step > 255 )
    {
        state->step = 0;
        return true;
    }
        
    return false;
}
*/


#pragma mark -

// State stack -----------------------------------------------------------------

void light_stack_push( LightFunc func )
{
    if( s_func_stack_index > STACK_MAX )
    {
        s_func_stack_index = STACK_MAX - 1;
        Serial.println( "light stack overflow!" );
        return;
    }
    s_func_stack[s_func_stack_index++] = func;
}


LightFunc light_stack_pop()
{
    LightFunc func = s_func_stack[--s_func_stack_index];
    if( s_func_stack_index < 0 )
    {
        s_func_stack_index = 0;
        return NULL;
    }
    return func;
}


int32_t light_stack_depth()
{
    return s_func_stack_index;
}

// EOF
