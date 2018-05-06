//hardware:
//MEGA 2560
//CLP MOTOR AND DRIVER
//note1.CLPSM closed-loop step motor
//ENA = disable will lost torque (this project doesn't control ENA)
//driver(Hybird Servo Drive) Pulse need to set 1600

#include <CLP_MOTOR.h>
#include <mega2560_timer4.h>
mega2560_timer4 tmr4;
//--------------------------------------------
//limit sensor
#define pin_limit_sensor1  18     //normal = 0 
#define pin_limit_sensor2  19     //normal = 0
bool limit_sensor1_on = 0;
bool limit_sensor2_on = 0;
//--------------------------------------------

//--------------------------------------------
//button
#define pin_BTN_ER_stop  2
#define pin_BTN_CLPSM_freeze  3
//--------------------------------------------

//--------------------------------------------
//timer4 
#define timer4_set t4_s4
bool Timer4_SW = 0;
//--------------------------------------------

//--------------------------------------------
//CLPSM closed-loop step motor
CLPMTR *CLPSM_controller= new CLPMTR;
#define pin_CLPSM_pulse 7   //control TIP41C, B
#define pin_CLPSM_DIR 6     //control TIP41C, B
//--------------------------------------------

void timer4_ISR(void) {
  tmr4.set_TCNT4(timer4_set);
  tmr4.counter_add();
  //set 0.5ms counter
  //0.5ms *2 = 1ms
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

void setup() {
  Serial.begin(9600);
  timer4_initial();
  interrupt_limit_sensor_initial();
  interrupt_BTN_initial();
  CLPSM_initial();
  Serial.println("start");
}

void loop() {
}

void timer4_initial() {
  tmr4.t4_initial(timer4_set, timer4_ISR);
  if(digitalRead(pin_BTN_ER_stop) == 1 && digitalRead(pin_BTN_CLPSM_freeze) == 1){
    tmr4.start();
  } else {
    tmr4.stop();     
  }
}

void interrupt_limit_sensor_initial() {
  digitalWrite(pin_limit_sensor1, HIGH); //turn on pullup resistor
  digitalWrite(pin_limit_sensor2, HIGH);  //turn on pullup resistor
  attachInterrupt(digitalPinToInterrupt(pin_limit_sensor1), limit_sensor1_ISR, RISING);
  attachInterrupt(digitalPinToInterrupt(pin_limit_sensor2), limit_sensor2_ISR, RISING);
}

void limit_sensor1_ISR() {
  if (!limit_sensor1_on) {
    static unsigned long last_interrupt_time = 0;
    unsigned long interrupt_time = millis();
    if (interrupt_time - last_interrupt_time > 50) {
      CLPSM_controller->setCLPMTR_CW();
    }
    last_interrupt_time = interrupt_time;
  }
  limit_sensor1_on = false;
}

void limit_sensor2_ISR() {
  if (!limit_sensor2_on) {
    static unsigned long last_interrupt_time = 0;
    unsigned long interrupt_time = millis();
    if (interrupt_time - last_interrupt_time > 50) {
      CLPSM_controller->setCLPMTR_CCW();
    }
    last_interrupt_time = interrupt_time;
  }
  limit_sensor2_on = false;
}

void interrupt_BTN_initial() {
  digitalWrite(pin_BTN_ER_stop, HIGH); //turn on pullup resistor
  digitalWrite(pin_BTN_CLPSM_freeze, HIGH);  //turn on pullup resistor  
  pinMode(pin_BTN_ER_stop, INPUT);
  pinMode(pin_BTN_CLPSM_freeze, INPUT);
  attachInterrupt(digitalPinToInterrupt(pin_BTN_ER_stop), BTN_ER_stop_ISR , CHANGE);
  attachInterrupt(digitalPinToInterrupt(pin_BTN_CLPSM_freeze), BTN_CLPSM_freeze_ISR, CHANGE);
}

void BTN_ER_stop_ISR() {
   if(digitalRead(pin_BTN_ER_stop) == 0){
        timer_stop_counter_zero();
     } else {
        tmr4.start();
    }
}

void BTN_CLPSM_freeze_ISR() {
   if(digitalRead(pin_BTN_CLPSM_freeze) == 0){ 
           timer_stop_counter_zero();
    } else {      
            tmr4.start();
    }  
}

void  CLPSM_initial(){
  CLPSM_controller ->CLP_MOTOR_Initial(pin_CLPSM_pulse, pin_CLPSM_DIR);
  CLPSM_controller->setCLPMTR_LOW();
  CLPSM_controller->setCLPMTR_CW();
}

void timer_stop_counter_zero() {
    tmr4.stop();
    tmr4.counter_clear();
}


