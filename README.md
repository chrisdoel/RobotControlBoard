# The Robot Control Board
This control board is intended for use with fighting robots, or other applications where high current brushed motors need to be driven wirelessly with a high reliability.

Features:
- Uses Bluepad32 to allow connection to a wide varienty of wireless game controllers (PS4, PS5, XBOX, Wii, Switch, etc..).
- Integrates two high current H bridges to control two brushed motors.
- Has a programmable current limit to prevent burning out motors or the control board.
- Current surge protection
- Voltage input of 8V to 26V (2S to 6S lipo)
- Has been extensively tested using stalled motors at 10A continuous - with some small mods it can do 20A continuous.
- Has ports to plug in and control ESCs or servos.
- Has ports to allow a standard RC controller receiver to be connected.
- Integrated 5V 2A regulator.

Safety warning
- Ensure you are familiar with lipo safety if you are using lithium batteries
- Do not connect the battery or power supply in reverse, you will destroy your board
- Do not exceed 26V input voltage
- If you wish to plug in your laptop while powering the control board from a battery or PSU, I recommend de-soldering D9 to ensure that your laptop has additional isolation from the board

## PCB overview
![image](https://github.com/chrisdoel/RobotControlBoard/assets/70950249/63dbf7ff-a471-4055-8c4e-d58655e2e8d9)

## Setting up the hardware
You will likely have recieved a PCB with no fuses or connectors soldered. Here are the steps to get the board fully constructed and ready to be programmed.

![20240301_194836](https://github.com/chrisdoel/RobotControlBoard/assets/70950249/4548a741-5fc7-4d4d-ba67-d56997ffe42e)

### First solder the fuses
I recommend using 25A fuses, as the maximum recommended current limit is 20A.
Here I have used 30A but this is all I had available at the time.
You can fit regular AT0 fuses and mini fuses.
If you try to power and run the board without fuses populated, the motor controller IC will report errors.

![20240301_195754](https://github.com/chrisdoel/RobotControlBoard/assets/70950249/8c336ac8-50c2-40e9-a44f-42d08409599a)

![20240301_195807](https://github.com/chrisdoel/RobotControlBoard/assets/70950249/1859b073-e129-4952-afe9-8b61196faf04)


### Solder the motor connectors
You can solder XT30 connectors for each motor.
It is not essential to solder motor connectors, you can instead solder the motor wires directly to the board if you want.

![20240301_200123](https://github.com/chrisdoel/RobotControlBoard/assets/70950249/f586a1dc-a1ba-4dfe-bfd2-f6dce86c58f9)

![20240301_200131](https://github.com/chrisdoel/RobotControlBoard/assets/70950249/f58992bf-caa0-4fbb-8f5f-371a3043fd39)

### Solder the input power connector
The input power connector size is a standard XT60

![image](https://github.com/chrisdoel/RobotControlBoard/assets/70950249/2a19109f-7689-4c37-8637-5e8946404135)


### Beefing up the power traces
The PCB traces use a 1oz copper thickness, which means that it can handle 10A continuously. However, if you require more current than this, i would massively recommend adding a bunch of solder over all exposed power traces to thicken them up.
Set the soldering iron to about 400 degrees and spread a layer of solder over the traces, try to be relatively quick to prevent overheating components on the board.

https://github.com/chrisdoel/RobotControlBoard/assets/70950249/c84ae59e-f64d-42ac-8d52-bb17911d7492

![20240301_204222](https://github.com/chrisdoel/RobotControlBoard/assets/70950249/078098b1-5f4f-4eb7-a612-0a27ff850def)

![image](https://github.com/chrisdoel/RobotControlBoard/assets/70950249/580665cc-3a7f-4102-a318-912b4b457ddc)


## Setting up the software
An arduino library and example code has been provided in this repository. There are a few initial steps to getting the Arduino IDE to support the control board.

### Install the Arduino IDE 2.0
Head over to https://www.arduino.cc/en/software to download the Arduino IDE 2.0

![image](https://github.com/chrisdoel/RobotControlBoard/assets/70950249/99cd5a1c-760d-4391-a930-e8107dc934d1)

Then install it on your machine.

![image](https://github.com/chrisdoel/RobotControlBoard/assets/70950249/cff76ad1-2a97-4e35-848c-1dba60f938b9)

### Add ESP32 and Bluepad32 support via the boards manager
The chip used on this board is an ESP32, therefore we need to add support for this chip via the arduino board manager.
Head to file->preferences.

![image](https://github.com/chrisdoel/RobotControlBoard/assets/70950249/0e889e2c-7efc-4666-bd14-fbd34571e05c)

Copy and paste the following two lines to the Additional Boards Manager URLs field.
https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
https://raw.githubusercontent.com/ricardoquesada/esp32-arduino-lib-builder/master/bluepad32_files/package_esp32_bluepad32_index.json

![image](https://github.com/chrisdoel/RobotControlBoard/assets/70950249/b95b3e9b-1c7c-456e-8231-1295a47ac43a)

Search for ESP32 and press the install button for esp32 and esp32_bluepad32.
![image](https://github.com/chrisdoel/RobotControlBoard/assets/70950249/8422713e-a1a6-4b66-aa61-0f2f474ba2c6)


### Download the library/code
Clone this github repository or download the repo as a zip.

![image](https://github.com/chrisdoel/RobotControlBoard/assets/70950249/993626e1-a75c-4aca-8c3a-a3eb2f23feed)

Copy the RobotMotors folder to C:\USERNAME\Documents\Arduino\libraries. This will add the Control Board library to the Arduino IDE

![image](https://github.com/chrisdoel/RobotControlBoard/assets/70950249/cfce2566-1dcf-49cd-8b29-c7cd66041f8d)

Extract the RobotControlBoard.ino file to C:\USERNAME\Documents\Arduino (or anywhere you like) and then open it in the Arduino IDE

![image](https://github.com/chrisdoel/RobotControlBoard/assets/70950249/3362a868-1fff-4a2f-9d7d-ff38e5854530)

This is the code that is used to control the Control Board.

## Uploading the code to the control board

Select the correct target board by going to tools->board->esp32_bluepad32->DOIT ESP32 DEVKIT V1 (you may have to scroll down a bit).

![image](https://github.com/chrisdoel/RobotControlBoard/assets/70950249/77eb5613-0b28-4a2e-ba8d-b92461c0a74c)

Plug in your board via the micro USB port. If your PC does not detect it then you will have to download the drivers from the following page:
https://docs.espressif.com/projects/esp-idf/en/stable/esp32/get-started/establish-serial-connection.html

If it is still not detected then try different USB cables.

Select the COM port that corresponds to the port of your control board
![image](https://github.com/chrisdoel/RobotControlBoard/assets/70950249/3a466e08-c685-4941-9a22-537931b6780c)

Hit the upload button. The IDE will then compile the code and start uploading it to the board.

![image](https://github.com/chrisdoel/RobotControlBoard/assets/70950249/81274514-d63f-425a-9c68-5efcf2e70cd6)

You may find that you get an error while connecting to the board. If this is the case then you will have to hold the Boot button on the top right corner of the control board during the connecting process. Release the button when you see it uploading the code.

![image](https://github.com/chrisdoel/RobotControlBoard/assets/70950249/76faad29-29f8-416e-a037-282b99bbdefb)

![image](https://github.com/chrisdoel/RobotControlBoard/assets/70950249/1acda865-ebcf-4fe6-9e5c-e8e6501e8803)

You have now uploaded the code to the control board!
Connect a controller and start driving some motors.
For information on supported controllers and how to pair controllers, visit https://github.com/ricardoquesada/bluepad32/blob/main/docs/supported_gamepads.md




