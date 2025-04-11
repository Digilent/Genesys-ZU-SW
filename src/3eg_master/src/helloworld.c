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

#include "xil_printf.h"
#include "xparameters.h"
#include "ZmodAwgAxiConfiguration.h"
#include "xuartps.h"
#include "xgpio.h"
#include "dpmutil/dpmutil.h"
#include "dpmutil/I2CHAL.h"
#include "sleep.h"

// Genesys ZU has a single SYZYGY port hidden behind channel 5 of an i2c mux
// https://digilent.com/reference/programmable-logic/genesys-zu/reference-manual#zmod
const BYTE szgI2cAddr = 0x30;
const BYTE muxI2cAddr = 0x70;
const BYTE szgMuxChan = 0x20;

// Note: Many DPMUTIL functions only apply to boards using Eclypse/USB104A7-style PMCU firmware.
//       The Genesys ZU has a different PMCU register set. As such, functionality for enumerating
//       the Zmod port must be reproduced here.
BOOL ZmodDetected() {
	static XGpio detect;
	static BOOL init = fTrue;
	if (init) {
		XGpio_Config *cfgptr;
		cfgptr = XGpio_LookupConfig(XPAR_SYZYGY_DETECTN_DEVICE_ID);
		if (cfgptr == NULL) {
			return fFalse;
		}

		INT32 status;
		status = XGpio_CfgInitialize(&detect, cfgptr, cfgptr->BaseAddress);
		if (status != XST_SUCCESS) {
			return fFalse;
		}

		init = fFalse;
	}

	if ((XGpio_DiscreteRead(&detect, 1) & 1) == 0) {
		return fTrue;
	}

	return fFalse;
}

XUartPs uart;
BOOL uartInit = fFalse;

BOOL InitializeUart(UINT16 deviceId) {
	if (!uartInit) {
		XUartPs_Config *cfgptr;
		cfgptr = XUartPs_LookupConfig(deviceId);
		if (cfgptr == NULL) {
			return fFalse;
		}

		INT32 status = XUartPs_CfgInitialize(&uart, cfgptr, cfgptr->BaseAddress);
		if (status != XST_SUCCESS) {
			return fFalse;
		}

		XUartPs_SetBaudRate(&uart, 115200);
		uartInit = fTrue;
	}
	return fTrue;
}

BOOL WaitForNewline() {
	UCHAR buffer;
	INT32 bytesReceived;

	xil_printf("Press enter to continue.\r\n");
	do {
		bytesReceived = XUartPs_Recv(&uart, &buffer, 1);
	} while (bytesReceived == 0 || buffer != '\n');

	return fTrue;
}


