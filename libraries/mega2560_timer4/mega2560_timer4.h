//------------------------
//Arduino Mega2560 timer4(16bit)
//pwm use timer 4 for pin 8 and0 7 and 6,so use timer4 can't use pwm
/*timer set example
    16MHz/64 = 0.25MHz
    1/0.25M = 4us
   if this timer interrupt at 20us
    20us/4us = 5 
    needs to count 5 timers!!
    65536-5 = 65531 
    from 65531 started to count until 65535,total 5 timers 
    65532(one)
    65533(two)
    65534(three)
    65535(four)
    65536(five)
    then go into interrupt function
    5 * 4us = 20us
*/
//------------------------
#ifndef MEGA2560_TIMER4_h
#define MEGA2560_TIMER4_h

typedef enum t4_Set {
    t4_s0 = 65531,  //0.02ms 20us    
    t4_s1 = 65524,  //0.048ms 48us
    t4_s2 = 65511,  //0.1ms 100us
    t4_s3 = 65473,  //0.25ms 250us
    t4_s4 = 65411,  //0.5ms 500us
    t4_s5 = 65286,  //1ms 1000us
    t4_s6 = 65036,  //2ms 2000us
    t4_s7 = 63036,  //10ms
    t4_s8 = 53036,  //50ms
    t4_s9 = 40536   //100ms
}t4_set;
class mega2560_timer4
{
private:
	int timer_counter;
public:
	mega2560_timer4();
	~mega2560_timer4();
    typedef void (*timer4_func_ptr)(void);
    void t4_initial(t4_set, timer4_func_ptr);
    int get_counter();
    void counter_add();
    void counter_clear();
    void stop();
    void start();
    void set_TCNT4(t4_set);
};

#endif
	

