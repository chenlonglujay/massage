#include <mega2560_timer4.h>
#define pin_LED 13
mega2560_timer4 tmr4;
bool state_LED = 0;

void timer4_ISR(void) {
  tmr4.set_TCNT4(t4_s9);
  tmr4.counter_add();
  //set 100ms counter
  //1sec: 100ms *10 = 1sec
  if (tmr4.get_counter()== 10) {
    tmr4.counter_clear();
    state_LED = !state_LED;
    digitalWrite(pin_LED, state_LED);
  }
}

void setup() {
  Serial.begin(9600);
  tmr4.t4_initial(t4_s9, timer4_ISR);
  tmr4.start();
  pinMode(pin_LED, OUTPUT);
  Serial.println("start");
}

void loop() {
  // put your main code here, to run repeatedly:
}




