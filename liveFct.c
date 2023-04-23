#include "liveFct.h"
#include "carLimites.h"

/**
 * Change actual gear level
 * @param up if true, add one gear level
 * @return true if it change
 */
bool rtChangeGearLevel (bool up, bool accel, uint16_t rpm){
    static uint16_t lastRPM = 0;
    static bool reducedRPM = true;
    uint8_t lastGearLevel = carState.gearLvl[0];
    if(up){
        /******
         * ++ *
         *****/
        if(lastGearLevel >= 5) return false;
        
        if(reducedRPM){
            reducedRPM = false;
            lastRPM = rpm;
            setGearLevel(lastGearLevel+1);
        }
        if((rpm < lastRPM-100) || (rpm == 0)) {
            reducedRPM = true;
        }
    } else {
        /******
         * -- *
         *****/
        if(lastGearLevel <= 0) return false;
        if(accel && (lastGearLevel <= 1)) return false;
        setGearLevel(lastGearLevel-1);
    }
    return true;
}

bool rtBrake(uint8_t brake) {
    if(brake >= 5){
        setLightBack(100);
        setPowerMotor(0, false);
        setPowerBrake(brake);
        return true;
    } else {
        setLightBack(50);
        return false;
    }
}

bool rtAccel(uint8_t accel, uint16_t rpm, int16_t speed) {
    static int8_t lastAccel = 0;
    const uint8_t sen = 10; // perfect value = 5, but put 10 for more fun
    
    if(/*rpm > RPM_LOW*/ accel >= 5){
        // Make an hysteresis for change power motor
        if((accel-sen) > lastAccel) lastAccel += 2*sen;
        if((accel+sen) < lastAccel) lastAccel -= 2*sen;
        
        if(rpm >= RPM_HIGH){
            // Reduce power motor if too many RPM and change GearLever if D mode
            lastAccel -= 2*sen;
        } else if((rpm <= RPM_CHANGE_LOW) && (mode == 'D')){
            // Reduce GearLevel in D mode if not enough 
            rtChangeGearLevel(false, true, rpm);
        } else if((mode=='D') && (rpm >= RPM_CHANGE_HIGH)){
            rtChangeGearLevel(true, true, rpm);
        } else if(mode == 'D' && (speed == 0)){
            setGearLevel(1);
        }
        
        // Fix limit out of range
        if(lastAccel <= PM_MIN) lastAccel = PM_MIN;
        if(lastAccel > 100) lastAccel = 100;
        
        if(rpm == 0) setPowerMotor(lastAccel, true);
        else setPowerMotor(lastAccel, false);
        return true;
    } else {
        
        return false;
    }
}

void rtManageMotor(uint8_t brake, uint8_t accel) {
    static uint8_t lastAccel = 0;
    
    uint16_t rpm = carState.motorStatus[0];
    rpm = (rpm<<8) + carState.motorStatus[1];
    int16_t speed = carState.motorStatus[2];
    speed = (speed<<8) + carState.motorStatus[3];
        
    if (rtBrake(brake)){
        /*********
         * BRAKE *
         ********/
        
        // not enough RPM -> reduce GearLevel
        if(rpm < RPM_LOW) rtChangeGearLevel(false, false, rpm);
        
    } else if (rtAccel(accel, rpm, speed)) {
        /**********************
         * NO BRAKE but ACCEL *
         *********************/
        
        // mode R & D without brake
        if((mode == 'R') || (mode=='D')) setPowerBrake(0);

        // mode R with gearLevel at 1
        if(mode == 'R') setGearLevel(1);
            
    } else {
        /***********************
         * NO BRAKE & NO ACCEL *
         **********************/
            
        if((mode == 'R') || (mode=='D')){
            
            if(speed <= 5) setPowerBrake(100);

            // gearLevel at 0 if RPM is low in R & D mode
            if(rpm <= RPM_LOW) setGearLevel(0);
        } else {
            // if P or N mode, no brake
            setPowerBrake(0);
        }

        // if rpm is 0, start and stop function
        if(rpm == 0) setPowerMotor(PM_MIN, true);

    }
}