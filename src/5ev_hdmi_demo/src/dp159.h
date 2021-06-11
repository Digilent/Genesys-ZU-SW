/******************************************************************************
*
* Copyright (C) 2018 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/

#ifndef DP159_H		 /* prevent circular inclusions */
#define DP159_H		 /* by using protection macros */

#include "xil_types.h"
#include "xvphy.h"

#define I2C_DP159_ZOMBIE_ADDR 	0x2C
#define I2C_DP159_ES_ADDR 		0x5E

// Function prototypes
u32 i2c_dp159(XVphy *VphyPtr, u8 QuadId, u64 TxLineRate);
u32 i2c_dp159_write(u8 dev, u8 addr, u8 dat);
void i2c_buffer_dump(uint8_t I2C_IC_Addr);
void i2c_buffer_clear_ber(uint8_t I2C_IC_Addr);
void i2c_buffer_enable_verify_datapath(uint8_t I2C_IC_Addr);
uint16_t i2c_buffer_lane_ber(uint8_t I2C_IC_Addr, uint8_t LaneNr);

#endif