int ReadCalibration (ZMOD_DAC_CAL *pFactoryCal) {
	int fdI2cDev = 0; // this isn't using linux so this doesn't matter
	char *PortName = "Zmod A";
	BOOL supported = fFalse; // Whether this demo supports the detected Zmod;

	SzgDnaHeader DnaHeader;
	SzgDnaStrings DnaStrings = {0};

	if (!ZmodDetected()) {
		xil_printf("ERROR: No Zmod detected!\r\n");
		return fFalse;
	}

	// Read the product id
	DWORD Pdid;
	if (!FZmodReadPdid(fdI2cDev, szgI2cAddr, &Pdid)) {
		xil_printf("ERROR: Failed to read Zmod product ID.");
		return fFalse;
	}
	// Read the standard DNA information
	SyzygyReadDNAHeader(fdI2cDev, szgI2cAddr, &DnaHeader, FALSE);
	SyzygyReadDNAStrings(fdI2cDev, szgI2cAddr, &DnaHeader, &DnaStrings);

	ZMOD_FAMILY Family;
	if (!FGetZmodFamily(Pdid, &Family)) {
		xil_printf("ERROR: Unsupported Zmod (%s) populated on %s.\r\n", DnaStrings.szProductName, PortName);
		SyzygyFreeDNAStrings(&DnaStrings);
		return fFalse;
	}

	switch (Family) {
	case ZMOD_FAMILY_ADC:
		xil_printf("========= %s : %s Calibration Coefficients =========\r\n", PortName, DnaStrings.szProductName);
		FDisplayZmodADCCal(fdI2cDev, szgI2cAddr);
		ZMOD_ADC_CAL ADCFactoryCalibration, ADCUserCalibration;
		FGetZmodADCCal(fdI2cDev, szgI2cAddr, &ADCFactoryCalibration, &ADCUserCalibration);
		xil_printf("\r\n");
		break;
	case ZMOD_FAMILY_DAC:
		xil_printf("========= %s : %s Calibration Coefficients =========\r\n", PortName, DnaStrings.szProductName);
		FDisplayZmodDACCal(fdI2cDev, szgI2cAddr);
		ZMOD_DAC_CAL userCal; // unused user calibration
		FGetZmodDACCal(fdI2cDev, szgI2cAddr, pFactoryCal, &userCal);
		supported = fTrue;
		xil_printf("\r\n");
		break;
	case ZMOD_FAMILY_DIGITIZER:
		xil_printf("========= %s : %s Calibration Coefficients =========\r\n", PortName, DnaStrings.szProductName);
		FDisplayZmodDigitizerCal(fdI2cDev, szgI2cAddr);
		ZMOD_DIGITIZER_CAL DigitizerFactoryCalibration, DigitizerUserCalibration;
		FGetZmodDigitizerCal(fdI2cDev, szgI2cAddr, &DigitizerFactoryCalibration, &DigitizerUserCalibration);
		xil_printf("\r\n");
		break;
	case ZMOD_FAMILY_UNSUPPORTED:
		xil_printf("========= Unsupported Zmod (%s) populated on %s =========\r\n", DnaStrings.szProductName, PortName);
	}

	SyzygyFreeDNAStrings(&DnaStrings);
	return supported;
}

