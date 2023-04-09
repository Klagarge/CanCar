#include "mcc_generated_files/mcc.h"
#include "can.h"
#include "car.h"

// functions prototype
void copyArray(uint8_t *tmpData, uint8_t *persistantData, uint8_t size);
bool checkAndCopyArray(uint8_t *tmpData, uint8_t *persistantData, uint8_t size);
void timerDone();
void canReceive();
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
    
    initCar();
   
        
    while (1) {
        
        if(timeToSendToCan){ // if flag is on
            timeToSendToCan = false;         
            while(sendTxObj()); // send all the stack
        }
        
        canReceive(); // handle the CAN messages reception
    }
}

// handle the CAN messages reception
// store the car data in registers
void canReceive()
{
    CAN_RX_MSGOBJ rxObj;
    uint8_t rxd[MAX_DATA_BYTES];
    
            if(CanReceive(&rxObj,rxd) == 0) {
            switch(rxObj.bF.id.ID>>4){
                
                case ID_TEMPOMAT:
                    if(!checkAndCopyArray(rxd, carState.tempomat, rxObj.bF.ctrl.DLC)){
                        // make stuff
                    }
                    break;
                 
                case ID_GEAR_SEL:
                    if(!checkAndCopyArray(rxd, carState.gearSel, rxObj.bF.ctrl.DLC)){
                        //if(carState.contactKey[0]) defineMode();
                    }
                    break;
                    
                case ID_FRONT_SENS_REQ:  
                    if(!checkAndCopyArray(rxd, carState.frontSensReq, rxObj.bF.ctrl.DLC));
                    break;
                
                case ID_MOTOR_STATUS:
                    if(!checkAndCopyArray(rxd, carState.motorStatus, rxObj.bF.ctrl.DLC));
                    break;
                    
                case ID_BRAKE_PEDAL:
                    if(!checkAndCopyArray(rxd, carState.brakePedal, rxObj.bF.ctrl.DLC));
                    break;
                    
                case ID_ACCEL_PEDAL:
                     if(!checkAndCopyArray(rxd, carState.accelPedal, rxObj.bF.ctrl.DLC)){
                        //setLightFront(carState.accelPedal[0]); // set front light with accel pedal data
                         pushFunction(setLightFront(carState.accelPedal[0]));
                    }                   
                    break;
                    
                case ID_CONTACT_KEY:
                    if(!checkAndCopyArray(rxd, carState.contactKey, rxObj.bF.ctrl.DLC)){
                        if(carState.contactKey[0]) {
                            //start();
                        } else {
                            //stop();
                        }
                    }
                    break;
                
                case ID_STEERING_W_REQ:  
                    if(!checkAndCopyArray(rxd, carState.steeringWReq, rxObj.bF.ctrl.DLC));
                    break;
                    
                case ID_BROKEN_CAR:
                    if(!checkAndCopyArray(rxd, carState.brokenCar, rxObj.bF.ctrl.DLC));
                    break;
                    
                case ID_BAD_MESSAGE:
                    if(!checkAndCopyArray(rxd, carState.badMessage, rxObj.bF.ctrl.DLC));
                    break;
                    
                case ID_SLOPE_REQ:
                    if(!checkAndCopyArray(rxd, carState.slopeReq, rxObj.bF.ctrl.DLC));
                    break;
                    
                case ID_RACE:
                    if(!checkAndCopyArray(rxd, carState.race, rxObj.bF.ctrl.DLC));
                    break;
                    
                case ID_CAR_ID:
                    if(!checkAndCopyArray(rxd, carState.carID, rxObj.bF.ctrl.DLC));
                    break;
                    
            };

        }
}

// Called on timer0 interrupt
void timerDone(){
    timeToSendToCan = true; // activate flag
    
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