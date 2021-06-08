/*
 * tmds181.h
 *
 *  Created on: Oct 26, 2020
 *      Author: icatuna
 */

#ifndef TMDS181_H_
#define TMDS181_H_

#include "xil_types.h"
#include "xvphy.h"

#define I2C_TMDS181_ADDR 		0x5C

// Function prototypes
u32 i2c_tmds181(XVphy *VphyPtr, u8 QuadId);
u32 i2c_tmds181_write(u8 addr, u8 dat);
u8 i2c_tmds181_read(u8 addr);

#endif /* TMDS181_H_ */
