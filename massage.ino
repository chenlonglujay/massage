//hardware:
//MEGA 2560
//CLP MOTOR AND DRIVER
//note1.CLPSM closed-loop step motor
//ENA = disable will lost torque (this project doesn't control ENA)
//driver(Hybird Servo Drive) Pulse need to set 1600

#include <CLP_MOTOR.h>
#include <mega2560_timer4.h>
mega2560_timer4 tmr4;
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
bool BTN_ER_stop_state = 1; 
bool BTN_CLPSM_freeze_state = 1; 
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
bool preparingLED = 1; 
#define preparing_LED_blink_time 3000  //t4_s2 0.1ms,0.1ms 0.1ms*3000 = 300ms
bool preparingLED_state = 0;
int preparingLED_counter = 0;
#define onehour_LED_time1 10000 //0.1ms *10000 = 1sec
#define onehour_LED_time2 3600  //1sec *3600 = 1hour
int onehour_counter1 = 0;
int onehour_counter2 = 0;
bool onehour_stopAll = 0;
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

  //one hour check
  onehour_counter1++;
  if(onehour_counter1 == onehour_LED_time1) {   
    //1 sec
    onehour_counter1 = 0;
    onehour_counter2++;
    if(onehour_counter2 == onehour_LED_time2) {
       //1 hour
       onehour_counter2 = 0;
       onehour_stopAll = 1;
    }
  }
  
  if(preparingLED) {
     preparingLED_counter++;
      if(preparingLED_counter == preparing_LED_blink_time) {
        preparingLED_state = !preparingLED_state;
          digitalWrite(pin_blue_LED, preparingLED_state);
          preparingLED_counter = 0;
      }      
  }  
}

void setup() {
  Serial.begin(9600);
  read_servo_angle_initial();
  servo_initial();  
  delay(100);  
  timer4_initial();
  tmr4.start();
  LED_initial();
  interrupt_limit_sensor_initial();
  interrupt_BTN_initial(); 
  limitM_sensor_initial();
  delay(5000);  
  CLPSM_initial();
  preparingLED = 0;   
  digitalWrite(pin_blue_LED, 1); //LED OFF
  Serial.println("start");
}

void loop() {
    uint8_t read_angle = read_servo_angle();     
    if(onehour_stopAll) {
        tmr4.stop();   
        CLPSM_start_stop(sm_stop);  
        digitalWrite(pin_yellow_LED, 0); //LED ON  
        return;
    }
    
    limitM_DIR_chcek();
    if(BTN_ER_stop_state) {     
        //no press BTN_ER_stop
          CLPSM_start_stop(sm_stop);  
          delay(500);  
          servo_move(servo_zero+read_angle , servo_zero-read_angle);     //l,r        
          delay(1200);  
          servo_standby(servo_standby_l, servo_standby_r);
          delay(1200);
          if(BTN_CLPSM_freeze_state) {
            //no BTN_CLPSM_freeze              
              CLPSM_start_stop(sm_start); 
               limitM_DIR_chcek();
              delay(500);  
          }
    }
    
}

void timer4_initial() {
  tmr4.t4_initial(timer4_set, timer4_ISR);
}

void LED_initial() {
  pinMode(pin_yellow_LED, OUTPUT);
  pinMode(pin_blue_LED, OUTPUT);
  digitalWrite(pin_yellow_LED, 1); //LED OFF
  digitalWrite(pin_blue_LED, 1); //LED OFF
}

void CLPSM_start_stop(bool CLPSM_move) {
    if(CLPSM_move) {  
        if(digitalRead(pin_BTN_ER_stop) == 1 && digitalRead(pin_BTN_CLPSM_freeze) == 1){
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

 void limitM_sensor_initial() {
    pinMode(pin_limitM_sensor1, INPUT);
    pinMode(pin_limitM_sensor2, INPUT);
    digitalWrite(pin_limitM_sensor1, HIGH);  //turn on pullup resistor
    digitalWrite(pin_limitM_sensor2, HIGH);  //turn on pullup resistor
 }

void limit_sensor1_ISR() {
  if (!limitE_sensor1_on) {
    static unsigned long last_interrupt_time = 0;
    unsigned long interrupt_time = millis();
    if (interrupt_time - last_interrupt_time > 50) {
      CLPSM_controller->setCLPMTR_CCW();
      Serial.println("s1");
    }
    last_interrupt_time = interrupt_time;
  }
  limitE_sensor1_on = false;
}

void limit_sensor2_ISR() {
  if (!limitE_sensor2_on) {
    static unsigned long last_interrupt_time = 0;
    unsigned long interrupt_time = millis();
    if (interrupt_time - last_interrupt_time > 50) {
      CLPSM_controller->setCLPMTR_CW();
            Serial.println("s2");
    }
    last_interrupt_time = interrupt_time;
  }
  limitE_sensor2_on = false;
}

void interrupt_BTN_initial() {
  digitalWrite(pin_BTN_ER_stop, HIGH); //turn on pullup resistor
  digitalWrite(pin_BTN_CLPSM_freeze, HIGH);  //turn on pullup resistor  
  pinMode(pin_BTN_ER_stop, INPUT);
  pinMode(pin_BTN_CLPSM_freeze, INPUT);
  if(!digitalRead(pin_BTN_ER_stop)) {
        BTN_ER_stop_state = 0; 
  }
  attachInterrupt(digitalPinToInterrupt(pin_BTN_ER_stop), BTN_ER_stop_ISR , CHANGE);
  attachInterrupt(digitalPinToInterrupt(pin_BTN_CLPSM_freeze), BTN_CLPSM_freeze_ISR, CHANGE);
}

void BTN_ER_stop_ISR() {
   if(digitalRead(pin_BTN_ER_stop) == 0){
        servo_standby(servo_standby_l, servo_standby_r);
        timer_stop_counter_zero();        
        BTN_ER_stop_state = 0;
     } else {
        //tmr4.start();
        CLPSM_stop = 0;
        BTN_ER_stop_state = 1;
    }
}

void BTN_CLPSM_freeze_ISR() {
   if(digitalRead(pin_BTN_CLPSM_freeze) == 0){ 
           timer_stop_counter_zero();
           BTN_CLPSM_freeze_state = 0;
    } else {      
           BTN_CLPSM_freeze_state  =1;  
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

void read_servo_angle_initial() {
  pinMode(pin_servo_angle, INPUT);  
}

uint8_t read_servo_angle() {
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

void servo_initial() {
  pinMode(pin_servo1, OUTPUT);
  pinMode(pin_servo2, OUTPUT);
  pinMode(pin_servo3, OUTPUT);
  pinMode(pin_servo4, OUTPUT);
#if init_to_zero
  analogWrite(pin_servo1, servo_zero); //ZERO
  analogWrite(pin_servo2, servo_zero);
  analogWrite(pin_servo3, servo_zero);
  analogWrite(pin_servo4, servo_zero);
#endif
  servo_standby(servo_standby_l, servo_standby_r);
}

void servo_standby(uint8_t l, uint8_t r) {
      analogWrite(pin_servo1, r);
      analogWrite(pin_servo2, l);
      analogWrite(pin_servo3, r);
      analogWrite(pin_servo4, l);   
}


void servo_move(uint8_t goal_l, uint8_t goal_r){
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

void limitM_DIR_chcek() {
      if(!digitalRead(pin_limitM_sensor1)){        
          //Serial.println("38");
          CLPSM_controller->setCLPMTR_CW();
      } else if(!digitalRead(pin_limitM_sensor2)){
          //Serial.println("39");
          CLPSM_controller->setCLPMTR_CCW();
      }
}



