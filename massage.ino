//hardware:
//MEGA 2560
//CLP MOTOR AND DRIVER
//note1.CLPSM closed-loop step motor
//ENA = disable will lost torque (this project doesn't control ENA)
//driver(Hybird Servo Drive) Pulse need to set 1600

#include <CLP_MOTOR.h>
#include <mega2560_timer4.h>
#include <massage.h>

mega2560_timer4 tmr4;
massage MSG;
//limit sensor
//M------------E---------E---------------M

//--------------------------------------------
//limit sensor
//if motor touchs  limit sensor E that will change direction
#define pin_limitE_sensor1  18     //normal = 0 
#define pin_limitE_sensor2  19     //normal = 0
bool limitE_sensor1_on = 0;
bool limitE_sensor2_on = 0;
//--------------------------------------------

//--------------------------------------------
//limit sensor M
//if limt sensorE broken ,system will use limit sensorM to detect motor arrives limit side. 
#define pin_limitM_sensor1  38     //normal = 0 
#define pin_limitM_sensor2  39     //normal = 0
//--------------------------------------------

//--------------------------------------------
//button
#define pin_BTN_ER_stop  2
#define pin_BTN_CLPSM_freeze  3
//--------------------------------------------

//--------------------------------------------
//timer4 
#define timer4_set t4_s2
bool Timer4_SW = 0;
//--------------------------------------------

//--------------------------------------------
//CLPSM closed-loop step motor
CLPMTR *CLPSM_controller= new CLPMTR;
#define pin_CLPSM_pulse 7   //control TIP41C, B
#define pin_CLPSM_DIR 6     //control TIP41C, B
enum {sm_stop=0 ,sm_start =1};
bool CLPSM_stop = 0;        //use for stop motor
//--------------------------------------------

//--------------------------------------------
//servo 
#define pin_servo_angle A0
#define servo_zero  180
#define servo_Min  110
#define servo_Max  250
#define servo_standby_r 210
#define servo_standby_l 150
#define init_to_zero  1 //0 no ,1 yes
//pair:4(l)-3(r) 2(l)-1(r) ,
#define pin_servo1 9
#define pin_servo2 10
#define pin_servo3 11
#define pin_servo4 12
//--------------------------------------------

//--------------------------------------------
//preparing and 1 hour detect
#define pin_yellow_LED 16 //remind alreay 1 hour
#define pin_blue_LED 17 //preparing
#define preparing_LED_blink_time 3000  //t4_s2 0.1ms,0.1ms 0.1ms*3000 = 300ms
#define onehour_LED_time1 10000 //0.1ms *10000 = 1sec
#define onehour_LED_time2 3600  //1sec *3600 = 1hour
//--------------------------------------------

void timer4_ISR(void) {
  tmr4.set_TCNT4(timer4_set);
   if(!CLPSM_stop) {
          tmr4.counter_add();
          //set 0.1ms counter
          //0.1ms *2 = 0.2ms
          if (tmr4.get_counter() == 2) {
            Timer4_SW = !Timer4_SW;
              tmr4.counter_clear();
              if (Timer4_SW) {
                CLPSM_controller->setCLPMTR_HIGH();
              }  else {
                CLPSM_controller->setCLPMTR_LOW();        
              }
          }
    }

  MSG.one_hour_check();
}

void setup() {
  Serial.begin(9600);
  MSG.massage_initial_servo(pin_servo_angle, servo_zero,
                               servo_Max, servo_Min, servo_standby_r, servo_standby_l,
                            pin_servo1, pin_servo2, pin_servo3, pin_servo4, init_to_zero);  

    MSG.massage_initial(pin_limitE_sensor1, pin_limitE_sensor2, pin_limitM_sensor1, pin_limitM_sensor2, 
                   pin_BTN_ER_stop, pin_BTN_CLPSM_freeze, pin_yellow_LED, pin_blue_LED,
                     preparing_LED_blink_time, onehour_LED_time1, onehour_LED_time2);                           
  delay(100);  
  timer4_initial();
  tmr4.start();                     
  interrupt_limit_sensor_initial();
  interrupt_BTN_initial();   
  delay(5000);  
  CLPSM_initial();
  MSG.set_blue_LED_off();
  Serial.println("start");
}

