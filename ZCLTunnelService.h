/* 
 * File:   ZCLTunnelService.h
 * Author: fascio
 *
 * Created on 27 de julio de 2016, 14:57 
 */

#ifndef __ZCL_TUNNEL_SERVICE_H__
#define __ZCL_TUNNEL_SERVICE_H__

//**********************************************************************
// ZCL_TUNNEL_SERVICE Includes
//**********************************************************************

#include "SystemEvents.h"
#include "ZCL_Interface.h"
#include "ZCL_Tunneling.h"
#include "Utility.h"
#include "zigbee.h"
#include "SystemLog.h"
#include "SE_Profile.h"



//**********************************************************************
// ZCL_TUNNEL_SERVICE Defines
//**********************************************************************
#define ZCL_TUNNEL_SERVICE_PROTOCOL_ID                                      (200)
#define ZCL_TUNNEL_SERVICE_MANUFACTURE_CODE                                 (0xFFFF)
#define ZCL_TUNNEL_SERVICE_FLOW_CONTROL                                     (TRUE)
#define ZCL_TUNNEL_SERVICE_DST_END_POINT                                    (1)

#define ZCL_TUNNEL_SERVICE_NO_TUNNEL_ID                                     (0xFFFF)
#define ZCL_TUNNEL_SERVICE_NO_SHORT_ADDRESS                                 (0xFFFF)

//!#define ZCL_TUNNEL_SERVICE_MAX_BUFFER_SIZE                                  (200)
#define ZCL_TUNNEL_SERVICE_CONNECT_FRAME_SIZE                               (6)
#define ZCL_TUNNEL_SERVICE_CLOSE_FRAME_SIZE                                 (4)


#define ZCL_TUNNEL_SERVICE_MAX_REQUEST_TUNNEL_RETRIES                       (3)
#define ZCL_TUNNEL_SERVICE_REQUEST_TUNNEL_TIMEOUT                           (_3000_MSEC_)

#define ZCL_TUNNEL_SERVICE_CLOSE_TUNNEL_TIMEOUT                             (_3000_MSEC_)

#define ZCL_TUNNEL_SERVICE_MAX_TRANSFER_DATA_RETRIES                        (3)
#define ZCL_TUNNEL_SERVICE_TRANSFER_DATA_TIMEOUT                            (_3000_MSEC_)

#define ZCL_TUNNEL_SERVICE_MAX_TX_DATA_WITHOUT_FRAGMENT                     (60)
//******************************************************************************
//* [ZCL TUNNEL CLUSTER HEADER]
//*
//* Direction   (1 byte)
//* nextSeqNum  (1 byte)
//* cmdId       (1 byte)
//* tunnelId    (2 bytes)
//******************************************************************************
#define ZCL_TUNNEL_SERVICE_CLUSTER_HEADER_SIZE                              (5)

#define ZCL_TUNNEL_SERVICE_NO_ERROR_CODE                                    (0)
#define ZCL_TUNNEL_SERVICE_NULL_PTR_ERROR_CODE                              (-1)
#define ZCL_TUNNEL_SERVICE_NO_NETWORK_AVAILABLE_ERROR_CODE                  (-2)
#define ZCL_TUNNEL_SERVICE_SHORT_ADDRESS_ERROR_CODE                         (-3)
#define ZCL_TUNNEL_SERVICE_API_IS_LOCK_ERROR_CODE                           (-4)
#define ZCL_TUNNEL_SERVICE_MAX_BUFFER_SIZE_ERROR_CODE                       (-5)
#define ZCL_TUNNEL_SERVICE_TYPE_NOT_SUPPORTED_ERROR_CODE		    (-6)
#define ZCL_TUNNEL_SERVICE_IS_NOT_CONNECTED_ERROR_CODE                      (-7)

//**********************************************************************
// ZCL_TUNNEL_SERVICE Defines
//**********************************************************************

