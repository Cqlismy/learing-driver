/**********************************************************************************
File Name	:	fm175xx.h
Description	:	FM175xx系列RFID读卡芯片定义
Revision	:
Author		:	Bryan Jiang <598612779@qq.com>
Date		:	2018/10/22
***********************************************************************************/

#ifndef __PN512_REG_H__
#define __PN512_REG_H__

/*******************************************
FM175xx 寄存器定义
********************************************/
#define CommandReg		0x01
#define ComIEnReg		0x02
#define DivIEnReg		0x03
#define ComIrqReg		0x04
#define DivIrqReg		0x05
#define ErrorReg		0x06
#define Status1Reg		0x07
#define Status2Reg		0x08
#define FIFODataReg		0x09
#define FIFOLevelReg	0x0A
#define WaterLevelReg	0x0B
#define ControlReg		0x0C
#define BitFramingReg	0x0D
#define CollReg			0x0E
#define ModeReg			0x11
#define TxModeReg		0x12
#define RxModeReg		0x13
#define TxControlReg	0x14
#define TxAutoReg		0x15
#define TxSelReg		0x16
#define RxSelReg		0x17
#define RxThresholdReg	0x18
#define DemodReg		0x19
#define MfTxReg			0x1C
#define MfRxReg			0x1D
#define SerialSpeedReg	0x1F
#define CRCMSBReg		0x21
#define CRCLSBReg		0x22
#define ModWidthReg		0x24
#define GsNOffReg		0x23
#define TxBitPhaseReg	0x25
#define RFCfgReg		0x26
#define GsNReg			0x27
#define CWGsPReg		0x28
#define ModGsPReg		0x29
#define TModeReg		0x2A
#define TPrescalerReg	0x2B
#define TReloadMSBReg	0x2C
#define TReloadLSBReg	0x2D
#define TCounterValMSBReg 0x2E
#define TCounterValLSBReg 0x2F
#define TestSel1Reg		0x31
#define TestSel2Reg		0x32
#define TestPinEnReg	0x33
#define TestPinValueReg 0x34
#define TestBusReg		0x35
#define AutoTestReg		0x36
#define VersionReg		0x37
#define AnalogTestReg	0x38
#define TestDAC1Reg		0x39
#define TestDAC2Reg		0x3A
#define TestADCReg		0x3B


#define Idle			0x00
#define Mem				0x01
#define RandomID		0x02
#define CalcCRC			0x03
#define Transmit		0x04
#define NoCmdChange		0x07
#define Receive			0x08
#define Transceive		0x0C
#define MFAuthent		0x0E
#define SoftReset		0x0F

#define MI_NOTAGERR		0xEE
#define MAXRLEN			64

#endif




