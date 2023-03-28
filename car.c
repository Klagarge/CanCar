/*
 * File:   car.c
 * Author: remi.heredero
 *
 * Created on March 28, 2023, 3:18 PM
 */

#include "car.h"

CAR_STATE carState;

void setLightFront(uint8_t power){
    if(power != carState.lightFront[0]){
        carState.lightFront[0] = power;
        bufferType bufferObj;
        //bufferObj.obj = ...;
        bufferObj.value = carState.lightFront;
        addOnBufferTxObj(bufferObj);
    }
}
