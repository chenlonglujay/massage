//For Arduino 1.0 and earlier
#if defined(ARDUINO) && ARDUINO >=100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include <massage.h>


massage::massage(void) {
  
 //default pin
    pin_limitE_sensor1 =  default_pin_limitE_sensor1;    
    pin_limitE_sensor2 = default_pin_limitE_sensor2;
    pin_limitM_sensor1 = default_pin_limitM_sensor1;
    pin_limitM_sensor2 = default_pin_limitM_sensor2;
    limitE_sensor1_first = 1;
    limitE_sensor2_first = 1;

    //button
    pin_BTN_ER_stop = default_pin_BTN_ER_stop;
    pin_BTN_CLPSM_freeze = default_pin_BTN_CLPSM_freeze;
    BTN_ER_stop_state = 1; 
    BTN_CLPSM_freeze_state = 1;
    
    m_preparing_LED_blink_time = default_preparing_LED_blink_time;
    m_onehour_LED_time1 = default_onehour_LED_time1;
    m_onehour_LED_time2 = default_onehour_LED_time2;

    //servo
    pin_servo_angle = default_pin_servo_angle;
    servo_zero = default_servo_zero;
    servo_Min = default_servo_Min;
    servo_Max = default_servo_Max;
    servo_standby_r = default_servo_standby_r;
    servo_standby_l = default_servo_standby_l;

    pin_servo1 = default_pin_servo1;     
    pin_servo2 = default_pin_servo2;     
    pin_servo3 = default_pin_servo3;     
    pin_servo4 = default_pin_servo4;     
 
}

massage::~massage(void) {

}

void massage::massage_initial(uint8_t lmtE1, uint8_t lmtE2, uint8_t lmtM1, uint8_t lmtM2,
                         uint8_t ER_stop, uint8_t CLPSM_freeze, uint8_t yellowLED, uint8_t blueLED,
                         int preparing_LED_blink, int onehour_LED_t1, int onehour_LED_t2) {
    
    pin_limitE_sensor1 = lmtE1;    
    pin_limitE_sensor2 = lmtE2;
    pin_limitM_sensor1 = lmtM1;                                          
    pin_limitM_sensor2 = lmtM2;                 
    pin_BTN_ER_stop = ER_stop;
    pin_BTN_CLPSM_freeze = CLPSM_freeze;

    //preparing and 1 hour detect
    pin_yellow_LED = yellowLED; //remind alreay 1 hour
    pin_blue_LED = blueLED; //preparing
     
    preparingLED = 1; 
    preparingLED_state = 0;
    preparingLED_counter = 0;
    onehour_counter1 = 0;
    onehour_counter2 = 0;
    onehour_stopAll = 0;

    m_preparing_LED_blink_time = preparing_LED_blink;
    m_onehour_LED_time1 = onehour_LED_t1;
    m_onehour_LED_time2 = onehour_LED_t2;

    limitM_sensor_initial();
    BTN_initial();
    LED_initial();

}

void massage::one_hour_check() {
    onehour_counter1++;
    if(onehour_counter1 == m_onehour_LED_time1) {   
        //1 sec
        onehour_counter1 = 0;
        onehour_counter2++;
        if(onehour_counter2 == m_onehour_LED_time2) {
        //1 hour
            onehour_counter2 = 0;
            onehour_stopAll = 1;
        }
    }

    if(preparingLED) {
        preparingLED_counter++;
        if(preparingLED_counter == m_preparing_LED_blink_time) {
            preparingLED_state = !preparingLED_state;
            digitalWrite(pin_blue_LED, preparingLED_state);
            preparingLED_counter = 0;
        } 
    }  

}

void massage::set_blue_LED_off() {
     preparingLED = 0;    
     digitalWrite(pin_blue_LED, 1); //LED OFF 
}

void massage::LED_initial() {
  pinMode(pin_yellow_LED, OUTPUT);
  pinMode(pin_blue_LED, OUTPUT);
  digitalWrite(pin_yellow_LED, 1);  //LED OFF
  digitalWrite(pin_blue_LED, 1);    //LED OFF
}


void massage::limitM_sensor_initial() {
    pinMode(pin_limitM_sensor1, INPUT);
    pinMode(pin_limitM_sensor2, INPUT);
    digitalWrite(pin_limitM_sensor1, HIGH);  //turn on pullup resistor
    digitalWrite(pin_limitM_sensor2, HIGH);  //turn on pullup resistor
}


void massage::BTN_initial() {
  digitalWrite(pin_BTN_ER_stop, HIGH); //turn on pullup resistor
  digitalWrite(pin_BTN_CLPSM_freeze, HIGH);  //turn on pullup resistor  
  pinMode(pin_BTN_ER_stop, INPUT);
  pinMode(pin_BTN_CLPSM_freeze, INPUT);
  if(!digitalRead(pin_BTN_ER_stop)) {
    //press button
    set_BTN_ER_stop_state(0);
  }

  if(digitalRead(pin_BTN_CLPSM_freeze) == 0){ 
    //press button
    set_BTN_CLPSM_freeze_state(0);
  } 

}


