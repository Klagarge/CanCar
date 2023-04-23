#include "eventCar.h"

void evtStart(){
    setPowerMotor(12, true);
    setLightFront(100);
    setLightBack(50);
    setAudio(50, true);
    mode = 'P';
}
void evtStop(){
    mode = 'S';
    carState.gearLvl[0] = 'P'; // !!!!!!!! Should not written, due to a big failure on the car design
    setGearLevel(0); 
    setPowerMotor(0, false);
    setLightFront(0);
    setLightBack(0);
    setAudio(0, false);
}