typedef enum {
    TIMEOUT_STOP,
    TIMEOUT_INITIALIZED,
    TIMEOUT_EXPIRED
} ZCL_TUNNEL_TIMEOUT;

typedef enum {
    ZCL_TUNNEL_SERVICE_IDLE_STATE,
    ZCL_TUNNEL_SERVICE_WAITING_FOR_CONNECTION_STATE,
    ZCL_TUNNEL_SERVICE_CONNECTED_STATE,
    ZCL_TUNNEL_SERVICE_PENDING_DATA_TRANSFER_STATE,
    ZCL_TUNNEL_SERVICE_START_DATA_TRANSFER_STATE,
    ZCL_TUNNEL_SERVICE_CLOSE_STATE,
    ZCL_TUNNEL_SERVICE_END_STATE
} ZCL_TUNNEL_STATE;

typedef enum {
    ZCL_TUNNEL_EVENT_NO_INIT,
    ZCL_TUNNEL_EVENT_REQUEST,
    ZCL_TUNNEL_EVENT_RESPONSE,
    ZCL_TUNNEL_EVENT_CONNECT,
    ZCL_TUNNEL_EVENT_CLOSE,
    ZCL_TUNNEL_EVENT_DATA_RECEIVE,
    ZCL_TUNNEL_EVENT_DATA_ERROR,
    ZCL_TUNNEL_EVENT_DATA_ACK_RECEIVE,
    ZCL_TUNNEL_EVENT_SEND_DATA_ACK_REQUEST,
    ZCL_TUNNEL_EVENT_SEND_DATA_ACK_CONFIRM,
    ZCL_TUNNEL_EVENT_DATA_TRANSMISSION_COMPLETE,
    ZCL_TUNNEL_EVENT_DATA_READY,
    ZCL_TUNNEL_EVENT_DATA_STOP
} ZCL_TUNNEL_EVENT;


typedef void (* ZCLTunnelService_HandleReceive)(BYTE * data, WORD dataLen);

typedef struct {
    //! Handle Connection Status
    WORD tunnelId;
    WORD shortAddress;
    BOOL connected;
    BOOL listening;
    BOOL sendingAcknowledgement;
    BOOL acknowledgement;
    BOOL isWaitingForAnswer;
    BOOL answerHasArrived;

    //! Handle Timming Connection Status
    ZCL_TUNNEL_TIMEOUT requestTunnelTimeout;
    BYTE requestTunnelRetries;
    ZCL_TUNNEL_TIMEOUT closeTunnelTimeout;
    ZCL_TUNNEL_TIMEOUT transferDataTimeout;
    BYTE transferDataRetries;

    //! Handle Tx Buffer Status
    BYTE * txHeaderBuffer;
    BYTE * txDataBuffer;
    WORD txDataBufferSize;



    WORD ReadyDataTransferSize;
    

    ZCLTunnelService_HandleReceive handleReceive;

} ZCL_TUNNEL_CONTROL, * ZCL_TUNNEL_CONTROL_PTR;


//**********************************************************************
// ZCL_TUNNEL_SERVICE Function Prototypes
//**********************************************************************
INT16 ZCLTunnelService_SetTunnelId(ZCL_TUNNEL_CONTROL_PTR tunnel, WORD tunnelId);
WORD ZCLTunnelService_GetTunnelId(ZCL_TUNNEL_CONTROL_PTR tunnel);

INT16 ZCLTunnelService_SetShortAddress(ZCL_TUNNEL_CONTROL_PTR tunnel, WORD shortAddress);
WORD ZCLTunnelService_GetShortAddress(ZCL_TUNNEL_CONTROL_PTR tunnel);

INT16 ZCLTunnelService_SetConnected(ZCL_TUNNEL_CONTROL_PTR tunnel, BOOL connected);
BOOL ZCLTunnelService_IsConnected(ZCL_TUNNEL_CONTROL_PTR tunnel);