int main()
{
	InitializeUart(XPAR_PSU_UART_0_DEVICE_ID);

    xil_printf("Entered main\r\n");

    if (!ZmodDetected()) {
    	xil_printf("Error: No Zmod is installed!\r\n");
    	return 1;
    }

    // Note: In order for dpmutil i2c reads to get to the SYZYGY DNA, the IIC multiplexer needs to have the
    //       correct channel selected. This demo does not account for potential additional I2C traffic from
    //       other masters taking over the bus and switching the multiplexor channels. Consider changing
    //       dpmutil to start any repeated-start I2C transfer with this mux channel set.
	BYTE muxChan = szgMuxChan;
    UINT32 bytes_sent = 0;
    do {
    	bytes_sent = I2CHALLowLevelSend(XPAR_AXI_IIC_DNA_DEVICE_ID, muxI2cAddr, &muxChan, 1);
    } while (!bytes_sent);

    // Read the calibration coefficients from DNA
    ZMOD_DAC_CAL fCal;
    ZMOD_DAC_CAL_S18 iCal;

	ReadCalibration(&fCal);
	FZmodDACCalConvertToS18(fCal, &iCal);

	// Write the calibration coefficients to the AWG controller configuration registers
    UINT32 AwgBaseAddress = XPAR_ZMODAWGAXICONFIGURAT_0_S_AXI_CONTROL_BASEADDR;
	ZmodAwgAxiConfiguration_WriteReg(AwgBaseAddress, ZMOD_AWG_AXI_CONFIG_EXTCH1LGMULTCOEFF_REG_OFFSET, iCal.cal[0][0][0]);
    ZmodAwgAxiConfiguration_WriteReg(AwgBaseAddress, ZMOD_AWG_AXI_CONFIG_EXTCH1LGADDCOEFF_REG_OFFSET,  iCal.cal[0][0][1]);
    ZmodAwgAxiConfiguration_WriteReg(AwgBaseAddress, ZMOD_AWG_AXI_CONFIG_EXTCH1HGMULTCOEFF_REG_OFFSET, iCal.cal[0][1][0]);
    ZmodAwgAxiConfiguration_WriteReg(AwgBaseAddress, ZMOD_AWG_AXI_CONFIG_EXTCH1HGADDCOEFF_REG_OFFSET,  iCal.cal[0][1][1]);
    ZmodAwgAxiConfiguration_WriteReg(AwgBaseAddress, ZMOD_AWG_AXI_CONFIG_EXTCH2LGMULTCOEFF_REG_OFFSET, iCal.cal[1][0][0]);
    ZmodAwgAxiConfiguration_WriteReg(AwgBaseAddress, ZMOD_AWG_AXI_CONFIG_EXTCH2LGADDCOEFF_REG_OFFSET,  iCal.cal[1][0][1]);
    ZmodAwgAxiConfiguration_WriteReg(AwgBaseAddress, ZMOD_AWG_AXI_CONFIG_EXTCH2HGMULTCOEFF_REG_OFFSET, iCal.cal[1][1][0]);
    ZmodAwgAxiConfiguration_WriteReg(AwgBaseAddress, ZMOD_AWG_AXI_CONFIG_EXTCH2HGADDCOEFF_REG_OFFSET,  iCal.cal[1][1][1]);

    xil_printf("========= Amplitude Tests =========\r\n");

    UINT32 AwgCtrl;
	// Go through each channel scale setting, pausing for user to press enter in the serial terminal to take measurements.
    xil_printf("1. Ch1: Low gain, Ch2: Low gain; Test mode disables calibration\r\n");
    AwgCtrl = ZMOD_AWG_AXI_CONFIG_CONTROL_TESTMODE_MASK |
           	  ZMOD_AWG_AXI_CONFIG_CONTROL_DAC_ENIN_MASK;
    ZmodAwgAxiConfiguration_WriteReg(AwgBaseAddress, ZMOD_AWG_AXI_CONFIG_CONTROL_REG_OFFSET, AwgCtrl);
    WaitForNewline();

    xil_printf("2. Ch1: High gain, Ch2: High gain; Test mode disables calibration\r\n");
    AwgCtrl = ZMOD_AWG_AXI_CONFIG_CONTROL_TESTMODE_MASK |
           	  ZMOD_AWG_AXI_CONFIG_CONTROL_DAC_ENIN_MASK |
	          ZMOD_AWG_AXI_CONFIG_CONTROL_EXTCH1SCALE_MASK |
	          ZMOD_AWG_AXI_CONFIG_CONTROL_EXTCH2SCALE_MASK;
    ZmodAwgAxiConfiguration_WriteReg(AwgBaseAddress, ZMOD_AWG_AXI_CONFIG_CONTROL_REG_OFFSET, AwgCtrl);
    WaitForNewline();

    xil_printf("3. Ch1: Low gain, Ch2: Low gain; expected Vpk-pk is 2.5 V\r\n");
    AwgCtrl = ZMOD_AWG_AXI_CONFIG_CONTROL_DAC_ENIN_MASK;
    ZmodAwgAxiConfiguration_WriteReg(AwgBaseAddress, ZMOD_AWG_AXI_CONFIG_CONTROL_REG_OFFSET, AwgCtrl);
    WaitForNewline();

    xil_printf("4. Ch1: High gain, Ch2: High gain; expected Vpk-pk is 10.0 V\r\n");
    AwgCtrl = ZMOD_AWG_AXI_CONFIG_CONTROL_DAC_ENIN_MASK |
              ZMOD_AWG_AXI_CONFIG_CONTROL_EXTCH1SCALE_MASK |
              ZMOD_AWG_AXI_CONFIG_CONTROL_EXTCH2SCALE_MASK;
    ZmodAwgAxiConfiguration_WriteReg(AwgBaseAddress, ZMOD_AWG_AXI_CONFIG_CONTROL_REG_OFFSET, AwgCtrl);
    WaitForNewline();

    // Disable the AWG before exiting
    AwgCtrl = 0;
    ZmodAwgAxiConfiguration_WriteReg(AwgBaseAddress, ZMOD_AWG_AXI_CONFIG_CONTROL_REG_OFFSET, AwgCtrl);

    xil_printf("Exiting main\r\n");
    return 0;
}