void loop() {
    uint8_t read_angle = MSG.read_servo_angle();    
    if(MSG.get_BTN_ER_stop_state() == 0) {
        timer_stop_counter_zero();        
    }
    
    if(MSG.get_BTN_ER_stop_state() == 0) {
        timer_stop_counter_zero();
    }
    
    if(MSG.get_onehour_stopAll()) {
        tmr4.stop();   
        CLPSM_start_stop(sm_stop);  
        digitalWrite(pin_yellow_LED, 0); //LED ON  
        return;
    }
    
    limitM_DIR_chcek();
    if(MSG.get_BTN_ER_stop_state()) {     
        //no press BTN_ER_stop
          CLPSM_start_stop(sm_stop);  
          delay(500);  
          MSG.servo_move(servo_zero+read_angle , servo_zero-read_angle);     //l,r        
          delay(1200);  
          MSG.servo_standby(servo_standby_l, servo_standby_r);
          delay(1200);
          if(MSG.get_BTN_CLPSM_freeze_state()) {
            //no press BTN_CLPSM_freeze              
              CLPSM_start_stop(sm_start); 
               limitM_DIR_chcek();
              delay(500);  
          }
    }
    
}

void timer4_initial() {
  tmr4.t4_initial(timer4_set, timer4_ISR);
}

void CLPSM_start_stop(bool CLPSM_move) {
    if(CLPSM_move) {  
        if(MSG.get_BTN_ER_stop_state() == 1 && MSG.get_BTN_CLPSM_freeze_state() == 1){
            CLPSM_stop = 0;
        } else {
          CLPSM_stop = 1;
        }  
     } else {
         CLPSM_stop = 1;
     }
}
void interrupt_limit_sensor_initial() {
  digitalWrite(pin_limitE_sensor1, HIGH); //turn on pullup resistor
  digitalWrite(pin_limitE_sensor2, HIGH);  //turn on pullup resistor
  attachInterrupt(digitalPinToInterrupt(pin_limitE_sensor1), limit_sensor1_ISR, RISING);
  attachInterrupt(digitalPinToInterrupt(pin_limitE_sensor2), limit_sensor2_ISR, RISING);
}

void limit_sensor1_ISR() {
  if (!MSG.get_limitE_sensor1_first()) {
    static unsigned long last_interrupt_time = 0;
    unsigned long interrupt_time = millis();
    if (interrupt_time - last_interrupt_time > 50) {
      CLPSM_controller->setCLPMTR_CCW();
    }
    last_interrupt_time = interrupt_time;
  }
  MSG.set_limitE_sensor1_first_off();
}

void limit_sensor2_ISR() {
    if (!MSG.get_limitE_sensor2_first())  {    
      static unsigned long last_interrupt_time = 0;
     unsigned long interrupt_time = millis();
    if (interrupt_time - last_interrupt_time > 50) {
      CLPSM_controller->setCLPMTR_CW();
    }
    last_interrupt_time = interrupt_time;
  }
  MSG.set_limitE_sensor2_first_off();
}

void interrupt_BTN_initial() {
  attachInterrupt(digitalPinToInterrupt(pin_BTN_ER_stop), BTN_ER_stop_ISR , CHANGE);
  attachInterrupt(digitalPinToInterrupt(pin_BTN_CLPSM_freeze), BTN_CLPSM_freeze_ISR, CHANGE);
}

void BTN_ER_stop_ISR() {
   if(digitalRead(pin_BTN_ER_stop) == 0){
        //press button
        MSG.servo_standby(servo_standby_l, servo_standby_r);
        timer_stop_counter_zero();        
        MSG.set_BTN_ER_stop_state(0);
     } else {
        CLPSM_stop = 0;
        MSG.set_BTN_ER_stop_state(1);
    }
}

void BTN_CLPSM_freeze_ISR() {     
   if(digitalRead(pin_BTN_CLPSM_freeze) == 0){ 
      //press button
           timer_stop_counter_zero();
           MSG.set_BTN_CLPSM_freeze_state(0);
    } else {      
           MSG.set_BTN_CLPSM_freeze_state(1); 
    }  
}

void  CLPSM_initial(){
  CLPSM_controller ->CLP_MOTOR_Initial(pin_CLPSM_pulse, pin_CLPSM_DIR);
  CLPSM_controller->setCLPMTR_LOW();
  CLPSM_controller->setCLPMTR_CW();
}

void timer_stop_counter_zero() { 
    CLPSM_stop = 1;
    tmr4.counter_clear();
}

void limitM_DIR_chcek() {        
      if(!MSG.get_limitM1()){        
          //Serial.println("38");
          CLPSM_controller->setCLPMTR_CW();
      } else if(!MSG.get_limitM2()){
          //Serial.println("39");
          CLPSM_controller->setCLPMTR_CCW();
      }
}