INT16 ZCLTunnelService_SetListening(ZCL_TUNNEL_CONTROL_PTR tunnel, BOOL listening);
BOOL ZCLTunnelService_IsListening(ZCL_TUNNEL_CONTROL_PTR tunnel);

INT16 ZCLTunnelService_SetAcknowledgement(ZCL_TUNNEL_CONTROL_PTR tunnel, BOOL acknowledgement);
BOOL ZCLTunnelService_GetAcknowledgement(ZCL_TUNNEL_CONTROL_PTR tunnel);

INT16 ZCLTunnelService_SetSendingAcknowledgement(ZCL_TUNNEL_CONTROL_PTR tunnel, BOOL sendingAcknowledgement);
BOOL ZCLTunnelService_GetSendingAcknowledgement(ZCL_TUNNEL_CONTROL_PTR tunnel);

INT16 ZCLTunnelService_SetWaitingForAnswer(ZCL_TUNNEL_CONTROL_PTR tunnel, BOOL isWaitingForAnswer);
BOOL ZCLTunnelService_IsWaitingForAnswer(ZCL_TUNNEL_CONTROL_PTR tunnel);

INT16 ZCLTunnelService_SetAnswerHasArrived(ZCL_TUNNEL_CONTROL_PTR tunnel, BOOL answerHasArrived);
BOOL ZCLTunnelService_GetAnswerHasArrived(ZCL_TUNNEL_CONTROL_PTR tunnel);

INT16 ZCLTunnelService_SetRequestTunnelTimeout(ZCL_TUNNEL_CONTROL_PTR tunnel, ZCL_TUNNEL_TIMEOUT requestTunnelTimeout, WORD value);
ZCL_TUNNEL_TIMEOUT ZCLTunnelService_GetRequestTunnelTimeout(ZCL_TUNNEL_CONTROL_PTR tunnel);

INT16 ZCLTunnelService_SetRequestTunnelRetries(ZCL_TUNNEL_CONTROL_PTR tunnel, BYTE retries);
BYTE ZCLTunnelService_GetRequestTunnelRetries(ZCL_TUNNEL_CONTROL_PTR tunnel);

INT16 ZCLTunnelService_SetCloseTunnelTimeout(ZCL_TUNNEL_CONTROL_PTR tunnel, ZCL_TUNNEL_TIMEOUT closeTunnelTimeout, WORD value);
ZCL_TUNNEL_TIMEOUT ZCLTunnelService_GetCloseConnectionTimeout(ZCL_TUNNEL_CONTROL_PTR tunnel);

INT16 ZCLTunnelService_SetTransferDataTimeout(ZCL_TUNNEL_CONTROL_PTR tunnel, ZCL_TUNNEL_TIMEOUT transferDataTimeout, WORD value);
ZCL_TUNNEL_TIMEOUT ZCLTunnelService_GetTransferDataTimeout(ZCL_TUNNEL_CONTROL_PTR tunnel);

INT16 ZCLTunnelService_SetTransferDataRetries(ZCL_TUNNEL_CONTROL_PTR tunnel, BYTE retries);
BYTE ZCLTunnelService_GetTransferDataRetries(ZCL_TUNNEL_CONTROL_PTR tunnel);

INT16 ZCLTunnelService_SetTxHeaderBuffer(ZCL_TUNNEL_CONTROL_PTR tunnel, BYTE * txHeaderBuffer);
BYTE * ZCLTunnelService_GetTxHeaderBuffer(ZCL_TUNNEL_CONTROL_PTR tunnel);

INT16 ZCLTunnelService_SetTxDataBuffer(ZCL_TUNNEL_CONTROL_PTR tunnel, BYTE * txDataBuffer);
BYTE * ZCLTunnelService_GetTxDataBuffer(ZCL_TUNNEL_CONTROL_PTR tunnel);

INT16 ZCLTunnelService_SetTxDataBufferSize(ZCL_TUNNEL_CONTROL_PTR tunnel, WORD txDataBufferSize);
WORD ZCLTunnelService_GetTxDataBufferSize(ZCL_TUNNEL_CONTROL_PTR tunnel);

