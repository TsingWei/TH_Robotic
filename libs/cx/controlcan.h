/*
 * @Author: xingzhang.Wu 
 * @Date: 2019-09-29 10:05:46 
 * @Last Modified by: xingzhang.Wu
 * @Last Modified time: 2019-09-29 10:51:08
 */
#ifndef CONTROLCAN_H
#define CONTROLCAN_H

#include "cantypes.h"
#include <qsystemdetection.h>

//文件版本：v2.01 20190527
#ifdef __cplusplus
#define EXTERN_C extern "C"
#else
#define EXTERN_C
#endif

EXTERN_C DWORD VCI_OpenDevice(DWORD DeviceType, DWORD DeviceInd, DWORD Reserved);
EXTERN_C DWORD VCI_CloseDevice(DWORD DeviceType, DWORD DeviceInd);
EXTERN_C DWORD VCI_InitCAN(DWORD DeviceType, DWORD DeviceInd, DWORD CANInd, PVCI_INIT_CONFIG pInitConfig);

EXTERN_C DWORD VCI_ReadBoardInfo(DWORD DeviceType, DWORD DeviceInd, PVCI_BOARD_INFO pInfo);

EXTERN_C DWORD VCI_SetReference(DWORD DeviceType, DWORD DeviceInd, DWORD CANInd, DWORD RefType, PVOID pData);

EXTERN_C ULONG VCI_GetReceiveNum(DWORD DeviceType, DWORD DeviceInd, DWORD CANInd);
EXTERN_C DWORD VCI_ClearBuffer(DWORD DeviceType, DWORD DeviceInd, DWORD CANInd);

EXTERN_C DWORD VCI_StartCAN(DWORD DeviceType, DWORD DeviceInd, DWORD CANInd);
EXTERN_C DWORD VCI_ResetCAN(DWORD DeviceType, DWORD DeviceInd, DWORD CANInd);

EXTERN_C ULONG VCI_Transmit(DWORD DeviceType, DWORD DeviceInd, DWORD CANInd, PVCI_CAN_OBJ pSend, UINT Len);
EXTERN_C ULONG VCI_Receive(DWORD DeviceType, DWORD DeviceInd, DWORD CANInd, PVCI_CAN_OBJ pReceive, UINT Len, INT WaitTime);

EXTERN_C DWORD VCI_UsbDeviceReset(DWORD DevType, DWORD DevIndex, DWORD Reserved);
EXTERN_C DWORD VCI_FindUsbDevice2(PVCI_BOARD_INFO pInfo);

#endif
