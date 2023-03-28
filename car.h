
#ifndef CAR_H
#define	CAR_H

#include <xc.h> 
#include "can.h"

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

struct CAR_STATE {
    
    /************
     * FROM CAR *
     ***********/
    uint8_t tempomat[2];
    uint8_t gearSel[1];
    uint8_t frontSensReq[2];
    uint8_t  motorStatus[4];
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
    
    
    
};
typedef struct CAR_STATE CAR_STATE;

extern CAR_STATE carState;
typedef struct bufferType{
    CAN_TX_MSGOBJ obj;
    uint8_t *value;
} bufferType;

bufferType bufferTxObj;
void addOnBufferTxObj(bufferType); // TODO

void setLightFront(uint8_t power);

#endif	/* CAR_H */