void massage::massage_initial_servo(uint8_t svo_angle, uint8_t svo_zero,                                                                                                        
                                    uint8_t Max, uint8_t Min, uint8_t standby_r, uint8_t standby_l,
                                    uint8_t s1, uint8_t s2, uint8_t s3, uint8_t s4, bool init_to_zero) {
    pin_servo_angle = svo_angle; 
    servo_zero = svo_zero; 
    servo_Max = Max; 
    servo_Min = Min; 
    servo_standby_r = standby_r;                                                  
    servo_standby_l = standby_l;                                                  
    //pair:4(l)-3(r) 2(l)-1(r)                                                
    pin_servo1 = s1; 
    pin_servo2 = s2; 
    pin_servo3 = s3; 
    pin_servo4 = s4;

    read_servo_angle_initial();
    servo_initial(init_to_zero);
}


bool massage::get_limitM1() {
    return digitalRead(pin_limitM_sensor1);
}

bool massage::get_limitM2() {
    return digitalRead(pin_limitM_sensor2);
}

void massage::read_servo_angle_initial() {
  pinMode(pin_servo_angle, INPUT);  
}

void massage::servo_initial(bool init_to_zero) {
  pinMode(pin_servo1, OUTPUT);
  pinMode(pin_servo2, OUTPUT);
  pinMode(pin_servo3, OUTPUT);
  pinMode(pin_servo4, OUTPUT);
  if(init_to_zero) {
    analogWrite(pin_servo1, servo_zero); //ZERO
    analogWrite(pin_servo2, servo_zero);
    analogWrite(pin_servo3, servo_zero);
    analogWrite(pin_servo4, servo_zero);
  }  
  servo_standby(servo_standby_l, servo_standby_r);
}

void massage::servo_standby(uint8_t l, uint8_t r) {
      analogWrite(pin_servo1, r);
      analogWrite(pin_servo2, l);
      analogWrite(pin_servo3, r);
      analogWrite(pin_servo4, l);   
}


uint8_t massage::read_servo_angle() {
  float read_ag = analogRead(pin_servo_angle);
  int div10 = 10;
  //Serial.println("read_ag(0-1023): ");
  //Serial.println(read_ag);
 div10 = read_ag/1024*div10;
  //Serial.println("div10(0-9): ");
  //Serial.println(div10);
  if(div10 == 0) {
    return 0;
  } else{
    div10 = (div10+1)*3;  //(1-9)change to (0-30)
    return div10;
  } 
}

void massage::set_BTN_ER_stop_state(bool state) {
    BTN_ER_stop_state = state;
}

bool massage::get_BTN_ER_stop_state() {
    return BTN_ER_stop_state;
}

void massage::servo_move(uint8_t goal_l, uint8_t goal_r){
    if(goal_r > servo_Max) {
     goal_r = servo_Max;
    }         
    if(goal_r < servo_Min) {
      goal_r = servo_Min;
    }   
    
     if(goal_l > servo_Max) {
     goal_l = servo_Max;
    }         
    if(goal_l < servo_Min) {
      goal_l = servo_Min;
    }   
    
    int i = 0, range = 0;
    int range_r = abs(goal_r - servo_standby_r) ;
    int range_l = abs(goal_l - servo_standby_l) ;
    if(range_r > range_l) {
      range = range_r;
          for(i =0; i < range; i++) {      
            range_r = i;
            analogWrite(pin_servo1,  servo_standby_r-range_r);           
            analogWrite(pin_servo3,  servo_standby_r-range_r);   
            if(range_r >=  range_l) {
              range_l =  range_l;
            } else {
              range_l = i;
            }
            analogWrite(pin_servo2,  servo_standby_l+range_l);   
            analogWrite(pin_servo4,  servo_standby_l+range_l);   
          }     
    } else {
         range = range_l;
         for(i =0; i < range; i++) {      
                range_l = i;    
                analogWrite(pin_servo2,  servo_standby_l+range_l);   
                analogWrite(pin_servo4,  servo_standby_l+range_l);   
                if(range_l >=  range_r) {
                  range_l =  range_r;
                } else {
                  range_l = i;
                }
                analogWrite(pin_servo1,  servo_standby_r-range_r);           
                analogWrite(pin_servo3,  servo_standby_r-range_r);   
          }     
    }     
}

bool massage::get_onehour_stopAll() {
    return onehour_stopAll;
}

bool massage::get_BTN_CLPSM_freeze_state() {
    return BTN_CLPSM_freeze_state;
}

void massage::set_BTN_CLPSM_freeze_state(bool state) {
    BTN_CLPSM_freeze_state = state;
}
  
bool massage::get_limitE_sensor1_first() {
    return limitE_sensor1_first;
}                                                                                                                                

bool massage::get_limitE_sensor2_first() {
    return limitE_sensor2_first;
}
   
void massage::set_limitE_sensor1_first_off() {
    limitE_sensor1_first = 0;
}

void massage::set_limitE_sensor2_first_off() {
    limitE_sensor2_first = 0;
}

 
