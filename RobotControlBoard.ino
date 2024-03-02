#include <drv8711.h>
#include <robot_motors.h>
#include <Bluepad32.h>
#include <cstring>

#define LED_PIN 2

ControllerPtr myController;

Motors robotMotors;

//Set this to either true or false to determine whether any controller can connect
const bool ALLOW_ANY_CONTROLLER_TO_CONNECT = true;

//Every controller has a unique bluetooth address, similar to how internet devices have a unique IP address
//If you want to only allow a particular controller to connect, then add the 6 digit bluetooth address below
//To find the bluetooth address of the controller: Simply put it in pairing mode, plug in the board and open the serial monitor
//The controllers bluetooth address will be printed in the serial monitor when it attempts to connect
uint8_t whitelistedControllerBTAddress[6] = {0, 0, 0, 0, 0, 0};
//Example of a whitelisted controller bluetooth address:
// uint8_t whitelistedControllerBTAddress[6] = {28, 160, 184, 87, 59, 170};


//This function is called when the controller is connected
void onConnectedController(ControllerPtr ctl) {
  //If there is not a controller already connected then we try to connect the new controller
  if (myController == nullptr) {
    //Get the controller properties
    ControllerProperties properties = ctl->getProperties();
    Serial.printf("Controller Bluetooth Address: %i %i %i %i %i %i\nAttempting to connect controller...\n", properties.btaddr[0], properties.btaddr[1], properties.btaddr[2], properties.btaddr[3], properties.btaddr[4], properties.btaddr[5]);

    //This checks whether the connected controller is whitelisted
    bool isControllerWhitelisted = (std::memcmp(whitelistedControllerBTAddress, properties.btaddr, 6) == 0);

    //Connect the controller if all controllers are allowed OR the controller is whitelisted
    if(ALLOW_ANY_CONTROLLER_TO_CONNECT){
      Serial.printf("Controller connected\nWARNING: Any controller can connect to this device, set ALLOW_ANY_CONTROLLER_TO_CONNECT to false and add a whitelisted controller bluetooth address to ensure only your controller can connect\n");
      myController = ctl;
      //We can set the controller LED colour if this is supported (e.g. PS4, PS5)
      myController->setColorLED(/*Red*/255, /*Green*/0, /*Blue*/0);

      //We can set the controller players LEDs if this is supported (e.g. Wii controller)
      myController->setPlayerLEDs(1);
      
      //We can also rumble the controller
      //255 duration is about 2 seconds
      ctl->setRumble(/*force*/128, /*duration*/128);
    }
    //Check if this new controller's bluetooth address has been whitelisted
    else if(isControllerWhitelisted){
        Serial.printf("Whitelisted controller connected\n");
        myController = ctl;
        myController->setColorLED(/*Red*/255, /*Green*/0, /*Blue*/0);
        myController->setPlayerLEDs(1);
        ctl->setRumble(/*force*/128, /*duration*/128);
    }
    //If it has not been whitelisted, disconnect it
    else{
      Serial.printf("This controller has not been whitelisted, disconnecting...\n");
      Serial.printf("To whitelist this controller, add the following bluetooth address to the whitelistedControllerBTAddress variable: %i %i %i %i %i %i\n", properties.btaddr[0], properties.btaddr[1], properties.btaddr[2], properties.btaddr[3], properties.btaddr[4], properties.btaddr[5]);
      ctl->disconnect();
    }
  }
  else{
    Serial.printf("CALLBACK: Controller could not be added as there is already one connected\n");
  }
}

//This function is called when the controller is disconnected
void onDisconnectedController(ControllerPtr ctl) {
  if (myController == ctl) {
      Serial.printf("Controller disconnected\n");
      myController = nullptr;
  }
  else{
    Serial.printf("WARNING: Could not disconnect controller\n");
  }
  BP32.forgetBluetoothKeys();

}

//This prints the controller information
void printController(ControllerPtr ctl) {
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


//This function maps the controller inputs to the motor speeds
void processControllerInputs(ControllerPtr myController) {
    int leftThrottle = myController->axisY();
    int rightThrottle = myController->axisRY();

    //map the controller input range to the motor speed range (-100.0 to 100.0)
    leftThrottle = map(leftThrottle, /*Min axis value*/-400, /*Max axis value*/420, -100, 100);
    rightThrottle = map(rightThrottle, /*Min axis value*/-400, /*Max axis value*/420, -100, 100);

    robotMotors.setMotorSpeed(LEFT_MOTOR, leftThrottle);
    robotMotors.setMotorSpeed(RIGHT_MOTOR, rightThrottle);

    printController(myController);
}

void setup() {
    //This sets up the serial monitor
    //Set the Arduino serial monitor to 115200 baud in order to see the print statements
    Serial.begin(115200);

    Serial.printf("Firmware: %s\n", BP32.firmwareVersion());

    const uint8_t* addr = BP32.localBdAddress();
    Serial.printf("BD Addr: %2X:%2X:%2X:%2X:%2X:%2X\n", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);

    //Setup the functions that are called when a controller conencts or disconnects
    BP32.setup(&onConnectedController, &onDisconnectedController);

    //This forgets paired bluetooth devices
    //Feel free to remove this line if you want it to auto connect your last controller
    //However this can sometimes cause connection issues
    BP32.forgetBluetoothKeys();

    //This tells the gamepad library that we dont want the controller to be registered as a mouse
    //The gamepad library supports 'virtual devices' such as mice, but we have no need for this
    BP32.enableVirtualDevice(false);

    robotMotors.init();
    robotMotors.setCurrentLimit(10.0);
    pinMode(LED_PIN, OUTPUT);
}

void loop() {
    //This needs to be called during every loop
    //It handles all the gamepad functions
    BP32.update();

    //This if statement handles bluetooth controller inputs
    //It checks if the controller is connected and maps controller inputs to motor speeds
    if (myController && myController->isConnected()) {
        if (myController->isGamepad()) {
            //Map controller inputs to motor speeds
            processControllerInputs(myController);
        }
        else {
            Serial.printf("Data not available yet\n");
        }
    }
    else{
      //If a controller is not connected then we set the motor speeds to 0
      robotMotors.setMotorSpeed(LEFT_MOTOR, 0);
      robotMotors.setMotorSpeed(RIGHT_MOTOR, 0);
    }

    //This checks for any motor controller faults
    //If any are detected it will print the error and try to automatically clear the faults
    robotMotors.checkFaults();

    //This toggles the LED every loop
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));

    //We add a delay of 30 milliseconds for each loop
    //This means that the loop will run aproximately 30 times per second
    delay(30);
}
