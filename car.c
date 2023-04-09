/*
 * File:   car.c
 * Author: remi.heredero
 *
 * Created on March 28, 2023, 3:18 PM
 */

#include "car.h"

CAR_STATE carState;
/*
void initCar(){
    sAccelPedal.id = 0x7;
    sAccelPedal.length = 1;
    sAccelPedal.value = malloc(sAccelPedal.length);
    
    sLightFront.id = 0x11;
    sLightFront.length = 1;
    sLightFront.value = malloc(sLightFront.length);
}
*/
CAN_TX_MSGOBJ defineTxMsgObj(uint32_t id, CAN_DLC dlc){
    CAN_TX_MSGOBJ msgObj;
    msgObj.bF.id.ID = ((id << 4) | idCar);
    msgObj.bF.ctrl.DLC = dlc;
    msgObj.bF.ctrl.RTR = 0;
    msgObj.bF.id.SID11 = 0;
    msgObj.bF.ctrl.FDF = 0;
    msgObj.bF.ctrl.IDE = 0;
    msgObj.bF.ctrl.BRS = 0;
    msgObj.bF.ctrl.ESI = 0;
    return msgObj;
}

void pushTxObj(bufferType value) {
    stackType *temp = (stackType*) malloc(sizeof(stackType));
    temp->data = value;
    temp->next = head;
    head = temp;
}
void pushFunction (carSetter fct) {
    stackFunction *temp = malloc(sizeof(stackFunction));
    temp->function = fct;
    //temp->function(5,);
    temp->next = headStack;
    headStack = temp;
}

bool sendTxObj() {
    if(head == NULL) return false;
    
    stackType *temp = head;
    head = head->next;
    
    CanSend(&temp->data.obj,temp->data.value);
    free(temp);
        
    return true;
}

bool sendFunction(){
    if(headStack == NULL) return false;
    stackFunction *temp = headStack;
    bufferType bf = temp->function;
    headStack = headStack->next;
    CanSend(bf.obj, bf.value);
    free(temp)
    return true;
}

/*****************
 * SET FUNCTIONS *
 ****************/

/*
int foobar(){
    carSetter *foo;
    carSetter lightFront;
    foo[0](5,5);
    foo[0] = lightFront(5,5);
} */

/**
 * Sets the headlights lightning level
 * @param power The value ?0? means no headlights, the value ?50? means headlights low beam and 100 means headlights high beam
 */
bufferType setLightFront(uint8_t power){
    if(power != carState.lightFront[0]){
        carState.lightFront[0] = power;
        bufferType bufferObj;
        bufferObj.obj = defineTxMsgObj(ID_LIGHT_FRONT, CAN_DLC_1);
        bufferObj.value = carState.lightFront;
        pushTxObj(bufferObj);
        return bufferObj;
    }
    return NULL;
}

/**
 * Sets the backlights lightning level
 * @param power The value ?0? means no backlights, the value ?50? means backlights low beam and 100 means brake lights
 */
void setLightBack(uint8_t power) {
    if(power != carState.lightBack[0]){
        carState.lightBack[0] = power;
        bufferType bufferObj;
        bufferObj.obj = defineTxMsgObj(ID_LIGHT_BACK, CAN_DLC_1);
        bufferObj.value = carState.lightBack;
        pushTxObj(bufferObj);
    } 
}

/**
 * Sets the time in cockpit
 * @param hour The current hours
 * @param minutes The current minutes
 * @param colon The colon (between hours and minutes) 0-not visible, 1-visible
 */
void setTime(uint8_t hour, uint8_t minutes, bool colon){
    bool send = false;
    if(hour != carState.time[0]){
        carState.time[0] = hour;
        send = true;
    }
    if(minutes != carState.time[1]){
        carState.time[1] = minutes;
        send = true;
    }
    if(colon != carState.time[2]){
        carState.time[2] = colon;
        send = true;
    }
    if(send){
        bufferType bufferObj;
        bufferObj.obj = defineTxMsgObj(ID_TIME, CAN_DLC_3);
        bufferObj.value = carState.time;
        pushTxObj(bufferObj);
    }
}

/**
 * Sets the current automatic gear level
 * Use this message change the current gear level. The value 0 means the wheels are not
driven by the motor (neutral behavior). The gear level 0 to 5 must be used for the drive
forward direction. The gear level 0 or 1 only must be used for the reverse direction. Cau-
tion, don?t change more than one gear each time.
 * @param level The current gear level (0 to 5)
 */
