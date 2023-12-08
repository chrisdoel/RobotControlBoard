#ifndef __DRV8711__
#define __DRV8711__
#include <Arduino.h>
#include <SPI.h>

//#########CTRL Register#############
#define CTRL_REG_ADDR 0

//Set 1 if motors enabled, 0 if disabled
#define CTRL_ENBL_BIT 0

//ISGAIN is bit 8 & 9
// 00: Gain of 5
// 01: Gain of 10
// 10: Gain of 20
// 11: Gain of 40
#define CTRL_ISGAIN_BIT 8

enum ISGAIN_GAIN {
    GAIN5 = 0b00,
    GAIN10 = 0b01,
    GAIN20 = 0b10,
    GAIN40 = 0b11
};

#define CTRL_DTIME_BIT 10

enum ISGAIN_DTIME {
    NS_400 = 0b00,
    NS_450 = 0b01,
    NS_650 = 0b10,
    NS_850 = 0b11
};

//#########TORQUE Register#############
#define TORQUE_REG_ADDR 1

#define TORQUE_TORQUE_BIT 0

//#########OFF Register#############
#define OFF_REG_ADDR 2

#define OFF_TOFF_BIT 0

#define OFF_PWMMODE_BIT 8

enum PWMMODE {
    STEPPER = 0,
    BRUSHED = 1
};

//#########BLANK Register#############
#define BLANK_REG_ADDR 3

#define BLANK_TBLANK_BIT 0

#define BLANK_ADAPTIVE_BLANKING_BIT 8

//#########DECAY Register#############
#define DECAY_REG_ADDR 4

#define DECAY_TDECAY_BIT 0

#define DECAY_DECMOD_BIT 8

enum DECAYMODE {
    FORCE_SLOW_DECAY = 0b000,
    SLOW_INCREASING_MIXED_DECREASING = 0b001,
    FORCE_FAST_DECAY = 0b010,
    FORCE_MIXED_DECAY = 0b011,
    SLOW_INCREASING_AUTO_MIXED_DECREASING = 0b100,
    FORCE_AUTO_MIXED_DECAY = 0b101
};


//#########STALL Register#############
#define STALL_REG_ADDR 5


//#########DRIVE Register#############
#define DRIVE_REG_ADDR 6

#define DRIVE_OVERCURRENT_BIT 0

enum OCP_THRESHOLD {
  mV_250 = 0b00,
  mV_500 = 0b01,
  mV_750 = 0b10,
  mV_1000 = 0b11
};

#define DRIVE_OVERCURRENT_DEGLITCH_BIT 2

enum OCP_DEGLITCH {
  us_1 = 0b00,
  us_2 = 0b01,
  us_4 = 0b10,
  us_8 = 0b11
};

#define DRIVE_LS_GATE_DRIVE_TIME_BIT 4

enum GATE_DRIVE_TIME {
  ns_250 = 0b00,
  ns_500 = 0b01,
  ns_1000 = 0b10,
  ns_2000 = 0b11
};

#define DRIVE_HS_GATE_DRIVE_TIME_BIT 6

#define DRIVE_LS_GATE_PEAK_CURRENT_BIT 8

enum LS_GATE_PEAK_CURRENT {
  LS_mA_100 = 0b00,
  LS_mA_200 = 0b01,
  LS_mA_300 = 0b10,
  LS_mA_400 = 0b11
};

#define DRIVE_HS_GATE_PEAK_CURRENT_BIT 10

enum HS_GATE_PEAK_CURRENT {
  HS_mA_50 = 0b00,
  HS_mA_100 = 0b01,
  HS_mA_150 = 0b10,
  HS_mA_200 = 0b11
};

//#########STATUS Register#############
#define STATUS_REG_ADDR 7

#define STATUS_OTS_BIT 0

#define STATUS_AOCP_BIT 1 

#define STATUS_BOCP_BIT 2

#define STATUS_APDF_BIT 3

#define STATUS_BPDF_BIT 4

#define STATUS_UVLO_BIT 5

#define STATUS_STD_BIT 6 

#define STATUS_STDLAT_BIT 7



// Class representing the DRV8711 register
class DRV8711 {
private:

public:
  DRV8711();

  void init();

  void writeRegister(uint8_t regAddress, uint16_t data);

  uint16_t readRegister(uint8_t regAddress);

  void setMotorEnabled(bool enableOrDisable);

  void setSenseAmplifierGain(ISGAIN_GAIN gain);

  void setDeadTime(ISGAIN_DTIME deadTimeSetting);

  void setTorque(uint8_t torque);

  void setTOFF(uint8_t toffx500ns);

  void setPWMMode(PWMMODE pwmmode);

  void setBlankingTime(uint8_t timeX20nsPlus1us);

  void setAdaptiveBlankingMode(bool enabled);

  void setDecayTime(uint8_t decayTimeX500ns);

  void setDecayMode(DECAYMODE decayMode);

  void setOverCurrentProtectionThreshold(OCP_THRESHOLD ocpThreshold);

  void setOverCurrentDeglitch(OCP_DEGLITCH ocpDeglitch);

  void setLSGateDriveTime(GATE_DRIVE_TIME gateDriveTime);

  void setHSGateDriveTime(GATE_DRIVE_TIME gateDriveTime);

  void setLSGatePeakCurrent(LS_GATE_PEAK_CURRENT gatePeakCurrent);

  void setHSGatePeakCurrent(HS_GATE_PEAK_CURRENT gatePeakCurrent);

  bool checkOTS();
  void clearOTS();

  bool checkAOCP();
  void clearAOCP();

  bool checkBOCP();
  void clearBOCP();

  bool checkAPDF();
  void clearAPDF();

  bool checkBPDF();
  void clearBPDF();

  bool checkUVLO();
  void clearUVLO();

  bool checkSTD();

  bool checkSTDLAT();
  void clearSTDLAT();

  void printStatus();

  void printRegister(uint8_t regAddress);

  void printUINT16Binary(uint16_t value);

  void configureDefaultBrushedMotorProfile();
  
  void setCurrentLimit(uint8_t amps);

  void testMotorEnable();

  void testSetTorque();

  void testSetGain();

private:

};

#endif