#include <Arduino.h>
#include <SPI.h>
#include "drv8711.h"
#include <bitset>

// Define SPI Pins
const int CS_PIN = 5; // Chip Select (CS) pin
const float CURRENT_SHUNT_RESISTANCE = 0.0025;
const float MOSFET_GATE_CHARGE_IN_NC = 30;

SPIClass DRVSPI(VSPI);

DRV8711::DRV8711(){
}

void DRV8711::init(){
  // Initialize SPI
  pinMode(19, INPUT_PULLUP);

  DRVSPI.begin();
  DRVSPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
  pinMode(19, INPUT_PULLUP);

    // Initialize Chip Select pin
  pinMode(CS_PIN, OUTPUT);
  digitalWrite(CS_PIN, LOW); // Set CS pin high (inactive)

  pinMode(4, OUTPUT);

  digitalWrite(4, HIGH);

  pinMode(34, INPUT_PULLUP);
}

void DRV8711::writeRegister(uint8_t regAddress, uint16_t data) {
    uint16_t spiData = ((regAddress & 0x07) << 12) | (data & 0x0FFF);
    spiData &= ~(1 << 15);

    // Select the SPI device (CS pin low)
    digitalWrite(CS_PIN, HIGH);

    // Send data to the DRV8711
    DRVSPI.transfer16(spiData);

    // Deselect the SPI device (CS pin high)
    digitalWrite(CS_PIN, LOW);
}

uint16_t DRV8711::readRegister(uint8_t regAddress) {
    uint16_t spiData = ((regAddress & 0x07) << 12);
    spiData |= (1 << 15);

    // Select the SPI device (CS pin low)
    digitalWrite(CS_PIN, HIGH);

    // Serial.print("Sending ");
    // printUINT16Binary(spiData);

    // Send address to initiate read and receive data
    uint16_t readData = DRVSPI.transfer16(spiData);

    // Serial.print("Recieved ");
    // printUINT16Binary(readData);

    // Deselect the SPI device (CS pin high)
    digitalWrite(CS_PIN, LOW);

    // Extract the 12 bits of data received
    return readData & 0x0FFF;
}

void DRV8711::setMotorEnabled(bool enableOrDisable) {
  // Read the current value of CTRL register
  uint16_t ctrlRegValue = readRegister(CTRL_REG_ADDR);

  // Set or clear the ENBL bit based on enableOrDisable
  if (enableOrDisable) {
    ctrlRegValue |= (1 << CTRL_ENBL_BIT); // Set ENBL bit
  } else {
    ctrlRegValue &= ~(1 << CTRL_ENBL_BIT); // Clear ENBL bit
  }

  // Write back the modified value to CTRL register
  writeRegister(CTRL_REG_ADDR, ctrlRegValue);
}

void DRV8711::setSenseAmplifierGain(ISGAIN_GAIN gain) {
    if(gain > 3){
        Serial.printf("Invalid amplifier gain provided\n");
        return;
    }

    // Read the current value of CTRL register
    uint16_t ctrlRegValue = readRegister(CTRL_REG_ADDR);

    // Clear the ISGAIN bits (bits 9-8)
    ctrlRegValue &= ~(0b11 << CTRL_ISGAIN_BIT);

    // Set the new gain value
    ctrlRegValue |= ((gain & 0b11) << CTRL_ISGAIN_BIT);

    // Write back the modified value to CTRL register
    writeRegister(CTRL_REG_ADDR, ctrlRegValue);
}

void DRV8711::setDeadTime(ISGAIN_DTIME deadTimeSetting) {
  // Read the current value of CTRL register
  uint16_t ctrlRegValue = readRegister(CTRL_REG_ADDR);

  // Clear the DTIME bits (bits 11-10)
  ctrlRegValue &= ~(0b11 << CTRL_DTIME_BIT);

  // Set the new dead time value
  ctrlRegValue |= ((deadTimeSetting & 0b11) << CTRL_DTIME_BIT);

  // Write back the modified value to CTRL register
  writeRegister(CTRL_REG_ADDR, ctrlRegValue);
}

