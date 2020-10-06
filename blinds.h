#ifndef _blinds_h
#define _blinds_h

#define MOTOR_INPUT_1 18
#define MOTOR_INPUT_2 19
#define POWER_SUPPLY_PIN 4
#define INPUT_BUTTON_DOWN 22
#define INPUT_BUTTON_UP 23
#define EDGE_SWITCH_UP 13
#define EDGE_SWITCH_DOWN 14


class Motor
{
    public:
        void rotate_right();
        void rotate_left();
        void stop();
};

class PowerSupply
{
    public:
    void on();
    void off();
};

class Blinds
{
    public:
   
    Motor motor;
    PowerSupply power;
    bool closed=0;
    String open_hour = "08:30";
    String close_hour = "00:00";
    String current_time;
    unsigned long last_trial_time = 0; // storing last moment of open attempt

    void close(bool manual_close=0);
    void open(bool manual_open=0);
    Blinds();
    void manage();

};

#endif
