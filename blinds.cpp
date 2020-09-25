#include "blinds.h"


void Motor::rotate_right()
{
    digitalWrite(MOTOR_INPUT_1,1);
    digitalWrite(MOTOR_INPUT_2,0);
}

void Motor::rotate_left()
{
    digitalWrite(MOTOR_INPUT_1,0);
    digitalWrite(MOTOR_INPUT_2,1);
}

void Motor::stop()
{
    digitalWrite(MOTOR_INPUT_2,0);
    digitalWrite(MOTOR_INPUT_1,0);
}


void PowerSupply:: on()
{
    digitalWrite(POWER_SUPPLY_PIN,0);
}

void PowerSupply:: off()
{
    digitalWrite(POWER_SUPPLY_PIN,1);
}

  Blinds::Blinds()
  {
    pinMode(MOTOR_INPUT_1,OUTPUT);
    pinMode(MOTOR_INPUT_2,OUTPUT);
    pinMode(POWER_SUPPLY_PIN,OUTPUT);
    pinMode(INPUT_BUTTON_DOWN,INPUT);
    pinMode(INPUT_BUTTON_UP,INPUT);
    pinMode(EDGE_SWITCH_UP,INPUT);
    motor.stop();
    power.off();
  }


  void Blinds:: close(bool manual_close)
  {
    if(!digitalRead(EDGE_SWITCH_DOWN) && ((millis()-last_trial_time>=50000)||(manual_close)))
    {
      last_trial_time = millis();
      power.on();
      delay(100);
      int i=0;
      while((!digitalRead(EDGE_SWITCH_DOWN))&& (i<11000)&& (!digitalRead(INPUT_BUTTON_DOWN)) &&(!digitalRead(INPUT_BUTTON_UP) ))
      { 
        i++; //timeout 11000ms
        motor.rotate_left();
        delay(1);
      }
      motor.stop();
      motor.rotate_right(); // loosing rope
      delay(400);
      motor.stop();
      power.off();

      while((digitalRead(INPUT_BUTTON_DOWN)) ||(digitalRead(INPUT_BUTTON_UP)))
      {
      //do nothing; wait for the user to release the button(s) after emergency stop
      }
    }

   
  }

    void Blinds:: open(bool manual_open)
  {
    if(!digitalRead(EDGE_SWITCH_UP) && ((millis()-last_trial_time>=50000)||(manual_open)))
    {
      last_trial_time = millis();
      power.on();
      delay(100);
      int i=0;
      while((!digitalRead(EDGE_SWITCH_UP))&& (i<16000) && (!digitalRead(INPUT_BUTTON_DOWN)) &&(!digitalRead(INPUT_BUTTON_UP) ))
      { 
        i++; //timeout 11000ms
        motor.rotate_right();
        delay(1);
      }
      motor.stop();
      motor.rotate_left(); // loosing rope
      delay(400);
      motor.stop();
      power.off();

      while((digitalRead(INPUT_BUTTON_DOWN)) ||(digitalRead(INPUT_BUTTON_UP)))
      {
      //do nothing; wait for the user to release the button(s) after emergency stop
      }
    }

  }



  void Blinds:: manage()
  {
  timeClient->update();
  current_time=timeClient->getFormattedTime();
  current_time = current_time.substring(0,5); //cutting out only hh:mm 

  if(open_hour!=close_hour) //if openhour = closehour => don't close/open
  {
    if((current_time==open_hour) && (open_hour!="00:00"))
      this->open();
    if((current_time==close_hour) && (close_hour!="00:00"))
      this->close();
  }

  if((digitalRead(INPUT_BUTTON_DOWN)) && (!digitalRead(EDGE_SWITCH_DOWN)))
  {
    power.on();
    delay(100);
    int milliseconds=0;
    while((digitalRead(INPUT_BUTTON_DOWN))&& (!digitalRead(EDGE_SWITCH_DOWN)))
    {
      motor.rotate_left();
           if(digitalRead(INPUT_BUTTON_UP)) //full manual opening with no need to hold button
        {
          milliseconds=0; // prevent double rope loosing
          while(digitalRead(INPUT_BUTTON_UP))
          {
            delay(100);
          } // wait to release the button 
          delay(100);
          this->close(1);
        }
      milliseconds++;
      delay(1);
    }
    motor.stop();
    if(milliseconds>=1000)
    {
      motor.rotate_right(); // loosing rope
      delay(400);
      motor.stop();
    }
    power.off();
    //closed=1; // at least a bit closed
  }

  if((digitalRead(INPUT_BUTTON_UP))&& (!digitalRead(EDGE_SWITCH_UP)))
  {
    power.on();
    delay(100);
    int milliseconds=0;
    while((digitalRead(INPUT_BUTTON_UP))&& (!digitalRead(EDGE_SWITCH_UP)))
    {
      motor.rotate_right();
      if(digitalRead(INPUT_BUTTON_DOWN)) //full manual opening with no need to hold button
        {
          milliseconds=0; // prevent double rope loosing
          while(digitalRead(INPUT_BUTTON_DOWN))
          {
            delay(100);
          } // wait to release the button 
          delay(100);
          this->open(1);
        }
      milliseconds++;
      delay(1);
    }
    motor.stop();
    if(milliseconds>=1000)
     { 
      motor.rotate_left(); // loosing rope
      delay(400);
      motor.stop();
     }
    power.off();
  }
  }
  