void DRV8711::setTorque(uint8_t torque) {
  // Read the current value of CTRL register
  uint16_t ctrlRegValue = readRegister(TORQUE_REG_ADDR);

  // Clear the DTIME bits (bits 11-10)
  ctrlRegValue &= ~(0b11111111 << TORQUE_TORQUE_BIT);

  // Set the new dead time value
  ctrlRegValue |= ((torque & 0b11111111) << TORQUE_TORQUE_BIT);

  // Write back the modified value to CTRL register
  writeRegister(TORQUE_REG_ADDR, ctrlRegValue);
}

void DRV8711::setTOFF(uint8_t toffX500ns) {
  // Read the current value of CTRL register
  uint16_t ctrlRegValue = readRegister(OFF_REG_ADDR);

  // Clear the DTIME bits (bits 11-10)
  ctrlRegValue &= ~(0b11111111 << OFF_TOFF_BIT);

  // Set the new dead time value
  ctrlRegValue |= ((toffX500ns & 0b11111111) << OFF_TOFF_BIT);

  // Write back the modified value to CTRL register
  writeRegister(OFF_REG_ADDR, ctrlRegValue);
}

void DRV8711::setPWMMode(PWMMODE pwmmode) {
  // Read the current value of CTRL register
  uint16_t ctrlRegValue = readRegister(OFF_REG_ADDR);

  // Clear the DTIME bits (bits 11-10)
  ctrlRegValue &= ~(0b11111111 << OFF_PWMMODE_BIT);

  // Set the new dead time value
  ctrlRegValue |= ((pwmmode & 0b11111111) << OFF_PWMMODE_BIT);

  // Write back the modified value to CTRL register
  writeRegister(OFF_REG_ADDR, ctrlRegValue);
}

void DRV8711::setBlankingTime(uint8_t timeX20nsPlus1us) {
  // Read the current value of CTRL register
  uint16_t ctrlRegValue = readRegister(BLANK_REG_ADDR);

  // Clear the DTIME bits (bits 11-10)
  ctrlRegValue &= ~(0b11111111 << BLANK_TBLANK_BIT);

  // Set the new dead time value
  ctrlRegValue |= ((timeX20nsPlus1us & 0b11111111) << BLANK_TBLANK_BIT);

  // Write back the modified value to CTRL register
  writeRegister(BLANK_REG_ADDR, ctrlRegValue);
}

void DRV8711::setAdaptiveBlankingMode(bool enabled) {
  // Read the current value of CTRL register
  uint16_t ctrlRegValue = readRegister(BLANK_TBLANK_BIT);

  // Clear the DTIME bits (bits 11-10)
  ctrlRegValue &= ~(0b1 << BLANK_ADAPTIVE_BLANKING_BIT);

  // Set the new dead time value
  ctrlRegValue |= ((enabled & 0b1) << BLANK_ADAPTIVE_BLANKING_BIT);

  // Write back the modified value to CTRL register
  writeRegister(BLANK_TBLANK_BIT, ctrlRegValue);
}

void DRV8711::setDecayTime(uint8_t decayTimeX500ns) {
  // Read the current value of CTRL register
  uint16_t ctrlRegValue = readRegister(DECAY_REG_ADDR);

  // Clear the DTIME bits (bits 11-10)
  ctrlRegValue &= ~(0b11111111 << DECAY_TDECAY_BIT);

  // Set the new dead time value
  ctrlRegValue |= ((decayTimeX500ns & 0b11111111) << DECAY_TDECAY_BIT);

  // Write back the modified value to CTRL register
  writeRegister(DECAY_REG_ADDR, ctrlRegValue);
}

void DRV8711::setDecayMode(DECAYMODE decayMode) {
  // Read the current value of CTRL register
  uint16_t ctrlRegValue = readRegister(DECAY_REG_ADDR);

  // Clear the DTIME bits (bits 11-10)
  ctrlRegValue &= ~(0b111 << DECAY_DECMOD_BIT);

  // Set the new dead time value
  ctrlRegValue |= ((decayMode & 0b111) << DECAY_DECMOD_BIT);

  // Write back the modified value to CTRL register
  writeRegister(DECAY_REG_ADDR, ctrlRegValue);
}

