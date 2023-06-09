#include "eventCar.h"
#include "carLimites.h"

void evtStart(){
    setPowerMotor(PM_MIN, true);
    setLightFront(100);
    setLightBack(50);
    setAudio(50, true);
    mode = 'P';
}
void evtStop(){
    mode = 'S';
    carState.gearSel[0] = 'P'; // !!!!!!!! Should not written, due to a big failure on the car design
    setGearLevel(0); 
    setPowerMotor(0, false);
    setLightFront(0);
    setLightBack(0);
    setAudio(0, false);
}

void evtRaceMode(){
    if((carState.race[0] == 2) && !raceModeOn){
        raceModeOn = true;
        setAutoSteering(0, true);
    } else if(raceModeOn) {
        raceModeOn = false;
        setAutoSteering(0, false);   
    }
}
