//====================================
//timer 定義區
//====================================
int   Timer4Counter = 0;
boolean TimerSW = 0;      //pulse high low change
 int Timer4CountSet[10];
#define CLPMTRSpeed 9
//==============================
#define pin_LED 13

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(pin_LED, OUTPUT);
  Timer4_initial();
}

void loop() {
  // put your main code here, to run repeatedly:

}


  //====================================
  //timer4 initial
  //====================================
  void Timer4_initial() {
  Timer4Counter = 0;
  Timer4CountSet[0] = 65531; //0.02ms 20us中斷設定
  Timer4CountSet[1] = 65524; //0.048ms 48us中斷設定
  Timer4CountSet[2] = 65511; //0.1ms 100us中斷設定
  Timer4CountSet[3] = 65473; //0.25ms 250us中斷設定
  Timer4CountSet[4] = 65411; //0.5ms 500us中斷設定
  Timer4CountSet[5] = 65286; //1ms 1000us中斷設定
  Timer4CountSet[6] = 65036; //2ms 2000us中斷設定
  Timer4CountSet[7] = 63036; //10ms 中斷設定
  Timer4CountSet[8] = 53036; //50ms 中斷設定
  Timer4CountSet[9] = 40536; //100ms 中斷設定
  TimerSW = true;
  TCCR4A =  0x00;
  TCCR4B =  0X03;          //設定 除頻=3 16Mhz/64=0.25Mhz
  //1/0.25Mhz=4us,每4us計數一次,假設設定為65531,
  //共65536-65531=5,第5次發生計時中斷,共經時間5*4us=20us
  TCCR4C =  0x00;
  //TCNT4 = Timer4CountSet[CLPMTRSpeed];
  TCNT4 = 40536;
  TimerStart();  
  }
  //==============================


  //====================================
  //timer4 interrput ISR
  //create Pulse for CLPMOTOR driver
  //====================================
  ISR (TIMER4_OVF_vect) {
  //TIMSK4 = 0x00;     //timer4 stop
   //TCNT4 = Timer4CountSet[CLPMTRSpeed];     //CLPMTRSpeed update in LCD_Callback function
   TCNT4 = 40536;
   Timer4Counter++;
      if (Timer4Counter == 10) {        
          //Serial.println("timer4");
          Timer4Counter = 0;
          TimerSW = !TimerSW;
          digitalWrite(pin_LED, TimerSW);
      } 
   //TIMSK4 = 0x01;       //timer4 start
  }
  //=============================

  //====================================
  //timer start
  //====================================
  void TimerStart() {
  TIMSK4 = 0x01;       //timer4 start
  }
  //=============================

  //====================================
  //timer stop
  //====================================
  void TimerStop() {
  TIMSK4 = 0x00;     //timer4 stop
  }
  //=============================