void DRV8711::setOverCurrentProtectionThreshold(OCP_THRESHOLD ocpThreshold) {
  // Read the current value of CTRL register
  uint16_t ctrlRegValue = readRegister(DRIVE_REG_ADDR);

  // Clear the DTIME bits (bits 11-10)
  ctrlRegValue &= ~(0b11 << DRIVE_OVERCURRENT_BIT);

  // Set the new dead time value
  ctrlRegValue |= ((ocpThreshold & 0b11) << DRIVE_OVERCURRENT_BIT);

  // Write back the modified value to CTRL register
  writeRegister(DRIVE_REG_ADDR, ctrlRegValue);
}


void DRV8711::setOverCurrentDeglitch(OCP_DEGLITCH ocpDeglitch) {
  // Read the current value of CTRL register
  uint16_t ctrlRegValue = readRegister(DRIVE_REG_ADDR);

  // Clear the DTIME bits (bits 11-10)
  ctrlRegValue &= ~(0b11 << DRIVE_OVERCURRENT_DEGLITCH_BIT);

  // Set the new dead time value
  ctrlRegValue |= ((ocpDeglitch & 0b11) << DRIVE_OVERCURRENT_DEGLITCH_BIT);

  // Write back the modified value to CTRL register
  writeRegister(DRIVE_REG_ADDR, ctrlRegValue);
}


void DRV8711::setLSGateDriveTime(GATE_DRIVE_TIME gateDriveTime) {
  // Read the current value of CTRL register
  uint16_t ctrlRegValue = readRegister(DRIVE_REG_ADDR);

  // Clear the DTIME bits (bits 11-10)
  ctrlRegValue &= ~(0b11 << DRIVE_LS_GATE_DRIVE_TIME_BIT);

  // Set the new dead time value
  ctrlRegValue |= ((gateDriveTime & 0b11) << DRIVE_LS_GATE_DRIVE_TIME_BIT);

  // Write back the modified value to CTRL register
  writeRegister(DRIVE_REG_ADDR, ctrlRegValue);
}


void DRV8711::setHSGateDriveTime(GATE_DRIVE_TIME gateDriveTime) {
  // Read the current value of CTRL register
  uint16_t ctrlRegValue = readRegister(DRIVE_REG_ADDR);

  // Clear the DTIME bits (bits 11-10)
  ctrlRegValue &= ~(0b11 << DRIVE_HS_GATE_DRIVE_TIME_BIT);

  // Set the new dead time value
  ctrlRegValue |= ((gateDriveTime & 0b11) << DRIVE_HS_GATE_DRIVE_TIME_BIT);

  // Write back the modified value to CTRL register
  writeRegister(DRIVE_REG_ADDR, ctrlRegValue);
}

void DRV8711::setLSGatePeakCurrent(LS_GATE_PEAK_CURRENT gatePeakCurrent) {
  // Read the current value of CTRL register
  uint16_t ctrlRegValue = readRegister(DRIVE_REG_ADDR);

  // Clear the DTIME bits (bits 11-10)
  ctrlRegValue &= ~(0b11 << DRIVE_LS_GATE_PEAK_CURRENT_BIT);

  // Set the new dead time value
  ctrlRegValue |= ((gatePeakCurrent & 0b11) << DRIVE_LS_GATE_PEAK_CURRENT_BIT);

  // Write back the modified value to CTRL register
  writeRegister(DRIVE_REG_ADDR, ctrlRegValue);
}

void DRV8711::setHSGatePeakCurrent(HS_GATE_PEAK_CURRENT gatePeakCurrent) {
  // Read the current value of CTRL register
  uint16_t ctrlRegValue = readRegister(DRIVE_REG_ADDR);

  // Clear the DTIME bits (bits 11-10)
  ctrlRegValue &= ~(0b11 << DRIVE_HS_GATE_PEAK_CURRENT_BIT);

  // Set the new dead time value
  ctrlRegValue |= ((gatePeakCurrent & 0b11) << DRIVE_HS_GATE_PEAK_CURRENT_BIT);

  // Write back the modified value to CTRL register
  writeRegister(DRIVE_REG_ADDR, ctrlRegValue);
}

bool DRV8711::checkOTS() {
  uint16_t status = readRegister(STATUS_REG_ADDR);
  return (status >> STATUS_OTS_BIT) & 1; 
}

