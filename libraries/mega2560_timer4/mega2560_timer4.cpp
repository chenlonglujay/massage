//For Arduino 1.0 and earlier
#if defined(ARDUINO) && ARDUINO >=100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include <mega2560_timer4.h>
typedef void (*t4_ptr)(void);
t4_ptr ptr;
mega2560_timer4::mega2560_timer4(void) {

}

mega2560_timer4::~mega2560_timer4(void) {

}

void mega2560_timer4::start() {
    TIMSK4 = 0x01;
}

void mega2560_timer4::stop() {
    TIMSK4 = 0x00;
}

void mega2560_timer4::set_TCNT4(t4_set set) {
    TCNT4 = set;
}

void mega2560_timer4::t4_initial(t4_set set, timer4_func_ptr t4fp) {
        timer_counter = 0;                  
        TCCR4A = 0x00;                      
        TCCR4B = 0x03;      //set =3 16Mhz/64=0.25Mhz
                             //please refer clock_select_bit
        TCCR4C = 0x00;                      
        set_TCNT4(set);
        ptr = t4fp; 
        stop();
}

void mega2560_timer4::counter_add(){
    timer_counter = timer_counter+1;
}

void mega2560_timer4::counter_clear(){
    timer_counter = 0;
}

int mega2560_timer4::get_counter() {
    int get_counter = timer_counter;
    return get_counter;
}

ISR(TIMER4_OVF_vect) {
   ptr(); 
}

