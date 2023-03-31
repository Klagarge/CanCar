/*
 * File:   car.c
 * Author: remi.heredero
 *
 * Created on March 28, 2023, 3:18 PM
 */

#include "car.h"

CAR_STATE carState;

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

void setLightFront(uint8_t power){
    if(power != carState.lightFront[0]){
        carState.lightFront[0] = power;
        bufferType bufferObj;
        bufferObj.obj = defineTxMsgObj(ID_LIGHT_FRONT, CAN_DLC_1);
        bufferObj.value = carState.lightFront;
        pushTxObj(bufferObj);
    }
}

void pushTxObj(bufferType value) {
    stackType *temp = (stackType*) malloc(sizeof(stackType));
    temp->data = value;
    temp->next = head;
    head = temp;
}

bool sendTxObj() {
    if(head == NULL) return false;
    
    stackType *temp = head;
    head = head->next;
    
    CanSend(&temp->data.obj,temp->data.value);
    free(temp);
        
    return true;
}