void DRV8711::clearOTS() {
  uint16_t status = readRegister(STATUS_REG_ADDR);
  status &= ~(1 << STATUS_OTS_BIT);
  writeRegister(STATUS_REG_ADDR, status);  
}

bool DRV8711::checkAOCP() {
  uint16_t status = readRegister(STATUS_REG_ADDR);
  return (status >> STATUS_AOCP_BIT) & 1;
} 

void DRV8711::clearAOCP() {
  uint16_t status = readRegister(STATUS_REG_ADDR);
  status &= ~(1 << STATUS_AOCP_BIT);
  writeRegister(STATUS_REG_ADDR, status);  
}

bool DRV8711::checkBOCP() {
  uint16_t status = readRegister(STATUS_REG_ADDR);
  return (status >> STATUS_BOCP_BIT) & 1;
} 

void DRV8711::clearBOCP() {
  uint16_t status = readRegister(STATUS_REG_ADDR);
  status &= ~(1 << STATUS_BOCP_BIT);
  writeRegister(STATUS_REG_ADDR, status);  
}

bool DRV8711::checkAPDF() {
  uint16_t status = readRegister(STATUS_REG_ADDR);
  return (status >> STATUS_APDF_BIT) & 1;
} 

void DRV8711::clearAPDF() {
  uint16_t status = readRegister(STATUS_REG_ADDR);
  status &= ~(1 << STATUS_APDF_BIT);
  writeRegister(STATUS_REG_ADDR, status);  
}

bool DRV8711::checkBPDF() {
  uint16_t status = readRegister(STATUS_REG_ADDR);
  return (status >> STATUS_BPDF_BIT) & 1;
} 

void DRV8711::clearBPDF() {
  uint16_t status = readRegister(STATUS_REG_ADDR);
  status &= ~(1 << STATUS_BPDF_BIT);
  writeRegister(STATUS_REG_ADDR, status);  
}

bool DRV8711::checkUVLO() {
  uint16_t status = readRegister(STATUS_REG_ADDR);
  return (status >> STATUS_UVLO_BIT) & 1;
} 

void DRV8711::clearUVLO() {
  uint16_t status = readRegister(STATUS_REG_ADDR);
  status &= ~(1 << STATUS_UVLO_BIT);
  writeRegister(STATUS_REG_ADDR, status);  
}

bool DRV8711::checkSTD() {
  uint16_t status = readRegister(STATUS_REG_ADDR); 
  return (status >> STATUS_STD_BIT) & 1;  
}

bool DRV8711::checkSTDLAT() {
  uint16_t status = readRegister(STATUS_REG_ADDR);
  return (status >> STATUS_STDLAT_BIT) & 1; 
}

void DRV8711::clearSTDLAT() {
  uint16_t status = readRegister(STATUS_REG_ADDR);
  status &= ~(1 << STATUS_STDLAT_BIT); 
  writeRegister(STATUS_REG_ADDR, status);
}

void DRV8711::printStatus(){
  Serial.printf("Over temp fault: \t\t%i\n", checkOTS());
  Serial.printf("Ch A Overcurrent fault: \t%i\n", checkAOCP());
  Serial.printf("Ch B Overcurrent fault: \t%i\n", checkBOCP());
  Serial.printf("Ch A Predriver fault: \t%i\n", checkAPDF());
  Serial.printf("Ch B Predriver fault: \t%i\n", checkBPDF());
  Serial.printf("Undervoltage fault: \t%i\n", checkUVLO());
  Serial.printf("Stall fault: \t%i\n", checkSTD());
  Serial.printf("Stall latched fault: \t%i\n", checkSTDLAT());
}

void DRV8711::printRegister(uint8_t regAddress){
  uint16_t regValue = readRegister(regAddress);
  printUINT16Binary(regValue);
}

void DRV8711::printUINT16Binary(uint16_t value){
  for (int i = 15; i >= 0; --i) {
    Serial.print((value & (1 << i)) ? '1' : '0');
  }
  Serial.println();
}

