/******************************************************************************
*
* Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
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
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/

/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

#include <stdio.h>
#include "platform.h"
#include "xparameters.h"
#include "ZmodAwgAxiConfiguration.h"
#include "xiic.h"
#include "dpmutil/dpmutil.h"

int PrintCalibration () {
	int fdI2cDev = 0; // this isn't using linux so this doesn't matter
	char *PortName;

	dpmutilPortInfo_t PortInfo[8] = {0};

	// enumerate the Syzygy ports to figure out which have Zmods installed
	dpmutilFEnum(FALSE, FALSE, PortInfo);

	SzgDnaHeader DnaHeader;
	SzgDnaStrings DnaStrings = {0};

	// Iterate over all of the ports enumerated
	for (u32 iPort = 0; iPort < 8; iPort++) {
		// check if a zmod is populated on that port
		if (PortInfo[iPort].portSts.fPresent == 0) {
			continue;
		}

		// Use group VIO to detect which port is which. For the Eclypse Z7, Zmod A = 0, Zmod B = 1
		switch (PortInfo[iPort].groupVio) {
		case 0:
			PortName = "Zmod Port A";
			break;
		default:
			PortName = "Invalid VIO Group";
		}

		// Read the standard DNA information
		SyzygyReadDNAHeader(fdI2cDev, PortInfo[iPort].i2cAddr, &DnaHeader, FALSE);
		SyzygyReadDNAStrings(fdI2cDev, PortInfo[iPort].i2cAddr, &DnaHeader, &DnaStrings);

		// Read the product id
		DWORD Pdid;
		if (!FZmodReadPdid(fdI2cDev, PortInfo[iPort].i2cAddr, &Pdid)) {
			continue;
		}

		ZMOD_FAMILY Family;
		if (!FGetZmodFamily(Pdid, &Family)) {
			printf("========= Unsupported Zmod (%s) populated on %s =========\r\n", DnaStrings.szProductName, PortName);
			continue;
		}

		switch (Family) {
		case ZMOD_FAMILY_ADC:
			printf("========= %s : %s Calibration Coefficients =========\r\n", PortName, DnaStrings.szProductName);
			FDisplayZmodADCCal(fdI2cDev, PortInfo[iPort].i2cAddr);
			ZMOD_ADC_CAL ADCFactoryCalibration, ADCUserCalibration;
			FGetZmodADCCal(fdI2cDev, PortInfo[iPort].i2cAddr, &ADCFactoryCalibration, &ADCUserCalibration);
			printf("\r\n");
			break;
		case ZMOD_FAMILY_DAC:
			printf("========= %s : %s Calibration Coefficients =========\r\n", PortName, DnaStrings.szProductName);
			FDisplayZmodDACCal(fdI2cDev, PortInfo[iPort].i2cAddr);
			ZMOD_DAC_CAL DACFactoryCalibration, DACUserCalibration;
			FGetZmodDACCal(fdI2cDev, PortInfo[iPort].i2cAddr, &DACFactoryCalibration, &DACUserCalibration);
			printf("\r\n");
			break;
		case ZMOD_FAMILY_DIGITIZER:
			printf("========= %s : %s Calibration Coefficients =========\r\n", PortName, DnaStrings.szProductName);
			FDisplayZmodDigitizerCal(fdI2cDev, PortInfo[iPort].i2cAddr);
			ZMOD_DIGITIZER_CAL DigitizerFactoryCalibration, DigitizerUserCalibration;
			FGetZmodDigitizerCal(fdI2cDev, PortInfo[iPort].i2cAddr, &DigitizerFactoryCalibration, &DigitizerUserCalibration);
			printf("\r\n");
			break;
		case ZMOD_FAMILY_UNSUPPORTED:
			printf("========= Unsupported Zmod (%s) populated on %s =========\r\n", DnaStrings.szProductName, PortName);
		}

		// Free memory allocated to hold DNA strings like product name
		SyzygyFreeDNAStrings(&DnaStrings);
	}

	return 0;
}

int main()
{
    init_platform();
    print("Entered main\r\n");

    // Enable only channel 5 on the IIC multiplexer
	u8 zmod_mux_ch = 0b00010000;
	u8 mux_i2caddr = 0b01110000;
    I2CHALLowLevelSend(XPAR_AXI_IIC_DNA_DEVICE_ID, mux_i2caddr, &zmod_mux_ch, 1);

	PrintCalibration();

	// Todo: Pull out cal coefficients and write them to the AXI controller.

	// Start the AWG controller in test mode
    u32 data = ZMOD_AWG_AXI_CONFIG_CONTROL_TESTMODE_MASK |
               ZMOD_AWG_AXI_CONFIG_CONTROL_DAC_ENIN_MASK |
               ZMOD_AWG_AXI_CONFIG_CONTROL_EXTCH1SCALE_MASK |
               ZMOD_AWG_AXI_CONFIG_CONTROL_EXTCH2SCALE_MASK;
    u32 AwgBaseAddress = XPAR_ZMODAWGAXICONFIGURAT_0_S_AXI_CONTROL_BASEADDR;
    ZmodAwgAxiConfiguration_WriteReg(AwgBaseAddress, ZMOD_AWG_AXI_CONFIG_CONTROL_REG_OFFSET, data);

    print("Exiting main\r\n");
    cleanup_platform();
    return 0;
}
