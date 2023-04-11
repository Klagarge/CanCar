
#ifndef CAR_H
#define	CAR_H

#include <xc.h> 
#include "can.h"

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
#define ID_SLOPE_REQ        0xC
#define ID_RACE             0xD
#define ID_CAR_ID           0xF

/**********
 * TO CAR *
 *********/
#define ID_LIGHT_FRONT      0x11
#define ID_LIGHT_BACK        0x12
#define ID_TIME             0x13
#define ID_GEAR_LVL         0x14
#define ID_AUDIO            0x15
#define ID_PWR_MOTOR        0x16
#define ID_POWER_BRAKE      0x17
#define ID_TEMPO_OFF        0x18
#define ID_KM_PULSE         0x19
#define ID_AUTO_STEERING    0x1A
#define ID_CAR_RST          0x1F

struct CAR_STATE {
    
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
    uint8_t slopeReq[1];
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
    uint8_t carRst[0];
    
    
    
};
typedef struct CAR_STATE CAR_STATE;

extern CAR_STATE carState;
typedef struct bufferType{
    CAN_TX_MSGOBJ obj;
    uint8_t *value;
} bufferType;

typedef struct stackType{
    bufferType data;
    struct stackType *next;
}stackType;

stackType * head = NULL;
stackType * tail = NULL;

void pushTxObj(bufferType value);
bool sendTxObj();

/*****************
 * SET FUNCTIONS *
 ****************/
void setLightFront(uint8_t power);
void setLightFront(uint8_t power);
void setLightBack(uint8_t power);
void setTime(uint8_t hour, uint8_t minutes, bool colon);
void setGearLevel(uint8_t level);
void setAudio(uint8_t power, bool drive);
void setPowerMotor(uint8_t power, bool starter);
void setPowerBrake(uint8_t power);
void setTempoOff();
void setKmPulse();
void setAutoSteering(int8_t position, bool automatic);
void setCarRst();

/********************
 * USEFUL FUNCTIONS *
 *******************/
char mode;
void defineMode();
void start();
void stop();

#endif	/* CAR_H */

