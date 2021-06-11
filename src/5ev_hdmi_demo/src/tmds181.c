/*
 * tmds181.c
 *
 *  Created on: Oct 26, 2020
 *      Author: icatuna
 */

#include "tmds181.h"
#include "sleep.h"
#include "xiic.h"

#define TMDS_181_VERBOSE      0

// I2C TMDS181 write
u32 i2c_tmds181_write(u8 addr, u8 dat)
{
  u32 r;
  u8 buf[2];

  buf[0] = addr;
  buf[1] = dat;

  r = XIic_Send(XPAR_AXI_IIC_0_BASEADDR, I2C_TMDS181_ADDR, (u8 *)&buf, 2, XIIC_STOP);

  if (r == 2)
	  return XST_SUCCESS;
  else
	  return XST_FAILURE;
}

// I2C TMDS181 read
u8 i2c_tmds181_read(u8 addr)
{
  u32 r;
  u8 buf[2];

  buf[0] = addr;

  r = XIic_Send(XPAR_AXI_IIC_0_BASEADDR, I2C_TMDS181_ADDR, (u8 *)&buf, 1, XII_REPEATED_START_OPTION);
  r = XIic_Recv(XPAR_AXI_IIC_0_BASEADDR, I2C_TMDS181_ADDR, (u8 *)&buf, 1, XIIC_STOP);

  if (r == 1)
	return buf[0];
  else
	return 0;
}

// TMDS181
u32 i2c_tmds181(XVphy *VphyPtr, u8 QuadId)
{
  u32 r;

  // Reset I2C controller before issuing new transaction. This is required to
  // recover the IIC controller in case a previous transaction is pending.
  XIic_WriteReg(XPAR_AXI_IIC_0_BASEADDR, XIIC_RESETR_OFFSET,
			XIIC_RESET_MASK);

  if (TMDS_181_VERBOSE)
    xil_printf("Program TMDS181... \r\n");

  r = i2c_tmds181_write(0x0C, 0x18);    // VSWING_DATA = 000 (default); VSWING_CLK to -14% = 110;
                                        // PRE_SEL = HDMI_TWPST = 00.

  r = i2c_tmds181_write(0x0D, 0x01);    // Disables TMDS_CLOCK_RATIO_STATUS control of the clock VOD
                                        // so output swing is at full swing.

  if (TMDS_181_VERBOSE) {
    xil_printf("Done programming TMDS181. Read back values:\r\n");
    u8 readback = i2c_tmds181_read(0x0C);
    xil_printf("0x0C register: 0x%x\r\n", readback);
    readback = i2c_tmds181_read(0x0D);
    xil_printf("0x0D register: 0x%x\r\n", readback);
  }

  if (r == 2)
  	 return XST_SUCCESS;
  else
  	 return XST_FAILURE;
}
