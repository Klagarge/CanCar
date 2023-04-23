#include "liveFct.h"
#include "carLimites.h"

static uint16_t lastLowRPM = 0;
static bool increasedRPM = true;

/**
 * Change actual gear level
 * @param up if true, add one gear level
 * @return true if it change
 */
bool rtChangeGearLevel (bool up, bool accel){
    uint8_t lastGearLevel = carState.gearLvl[0];
    if(up){
        /******
         * ++ *
         *****/
        if(lastGearLevel >= 5) return false;
        setGearLevel(lastGearLevel+1);
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
    static uint16_t lastHightRPM = 0;
    static bool reducedRPM = true;
    const uint8_t sen = 10; // perfect value = 5, but put 10 for more fun
    
    if((rpm < lastHightRPM-100) || (rpm==0)) reducedRPM = true;
    
    if(/*rpm > RPM_LOW*/ accel >= 5){
        // Make an hysteresis for change power motor
        if((accel-sen) > lastAccel) lastAccel += 2*sen;
        if((accel+sen) < lastAccel) lastAccel -= 2*sen;
        
        if(rpm >= RPM_HIGH){
            // Reduce power motor if too many RPM and change GearLever if D mode
            lastAccel -= 2*sen;
        } else if(mode=='D'){
            // Reduce GearLevel in D mode if not enough
            if((rpm<RPM_CHANGE_LOW) && increasedRPM){
                increasedRPM = false;
                lastLowRPM = rpm;
                rtChangeGearLevel(false, true);
            }
            // Increse GearLevel in D mode if RPM are too high
            if((rpm > RPM_CHANGE_HIGH) && reducedRPM){
                reducedRPM = false;
                lastHightRPM = rpm;
                rtChangeGearLevel(true, true);
            }
            if(speed == 0) setGearLevel(0);
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
    
    if(rpm>lastLowRPM+100) increasedRPM = true;
        
    if (rtBrake(brake)){
        /*********
         * BRAKE *
         ********/
        
        // not enough RPM -> reduce GearLevel
        if((mode == 'D') && (rpm<RPM_CHANGE_LOW) && increasedRPM){
            // If D mode, multiple gearLevel to reduce
            increasedRPM = false;
            lastLowRPM = rpm;
            rtChangeGearLevel(false, false);
        } else if((mode != 'D') && (rpm < RPM_LOW)) rtChangeGearLevel(false, false);
        
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