/*
 * Scheduler.h
 *
 *  Created on: 25JUN2023
 *      Author: VIn D'Agostino, D'Agostino Industries Group, Inc.
 */


#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_


// Private Define (PD) section:

// scheduler stuff
#define TEN_mS_SEED         10
#define TWENTYFIVE_mS_SEED  25
#define ONEHUNDRED_mS_SEED  100
#define ONE_S_SEED          1000
#define I2C_TIMEOUT_INIT    0


// Public Variable (PuV) section:

// scheduler stuff
extern volatile uint8_t ten_mS_Flag;
extern volatile uint8_t twentyfive_mS_Flag;
extern volatile uint8_t hundred_mS_Flag;
extern volatile uint8_t one_S_Flag;
extern volatile uint8_t i2c_timeout_Flag;
extern volatile uint8_t sendReport_Flag;

// prototypes
void SysTick_Handler(void);
void SetI2cTimeout(uint16_t _timeout);
    
    
// ======================================
// add to main.c (not as comments)


// In the main loop section (just after "while(1) {")

//    //---------------------------------
//    // 10mS Tasks 
//    if (ten_mS_Flag) {
//      ten_mS_Flag = false;
//
//    }  // end of 10mS Tasks
//    //---------------------------------
//
//    
//    //---------------------------------
//    // 25mS Tasks 
//    if (twentyfive_mS_Flag) {
//      twentyfive_mS_Flag = false;
//
//    }  // end of 25mS Tasks
//    //---------------------------------
//
//    
//    //---------------------------------
//    // 100mS Tasks 
//    if (hundred_mS_Flag) {
//      hundred_mS_Flag = false;
//
//    }  // end of 100mS Tasks
//    //---------------------------------
//
//    
//    //---------------------------------
//    // 1 Sec Tasks 
//    if (one_S_Flag) {
//      one_S_Flag = false;
//      
//    } // end of 1Sec Tasks
//    //---------------------------------
//
//    
//    //---------------------------------
//    // Every time through the loop
//    
//    // end Every time through the loop
//    //---------------------------------
//    
    

#endif
