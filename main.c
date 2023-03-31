#include "mcc_generated_files/mcc.h"
#include "can.h"
#include "car.h"
void copyArray(uint8_t *tmpData, uint8_t *persistantData, uint8_t size);
bool checkAndCopyArray(uint8_t *tmpData, uint8_t *persistantData, uint8_t size);
void timerDone();
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
    
    
    TMR0_SetInterruptHandler(timerDone);
    CanInit(1, CAN_250K_1M);
    
//    uint8_t idCar = 0x8;
    CS_SetHigh();
    

    CAN_TX_MSGOBJ foo;
    foo.bF.ctrl.RTR = 0;
    foo.bF.id.SID11 = 0;
    foo.bF.ctrl.FDF = 0;
    foo.bF.ctrl.IDE = 0;
    foo.bF.ctrl.BRS = 0;
    foo.bF.ctrl.ESI = 0;
    foo.bF.ctrl.DLC = CAN_DLC_1; // number of byte to send
    CAN_RX_MSGOBJ rxObj;
    uint8_t txd[1];
    uint8_t rxd[MAX_DATA_BYTES];
    
    CAN_FILTEROBJ_ID fObj;
    fObj.ID = idCar;              // standard filter 11 bits value
    fObj.SID11 = 0;               // 12 bits only used in FD mode
    fObj.EXIDE = 0;               // assign to standard identifiers
    
    CAN_MASKOBJ_ID mObj;
    mObj.MID = 0xF;             // check all the 11 bits in standard ID
    mObj.MSID11 = 0;              // 12 bits only used in FD mode
    mObj.MIDE = 1;                // match identifier size in filter
    CanSetFilter(CAN_FILTER0,&fObj,&mObj);
    
    timeToSendToCan = false;
    TMR0_StartTimer();
        
        
    while (1) {
        
        if(timeToSendToCan){
            timeToSendToCan = false;
            
            //foo.bF.id.ID = ((0x11 << 4) | idCar);
            //CanSend(&foo, txd);
            // Pop stack with Can Send          
            while(sendTxObj()); // send all the stack
        }
        
        if(CanReceive(&rxObj,rxd) == 0) {
            switch(rxObj.bF.id.ID>>4){
                
                case ID_TEMPOMAT:
                    if(!checkAndCopyArray(rxd, carState.tempomat, rxObj.bF.ctrl.DLC)){
                        // make stuff
                    }
                    break;
                case ID_ACCEL_PEDAL:
                    if(!checkAndCopyArray(rxd, carState.accelPedal, rxObj.bF.ctrl.DLC)){
                        setLightFront(carState.accelPedal[0]);
                    }
                    break;
                 
                
            };
            /*
            if(rxObj.bF.id.ID == ((0x7 << 4) | idCar)) {
              foo.bF.id.ID = ((0x11 << 4) | idCar);
              // RTR + DLC
              txd[0] = rxd[0];
            }
             */
        }
    }
    
    
    /* TODO
     * Add Timer0 @ 10ms for call CanSend
     * add function for sending stuff with static local variable for ckeck if same than previous send
     */
}
void timerDone(){
    timeToSendToCan = true;
    TMR0_Reload();
    TMR0_StartTimer();
    
}

void copyArray(uint8_t *tmpData, uint8_t *persistantData, uint8_t size){
    for(uint8_t i = 0; i < size; i++){
        persistantData[i] = tmpData[i];
    }
}

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