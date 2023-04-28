#include "liveFct.h"
#include "carLimites.h"

static uint16_t lastLowRPM = 0;
static bool increasedRPM = true;

enum changeGL{
    UP,
    DOWNto0,
    DOWNto1
};

bool rtMotorOff(uint16_t rpm){
    if(rpm >= 100) return false;
    if(carState.pwrMotor[0] > 0) return false;
    if(carState.pwrMotor[1] == true) return false;
    if(!stop) return false;
    return true;
}
bool rtMotorOn(uint16_t rpm){
    if(rpm <= 400) false;
    if(stop) return false;
    return true;
}

/**
 * Change actual gear level
 * @param up if true, add one gear level
 * @return true if it change
 */
bool rtChangeGearLevel (enum changeGL type){
    uint8_t lastGearLevel = carState.gearLvl[0];
    switch(type){
        case UP:
            /******
             * ++ *
             *****/
            if(lastGearLevel >= 5) return false;
            setGearLevel(lastGearLevel+1);
            break;
        case DOWNto0:
            if(lastGearLevel <= 0){
                stop = true;
                start = false;
                return false;
            }
            setGearLevel(lastGearLevel-1);
            break;
        case DOWNto1:
            if(lastGearLevel <= 1) return false;
            setGearLevel(lastGearLevel-1);
            break;
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
    static uint8_t lastAccel = 0;
    static uint16_t lastHightRPM = 0;
    static bool reducedRPM = true;
    const uint8_t sen = 5; // perfect value = 5, but put 10 for more fun
    
    if((rpm < lastHightRPM-100) || (rpm==0)) reducedRPM = true;
    
    if(accel >= 5){
        // Make an hysteresis for change power motor
        if((accel-sen) > lastAccel+sen) lastAccel += 2*sen;
        if((accel+sen) < lastAccel-sen) lastAccel -= 2*sen;
        
        if((rpm >= RPM_HIGH) || (speed >= SPEED_MAX)){
            // Reduce power motor if too many RPM and change GearLever if D mode
            lastAccel -= 2*sen;
        }
        if((rpm >= RPM_HIGH+500) || (speed >= SPEED_MAX+10)){
            // Reduce power motor if too many RPM and change GearLever if D mode
            lastAccel -= 4*sen;
        }
        if(mode=='D'){
            // Reduce GearLevel in D mode if not enough
            if((rpm<RPM_CHANGE_LOW) && increasedRPM){
                increasedRPM = false;
                lastLowRPM = rpm;
                rtChangeGearLevel(DOWNto1);
            }
            // Increase GearLevel in D mode if RPM are too high
            if((rpm > RPM_CHANGE_HIGH) && reducedRPM){
                reducedRPM = false;
                lastHightRPM = rpm;
                rtChangeGearLevel(UP);
            }
            if(speed <= 0){
                if(rpm <= RPM_LOW-200) {
                    setGearLevel(0);
                    stop = true;
                    start = false;
                } else {
                    lastAccel += 2*sen;
                }
            }
        }
        
        // Fix limit out of range
        if(lastAccel <= PM_MIN) lastAccel = PM_MIN;
        if(lastAccel > 100) lastAccel = 100;
        
        if(rtMotorOff(rpm)) setPowerMotor(lastAccel, true);
        else if(rtMotorOn(rpm)) setPowerMotor(lastAccel, false);
        
        return true;
    } else if(carState.tempomat[0]){
        uint8_t speedTarget = carState.tempomat[1];
        if(speed < speedTarget) lastAccel += 1;
        if(speed > speedTarget) lastAccel -= 1;
        
        if((rpm >= RPM_HIGH) || (speed >= SPEED_MAX)){
            // Reduce power motor if too many RPM and change GearLever if D mode
            lastAccel -= 2*sen;
        }
        
        // Fix limit out of range
        if(lastAccel <= PM_MIN) lastAccel = PM_MIN;
        if(lastAccel > 100) lastAccel = 100;
        
        if(rtMotorOff(rpm)) setPowerMotor(lastAccel, true);
        else if(rtMotorOn(rpm)) setPowerMotor(lastAccel, false);
        
    } else {
        return false;
    }
}

void rtManageMotor(uint8_t brake, uint8_t accel) {
    static uint8_t lastAccel = 0;
    static bool wasGL0 = true;
    
    uint16_t rpm = carState.motorStatus[0];
    rpm = (rpm<<8) + carState.motorStatus[1];
    int16_t speed = carState.motorStatus[2];
    speed = (speed<<8) + carState.motorStatus[3];
    
    if(rpm>lastLowRPM+100) increasedRPM = true;
    if(rpm>100) stop = false;
    if(carState.gearLvl[0] == 0) wasGL0 = true;
        
    if (rtBrake(brake)){
        /*********
         * BRAKE *
         ********/
        
        // not enough RPM -> reduce GearLevel
        if((mode == 'D') && (rpm<RPM_CHANGE_LOW) && increasedRPM){
            // If D mode, multiple gearLevel to reduce
            increasedRPM = false;
            lastLowRPM = rpm;
            rtChangeGearLevel(DOWNto1);
        } else if((rpm < RPM_LOW)) rtChangeGearLevel(DOWNto0);
        
    } else if (rtAccel(accel, rpm, speed)) {
        /**********************
         * NO BRAKE but ACCEL *
         *********************/
        
        // mode R & D without brake
        if((mode == 'R') || (mode=='D')) setPowerBrake(0);

        // mode R with gearLevel at 1
        if(mode == 'R') setGearLevel(1);
        if((mode=='D') && wasGL0){
            wasGL0 = false;
            setGearLevel(1);
        }
            
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
        
        if((mode == 'D') && (rpm<RPM_CHANGE_LOW) && increasedRPM){
            // If D mode, multiple gearLevel to reduce
            increasedRPM = false;
            lastLowRPM = rpm;
            rtChangeGearLevel(DOWNto1);
        }

        // if rpm is 0, start and stop function
        if(rtMotorOff(rpm)) setPowerMotor(PM_MIN, true);
        else setPowerMotor(PM_MIN,false);
    }
}

void rtManageWheel(){
    uint8_t right = carState.frontSensReq[1];
    uint8_t left = carState.frontSensReq[0];
    const uint8_t wallDist = 50;
    int16_t speed = carState.motorStatus[2];
    speed = (speed<<8) + carState.motorStatus[3];
    const uint8_t speedFactor = 3;//speed/20;
    if((right >= wallDist) && (left >= wallDist)) return;
    uint8_t delta;
    
    if(right > left){
        /********************
         * Wall on the left *
         *******************/
        if(right > wallDist) right = wallDist;
        if(left > wallDist) left = wallDist;
        delta = (wallDist-left) - (wallDist-right); 
        setAutoSteering(delta*speedFactor, true);
        
    } else if (left > right){
        /*********************
         * Wall on the right *
         ********************/
        if(right > wallDist) right = wallDist;
        if(left > wallDist) left = wallDist;
        delta = (wallDist-right) - (wallDist-left);
        setAutoSteering(-(delta*speedFactor), true);
        
    } else {
        /*****************************
         * Wall on the fucking front *
         ****************************/
        
    }
}

void rtOdometer(){
    static const double ratio = 0.00277777777777777777777777777778;
    static float total = 0;
    int16_t speed = carState.motorStatus[2];
    speed = (speed<<8) + carState.motorStatus[3];
    if(speed == 0) return;
    speed = abs(speed);
    total += (float)(speed*ratio);
    if(total >= 100){ 
        total = 0;
        setKmPulse();
    }
}

void rtClock(){
    static uint8_t count = 0;
    if(++count < 100) return;
    count = 0;
    
    static uint8_t s = 0;
    uint8_t h = carState.time[0];
    uint8_t m = carState.time[1];
    bool c = carState.time[2];
    
    c = c ? false:true;
    if(++s >= 60){
        s = 0;
        m++;
    }
    if(m >= 60){
        m = 0;
        h++;
    } 
    if(h >= 24){
        h = 0;
    }
    setTime(h, m, c);
}