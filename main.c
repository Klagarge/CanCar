#include "mcc_generated_files/mcc.h"
#include "can.h"
#include "carInterface.h"
#include "eventCar.h"
#include "liveFct.h"
#include "carLimites.h"

// functions prototype
void copyArray(uint8_t *tmpData, uint8_t *persistantData, uint8_t size);
bool checkAndCopyArray(uint8_t *tmpData, uint8_t *persistantData, uint8_t size);
void timerDone();
void canReceive();
void periodicCall();
bool timeToSendToCan;

/*
                         Main application
 */
void main(void) {
    // Initialize the device
    SYSTEM_Initialize();

    // If using interrupts in PIC18 High/Low Priority Mode you need to enable the Global High and Low Interrupts
    // If using interrupts in PIC Mid-Range Compatibility Mode you need to enable the Global and Peripheral Interrupts
    // Use the following macros to:

    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();

    // Disable the Global Interrupts
    //INTERRUPT_GlobalInterruptDisable();

    // Enable the Peripheral Interrupts
    INTERRUPT_PeripheralInterruptEnable();

    // Disable the Peripheral Interrupts
    //INTERRUPT_PeripheralInterruptDisable();
    
    
    TMR0_SetInterruptHandler(timerDone); // timerDone is called on timer0 interrupt
    timeToSendToCan = false; // initialise flag
    TMR0_StartTimer(); // start timer0
    
    // initialise CAN
    CanInit(1, CAN_250K_1M);
    CS_SetHigh(); // chip select
    
    // create and set a filter to CAN driver
    CAN_FILTEROBJ_ID fObj;
    fObj.ID = idCar;              // standard filter 11 bits value
    fObj.SID11 = 0;               // 12 bits only used in FD mode
    fObj.EXIDE = 0;               // assign to standard identifiers
    
    CAN_MASKOBJ_ID mObj;
    mObj.MID = 0xF;             // check all the 11 bits in standard ID
    mObj.MSID11 = 0;              // 12 bits only used in FD mode
    mObj.MIDE = 1;                // match identifier size in filter
    CanSetFilter(CAN_FILTER0,&fObj,&mObj);
    
    carState.accelPedal[0] = 0;
    carState.brakePedal[0] = 0;
    carState.motorStatus[0] = 0;
    carState.motorStatus[1] = 0;
    carState.motorStatus[2] = 0;
    carState.motorStatus[3] = 0;
    carState.tempomat[0] = 0;
    raceModeOn = false;
    carState.time[0] = 0;
    carState.time[1] = 0;
    carState.time[2] = 0;
   
        
    while (1) {
        
        if(timeToSendToCan){ // if flag is on
            timeToSendToCan = false;         
            sendTxObj();
        }
        canReceive(); // handle the CAN messages reception
        periodicCall();
        
    }
}

// handle the CAN messages reception
// store the car data in registers
void canReceive() {
    CAN_RX_MSGOBJ rxObj;
    uint8_t rxd[MAX_DATA_BYTES];
    
    if(CanReceive(&rxObj,rxd) == 0) {
        switch(rxObj.bF.id.ID>>4){

            case ID_TEMPOMAT:
                checkAndCopyArray(rxd, carState.tempomat, rxObj.bF.ctrl.DLC);
                break;

            case ID_GEAR_SEL:
                if(!checkAndCopyArray(rxd, carState.gearSel, rxObj.bF.ctrl.DLC)){
                    if(carState.contactKey[0]) defineMode();
                }
                break;

            case ID_FRONT_SENS_REQ:  
                if(!checkAndCopyArray(rxd, carState.frontSensReq, rxObj.bF.ctrl.DLC)){
                    
                }
                break;

            case ID_MOTOR_STATUS:
                checkAndCopyArray(rxd, carState.motorStatus, rxObj.bF.ctrl.DLC);
                break;

            case ID_BRAKE_PEDAL:
                if(!checkAndCopyArray(rxd, carState.brakePedal, rxObj.bF.ctrl.DLC)){
                    setTempoOff();
                    carState.tempomat[0] = 0;
                }
                break;

            case ID_ACCEL_PEDAL:
                checkAndCopyArray(rxd, carState.accelPedal, rxObj.bF.ctrl.DLC);
                break;

            case ID_CONTACT_KEY:
                if(!checkAndCopyArray(rxd, carState.contactKey, rxObj.bF.ctrl.DLC)){
                    if(carState.contactKey[0]) {
                        evtStart();
                    } else {
                        evtStop();
                    }
                }
                break;

            case ID_STEERING_W_REQ:  
                if(!checkAndCopyArray(rxd, carState.steeringWReq, rxObj.bF.ctrl.DLC)){
                    
                }
                break;

            case ID_BROKEN_CAR:
                if(!checkAndCopyArray(rxd, carState.brokenCar, rxObj.bF.ctrl.DLC)){
                    switch(carState.brokenCar[0]){
                        case 1:
                            // Starter activated with RPM not 0
                            break;
                        case 2:
                            // RPM too high (the motor RPM was over 8000 RPM)
                            break;
                        case 3:
                            // Speed is too high (the car speed was over 280 km/h)
                            break;
                        case 4:
                            // The gear selection is wrong (gear in P or N, delta gear > 2)
                            break;
                        case 5:
                            // Power motor activated without starter
                            break;
                        case 6:
                            //On race mode, car hits a wall
                            break;
                        case 7:
                            // The race is finished
                            break;
                    }
                    while(1);
                }
                break;

            case ID_BAD_MESSAGE:
                if(!checkAndCopyArray(rxd, carState.badMessage, rxObj.bF.ctrl.DLC)){
                    switch(carState.badMessage[0]){
                        case 0:
                            // The length of the message is wrong
                            break;
                        case 1:
                            // The value of a parameter is out of bounds
                            break;
                        case 2:
                            // The message identifier is unknown
                            break;
                    }
                }
                break;

            case ID_RACE:
                if(!checkAndCopyArray(rxd, carState.race, rxObj.bF.ctrl.DLC)){
                    evtRaceMode();
                }
                break;

            case ID_CAR_ID:
                if(!checkAndCopyArray(rxd, carState.carID, rxObj.bF.ctrl.DLC)){
                    
                }
                break;
        };
    }
}

void periodicCall(){
    if(mode != 'S'){
        rtManageMotor(carState.brakePedal[0], carState.accelPedal[0]);
    }
    if(raceModeOn) rtManageWheel();
    if(tick) {
        tick = false;
        rtOdometer();
        rtClock();
    }
    
}

// Called on timer0 interrupt
void timerDone(){
    timeToSendToCan = true; // activate flag
    tick = true; // Tick every 10 ms
    
    // reload timer
    TMR0_Reload();
    TMR0_StartTimer();
}

// copy tmpData in persistantData
void copyArray(uint8_t *tmpData, uint8_t *persistantData, uint8_t size){
    for(uint8_t i = 0; i < size; i++){
        persistantData[i] = tmpData[i];
    }
}

// if two arrays are different it copies tmpData in persistantData 
bool checkAndCopyArray(uint8_t *tmpData, uint8_t *persistantData, uint8_t size) {
    bool same = true;
    for(uint8_t i = 0; i < size; i++){
        if(tmpData[i] != persistantData[i]){
            same = false;
            break;
        }
    }
    
    if(!same){
        copyArray(tmpData, persistantData, size);
    }
    
    return same;
}
/**
 End of File
*/