INT16 ZCLTunnelService_SetHandleReceive(ZCL_TUNNEL_CONTROL_PTR tunnel, ZCLTunnelService_HandleReceive handleReceive);
ZCLTunnelService_HandleReceive ZCLTunnelService_GetHandleReceive(ZCL_TUNNEL_CONTROL_PTR tunnel);

INT16 ZCLTunnelService_Setup(ZCL_TUNNEL_CONTROL_PTR tunnel, WORD tunnelId, WORD shortAddress, BOOL connected, BOOL listening);
INT16 ZCLTunnelService_Reset(ZCL_TUNNEL_CONTROL_PTR tunnel);

BOOL ZCLTunnelService_IsNetworkAvailable(void);

INT16 ZCLTunnelService_Connect(WORD shortAddress, BYTE protocolId, WORD manufactureCode, BYTE flowControl, BYTE destEndPoint);
BOOL ZCLTunnelService_IsConnectedWithThisShortAddress(WORD shortAddress);
INT16 ZCLTunnelService_Close(WORD shortAddress, WORD tunnelId, BYTE destEndPoint, BOOL listening);

INT16 ZCLTunnelService_StopDataTransfer(WORD tunnelId);
INT16 ZCLTunnelService_ReadyDataTransfer(WORD tunnelId);

INT16 ZCLTunnelService_SendDataWithLoadedData( WORD shortAddress, WORD tunnelId, WORD dataLen, BYTE destEndPoint);
INT16 ZCLTunnelService_SendFragmentDataRequest(BYTE addressMode, BYTE* pDestAddress, BYTE destEndpoint, BYTE asduLength, WORD clusterId, BYTE* pAsdu);
INT16 ZCLTunnelService_ReceiveData(WORD shortAddress, WORD tunnelId, BYTE * data, WORD dataLen);

void ZCLTunnelService_StateMachine(void);
void ZCLTunnelService_UnsynchronizeCallback(ZCL_TUNNEL_EVENT event, BYTE * data, WORD dataLen);

INT16 API_ZCLTunnelService_Setup(ZCLTunnelService_HandleReceive handleReceive, BYTE * txBuffer);
INT16 API_ZCLTunnelService_Connect(WORD shortAddress);
INT16 API_ZCLTunnelService_Close(WORD shortAddress, WORD tunnelId);
//!INT16 API_ZCLTunnelService_SendData(WORD shortAddress, BYTE * data, WORD dataLen, BOOL isWaitingForAnswer);
INT16 API_ZCLTunnelService_SendDataWithLoadedData(WORD shortAddress, WORD dataLen, BOOL isWaitingForAnswer);
BYTE * API_ZCLTunnelService_GetTxHeaderBuffer( void );
BYTE * API_ZCLTunnelService_GetTxDataBuffer( void );
INT16 API_ZCLTunnelService_SetTxDataBufferSize( WORD txDataBufferSize );
WORD API_ZCLTunnelService_GetTxDataBufferSize( void );
BOOL API_ZCLTunnelService_IsSendingAcknowledgement(void);


void ZCLTunnelService_RequestTunnelTimeoutOneshot(void);
void ZCLTunnelService_CloseTunnelTimeoutOneshot(void);
void ZCLTunnelService_TransferDataTimeoutOneshot(void);
//******************************************************************************
//                  LOCAL ZCL TUNNEL SERVICE STATE MACHINE
//******************************************************************************
void ZCLTunnelService_Driver(void);
void ZCLTunnelService_SetStateMachine(ZCL_TUNNEL_STATE actualState, ZCL_TUNNEL_STATE nextState);
ZCL_TUNNEL_STATE ZCLTunnelService_GetActualState(void);
ZCL_TUNNEL_STATE ZCLTunnelService_GetNextState(void);
void ZCLTunnelService_ErrorReset(void);





#endif /* __ZCL_TUNNEL_SERVICE_H__ */


