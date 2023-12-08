#ifndef __ROBOT_MOTORS__
#define __ROBOT_MOTORS__
#include <Arduino.h>
#include "drv8711.h"
#include "driver/mcpwm.h"
#include "soc/mcpwm_periph.h"


//Motor A drive pins
#define AOUT1 26
#define AOUT2 25

//Motor B drive pins
#define BOUT1 33
#define BOUT2 32

#define PWM_FREQ 10000

extern DRV8711 drv8711Driver;

enum MOTOR {
    LEFT_MOTOR = 0,
    RIGHT_MOTOR = 1
};

enum BRAKE_MODE {
    NEUTRAL = 0,
    AUTO_BRAKE = 1
};

extern float currentLimit;

class Motors {
  private:
    float validateSpeed(float speed);

    void setMotorForwardSpeed(MOTOR leftOrRightMotor, float speed);

    void setMotorBackwardSpeed(MOTOR leftOrRightMotor, float speed);

    float validateCurrent(float current);

  public:
    Motors();

    void init();

    void setMotorSpeed(MOTOR leftOrRightMotor, float speed);

    void setMotorBrakeMode(BRAKE_MODE brakeMode);

    void setCurrentLimit(float current);

    void checkFaults();

};

#endif