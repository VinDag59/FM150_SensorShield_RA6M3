// Number Conversions
/*
 * ascii_numbers.c
 *
 *  Created on: 25JUN2023
 *      Author: VIn D'Agostino, D'Agostino Industries Group, Inc.
 */


/***********************************************************************************************************************
 * Copyright [2020-2023] Renesas Electronics Corporation and/or its affiliates.  All Rights Reserved.
 *
 * This software and documentation are supplied by Renesas Electronics America Inc. and may only be used with products
 * of Renesas Electronics Corp. and its affiliates ("Renesas").  No other uses are authorized.  Renesas products are
 * sold pursuant to Renesas terms and conditions of sale.  Purchasers are solely responsible for the selection and use
 * of Renesas products and Renesas assumes no liability.  No license, express or implied, to any intellectual property
 * right is granted by Renesas. This software is protected under all applicable laws, including copyright laws. Renesas
 * reserves the right to change or discontinue this software and/or this documentation. THE SOFTWARE AND DOCUMENTATION
 * IS DELIVERED TO YOU "AS IS," AND RENESAS MAKES NO REPRESENTATIONS OR WARRANTIES, AND TO THE FULLEST EXTENT
 * PERMISSIBLE UNDER APPLICABLE LAW, DISCLAIMS ALL WARRANTIES, WHETHER EXPLICITLY OR IMPLICITLY, INCLUDING WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND NONINFRINGEMENT, WITH RESPECT TO THE SOFTWARE OR
 * DOCUMENTATION.  RENESAS SHALL HAVE NO LIABILITY ARISING OUT OF ANY SECURITY VULNERABILITY OR BREACH.  TO THE MAXIMUM
 * EXTENT PERMITTED BY LAW, IN NO EVENT WILL RENESAS BE LIABLE TO YOU IN CONNECTION WITH THE SOFTWARE OR DOCUMENTATION
 * (OR ANY PERSON OR ENTITY CLAIMING RIGHTS DERIVED FROM YOU) FOR ANY LOSS, DAMAGES, OR CLAIMS WHATSOEVER, INCLUDING,
 * WITHOUT LIMITATION, ANY DIRECT, CONSEQUENTIAL, SPECIAL, INDIRECT, PUNITIVE, OR INCIDENTAL DAMAGES; ANY LOST PROFITS,
 * OTHER ECONOMIC DAMAGE, PROPERTY DAMAGE, OR PERSONAL INJURY; AND EVEN IF RENESAS HAS BEEN ADVISED OF THE POSSIBILITY
 * OF SUCH LOSS, DAMAGES, CLAIMS OR COSTS.
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * This file is used with the contents of the "Serial_ASCII" module which is supplied for Renesas use by
 * D'Agostino Industries Group, Inc. It is part of a simple serial interface management system. "<module>_user" files
 * (.c and .h) are intended to be modified on an application by application basis, and <module> files (.c and .h) are
 * intended to be used as provided. <module_user.h is the only file required to be included in referencing files.
 **********************************************************************************************************************/


// includes
#include "Project.h"
#include "ASCII_numbers.h"


// modular variables


// external variables



// Code
uint8_t ConvertASCII2UINT16(const char * numstring, uint8_t _len, const char _terminator, volatile uint16_t * _number)
{
  uint8_t i;
  uint8_t error;
  uint8_t done;
  uint16_t value;
  
  
  error = 0;
  i = 0;
  value = 0;
  done = false;
  
  while ((i< _len) && (done != true) && (error == 0)) {
    if ((numstring[i] >= '0') && (numstring[i] <= '9')) { 
      value *=10;
      value += (uint16_t)(numstring[i] - 0x30);
      i++;
      if ((numstring[i] == _terminator) || (numstring[i] == 0)) {
        done = true;
      }
    }
    else {
      error = 1;
    }
  }
  
  if (error == 0) {
    *_number = value;
  }
  
  return error;
}


uint8_t ConvertASCII2Float(const char * numstring, uint8_t _len, const char _terminator, volatile float * _number)
{
  uint8_t i;
  uint8_t error;
  uint8_t done;
  float value;
  uint8_t decimalFound;
  uint8_t divisor;


  error = 0;
  i = 0;
  value = 0;
  done = false;
  decimalFound = false;
  divisor = 1;

  while ((i< _len) && (done != true) && (error == 0)) {
    if ((numstring[i] >= '0') && (numstring[i] <= '9')) {
      value *= 10;
      value += (uint16_t)(numstring[i] - 0x30);
      if (decimalFound == true) {
          divisor *= 10;
      }
      i++;
      if ((numstring[i] == _terminator) || (numstring[i] == 0)) {
        done = true;
      }
    }
    else if (numstring[i] == '.') {
        decimalFound = true;
    }
    else {
      error = 1;
    }
  }

  if (error == 0) {
    *_number = value / divisor;
  }

  return error;
}
