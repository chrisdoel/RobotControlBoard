#include <Arduino.h>
#include "robot_motors.h"

DRV8711 drv8711Driver;

float currentLimit = 10.0f;

Motors::Motors(){
}

void Motors::init(){
  drv8711Driver.init();
  drv8711Driver.writeRegister(STATUS_REG_ADDR, 0);
  drv8711Driver.configureDefaultBrushedMotorProfile();

  mcpwm_config_t pwm_config;
  pwm_config.frequency = PWM_FREQ;
  pwm_config.cmpr_a = 0;    //duty cycle of PWMxA = 0
  pwm_config.cmpr_b = 0;    //duty cycle of PWMxb = 0
  pwm_config.counter_mode = MCPWM_UP_COUNTER;
  pwm_config.duty_mode = MCPWM_DUTY_MODE_0;
  mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config);    //Configure PWM0A & PWM0B with above settings
  mcpwm_init(MCPWM_UNIT_1, MCPWM_TIMER_1, &pwm_config);    //Configure PWM1A & PWM1B with above settings


  //Initialise motor control PWM for the right motor
  mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, AOUT1);
  mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0B, AOUT2);

  //Initialise motor control PWM for the left motor
  mcpwm_gpio_init(MCPWM_UNIT_1, MCPWM1A, BOUT1);
  mcpwm_gpio_init(MCPWM_UNIT_1, MCPWM1B, BOUT2);
}


void Motors::setMotorSpeed(MOTOR leftOrRightMotor, float speed){
  if(speed >= 0.0f){
    setMotorForwardSpeed(leftOrRightMotor, speed);
  }
  else{
    setMotorBackwardSpeed(leftOrRightMotor, speed);
  }
}

float Motors::validateSpeed(float speed){
  if(speed > 100.0f){
    Serial.println("Warning: Motor speed set above 100%");
    speed = 100.0f;
  }
  else if(speed < -100.0f){
    Serial.println("Warning: Motor speed set below -100%");
    speed = -100.0f;
  }
  return speed;
}

void Motors::setMotorForwardSpeed(MOTOR leftOrRightMotor, float speed){
  speed = validateSpeed(speed);

  if(leftOrRightMotor == RIGHT_MOTOR){
    // Serial.printf("Setting right motor pwm to %f\n", speed);

    mcpwm_set_signal_low(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B);
    mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, speed);
    mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, MCPWM_DUTY_MODE_0);
  }
  else{
    // Serial.printf("Setting left motor pwm to %f\n", speed);

    mcpwm_set_signal_low(MCPWM_UNIT_1, MCPWM_TIMER_1, MCPWM_OPR_B);
    mcpwm_set_duty(MCPWM_UNIT_1, MCPWM_TIMER_1, MCPWM_OPR_A, speed);
    mcpwm_set_duty_type(MCPWM_UNIT_1, MCPWM_TIMER_1, MCPWM_OPR_A, MCPWM_DUTY_MODE_0);
  }
}

void Motors::setMotorBackwardSpeed(MOTOR leftOrRightMotor, float speed){
  speed = validateSpeed(speed);

  //Convert the negative speed to a positive speed
  speed = -speed;
  if(leftOrRightMotor == RIGHT_MOTOR){
    // Serial.printf("Setting right motor pwm to reverse %f\n", speed);

    mcpwm_set_signal_low(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A);
    mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B, speed);
    mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B, MCPWM_DUTY_MODE_0);
  }
  else{
    // Serial.printf("Setting left motor pwm to reverse %f\n", speed);

    mcpwm_set_signal_low(MCPWM_UNIT_1, MCPWM_TIMER_1, MCPWM_OPR_A);
    mcpwm_set_duty(MCPWM_UNIT_1, MCPWM_TIMER_1, MCPWM_OPR_B, speed);
    mcpwm_set_duty_type(MCPWM_UNIT_1, MCPWM_TIMER_1, MCPWM_OPR_B, MCPWM_DUTY_MODE_0);
  }
}

void Motors::setMotorBrakeMode(BRAKE_MODE brakeMode){
  if(brakeMode == AUTO_BRAKE){
    drv8711Driver.setDecayMode(FORCE_FAST_DECAY);
  }
  else if(brakeMode == NEUTRAL){
    drv8711Driver.setDecayMode(FORCE_SLOW_DECAY);
  }
}

float Motors::validateCurrent(float current){
  if(current > 20.0f){
    Serial.println("Warning: Current limit >20 A provided, defaulting to 20 A");
    current = 20.0f;
  }
  else if(current < 0.0f){
    Serial.println("Warning: Negative current limit provided, defaulting to 10 A");
    current = 0;
  }
  return current;
}

void Motors::setCurrentLimit(float current){
  currentLimit = current;
  current = validateCurrent(current);
  drv8711Driver.setCurrentLimit((uint8_t) current);
}

void Motors::checkFaults(){
  //This is a hacky way of detecting whether we can talk to the drv8711 or not
  //If there is no SPI connection then the stall register will be all 1s
  //We do not use this register so we can check if its all 1s or not
  if(drv8711Driver.readRegister(STALL_REG_ADDR) == 0xFFF){
    Serial.println("Error: lost communication with motor driver");
    Serial.println("Attempting to reconnect");
    drv8711Driver.writeRegister(STATUS_REG_ADDR, 0);
    drv8711Driver.configureDefaultBrushedMotorProfile();

    setCurrentLimit(currentLimit);
    delay(1000);
  }
  else if(drv8711Driver.readRegister(STATUS_REG_ADDR) != 0){
    Serial.println("Error: Motor Driver Fault detected");
    drv8711Driver.printStatus();
    Serial.println("Attempting to reset faults...");
    drv8711Driver.writeRegister(STATUS_REG_ADDR, 0);
    delay(1000);
  }
}