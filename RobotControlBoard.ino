#include <drv8711.h>
#include <robot_motors.h>

#include <Bluepad32.h>
#include <cstring>

ControllerPtr myController;

Motors robotMotors;

const bool ALLOW_ANY_CONTROLLER_TO_CONNECT = 1;

// uint8_t whitelistedControllerBTAddress[6] = {164, 192, 225, 58, 48, 63};
uint8_t whitelistedControllerBTAddress[6] = {0, 0, 0, 0, 0, 0};


void onConnectedController(ControllerPtr ctl) {

  //If there is not a controller already connected then we try to connect this new controller
  if (myController == nullptr) {
    ControllerProperties properties = ctl->getProperties();
    Serial.printf("Controller Bluetooth Address: %i %i %i %i %i %i\nAttempting to connect controller...\n", properties.btaddr[0], properties.btaddr[1], properties.btaddr[2], properties.btaddr[3], properties.btaddr[4], properties.btaddr[5]);

    //Check whether we allow any controllers to connect
    if(ALLOW_ANY_CONTROLLER_TO_CONNECT){
      Serial.printf("Controller connected\nWARNING: Any controller can connect to this device, set ALLOW_ANY_CONTROLLER_TO_CONNECT to 0 and add a whitelisted controller bluetooth address to ensure only your controller can connect\n");
      myController = ctl;
    }
    else{
      //Check if this new controller's bluetooth address has been whitelisted
      if(std::memcmp(whitelistedControllerBTAddress, properties.btaddr, 6) == 0){
        Serial.printf("Whitelisted controller connected\n");
        myController = ctl;
        
        //We can set the controller LED colour if this is supported (e.g. PS4, PS5)
        myController->setColorLED(/*Red*/255, /*Green*/0, /*Blue*/0);

        //We can set the controller players LEDs if this is supported (e.g. Wii controller)
        myController->setPlayerLEDs(1);
        
        //255 duration is about 2 seconds
        ctl->setRumble(/*force*/128, /*duration*/128);
      }
      //If it has not been whitelisted, disconnect it
      else{
        Serial.printf("This controller has not been whitelisted, disconnecting...\n");
        Serial.printf("To whitelist this controller, add the following bluetooth address to the whitelistedControllerBTAddress variable: %i %i %i %i %i %i\n", properties.btaddr[0], properties.btaddr[1], properties.btaddr[2], properties.btaddr[3], properties.btaddr[4], properties.btaddr[5]);
        ctl->disconnect();
      }
    }
  }
  else{
    Serial.printf("CALLBACK: Controller could not be added as there is already one connected\n");
  }
}

void onDisconnectedController(ControllerPtr ctl) {
  //Disconnect the controller
  if (myController == ctl) {
      Serial.printf("Controller disconnected\n");
      myController = nullptr;
  }
  else{
    Serial.printf("WARNING: Could not disconnect controller\n");
  }
}

void dumpGamepad(ControllerPtr ctl) {
    Serial.printf(
        "idx=%d, dpad: 0x%02x, buttons: 0x%04x, axis L: %4d, %4d, axis R: %4d, %4d, brake: %4d, throttle: %4d, "
        "misc: 0x%02x, gyro x:%6d y:%6d z:%6d, accel x:%6d y:%6d z:%6d\n",
        ctl->index(),        // Controller Index      
        ctl->dpad(),         // DPAD
        ctl->buttons(),      // bitmask of pressed buttons
        ctl->axisX(),        // (-511 - 512) left X Axis
        ctl->axisY(),        // (-511 - 512) left Y axis
        ctl->axisRX(),       // (-511 - 512) right X axis
        ctl->axisRY(),       // (-511 - 512) right Y axis
        ctl->brake(),        // (0 - 1023): brake button
        ctl->throttle(),     // (0 - 1023): throttle (AKA gas) button
        ctl->miscButtons(),  // bitmak of pressed "misc" buttons
        ctl->gyroX(),        // Gyro X
        ctl->gyroY(),        // Gyro Y
        ctl->gyroZ(),        // Gyro Z
        ctl->accelX(),       // Accelerometer X
        ctl->accelY(),       // Accelerometer Y
        ctl->accelZ()        // Accelerometer Z
    );
}

void processGamepad(ControllerPtr ctl) {
    float rightScaler = 1.0;
    float leftScaler = 1.0;
    int turnAmount = ctl->axisX();
    int throttle = ctl->axisY();

    throttle = map(throttle, /*Min axis value*/-400, /*Max axis value*/420, 100, -100);

    turnAmount = map(turnAmount, /*Min axis value*/-400, /*Max axis value*/420, -100, 100);

    if(throttle > 50.0 or throttle < -50.0){
      if(turnAmount < 0){
        turnAmount = -turnAmount;
        leftScaler = (100.0 - float(turnAmount))/100.0;
      }
      else{
        rightScaler = (100.0 - float(turnAmount))/100.0;
      }
      Serial.printf("left %f right %f\n", rightScaler, leftScaler);
      robotMotors.setMotorSpeed(LEFT_MOTOR, -throttle*leftScaler);
      robotMotors.setMotorSpeed(RIGHT_MOTOR, -throttle*rightScaler);
    }
    else{
      robotMotors.setMotorSpeed(LEFT_MOTOR, -turnAmount);
      robotMotors.setMotorSpeed(RIGHT_MOTOR, turnAmount);
    }
    // Another way to query controller data is by getting the buttons() function.
    // See how the different "dump*" functions dump the Controller info.
    dumpGamepad(ctl);
}

// Arduino setup function. Runs in CPU 1
void setup() {
    Serial.begin(115200);
    Serial.printf("Firmware: %s\n", BP32.firmwareVersion());
    const uint8_t* addr = BP32.localBdAddress();
    Serial.printf("BD Addr: %2X:%2X:%2X:%2X:%2X:%2X\n", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);

    // Setup the Bluepad32 callbacks
    BP32.setup(&onConnectedController, &onDisconnectedController);

    // "forgetBluetoothKeys()" should be called when the user performs
    // a "device factory reset", or similar.
    // Calling "forgetBluetoothKeys" in setup() just as an example.
    // Forgetting Bluetooth keys prevents "paired" gamepads to reconnect.
    // But might also fix some connection / re-connection issues.
    BP32.forgetBluetoothKeys();

    // Enables mouse / touchpad support for gamepads that support them.
    // When enabled controllers like DualSense and DualShock4 generate two connected devices:
    // - First one: the gamepad
    // - Second one, which is a "vritual device", is a mouse
    // By default it is disabled.
    BP32.enableVirtualDevice(false);

    robotMotors.init();
    robotMotors.setCurrentLimit(20.0);
    robotMotors.setMotorBrakeMode(AUTO_BRAKE);
}

void loop() {
    //This needs to be called during every loop
    BP32.update();

    if (myController && myController->isConnected()) {
        if (myController->isGamepad()) {
            processGamepad(myController);
        }
        else {
            Serial.printf("Data not available yet\n");
        }
        // See ArduinoController.h for all the available functions.
    }
    else{
      robotMotors.setMotorSpeed(LEFT_MOTOR, 0);
      robotMotors.setMotorSpeed(RIGHT_MOTOR, 0);
    }

    //This checks for any motor controller faults
    //If any are detected it will try to clear the faults
    robotMotors.checkFaults();
    delay(30);
}
