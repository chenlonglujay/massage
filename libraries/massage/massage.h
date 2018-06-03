//------------------------
//massage project
//hardware:Arduino Mega2560
//limit sensor
//M------------E---------E---------------M
//------------------------
#ifndef MASSAGE_h
#define MASSAGE_h


#define default_pin_limitE_sensor1 18
#define default_pin_limitE_sensor2 19
#define default_pin_limitM_sensor1 38
#define default_pin_limitM_sensor2 39
#define default_pin_BTN_ER_stop 2
#define default_pin_BTN_CLPSM_freeze 3
#define default_preparing_LED_blink_time 3000  //t4_s2 0.1ms,0.1ms 0.1ms*3000 = 300ms
#define default_onehour_LED_time1 10000 //0.1ms *10000 = 1sec
#define default_onehour_LED_time2 3600  //1sec *3600 = 1hour
#define default_pin_servo_angle A0
#define default_servo_zero  180
#define default_servo_Min  110
#define default_servo_Max  250
#define default_servo_standby_r 210
#define default_servo_standby_l 150
#define default_pin_servo1 9
#define default_pin_servo2 10
#define default_pin_servo3 11
#define default_pin_servo4 12 
class massage
{
private:
    int m_preparing_LED_blink_time;
    int m_onehour_LED_time1;
    int m_onehour_LED_time2;
    //limit sensor E
    //if motor touchs  limit sensor E that will change direction
    uint8_t pin_limitE_sensor1;
    uint8_t pin_limitE_sensor2;
    bool limitE_sensor1_first;     //power on system will go into interrupt once,this flag can avoid  power on will execute interrupt function
    bool limitE_sensor2_first;
    //limit sensor M
    //if limt sensorE broken ,system will use limit sensorM to detect motor arrives limit side. 
    uint8_t pin_limitM_sensor1;
    uint8_t pin_limitM_sensor2;
    
    //button
    uint8_t  pin_BTN_ER_stop;
    uint8_t  pin_BTN_CLPSM_freeze;
    bool BTN_ER_stop_state; 
    bool BTN_CLPSM_freeze_state;

    //preparing and 1 hour detect,use LED expressed
    uint8_t pin_yellow_LED; //remind alreay 1 hour
    uint8_t pin_blue_LED; //preparing
    bool preparingLED; 
    bool preparingLED_state;
    int preparingLED_counter;
    int onehour_counter1;
    int onehour_counter2;
    bool onehour_stopAll;

    void LED_initial();
    void limitM_sensor_initial();
    void BTN_initial(); 
    //servo 
    uint8_t pin_servo_angle;
    uint8_t servo_zero;
    uint8_t servo_Min;
    uint8_t servo_Max;
    uint8_t servo_standby_r;
    uint8_t servo_standby_l;
    //pair:4(l)-3(r) 2(l)-1(r)
    uint8_t pin_servo1;
    uint8_t pin_servo2;
    uint8_t pin_servo3;
    uint8_t pin_servo4;

    void read_servo_angle_initial();
    void servo_initial(bool init_to_zero);

public:
	massage();
    ~massage();
    
    void massage_initial(uint8_t lmtE1, uint8_t lmtE2, uint8_t lmtM1, uint8_t lmtM2, 
                        uint8_t ER_stop, uint8_t CLPSM_freeze, uint8_t yellowLED, uint8_t blueLED,
                        int perparing_LED_blink, int onehour_LED_t1, int onehour_LED_t2);

    void massage_initial_servo(uint8_t svo_angle, uint8_t svo_zero, 
                                uint8_t Max, uint8_t Min, uint8_t standby_r, uint8_t standby_l,
                                uint8_t s1, uint8_t s2, uint8_t s3, uint8_t s4, bool init_to_zero);
    
    void one_hour_check();
    bool get_limitM1();
    bool get_limitM2();
    void servo_standby(uint8_t l, uint8_t r);
    uint8_t read_servo_angle();
    void set_BTN_ER_stop_state(bool state);
    bool get_BTN_ER_stop_state();
    void servo_move(uint8_t goal_l, uint8_t goal_r);
    bool get_onehour_stopAll();
    void set_BTN_CLPSM_freeze_state(bool state);
    bool get_BTN_CLPSM_freeze_state();
    bool get_limitE_sensor1_first();
    bool get_limitE_sensor2_first();
    void set_limitE_sensor1_first_off();
    void set_limitE_sensor2_first_off();
    void set_blue_LED_off();
};

#endif
	

