#ifndef __ZCL_TUNNEL_PROTOCOL_H__
#define __ZCL_TUNNEL_PROTOCOL_H__

//**********************************************************************
//* ZCL_TUNNEL_PROTOCOL Includes
//**********************************************************************
#include "ZCLTunnelService.h"

//**********************************************************************
//* ZCL_TUNNEL_PROTOCOL Defines
//**********************************************************************
#define ZCL_TUNNEL_PROTOCOL_MAX_BUFFER_SIZE				(255)
#define ZCL_TUNNEL_PROTOCOL_HANDLER_CRC_BASE				(0xFFFF)
#define ZCL_TUNNEL_PROTOCOL_HANDLER_CRC_OK				(0)

//******************************************************************************
//* [ZCL TUNNEL PROTOCOL HEADER]
//*
//* FrameSize   (2 byte)
//* commandId   (1 byte)
//******************************************************************************
#define ZCL_TUNNEL_PROTOCOL_HEADER_SIZE                              (3)

//**********************************************************************
//* ZCL_TUNNEL_PROTOCOL Error Code
//**********************************************************************
#define ZCL_TUNNEL_PROTOCOL_NO_ERROR_CODE				(0)
#define ZCL_TUNNEL_PROTOCOL_FRAME_SIZE_ERROR_CODE			(-1)
#define ZCL_TUNNEL_PROTOCOL_CRC_ERROR_CODE				(-2)
#define ZCL_TUNNEL_PROTOCOL_NULL_PTR_ERROR_CODE				(-3)
#define ZCL_TUNNEL_PROTOCOL_MAX_BUFFER_SIZE_ERROR_CODE			(-4)

//**********************************************************************
//* ZCL_TUNNEL_PROTOCOL
//**********************************************************************

typedef struct {

    //**************************************************************************
    //* Reception Process
    //**************************************************************************
    //
    // It is neccessary to implement a pointer buffer for received data
    //

    WORD frameSize;
    BYTE commandId;
    BYTE data[ZCL_TUNNEL_PROTOCOL_MAX_BUFFER_SIZE];
    WORD dataLen;

    //**************************************************************************
    //* Transmission Process
    //**************************************************************************
    BYTE * txHeader;
    BYTE * txData;
    WORD txDataSize;

} ZCL_TUNNEL_PROTOCOL, * ZCL_TUNNEL_PROTOCOL_PTR;


//**********************************************************************
//* ZCL_TUNNEL_PROTOCOL Functions
//**********************************************************************

void ZCLTunnelProtocol_SetFrameSize( WORD frameSize);
WORD ZCLTunnelProtocol_GetFrameSize(void);

void ZCLTunnelProtocol_SetCommandId( BYTE commandId);
BYTE ZCLTunnelProtocol_GetCommandId(void);

INT16 ZCLTunnelProtocol_SetData( BYTE * data, WORD dataLen);
BYTE * ZCLTunnelProtocol_GetData(void);
WORD ZCLTunnelProtocol_GetDataLen(void);

INT16 ZCLTunnelProtocol_SetTxHeader( BYTE * txHeader);
BYTE * ZCLTunnelProtocol_GetTxHeader(void);
INT16 ZCLTunnelProtocol_SetTxData(BYTE * txData);
BYTE * ZCLTunnelProtocol_GetTxData(void);
void ZCLTunnelProtocol_SetTxDataSize(WORD txDataSize);
WORD ZCLTunnelProtocol_GetTxDataSize(void);

INT16 ZCLTunnelProtocol_Setup(BYTE *  txBuffer);
INT16 ZCLTunnelProtocol_SetReception( WORD frameSize, BYTE commandId, BYTE * data, WORD dataLen);
//!INT16 ZCLTunnelProtocol_BuildFrame(BYTE commandId, BYTE * data, WORD dataSize, BYTE * frame, WORD * frameLen);
//!INT16 ZCLTunnelProtocol_SendCustomFrame(WORD shortAddress, BYTE commandId, BYTE * data, WORD dataLen, BOOL isWaitingForAnswer);
INT16 ZCLTunnelProtocol_BuildFrameWithLoadedData(BYTE commandId, WORD dataSize, BYTE * frame, WORD * frameLen);
INT16 ZCLTunnelProtocol_SendFrameWithLoadedData( WORD shortAddress, BYTE commandId, BOOL isWaitingForAnswer);
INT16 ZCLTunnelProtocol_SendFrame( WORD shortAddress, BYTE commandId, BYTE * data, WORD dataLen, BOOL isWaitingForAnswer);
INT16 ZCLTunnelProtocol_HandleReceive(BYTE * data, WORD dataLen);
INT16 ZCLTunnelProtocol_ErrorProcess( void );

//******************************************************************************
//                  ZCL TUNNEL SERVICE SHELL
//******************************************************************************
INT8 Shell_tunnel(UINT8 argc, char * argv[]);

#endif /* __ZCL_TUNNEL_PROTOCOL_H__ */