void DRV8711::configureDefaultBrushedMotorProfile(){
  //We set a large dead time to ensure that there is
  //no shoot through
  setDeadTime(NS_850);

  //Fast decay mode means that the motor will spin down neutrally when the throttle is reduced
  setDecayMode(FORCE_FAST_DECAY);

  //Set the current trip blanking time to 2*x + 1uS
  //This basically prevents any overcurrent faults occurring within 1us of the switching time
  //Therefore avoiding instantaneous peaks from causing an overcurrent fault
  //128*20 nS + 1 uS = 3.5 uS
  setBlankingTime(255);
  
  //Lets set the gate drive current to the maximum
  //This will need validating to ensure the rise times
  //are not too fast and do not cause gate ringing
  setLSGatePeakCurrent(LS_mA_400);
  setHSGatePeakCurrent(HS_mA_200);

  //30nC gate charge / 0.2A gate current = 150nS rise time
  //Therefore we select 250 nS drive time
  //Manually check that the gate is charging sufficiently
  //If not, raise the drive time until it charges completely
  setLSGateDriveTime(ns_1000);
  setHSGateDriveTime(ns_1000);

  //33.3A across the mosfets will cause a 250 mV voltage drop
  //due to the 7.5 mOhm internal resistance
  //This is sensed by the chip and will throw an overcurrent fault 
  setOverCurrentProtectionThreshold(mV_250);
  setOverCurrentDeglitch(us_8);

  //We are using brushed motor so we tell the DRV8711
  //to listen to the direct PWM inputs
  setPWMMode(BRUSHED);

  setCurrentLimit(10);
  setTOFF(200);

  setMotorEnabled(true);
}

void DRV8711::setCurrentLimit(uint8_t amps){


  //I have no idea why i need the following settings for the current limit work
  //It should be CurrentLimit = 2.75 * torqueValue / 256 * GAIN * CURRENT_SHUNT_RESISTANCE
  //But as you can see below I am having to offset the current by 4 amps
  //And I am also setting the drv8711 to a gain of 5, when i am using 20 in my equation
  //Anyway, it has been validated experimentally from 0.5 amps to 10A in increments of 0.5A
  //So i guess it just works
  setSenseAmplifierGain(GAIN5);

  float torqueValue = (float(amps + 4.0f) * (256.0f * 20.0 * CURRENT_SHUNT_RESISTANCE))/2.75f;

  if(torqueValue > 255.0){
    Serial.printf("Error, requested current limit too high: %f\n", torqueValue);
    // Serial.printf("Modify the DRV8711::setCurrentLimit function if you really want it this high\n");
    // Serial.printf("Do so at your own risk\n");
    return;
  }
  else{
    setTorque(uint8_t(torqueValue));
  }
  Serial.printf("Setting torque to %i\n", uint8_t(torqueValue));
}


//################## TEST FUNCTIONS #####################
void DRV8711::testMotorEnable() {
  // Disable motor
  setMotorEnabled(false);
  
  // Verify motor is disabled
  uint16_t ctrlReg = readRegister(CTRL_REG_ADDR);
  if(0 != (ctrlReg >> CTRL_ENBL_BIT) & 1){
    Serial.printf("Disabling motor failed\n");
  }

  // Enable motor
  setMotorEnabled(true);

  // Verify motor is enabled
  ctrlReg = readRegister(CTRL_REG_ADDR); 
  if(1 != (ctrlReg >> CTRL_ENBL_BIT) & 1){
    Serial.printf("Enabling motor failed\n");
  }
}

void DRV8711::testSetTorque() {
  // Set torque to 127 (max)
  setTorque(127);
  
  // Read back torque setting
  uint16_t torqueReg = readRegister(TORQUE_REG_ADDR);
  if(127 !=(torqueReg >> TORQUE_TORQUE_BIT) & 0xFF){
    Serial.printf("Setting torque failed\n");
  }
}

void DRV8711::testSetGain() {

  // Try various gain settings
  setSenseAmplifierGain(GAIN5);
  if(GAIN5 != readRegister(CTRL_REG_ADDR) >> CTRL_ISGAIN_BIT & 0b11){
      Serial.printf("Setting gain failed\n");
  }
  
  setSenseAmplifierGain(GAIN40);
  if(GAIN40 != readRegister(CTRL_REG_ADDR) >> CTRL_ISGAIN_BIT & 0b11){
      Serial.printf("Setting gain failed\n");
  }
}