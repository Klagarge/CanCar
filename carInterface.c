#include "carInterface.h"

CAR_STATE carState;

typedef struct stackType{
    bufferType data;
    struct stackType *next;
}stackType;

stackType * head = NULL;
stackType * tail = NULL;

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

bool pushTxObj(bufferType value) {    stackType *temp = (stackType*) malloc(sizeof(stackType));
    if(temp==0){
        return false;
    }
//    memcpy(&(temp->data),&value,sizeof(bufferType));
    temp->data = value;
    temp->next = NULL;
    
    // if it is the first node
    if(head == NULL && tail == NULL) {
        // make both head and tail points to the new node
        head = tail = temp;
    } else {
        tail->next = temp;
        tail = temp;
    }
    return true;
}

bool sendTxObj() {
    if(head == NULL) return false;
    
    stackType *temp = head;
    head = head->next;
    
    if(head == NULL) tail = NULL;
    
    CanSend(&temp->data.obj,temp->data.value);
    free(temp);
        
    return true;
}

/*****************
 * SET FUNCTIONS *
 ****************/

/**
 * Sets the headlights lightning level
 * @param power The value ?0? means no headlights, the value ?50? means headlights low beam and 100 means headlights high beam
 */
void setLightFront(uint8_t power){
    if(power != carState.lightFront[0]){
        bufferType bufferObj;
        bufferObj.obj = defineTxMsgObj(ID_LIGHT_FRONT, CAN_DLC_1);
        bufferObj.value = carState.lightFront;
        if(pushTxObj(bufferObj)) carState.lightFront[0] = power;
    }
}

/**
 * Sets the backlights lightning level
 * @param power The value ?0? means no backlights, the value ?50? means backlights low beam and 100 means brake lights
 */
void setLightBack(uint8_t power) {
    if(power != carState.lightBack[0]){
        bufferType bufferObj;
        bufferObj.obj = defineTxMsgObj(ID_LIGHT_BACK, CAN_DLC_1);
        bufferObj.value = carState.lightBack;
        if(pushTxObj(bufferObj)) carState.lightBack[0] = power;
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
    if(hour != carState.time[0])    send = true;
    if(minutes != carState.time[1]) send = true;
    if(colon != carState.time[2])   send = true;
    if(send){
        bufferType bufferObj;
        bufferObj.obj = defineTxMsgObj(ID_TIME, CAN_DLC_3);
        bufferObj.value = carState.time;
        if(pushTxObj(bufferObj)) {
            carState.time[0] = hour;
            carState.time[1] = minutes;
            carState.time[2] = colon;
        }
    }
}

/**
 * Sets the current automatic gear level
 * Use this message change the current gear level. The value 0 means the wheels are not
driven by the motor (neutral behavior). The gear level 0 to 5 must be used for the drive
forward direction. The gear level 0 or 1 only must be used for the reverse direction. Cau-
tion, don't change more than one gear each time.
 * @param level The current gear level (0 to 5)
 */
void setGearLevel(uint8_t level){
    if(level != carState.gearLvl[0]){
        bufferType bufferObj;
        bufferObj.obj = defineTxMsgObj(ID_GEAR_LVL, CAN_DLC_1);
        bufferObj.value = carState.gearLvl;
        if(pushTxObj(bufferObj)) carState.gearLvl[0] = level;
    } 
}

/**
 * Used for quiet mode in laboratories
 * @param power The motor audio level (0 to 100%)
 * @param drive The wheels drive (0: no drive, 1: drive wheels)
 */
void setAudio(uint8_t power, bool drive){
    bool send = false;
    if(power != carState.audio[0])  send = true;
    if(drive != carState.audio[1])  send = true;
    if(send){
        bufferType bufferObj;
        bufferObj.obj = defineTxMsgObj(ID_AUDIO, CAN_DLC_2);
        bufferObj.value = carState.audio;
        if(pushTxObj(bufferObj)) {
            carState.audio[0] = power;
            carState.audio[1] = drive;
        }
    }
}

/**
 * To control the motor RPM
 * @param power The motor power applied (0 to 100%)
 * @param starter Starter (0 or 1), set to 1 when RPM is 0, else set to 0
 */
void setPowerMotor(uint8_t power, bool starter){
    bool send = false;
    uint8_t tmpPowerMotor[2];
    const uint8_t sensitivity = 5;
    if(power < carState.pwrMotor[0]-sensitivity)send = true;
    if(power > carState.pwrMotor[0]+sensitivity)send = true;
    if(starter != carState.pwrMotor[1])         send = true;
    if(send){
        bufferType bufferObj;
        bufferObj.obj = defineTxMsgObj(ID_PWR_MOTOR, CAN_DLC_2);
        bufferObj.value = carState.pwrMotor;
        if(pushTxObj(bufferObj)){
            carState.pwrMotor[0] = power;
            carState.pwrMotor[1] = starter;
        }
    }
}

/**
 * To control the car wheels brake
 * @param power The car brake factor applied (0 to 100%)
 */
void setPowerBrake(uint8_t power) {
    if(power != carState.pwrBrake[0]){
        bufferType bufferObj;
        bufferObj.obj = defineTxMsgObj(ID_POWER_BRAKE, CAN_DLC_1);
        bufferObj.value = carState.pwrBrake;
        if(pushTxObj(bufferObj)) carState.pwrBrake[0] = power;
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
    if(position != carState.autoSteering[0]) send = true;
    if(automatic != carState.autoSteering[1]) send = true;
    if(send){
        bufferType bufferObj;
        bufferObj.obj = defineTxMsgObj(ID_AUTO_STEERING, CAN_DLC_2);
        bufferObj.value = carState.autoSteering;
        if(pushTxObj(bufferObj)){
            carState.autoSteering[0] = position;
            carState.autoSteering[1] = automatic;
        }
    }
}


void defineMode(){
    switch(carState.gearSel[0]){
        case 'P':
            mode = 'P';
            setGearLevel(0);
            break;
        case 'R':
            mode = 'R';
            setGearLevel(0);
            break;
        case 'N':
            mode = 'N';
            setGearLevel(0);
            break;
        case 'D':
            mode = 'D';
            setGearLevel(0);
            break;
        default:
            mode = 'S';
            setGearLevel(0);
            break;
    }
}
