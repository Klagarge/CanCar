#ifndef CARINTERFACE_H
#define	CARINTERFACE_H

#include <xc.h>
#include "can.h"
#include <stdio.h>
#include <string.h>

const uint8_t idCar = 0x8;

/************
 * FROM CAR *
 ***********/
#define ID_TEMPOMAT         0x1
#define ID_GEAR_SEL         0x2
#define ID_FRONT_SENS_REQ   0x3
#define ID_MOTOR_STATUS     0x4
#define ID_BRAKE_PEDAL      0x6
#define ID_ACCEL_PEDAL      0x7
#define ID_CONTACT_KEY      0x8
#define ID_STEERING_W_REQ   0x9
#define ID_BROKEN_CAR       0xA
#define ID_BAD_MESSAGE      0xB
#define ID_RACE             0xD
#define ID_CAR_ID           0xF

/**********
 * TO CAR *
 *********/
#define ID_LIGHT_FRONT      0x11
#define ID_LIGHT_BACK       0x12
#define ID_TIME             0x13
#define ID_GEAR_LVL         0x14
#define ID_AUDIO            0x15
#define ID_PWR_MOTOR        0x16
#define ID_POWER_BRAKE      0x17
#define ID_TEMPO_OFF        0x18
#define ID_KM_PULSE         0x19
#define ID_AUTO_STEERING    0x1A

typedef struct CAR_STATE {
    
    /************
     * FROM CAR *
     ***********/
    uint8_t tempomat[2];
    uint8_t gearSel[1];
    uint8_t frontSensReq[2];
    uint8_t motorStatus[4];
    uint8_t brakePedal[1];
    uint8_t accelPedal[1];
    uint8_t contactKey[1];
    uint8_t steeringWReq[1];
    uint8_t brokenCar[1];
    uint8_t badMessage[1];
    uint8_t race[1];
    uint8_t carID[1];
    
    /**********
     * TO CAR *
     *********/
    uint8_t lightFront[1];
    uint8_t lightBack[1];
    uint8_t time[3];
    uint8_t gearLvl[1];
    uint8_t audio[2];
    uint8_t pwrMotor[2];
    uint8_t pwrBrake[1];
    uint8_t tempoOff[0];
    uint8_t kmPulse[0];
    uint8_t autoSteering[2];
    
    
    
} CAR_STATE;

typedef struct bufferType{
    CAN_TX_MSGOBJ obj;
    uint8_t value[3];
} bufferType;

bool pushTxObj(bufferType value);
bool sendTxObj();

extern CAR_STATE carState;

/*****************
 * SET FUNCTIONS *
 ****************/

/**
 * Sets the headlights lightning level
 * @param power The value ?0? means no headlights, the value ?50? means headlights low beam and 100 means headlights high beam
 */
void setLightFront(uint8_t power);

/**
 * Sets the backlights lightning level
 * @param power The value ?0? means no backlights, the value ?50? means backlights low beam and 100 means brake lights
 */
void setLightBack(uint8_t power);

/**
 * Sets the time in cockpit
 * @param hour The current hours
 * @param minutes The current minutes
 * @param colon The colon (between hours and minutes) 0-not visible, 1-visible
 */
void setTime(uint8_t hour, uint8_t minutes, bool colon);

/**
 * Sets the current automatic gear level
 * Use this message change the current gear level. The value 0 means the wheels are not
driven by the motor (neutral behavior). The gear level 0 to 5 must be used for the drive
forward direction. The gear level 0 or 1 only must be used for the reverse direction. Cau-
tion, don't change more than one gear each time.
 * @param level The current gear level (0 to 5)
 */
void setGearLevel(uint8_t level);

/**
 * Used for quiet mode in laboratories
 * @param power The motor audio level (0 to 100%)
 * @param drive The wheels drive (0: no drive, 1: drive wheels)
 */
void setAudio(uint8_t power, bool drive);

/**
 * To control the motor RPM
 * @param power The motor power applied (0 to 100%)
 * @param starter Starter (0 or 1), set to 1 when RPM is 0, else set to 0
 */
void setPowerMotor(uint8_t power, bool starter);

/**
 * To control the car wheels brake
 * @param power The car brake factor applied (0 to 100%)
 */
void setPowerBrake(uint8_t power);

/**
 * To stop the tempomat mode
 */
void setTempoOff();

/**
 * Indicate that 100 meter have passed
 */
void setKmPulse();

/**
 * Used to drive the car in race mode
 * @param position Steering wheel position (-100 to 100)
 * @param automatic 0: no automatic control, 1: automatic control
 */
void setAutoSteering(int8_t position, bool automatic);


char mode = 'S';
void defineMode();
#endif	/* CARINTERFACE_H */

