//
//  light_state_machine.h
//  
//
//  Created by Alex Lelievre on 2/9/22.
//

#ifndef light_state_machine_h
#define light_state_machine_h

#include <stdio.h>
#include <Arduino.h>


// a fairly generic container for most light functions
typedef struct
{
    uint32_t step;
    uint32_t start_time;
    uint32_t param;
    uint32_t counter;
} LightState;

// this is the light function, when it returns true it is done processing and the next function will execute if there's one (when in serial mode, otherwise all functions on the stack run at the same time)...
typedef bool (*LightFunc)( LightState* state );


void        light_state_machine_setup( bool run_parallel );
void        light_state_machine_tick();
void        light_stack_push( LightFunc );
LightFunc   light_stack_pop();
int32_t     light_stack_depth();


void     flash_led( uint8_t num_pulses = 1 );
void     toggle_led();

 
#endif // light_state_machine_h
// EOF