void setGearLevel(uint8_t level){
    if(level != carState.gearLvl[0]){
        carState.gearLvl[0] = level;
        bufferType bufferObj;
        bufferObj.obj = defineTxMsgObj(ID_GEAR_LVL, CAN_DLC_1);
        bufferObj.value = carState.gearLvl;
        pushTxObj(bufferObj);
    } 
}

/**
 * Used for quiet mode in laboratories
 * @param power The motor audio level (0 to 100%)
 * @param drive The wheels drive (0: no drive, 1: drive wheels)
 */
void setAudio(uint8_t power, bool drive){
    bool send = false;
    if(power != carState.audio[0]){
        carState.audio[0] = power;
        send = true;
    }
    if(drive != carState.audio[1]){
        carState.audio[1] = drive;
        send = true;
    }
    if(send){
        bufferType bufferObj;
        bufferObj.obj = defineTxMsgObj(ID_AUDIO, CAN_DLC_2);
        bufferObj.value = carState.audio;
        pushTxObj(bufferObj);
    }
}

/**
 * To control the motor RPM
 * @param power The motor power applied (0 to 100%)
 * @param starter Starter (0 or 1), set to 1 when RPM is 0, else set to 0
 */
void setPowerMotor(uint8_t power, bool starter){
    bool send = false;
    if(power != carState.pwrMotor[0]){
        carState.pwrMotor[0] = power;
        send = true;
    }
    if(starter != carState.pwrMotor[1]){
        carState.pwrMotor[1] = starter;
        send = true;
    }
    if(send){
        bufferType bufferObj;
        bufferObj.obj = defineTxMsgObj(ID_PWR_MOTOR, CAN_DLC_2);
        bufferObj.value = carState.pwrMotor;
        pushTxObj(bufferObj);
    }
}

/**
 * To control the car wheels brake
 * @param power The car brake factor applied (0 to 100%)
 */
void setPowerBrake(uint8_t power) {
    if(power != carState.pwrBrake[0]){
        carState.pwrBrake[0] = power;
        bufferType bufferObj;
        bufferObj.obj = defineTxMsgObj(ID_POWER_BRAKE, CAN_DLC_1);
        bufferObj.value = carState.pwrBrake;
        pushTxObj(bufferObj);
    } 
}

/**
 * To stop the tempomat mode
 */
void setTempoOff(){
    bufferType bufferObj;
    bufferObj.obj = defineTxMsgObj(ID_TEMPO_OFF, CAN_DLC_0);
    bufferObj.value = carState.tempoOff;
    pushTxObj(bufferObj);
}

/**
 * Indicate that 100 meter have passed
 */
void setKmPulse(){
    bufferType bufferObj;
    bufferObj.obj = defineTxMsgObj(ID_KM_PULSE, CAN_DLC_0);
    bufferObj.value = carState.kmPulse;
    pushTxObj(bufferObj);
}

/**
 * Used to drive the car in race mode
 * @param position Steering wheel position (-100 to 100)
 * @param automatic 0: no automatic control, 1: automatic control
 */
void setAutoSteering(int8_t position, bool automatic){
    bool send = false;
    if(position != carState.autoSteering[0]){
        carState.autoSteering[0] = position;
        send = true;
    }
    if(automatic != carState.autoSteering[1]){
        carState.autoSteering[2] = automatic;
        send = true;
    }
    if(send){
        bufferType bufferObj;
        bufferObj.obj = defineTxMsgObj(ID_AUTO_STEERING, CAN_DLC_2);
        bufferObj.value = carState.autoSteering;
        pushTxObj(bufferObj);
    }
}

/**
 * Used to reset the car after a crash
 */
void setCarRst(){
    bufferType bufferObj;
    bufferObj.obj = defineTxMsgObj(ID_CAR_RST, CAN_DLC_0);
    bufferObj.value = carState.carRst;
    pushTxObj(bufferObj);
}



/********************
 * USEFUL FUNCTIONS *
 *******************/

void defineMode(){
    switch(carState.gearSel[0]){
        case 'P':
            mode = 'P';
            break;
        case 'R':
            mode = 'R';
            break;
        case 'N':
            mode = 'N';
            break;
        case 'D':
            mode = 'D';
            break;
        default:
            mode = 'S';
            break;
    }
}

void start(){
    setPowerMotor(12, true);
    setLightFront(100);
    setLightBack(50);
    setAudio(50, false);
}
void stop(){
    mode = 'S';
    setPowerMotor(0, false);
    setLightFront(0);
    setLightBack(0);
    setAudio(0, false);
}

