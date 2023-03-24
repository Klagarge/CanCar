/*
 * File:   spi.c
 * Author: pascal.sartoret
 *
 * Created on 8. january 2021
 */

//------------------------------------------------------------------------------
// interrupts usage:
// mikroBus 1 : INT0
// mikrobus 2 : INT1 or IOC1 or IOC5 (to select with PPS lite)
// mikrobus 3 : IOC4 or IOC0/conflict with touch (to select with PPS lite)
// mikrobus 4 : INT3 or IOC3 or IOC7 (to select with PPS lite)
// mikrobus 5 : INT2 or IOC2 or IOC6 (to select with PPS lite)
//------------------------------------------------------------------------------
  //----------------------------------------------------------------------------
  // inits SSP2 for SPI communication (mikcro 1,2,3) or SSSP1 (mikro 4,5)
  // caution, SSP1 is already used for the touchscreen
  // ---------------------------------------------------------------------------

#include <xc.h>

#include <stdint.h>

#include "mcc_generated_files/pin_manager.h"

volatile uint8_t * ptrCS_SPI;
uint8_t csBitNr;

uint8_t  mikroNr=0;

int8_t SpiTransfer(uint8_t * txPtr, uint8_t * rxPtr, uint16_t size) {
  volatile uint8_t dummy;
  CS_SetLow();
  while(size > 0) {
    //SSP2STATbits.BF = 0;        // clear transfer flag
    dummy = SSP2BUF;
    SSP2BUF = *txPtr;           // send data
    while(SSP2STATbits.BF == 0){}
    *rxPtr = SSP2BUF;           // get received data
    
    txPtr++;                      // increment tx pointer
    rxPtr++;                      // increment rx pointer
    size--;                       // decrement bytes counter
  };
  CS_SetHigh();                    // deactivate chip select
  return 0;
}