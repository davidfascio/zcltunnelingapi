//******************************************************************************
// Includes
//******************************************************************************
#include "ZCLTunnelService.h"



#if(I_AM_MTR == 1)
#include "SE_MTR.h"


#elif (I_AM_COORDINATOR == 1 )
#include "SE_ESP.h"
#include "SE_Interface.h"
#endif


//******************************************************************************
// Meter Control Variables
//******************************************************************************
ZCL_TUNNEL_CONTROL zclTunnelControl;

//******************************************************************************
//                  LOCAL ZCL TUNNEL SERVICE STATE MACHINE
//******************************************************************************
void ZCLTunnelService_IdleState(void);
void ZCLTunnelService_WaitingForConnectionState(void);
void ZCLTunnelService_ConnectedState(void);
void ZCLTunnelService_PendingDataTransferState(void);
void ZCLTunnelService_StartDataTransferState(void);
void ZCLTunnelService_CloseState(void);
void ZCLTunnelService_EndState(void);

void (* const zclTunnelServiceDriverState[]) (void) = {
    ZCLTunnelService_IdleState,
    ZCLTunnelService_WaitingForConnectionState,
    ZCLTunnelService_ConnectedState,
    ZCLTunnelService_PendingDataTransferState,
    ZCLTunnelService_StartDataTransferState,
    ZCLTunnelService_CloseState,
    ZCLTunnelService_EndState
};

sSM zclTunnelSM = {ZCL_TUNNEL_SERVICE_IDLE_STATE, /* bActualState     */
    ZCL_TUNNEL_SERVICE_IDLE_STATE, /* bNextState       */
    ZCL_TUNNEL_SERVICE_IDLE_STATE, /* bPrevState       */
    ZCL_TUNNEL_SERVICE_IDLE_STATE}; /* bErrorState      */

//******************************************************************************
//                  LOCAL ZCL TUNNEL SERVICE STATE MACHINE
//******************************************************************************

INT16 ZCLTunnelService_SetTunnelId(ZCL_TUNNEL_CONTROL_PTR tunnel, WORD tunnelId) {

    if (tunnel == NULL)
        return ZCL_TUNNEL_SERVICE_NULL_PTR_ERROR_CODE;

    tunnel->tunnelId = tunnelId;

    return ZCL_TUNNEL_SERVICE_NO_ERROR_CODE;
}

WORD ZCLTunnelService_GetTunnelId(ZCL_TUNNEL_CONTROL_PTR tunnel) {

    return tunnel->tunnelId;
}

INT16 ZCLTunnelService_SetShortAddress(ZCL_TUNNEL_CONTROL_PTR tunnel, WORD shortAddress) {

    if (tunnel == NULL)
        return ZCL_TUNNEL_SERVICE_NULL_PTR_ERROR_CODE;

    tunnel->shortAddress = shortAddress;

    return ZCL_TUNNEL_SERVICE_NO_ERROR_CODE;
}

WORD ZCLTunnelService_GetShortAddress(ZCL_TUNNEL_CONTROL_PTR tunnel) {

    return tunnel->shortAddress;
}

INT16 ZCLTunnelService_SetConnected(ZCL_TUNNEL_CONTROL_PTR tunnel, BOOL connected) {

    if (tunnel == NULL)
        return ZCL_TUNNEL_SERVICE_NULL_PTR_ERROR_CODE;

    tunnel->connected = connected;

    return ZCL_TUNNEL_SERVICE_NO_ERROR_CODE;
}

BOOL ZCLTunnelService_IsConnected(ZCL_TUNNEL_CONTROL_PTR tunnel) {

    return tunnel->connected;
}

INT16 ZCLTunnelService_SetListening(ZCL_TUNNEL_CONTROL_PTR tunnel, BOOL listening){

    if (tunnel == NULL)
        return ZCL_TUNNEL_SERVICE_NULL_PTR_ERROR_CODE;

    tunnel->listening = listening;

    return ZCL_TUNNEL_SERVICE_NO_ERROR_CODE;
}

BOOL ZCLTunnelService_IsListening(ZCL_TUNNEL_CONTROL_PTR tunnel){

    return tunnel->listening;
}

INT16 ZCLTunnelService_SetAcknowledgement(ZCL_TUNNEL_CONTROL_PTR tunnel, BOOL acknowledgement) {

    if (tunnel == NULL)
        return ZCL_TUNNEL_SERVICE_NULL_PTR_ERROR_CODE;

    tunnel->acknowledgement = acknowledgement;

    return ZCL_TUNNEL_SERVICE_NO_ERROR_CODE;
}

BOOL ZCLTunnelService_GetAcknowledgement(ZCL_TUNNEL_CONTROL_PTR tunnel) {

    return tunnel->acknowledgement;
}

INT16 ZCLTunnelService_SetSendingAcknowledgement(ZCL_TUNNEL_CONTROL_PTR tunnel, BOOL sendingAcknowledgement){

    if (tunnel == NULL)
        return ZCL_TUNNEL_SERVICE_NULL_PTR_ERROR_CODE;

    tunnel->sendingAcknowledgement = sendingAcknowledgement;

    return ZCL_TUNNEL_SERVICE_NO_ERROR_CODE;
}

BOOL ZCLTunnelService_GetSendingAcknowledgement(ZCL_TUNNEL_CONTROL_PTR tunnel){

    return tunnel->sendingAcknowledgement;
}

INT16 ZCLTunnelService_SetWaitingForAnswer(ZCL_TUNNEL_CONTROL_PTR tunnel, BOOL isWaitingForAnswer) {

    if (tunnel == NULL)
        return ZCL_TUNNEL_SERVICE_NULL_PTR_ERROR_CODE;

    tunnel->isWaitingForAnswer = isWaitingForAnswer;

    return ZCL_TUNNEL_SERVICE_NO_ERROR_CODE;
}

BOOL ZCLTunnelService_IsWaitingForAnswer(ZCL_TUNNEL_CONTROL_PTR tunnel) {

    return tunnel->isWaitingForAnswer;
}

INT16 ZCLTunnelService_SetAnswerHasArrived(ZCL_TUNNEL_CONTROL_PTR tunnel, BOOL answerHasArrived) {

    if (tunnel == NULL)
        return ZCL_TUNNEL_SERVICE_NULL_PTR_ERROR_CODE;

    tunnel->answerHasArrived = answerHasArrived;

    return ZCL_TUNNEL_SERVICE_NO_ERROR_CODE;
}

BOOL ZCLTunnelService_GetAnswerHasArrived(ZCL_TUNNEL_CONTROL_PTR tunnel) {

    return tunnel->answerHasArrived;
}

INT16 ZCLTunnelService_SetRequestTunnelTimeout(ZCL_TUNNEL_CONTROL_PTR tunnel, ZCL_TUNNEL_TIMEOUT requestTunnelTimeout, WORD value) {


    if (tunnel == NULL)
        return ZCL_TUNNEL_SERVICE_NULL_PTR_ERROR_CODE;

    switch (requestTunnelTimeout) {

        case TIMEOUT_STOP:

            vfnOneShotDisable(ZCL_TUNNEL_SERVICE_REQUEST_TUNNEL_TIMEOUT_ONESHOT);
            tunnel->requestTunnelTimeout = TIMEOUT_STOP;
            println_debug(SYSTEM_LOG_ZCL_TUNNEL_SERVICE_ID, "ZCL Tunnel request timeout stopped");

            break;

        case TIMEOUT_INITIALIZED:

            if (value == 0) {

                ZCLTunnelService_SetRequestTunnelTimeout(tunnel, TIMEOUT_STOP, value);

            } else {

                vfnOneShotReload(ZCL_TUNNEL_SERVICE_REQUEST_TUNNEL_TIMEOUT_ONESHOT, value);
                tunnel->requestTunnelTimeout = TIMEOUT_INITIALIZED;
            }

            break;

        case TIMEOUT_EXPIRED:

            vfnOneShotDisable(ZCL_TUNNEL_SERVICE_REQUEST_TUNNEL_TIMEOUT_ONESHOT);
            tunnel->requestTunnelTimeout = TIMEOUT_EXPIRED;
            println_warn(SYSTEM_LOG_ZCL_TUNNEL_SERVICE_ID, "ZCL Tunnel request timeout expired");

            break;

        default:
            return ZCL_TUNNEL_SERVICE_TYPE_NOT_SUPPORTED_ERROR_CODE;
    }

    return ZCL_TUNNEL_SERVICE_NO_ERROR_CODE;
}

ZCL_TUNNEL_TIMEOUT ZCLTunnelService_GetRequestTunnelTimeout(ZCL_TUNNEL_CONTROL_PTR tunnel) {

    return tunnel->requestTunnelTimeout;
}

INT16 ZCLTunnelService_SetRequestTunnelRetries(ZCL_TUNNEL_CONTROL_PTR tunnel, BYTE retries) {

    if (tunnel == NULL)
        return ZCL_TUNNEL_SERVICE_NULL_PTR_ERROR_CODE;

    tunnel->requestTunnelRetries = retries;

    return ZCL_TUNNEL_SERVICE_NO_ERROR_CODE;
}

BYTE ZCLTunnelService_GetRequestTunnelRetries(ZCL_TUNNEL_CONTROL_PTR tunnel) {

    return tunnel->requestTunnelRetries;
}

INT16 ZCLTunnelService_SetCloseTunnelTimeout(ZCL_TUNNEL_CONTROL_PTR tunnel, ZCL_TUNNEL_TIMEOUT closeTunnelTimeout, WORD value) {

    if (tunnel == NULL)
        return ZCL_TUNNEL_SERVICE_NULL_PTR_ERROR_CODE;

    switch (closeTunnelTimeout) {

        case TIMEOUT_STOP:

            vfnOneShotDisable(ZCL_TUNNEL_SERVICE_CLOSE_TUNNEL_TIMEOUT_ONESHOT);
            tunnel->closeTunnelTimeout = TIMEOUT_STOP;
            println_debug(SYSTEM_LOG_ZCL_TUNNEL_SERVICE_ID, "ZCL Close Tunnel timeout stopped");

            break;

        case TIMEOUT_INITIALIZED:

            if (value == 0) {

                ZCLTunnelService_SetCloseTunnelTimeout(tunnel, TIMEOUT_STOP, value);

            } else {

                vfnOneShotReload(ZCL_TUNNEL_SERVICE_CLOSE_TUNNEL_TIMEOUT_ONESHOT, value);
                tunnel->closeTunnelTimeout = TIMEOUT_INITIALIZED;
            }

            break;

        case TIMEOUT_EXPIRED:

            vfnOneShotDisable(ZCL_TUNNEL_SERVICE_CLOSE_TUNNEL_TIMEOUT_ONESHOT);
            tunnel->closeTunnelTimeout = TIMEOUT_EXPIRED;
            println_warn(SYSTEM_LOG_ZCL_TUNNEL_SERVICE_ID, "ZCL Close Tunnel timeout expired");

            break;

        default:
            return ZCL_TUNNEL_SERVICE_TYPE_NOT_SUPPORTED_ERROR_CODE;
    }

    return ZCL_TUNNEL_SERVICE_NO_ERROR_CODE;

}

ZCL_TUNNEL_TIMEOUT ZCLTunnelService_GetCloseConnectionTimeout(ZCL_TUNNEL_CONTROL_PTR tunnel) {

    return tunnel->closeTunnelTimeout;
}

INT16 ZCLTunnelService_SetTransferDataTimeout(ZCL_TUNNEL_CONTROL_PTR tunnel, ZCL_TUNNEL_TIMEOUT transferDataTimeout, WORD value) {

    if (tunnel == NULL)
        return ZCL_TUNNEL_SERVICE_NULL_PTR_ERROR_CODE;

    switch (transferDataTimeout) {

        case TIMEOUT_STOP:

            vfnOneShotDisable(ZCL_TUNNEL_SERVICE_TRANSFER_DATA_TIMEOUT_ONESHOT);
            tunnel->transferDataTimeout = TIMEOUT_STOP;
            println_debug(SYSTEM_LOG_ZCL_TUNNEL_SERVICE_ID, "ZCL Transfer Data timeout stopped");

            break;

        case TIMEOUT_INITIALIZED:

            if (value == 0) {

                ZCLTunnelService_SetTransferDataTimeout(tunnel, TIMEOUT_STOP, value);

            } else {

                vfnOneShotReload(ZCL_TUNNEL_SERVICE_TRANSFER_DATA_TIMEOUT_ONESHOT, value);
                tunnel->transferDataTimeout = TIMEOUT_INITIALIZED;
            }

            break;

        case TIMEOUT_EXPIRED:

            vfnOneShotDisable(ZCL_TUNNEL_SERVICE_TRANSFER_DATA_TIMEOUT_ONESHOT);
            tunnel->transferDataTimeout = TIMEOUT_EXPIRED;
            println_warn(SYSTEM_LOG_ZCL_TUNNEL_SERVICE_ID, "ZCL Transfer Data timeout expired");
            break;

        default:
            return ZCL_TUNNEL_SERVICE_TYPE_NOT_SUPPORTED_ERROR_CODE;
    }

    return ZCL_TUNNEL_SERVICE_NO_ERROR_CODE;
}

ZCL_TUNNEL_TIMEOUT ZCLTunnelService_GetTransferDataTimeout(ZCL_TUNNEL_CONTROL_PTR tunnel) {

    return tunnel->transferDataTimeout;
}

INT16 ZCLTunnelService_SetTransferDataRetries(ZCL_TUNNEL_CONTROL_PTR tunnel, BYTE retries) {

    if (tunnel == NULL)
        return ZCL_TUNNEL_SERVICE_NULL_PTR_ERROR_CODE;

    tunnel->transferDataRetries = retries;

    return ZCL_TUNNEL_SERVICE_NO_ERROR_CODE;
}

BYTE ZCLTunnelService_GetTransferDataRetries(ZCL_TUNNEL_CONTROL_PTR tunnel) {

    return tunnel->transferDataRetries;
}

INT16 ZCLTunnelService_SetTxHeaderBuffer(ZCL_TUNNEL_CONTROL_PTR tunnel, BYTE * txHeaderBuffer){

    if (tunnel == NULL)
        return ZCL_TUNNEL_SERVICE_NULL_PTR_ERROR_CODE;

    tunnel->txHeaderBuffer = txHeaderBuffer;

    return ZCL_TUNNEL_SERVICE_NO_ERROR_CODE;
}

BYTE * ZCLTunnelService_GetTxHeaderBuffer(ZCL_TUNNEL_CONTROL_PTR tunnel){

    return tunnel->txHeaderBuffer;
}

INT16 ZCLTunnelService_SetTxDataBuffer(ZCL_TUNNEL_CONTROL_PTR tunnel, BYTE * txDataBuffer) {

    if (tunnel == NULL)
        return ZCL_TUNNEL_SERVICE_NULL_PTR_ERROR_CODE;

    if (txDataBuffer == NULL)
        return ZCL_TUNNEL_SERVICE_NULL_PTR_ERROR_CODE;
    
    tunnel->txDataBuffer = txDataBuffer;
    
    return ZCL_TUNNEL_SERVICE_NO_ERROR_CODE;
}

BYTE * ZCLTunnelService_GetTxDataBuffer(ZCL_TUNNEL_CONTROL_PTR tunnel) {

    return tunnel->txDataBuffer;
}

INT16 ZCLTunnelService_SetTxDataBufferSize(ZCL_TUNNEL_CONTROL_PTR tunnel, WORD txDataBufferSize){

    if (tunnel == NULL)
        return ZCL_TUNNEL_SERVICE_NULL_PTR_ERROR_CODE;

    tunnel->txDataBufferSize = txDataBufferSize;

    return ZCL_TUNNEL_SERVICE_NO_ERROR_CODE;
}

WORD ZCLTunnelService_GetTxDataBufferSize(ZCL_TUNNEL_CONTROL_PTR tunnel) {

    return tunnel->txDataBufferSize;
}

INT16 ZCLTunnelService_SetHandleReceive(ZCL_TUNNEL_CONTROL_PTR tunnel, ZCLTunnelService_HandleReceive handleReceive) {

    if (tunnel == NULL)
        return ZCL_TUNNEL_SERVICE_NULL_PTR_ERROR_CODE;

    tunnel->handleReceive = handleReceive;

    return ZCL_TUNNEL_SERVICE_NO_ERROR_CODE;
}

ZCLTunnelService_HandleReceive ZCLTunnelService_GetHandleReceive(ZCL_TUNNEL_CONTROL_PTR tunnel) {

    return tunnel->handleReceive;
}

INT16 ZCLTunnelService_Setup(ZCL_TUNNEL_CONTROL_PTR tunnel, WORD tunnelId, WORD shortAddress, BOOL connected, BOOL listening) {

    ZCLTunnelService_SetTunnelId(tunnel, tunnelId);
    ZCLTunnelService_SetShortAddress(tunnel, shortAddress);
    ZCLTunnelService_SetConnected(tunnel, connected);
    ZCLTunnelService_SetListening(tunnel, listening);

    ZCLTunnelService_SetAnswerHasArrived(tunnel, FALSE);

    ZCLTunnelService_SetRequestTunnelTimeout(tunnel, TIMEOUT_STOP, 0);
    ZCLTunnelService_SetRequestTunnelRetries(tunnel, 0);

    ZCLTunnelService_SetCloseTunnelTimeout(tunnel, TIMEOUT_STOP, 0);

    ZCLTunnelService_SetTransferDataTimeout(tunnel, TIMEOUT_STOP, 0);
    ZCLTunnelService_SetTransferDataRetries(tunnel, 0);

    ZCLTunnelService_SetAcknowledgement(tunnel, FALSE);
    ZCLTunnelService_SetWaitingForAnswer(tunnel, FALSE);

    return ZCL_TUNNEL_SERVICE_NO_ERROR_CODE;
}

INT16 ZCLTunnelService_Reset(ZCL_TUNNEL_CONTROL_PTR tunnel) {

    return ZCLTunnelService_Setup(tunnel, ZCL_TUNNEL_SERVICE_NO_TUNNEL_ID, ZCL_TUNNEL_SERVICE_NO_SHORT_ADDRESS, FALSE, FALSE);
}

BOOL ZCLTunnelService_IsNetworkAvailable(void) {

    return /* zstack condition for network */ TRUE;
}

INT16 ZCLTunnelService_Connect(WORD shortAddress, BYTE protocolId, WORD manufactureCode, BYTE flowControl, BYTE destEndPoint) {

    BYTE zclTunnelCluster[ZCL_TUNNEL_SERVICE_CONNECT_FRAME_SIZE];
    BYTE * zclTunnelCluster_ptr = zclTunnelCluster;

    // dstShortAddress
    memcpy(zclTunnelCluster_ptr, (BYTE *) & shortAddress, sizeof (shortAddress));
    zclTunnelCluster_ptr += sizeof (shortAddress);

    // protocolId
    * zclTunnelCluster_ptr = protocolId;
    zclTunnelCluster_ptr++;

    // manufactureCode
    memcpy(zclTunnelCluster_ptr, (BYTE *) & manufactureCode, sizeof (manufactureCode));
    zclTunnelCluster_ptr += sizeof (manufactureCode);

    // flow control
    * zclTunnelCluster_ptr = flowControl;
    zclTunnelCluster_ptr++;

    //! Copy the Tunnel Information so that tunnel entry be added into the table upon
    //! receipt of Request Tunnel Response command based on the status of the Tunnel Request

    /*TunnelingReqEntry.ProtocolId = protocolId;
    memcpy((BYTE *) & TunnelingReqEntry.ManufaturerCode, &manufactureCode, sizeof (manufactureCode));
    TunnelingReqEntry.FlowControl = flowControl;*/

    //! Create  Request Tunnel Command for Tunneling cluster with
    //! length of the payload as 4 bytes
    AppSendData
            (
            ZCL_ClientToServer | ZCL_ClusterSpecificCommand,
            ZCL_TUNNELING_CLUSTER,
            ZCL_TunnelingRequestTunnelCmd,
            zclTunnelCluster_ptr - zclTunnelCluster - sizeof (shortAddress),
            zclTunnelCluster,
            destEndPoint
            );
}

BOOL ZCLTunnelService_IsConnectedWithThisShortAddress(WORD shortAddress) {

    WORD tunnelShortAddress;

    tunnelShortAddress = ZCLTunnelService_GetShortAddress(&zclTunnelControl);

    if (tunnelShortAddress != shortAddress)
        return FALSE;

    return ZCLTunnelService_IsConnected(&zclTunnelControl);
}

INT16 ZCLTunnelService_Close(WORD shortAddress, WORD tunnelId, BYTE destEndPoint, BOOL listening) {

    BYTE zclTunnelCluster[ZCL_TUNNEL_SERVICE_CLOSE_FRAME_SIZE];
    BYTE * zclTunnelCluster_ptr = zclTunnelCluster;

    // dstShortAddress
    memcpy(zclTunnelCluster_ptr, (BYTE *) & shortAddress, sizeof (shortAddress));
    zclTunnelCluster_ptr += sizeof (shortAddress);

    // tunnelId
    memcpy(zclTunnelCluster_ptr, (BYTE *) & tunnelId, sizeof (tunnelId));
    zclTunnelCluster_ptr += sizeof (tunnelId);

    SE_CloseTunnelingEntry(tunnelId);

    if(listening != TRUE){

        AppSendData
            (
            ZCL_ClientToServer | ZCL_ClusterSpecificCommand,
            ZCL_TUNNELING_CLUSTER,
            ZCL_TunnelingCloseTunnelCmd,
            zclTunnelCluster_ptr - zclTunnelCluster - sizeof (shortAddress),
            zclTunnelCluster,
            destEndPoint
            );
    }

    return ZCL_TUNNEL_SERVICE_NO_ERROR_CODE;
}

INT16 ZCLTunnelService_StopDataTransfer(WORD tunnelId) {

    // ACk with 0 Lenght
}

INT16 ZCLTunnelService_ReadyDataTransfer(WORD tunnelId) {

    SE_SendReadyDataCmd(tunnelId);
}

INT16 ZCLTunnelService_SendDataWithLoadedData( WORD shortAddress, WORD tunnelId, WORD dataLen, BYTE destEndPoint) {

    BYTE * zclTunnelCluster = ZCLTunnelService_GetTxHeaderBuffer(&zclTunnelControl);
    BYTE * zclTunnelCluster_ptr = zclTunnelCluster;

    //! Direction
    * zclTunnelCluster_ptr++ = ZCL_ClientToServer | ZCL_ClusterSpecificCommand;

    //! nextSeqNum
    * zclTunnelCluster_ptr++ = appNextSeqNum++;

    //! cmdId
    * zclTunnelCluster_ptr++ = ZCL_TunnelingTransferDataCmd;

    // tunnelId
    memcpy(zclTunnelCluster_ptr, (BYTE *) & tunnelId, sizeof (tunnelId));
    zclTunnelCluster_ptr += sizeof (tunnelId);

    // Data Loadded
    zclTunnelCluster_ptr += dataLen;

    if(dataLen <= ZCL_TUNNEL_SERVICE_MAX_TX_DATA_WITHOUT_FRAGMENT){
    
        App_SendData(APS_ADDRESS_16_BIT, (BYTE *) & shortAddress, destEndPoint, zclTunnelCluster_ptr - zclTunnelCluster, ZCL_TUNNELING_CLUSTER, zclTunnelCluster);

    } else {
        
        ZCLTunnelService_SendFragmentDataRequest(APS_ADDRESS_16_BIT, (BYTE *) & shortAddress, destEndPoint, zclTunnelCluster_ptr - zclTunnelCluster, ZCL_TUNNELING_CLUSTER, zclTunnelCluster);
    }
}

INT16 ZCLTunnelService_SendFragmentDataRequest(BYTE addressMode, BYTE* pDestAddress, BYTE destEndpoint, BYTE asduLength, WORD clusterId, BYTE* pAsdu) {

    UpdateFragmentParams( 0x02, PROFILE_FRAGMENT_INTERFRAME_DELAY, 0x32, asduLength);

    params.APSDE_DATA_request.ProfileId.Val = SE_PROFILE_ID;
    params.APSDE_DATA_request.SrcEndpoint = SOURCE_ENDPOINT;
    params.APSDE_DATA_request.DstEndpoint = destEndpoint;
    params.APSDE_DATA_request.ClusterId.Val = clusterId;
    params.APSDE_DATA_request.TxOptions.Val = SE_GetClusterSpecificSecurity(clusterId);
    params.APSDE_DATA_request.DstAddrMode = addressMode;

    //!params.APSDE_DATA_request.RadiusCounter = DEFAULT_RADIUS;

    memcpy((BYTE*) & params.APSDE_DATA_request.DstAddress.ShortAddr.Val, pDestAddress, SHORT_ADDRESS_LENGTH);
    HandleFragmentDataRequest(asduLength, pAsdu);
}

INT16 ZCLTunnelService_ReceiveData(WORD shortAddress, WORD tunnelId, BYTE * data, WORD dataLen) {

    //! TODO
}

void ZCLTunnelService_UnsynchronizeCallback(ZCL_TUNNEL_EVENT event, BYTE * data, WORD dataLen) {

    TunnelTableEntry * tunnelEntry;
    WORD shortAddress;
    WORD tunnelId;
    ZCL_TunnelingRequestTunnelResponse * tunnelResponse;
    ZCL_TunnelingCloseTunnelCommandPayload * closeTunnel;
    ZCL_TunnelingTransferDataErrorCommandPayload * transferDataError;

    switch (event) {

        case ZCL_TUNNEL_EVENT_NO_INIT:

            // --> Implementation source code for this ZCL_TUNNEL_EVENT here <--

            break;

        case ZCL_TUNNEL_EVENT_REQUEST:

            //************************************************************************************
            // ZCL TUNNEL SERVER EVENT
            //************************************************************************************

            println_info(SYSTEM_LOG_ZCL_TUNNEL_SERVICE_ID, "[UnsynchronizeCallback ZCL_TUNNEL_EVENT_REQUEST]");

            tunnelEntry = (TunnelTableEntry *) data;

            memcpy(&shortAddress, &tunnelEntry->destaddress, sizeof (shortAddress));
            println_log(SYSTEM_LOG_ZCL_TUNNEL_SERVICE_ID, "Tunnel Information");
            println_log(SYSTEM_LOG_ZCL_TUNNEL_SERVICE_ID, "ShortAddress: %04X TunnelId: %04X", shortAddress, tunnelEntry->TunnelId);

            ZCLTunnelService_Setup(&zclTunnelControl, tunnelEntry->TunnelId, shortAddress, TRUE, TRUE);
            API_ZCLTunnelService_Connect(shortAddress);
            ZCLTunnelService_SetCloseTunnelTimeout(&zclTunnelControl, TIMEOUT_INITIALIZED, ZCL_TUNNEL_SERVICE_CLOSE_TUNNEL_TIMEOUT);

            //************************************************************************************
            // ZCL TUNNEL SERVER EVENT
            //************************************************************************************

            break;

        case ZCL_TUNNEL_EVENT_RESPONSE:

            //************************************************************************************
            // ZCL TUNNEL CLIENT EVENT
            //************************************************************************************

            println_info(SYSTEM_LOG_ZCL_TUNNEL_SERVICE_ID, "[UnsynchronizeCallback ZCL_TUNNEL_EVENT_RESPONSE]");
            println_log(SYSTEM_LOG_ZCL_TUNNEL_SERVICE_ID, "Arrived Data: ");
            print_buffer_log(SYSTEM_LOG_ZCL_TUNNEL_SERVICE_ID, data, dataLen);

            tunnelResponse = (ZCL_TunnelingRequestTunnelResponse *) data;

            if (tunnelResponse->TunnelStatus != TUNNEL_SUCCESS)
                return;

            ZCLTunnelService_SetRequestTunnelTimeout(&zclTunnelControl, TIMEOUT_STOP, ZCL_TUNNEL_SERVICE_REQUEST_TUNNEL_TIMEOUT);

            ZCLTunnelService_SetTunnelId(&zclTunnelControl, tunnelResponse->TunnelId);
            ZCLTunnelService_SetConnected(&zclTunnelControl, TRUE);

            ZCLTunnelService_SetCloseTunnelTimeout(&zclTunnelControl, TIMEOUT_INITIALIZED, ZCL_TUNNEL_SERVICE_CLOSE_TUNNEL_TIMEOUT);

            //************************************************************************************
            // ZCL TUNNEL CLIENT EVENT
            //************************************************************************************

            break;

        case ZCL_TUNNEL_EVENT_CONNECT:

            // --> Implementation source code for this ZCL_TUNNEL_EVENT here <--

            break;

        case ZCL_TUNNEL_EVENT_CLOSE:

            //************************************************************************************
            // ZCL TUNNEL SERVER EVENT
            //************************************************************************************

            closeTunnel = (ZCL_TunnelingCloseTunnelCommandPayload *) data;

            println_info(SYSTEM_LOG_ZCL_TUNNEL_SERVICE_ID, "[UnsynchronizeCallback ZCL_TUNNEL_EVENT_CLOSE]");
            println_log(SYSTEM_LOG_ZCL_TUNNEL_SERVICE_ID, "Tunnel Information");
            println_log(SYSTEM_LOG_ZCL_TUNNEL_SERVICE_ID, "TunnelId: %04X", closeTunnel->TunnelId);

            if (closeTunnel->TunnelId != ZCLTunnelService_GetTunnelId(&zclTunnelControl)) {

                println_error(SYSTEM_LOG_ZCL_TUNNEL_SERVICE_ID, "TunnelId: %04X Not found", closeTunnel->TunnelId);
                break;
            }

            //!ZCLTunnelService_ErrorReset();
            ZCLTunnelService_SetStateMachine(ZCL_TUNNEL_SERVICE_CLOSE_STATE, ZCL_TUNNEL_SERVICE_CLOSE_STATE);

            //************************************************************************************
            // ZCL TUNNEL SERVER EVENT
            //************************************************************************************

            break;

        case ZCL_TUNNEL_EVENT_DATA_RECEIVE:

            //************************************************************************************
            // ZCL TUNNEL CLIENT/SERVER EVENT
            //************************************************************************************

            println_info(SYSTEM_LOG_ZCL_TUNNEL_SERVICE_ID, "[UnsynchronizeCallback ZCL_TUNNEL_EVENT_DATA_RECEIVE]");

            memcpy(&tunnelId, data, sizeof (tunnelId));

            data += sizeof (tunnelId);
            dataLen -= sizeof (tunnelId);

            // It needs ti implement tunnelId validation for active connection
            if (tunnelId != ZCLTunnelService_GetTunnelId(&zclTunnelControl)) {

                println_error(SYSTEM_LOG_ZCL_TUNNEL_SERVICE_ID, "TunnelId: %04X Not found", tunnelId);
                break;
            }

            println_log(SYSTEM_LOG_ZCL_TUNNEL_SERVICE_ID, "Tunnel Id %04X Arrived Data (%d Bytes): ", tunnelId, dataLen);
            print_buffer_log(SYSTEM_LOG_ZCL_TUNNEL_SERVICE_ID, data, dataLen);

            ZCLTunnelService_HandleReceive handleReceive = ZCLTunnelService_GetHandleReceive(&zclTunnelControl);

            if (handleReceive != NULL) {

                handleReceive(data, dataLen);

            } else {

                //! DEBUG SORUCE CODE
                println_warn(SYSTEM_LOG_ZCL_TUNNEL_SERVICE_ID, "ZCLTunnelService_HandleReceive is not implemented");
                ZCLTunnelService_SetAnswerHasArrived(&zclTunnelControl, TRUE);
                //! DEBUG SORUCE CODE
            }

            //************************************************************************************
            // ZCL TUNNEL CLIENT/SERVER EVENT
            //************************************************************************************

            break;

        case ZCL_TUNNEL_EVENT_DATA_ERROR:

            // --> Implementation source code for this ZCL_TUNNEL_EVENT here <--
            //!ZCL_TunnelingTransferDataErrorCommandPayload n;

            //************************************************************************************
            // ZCL TUNNEL SERVER EVENT
            //************************************************************************************

            transferDataError = (ZCL_TunnelingTransferDataErrorCommandPayload *) data;

            println_info(SYSTEM_LOG_ZCL_TUNNEL_SERVICE_ID, "[UnsynchronizeCallback ZCL_TUNNEL_EVENT_DATA_ERROR]");
            println_log(SYSTEM_LOG_ZCL_TUNNEL_SERVICE_ID, "Tunnel Information");
            println_log(SYSTEM_LOG_ZCL_TUNNEL_SERVICE_ID, "TunnelId: %04X, Status: %d", transferDataError->TunnelId, transferDataError->TransferDataStatus);

            if (transferDataError->TunnelId != ZCLTunnelService_GetTunnelId(&zclTunnelControl)) {

                println_error(SYSTEM_LOG_ZCL_TUNNEL_SERVICE_ID, "TunnelId: %04X Not found", transferDataError->TunnelId);
                break;
            }

            //!ZCLTunnelService_ErrorReset();
            ZCLTunnelService_SetStateMachine(ZCL_TUNNEL_SERVICE_CLOSE_STATE, ZCL_TUNNEL_SERVICE_CLOSE_STATE);

            //************************************************************************************
            // ZCL TUNNEL SERVER EVENT
            //************************************************************************************

            break;

        case ZCL_TUNNEL_EVENT_DATA_ACK_RECEIVE:

            println_info(SYSTEM_LOG_ZCL_TUNNEL_SERVICE_ID, "[UnsynchronizeCallback ZCL_TUNNEL_EVENT_DATA_ACK_RECEIVE]");
            println_log(SYSTEM_LOG_ZCL_TUNNEL_SERVICE_ID, "Arrived Data: ");
            print_buffer_log(SYSTEM_LOG_ZCL_TUNNEL_SERVICE_ID, data, dataLen);

            ZCL_TunnelingAckTransferDataCommandPayload * tunnelAckTransferData = (ZCL_TunnelingAckTransferDataCommandPayload *) data;

            if (tunnelAckTransferData->TunnelId != ZCLTunnelService_GetTunnelId(&zclTunnelControl))
                return;

            ZCLTunnelService_SetTransferDataTimeout(&zclTunnelControl, TIMEOUT_STOP, ZCL_TUNNEL_SERVICE_TRANSFER_DATA_TIMEOUT);
            ZCLTunnelService_SetAcknowledgement(&zclTunnelControl, TRUE);

            ZCLTunnelService_SetCloseTunnelTimeout(&zclTunnelControl, TIMEOUT_INITIALIZED, ZCL_TUNNEL_SERVICE_CLOSE_TUNNEL_TIMEOUT);


            break;

        case ZCL_TUNNEL_EVENT_SEND_DATA_ACK_REQUEST:

            println_info(SYSTEM_LOG_ZCL_TUNNEL_SERVICE_ID, "[UnsynchronizeCallback ZCL_TUNNEL_EVENT_SEND_DATA_ACK_REQUEST]");
            ZCLTunnelService_SetSendingAcknowledgement(&zclTunnelControl, TRUE);

            break;

        case ZCL_TUNNEL_EVENT_SEND_DATA_ACK_CONFIRM:

            println_info(SYSTEM_LOG_ZCL_TUNNEL_SERVICE_ID, "[UnsynchronizeCallback ZCL_TUNNEL_EVENT_SEND_DATA_ACK_CONFIRM]");
            ZCLTunnelService_SetSendingAcknowledgement(&zclTunnelControl, FALSE);

            break;

        case ZCL_TUNNEL_EVENT_DATA_TRANSMISSION_COMPLETE:

            // --> Implementation source code for this ZCL_TUNNEL_EVENT here <--

            break;

        case ZCL_TUNNEL_EVENT_DATA_READY:

            // --> Implementation source code for this ZCL_TUNNEL_EVENT here <--

            break;

        case ZCL_TUNNEL_EVENT_DATA_STOP:

            // --> Implementation source code for this ZCL_TUNNEL_EVENT here <--

            break;

        default:

            // --> Implementation source code for this ZCL_TUNNEL_EVENT here <--

            break;
    }
}

INT16 API_ZCLTunnelService_Setup(ZCLTunnelService_HandleReceive handleReceive, BYTE * txBuffer) {

    ZCLTunnelService_Reset(&zclTunnelControl);
    ZCLTunnelService_SetHandleReceive(&zclTunnelControl, handleReceive);

    ZCLTunnelService_SetTxHeaderBuffer(&zclTunnelControl, txBuffer);
    ZCLTunnelService_SetTxDataBuffer(&zclTunnelControl, txBuffer + ZCL_TUNNEL_SERVICE_CLUSTER_HEADER_SIZE);
    ZCLTunnelService_SetTxDataBufferSize(&zclTunnelControl, 0);

    return ZCL_TUNNEL_SERVICE_NO_ERROR_CODE;
}

INT16 API_ZCLTunnelService_Connect(WORD shortAddress) {

    BYTE state;

    if (ZCLTunnelService_IsNetworkAvailable() != TRUE)
        return ZCL_TUNNEL_SERVICE_NO_NETWORK_AVAILABLE_ERROR_CODE;

    state = ZCLTunnelService_GetActualState();

    //! MANAGE in a BETTER WAY
    if (state != ZCL_TUNNEL_SERVICE_IDLE_STATE)
        return ZCL_TUNNEL_SERVICE_API_IS_LOCK_ERROR_CODE;
    //! MANAGE in a BETTER WAY


    if (ZCLTunnelService_IsConnectedWithThisShortAddress(shortAddress) != TRUE) {

        ZCLTunnelService_SetShortAddress(&zclTunnelControl, shortAddress);
        ZCLTunnelService_SetStateMachine(ZCL_TUNNEL_SERVICE_WAITING_FOR_CONNECTION_STATE,
                ZCL_TUNNEL_SERVICE_CONNECTED_STATE);
        ZCLTunnelService_SetRequestTunnelRetries(&zclTunnelControl, 0);

    } else {

        ZCLTunnelService_SetStateMachine(ZCL_TUNNEL_SERVICE_CONNECTED_STATE,
                ZCL_TUNNEL_SERVICE_CONNECTED_STATE);
    }

    return ZCL_TUNNEL_SERVICE_NO_ERROR_CODE;
}

INT16 API_ZCLTunnelService_Close(WORD shortAddress, WORD tunnelId) {

    BYTE state;

    if (ZCLTunnelService_IsNetworkAvailable() != TRUE)
        return ZCL_TUNNEL_SERVICE_NO_NETWORK_AVAILABLE_ERROR_CODE;

    state = ZCLTunnelService_GetActualState();

    //! MANAGE in a BETTER WAY
    if (state != ZCL_TUNNEL_SERVICE_CONNECTED_STATE)
        return ZCL_TUNNEL_SERVICE_API_IS_LOCK_ERROR_CODE;
    //! MANAGE in a BETTER WAY


    if (ZCLTunnelService_IsConnectedWithThisShortAddress(shortAddress) != TRUE)
        return ZCL_TUNNEL_SERVICE_IS_NOT_CONNECTED_ERROR_CODE;

    return ZCLTunnelService_Close(shortAddress, tunnelId, ZCL_TUNNEL_SERVICE_DST_END_POINT, ZCLTunnelService_IsListening(&zclTunnelControl));
}

/*
INT16 API_ZCLTunnelService_SendData(WORD shortAddress, BYTE * data, WORD dataLen, BOOL isWaitingForAnswer) {

    BYTE state;

    if (ZCLTunnelService_IsNetworkAvailable() != TRUE)
        return ZCL_TUNNEL_SERVICE_NO_NETWORK_AVAILABLE_ERROR_CODE;

    state = ZCLTunnelService_GetActualState();

    //! MANAGE in a BETTER WAY
    if (state != ZCL_TUNNEL_SERVICE_IDLE_STATE)
        return ZCL_TUNNEL_SERVICE_API_IS_LOCK_ERROR_CODE;
    //! MANAGE in a BETTER WAY


    if (ZCLTunnelService_IsConnectedWithThisShortAddress(shortAddress) != TRUE) {

        ZCLTunnelService_SetShortAddress(&zclTunnelControl, shortAddress);
        ZCLTunnelService_SetStateMachine(ZCL_TUNNEL_SERVICE_WAITING_FOR_CONNECTION_STATE,
                ZCL_TUNNEL_SERVICE_START_DATA_TRANSFER_STATE);
        ZCLTunnelService_SetRequestTunnelRetries(&zclTunnelControl, 0);

    } else {

        ZCLTunnelService_SetStateMachine(ZCL_TUNNEL_SERVICE_START_DATA_TRANSFER_STATE,
                ZCL_TUNNEL_SERVICE_CONNECTED_STATE);
    }

    ZCLTunnelService_SetWaitingForAnswer(&zclTunnelControl, isWaitingForAnswer);
    ZCLTunnelService_SetTransferDataRetries(&zclTunnelControl, 0);
    return ZCLTunnelService_SetTxDataBuffer(&zclTunnelControl, data, dataLen);
}*/

INT16 API_ZCLTunnelService_SendDataWithLoadedData(WORD shortAddress, WORD dataLen, BOOL isWaitingForAnswer) {

    BYTE state;

    if (ZCLTunnelService_IsNetworkAvailable() != TRUE)
        return ZCL_TUNNEL_SERVICE_NO_NETWORK_AVAILABLE_ERROR_CODE;

    state = ZCLTunnelService_GetActualState();

    //! MANAGE in a BETTER WAY
    if ((state != ZCL_TUNNEL_SERVICE_IDLE_STATE) &&
        (state != ZCL_TUNNEL_SERVICE_CONNECTED_STATE))
        return ZCL_TUNNEL_SERVICE_API_IS_LOCK_ERROR_CODE;
    //! MANAGE in a BETTER WAY


    if (ZCLTunnelService_IsConnectedWithThisShortAddress(shortAddress) != TRUE) {

        ZCLTunnelService_SetShortAddress(&zclTunnelControl, shortAddress);
        ZCLTunnelService_SetStateMachine(ZCL_TUNNEL_SERVICE_WAITING_FOR_CONNECTION_STATE,
                ZCL_TUNNEL_SERVICE_START_DATA_TRANSFER_STATE);
        ZCLTunnelService_SetRequestTunnelRetries(&zclTunnelControl, 0);

    } else {

        ZCLTunnelService_SetStateMachine(ZCL_TUNNEL_SERVICE_START_DATA_TRANSFER_STATE,
                ZCL_TUNNEL_SERVICE_CONNECTED_STATE);
    }

    ZCLTunnelService_SetWaitingForAnswer(&zclTunnelControl, isWaitingForAnswer);
    ZCLTunnelService_SetTransferDataRetries(&zclTunnelControl, 0);
    return ZCLTunnelService_SetTxDataBufferSize(&zclTunnelControl, dataLen);
}

BYTE * API_ZCLTunnelService_GetTxHeaderBuffer( void ){

    return ZCLTunnelService_GetTxHeaderBuffer(&zclTunnelControl);
}

BYTE * API_ZCLTunnelService_GetTxDataBuffer( void ){

    return ZCLTunnelService_GetTxDataBuffer(&zclTunnelControl);
}

INT16 API_ZCLTunnelService_SetTxDataBufferSize( WORD txDataBufferSize ){

    return ZCLTunnelService_SetTxDataBufferSize(&zclTunnelControl, txDataBufferSize);
}

WORD API_ZCLTunnelService_GetTxDataBufferSize( void ){

    return ZCLTunnelService_GetTxDataBufferSize(&zclTunnelControl);
}

BOOL API_ZCLTunnelService_IsSendingAcknowledgement(void){

    return ZCLTunnelService_GetSendingAcknowledgement(&zclTunnelControl);
}

void ZCLTunnelService_RequestTunnelTimeoutOneshot(void) {

    ZCLTunnelService_SetRequestTunnelTimeout(&zclTunnelControl, TIMEOUT_EXPIRED, ZCL_TUNNEL_SERVICE_REQUEST_TUNNEL_TIMEOUT);
    //!println_warn(SYSTEM_LOG_ZCL_TUNNEL_ID, "ZCL tunnel request connection timeout expired");

}

void ZCLTunnelService_CloseTunnelTimeoutOneshot(void) {

    ZCLTunnelService_SetCloseTunnelTimeout(&zclTunnelControl, TIMEOUT_EXPIRED, ZCL_TUNNEL_SERVICE_CLOSE_TUNNEL_TIMEOUT);
    //!ZCLTunnelService_ErrorReset();
}

void ZCLTunnelService_TransferDataTimeoutOneshot(void) {

    ZCLTunnelService_SetTransferDataTimeout(&zclTunnelControl, TIMEOUT_EXPIRED, ZCL_TUNNEL_SERVICE_REQUEST_TUNNEL_TIMEOUT);
    //!println_warn(SYSTEM_LOG_ZCL_TUNNEL_ID, "ZCL tunnel transfer data timeout expired");
}

//******************************************************************************
//                  LOCAL ZCL TUNNEL SERVICE STATE MACHINE
//******************************************************************************

void ZCLTunnelService_Driver(void) {

    zclTunnelServiceDriverState[zclTunnelSM.bActualState]();
}

void ZCLTunnelService_SetStateMachine(ZCL_TUNNEL_STATE actualState, ZCL_TUNNEL_STATE nextState) {

    zclTunnelSM.bActualState = actualState;
    zclTunnelSM.bNextState = nextState;

    vfnEventEnable(ZCL_TUNNEL_SERVICE_DRIVER);
    vfnEventPost(ZCL_TUNNEL_SERVICE_DRIVER);
}

ZCL_TUNNEL_STATE ZCLTunnelService_GetActualState(void) {

    return zclTunnelSM.bActualState;
}

ZCL_TUNNEL_STATE ZCLTunnelService_GetNextState(void) {

    return zclTunnelSM.bNextState;
}

void ZCLTunnelService_ErrorReset(void) {

    ZCLTunnelService_SetStateMachine(ZCL_TUNNEL_SERVICE_IDLE_STATE, ZCL_TUNNEL_SERVICE_IDLE_STATE);
    ZCLTunnelService_Reset(&zclTunnelControl);
}

void ZCLTunnelService_IdleState(void) {

    //! TODO
}

void ZCLTunnelService_WaitingForConnectionState(void) {

    WORD shortAddress;
    BYTE requestTunnelRetries;
    ZCL_TUNNEL_TIMEOUT requestTunnelTimeout;
    BYTE actualState;
    WORD tunnelId;

    shortAddress = ZCLTunnelService_GetShortAddress(&zclTunnelControl);

    if (ZCLTunnelService_IsConnected(&zclTunnelControl) == TRUE) {

        actualState = ZCLTunnelService_GetNextState();

        ZCLTunnelService_SetStateMachine(actualState, ZCL_TUNNEL_SERVICE_CONNECTED_STATE);


        tunnelId = ZCLTunnelService_GetTunnelId(&zclTunnelControl);

        println_info(SYSTEM_LOG_ZCL_TUNNEL_SERVICE_ID,
                "ZCL Tunnel Request Processed Successfully ShortAddress: %04X and TunnelId: %04X",
                shortAddress,
                tunnelId);

        return;
    }

    requestTunnelTimeout = ZCLTunnelService_GetRequestTunnelTimeout(&zclTunnelControl);

    if (requestTunnelTimeout == TIMEOUT_INITIALIZED)
        return;

    requestTunnelRetries = ZCLTunnelService_GetRequestTunnelRetries(&zclTunnelControl);

    if ((requestTunnelTimeout != TIMEOUT_INITIALIZED) &&
            (requestTunnelRetries < ZCL_TUNNEL_SERVICE_MAX_REQUEST_TUNNEL_RETRIES)) {

        ZCLTunnelService_Connect(shortAddress,
                ZCL_TUNNEL_SERVICE_PROTOCOL_ID,
                ZCL_TUNNEL_SERVICE_MANUFACTURE_CODE,
                ZCL_TUNNEL_SERVICE_FLOW_CONTROL,
                ZCL_TUNNEL_SERVICE_DST_END_POINT);

        ZCLTunnelService_SetRequestTunnelTimeout(&zclTunnelControl,
                TIMEOUT_INITIALIZED,
                ZCL_TUNNEL_SERVICE_REQUEST_TUNNEL_TIMEOUT);

        requestTunnelRetries++;
        ZCLTunnelService_SetRequestTunnelRetries(&zclTunnelControl, requestTunnelRetries);
        return;

    }

    if ((requestTunnelTimeout == TIMEOUT_EXPIRED) &&
            (requestTunnelRetries == ZCL_TUNNEL_SERVICE_MAX_REQUEST_TUNNEL_RETRIES)) {
        println_error(SYSTEM_LOG_ZCL_TUNNEL_SERVICE_ID, "Short Address %04X ZCl Tunnel Error Request", shortAddress);

        ZCLTunnelService_ErrorReset();
    }

    return;
}

void ZCLTunnelService_ConnectedState(void) {

    WORD shortAddress;
    ZCL_TUNNEL_TIMEOUT closeTunnelTimeout;
    WORD tunnelId;

    if (ZCLTunnelService_IsConnected(&zclTunnelControl) != TRUE) {

        ZCLTunnelService_ErrorReset();
    }

    shortAddress = ZCLTunnelService_GetShortAddress(&zclTunnelControl);
    tunnelId = ZCLTunnelService_GetTunnelId(&zclTunnelControl);

    if (ZCLTunnelService_GetAnswerHasArrived(&zclTunnelControl) == TRUE) {

        // --> Implementation source code for AnswerHasArrived here <--

        ZCLTunnelService_SetStateMachine(ZCL_TUNNEL_SERVICE_CLOSE_STATE, ZCL_TUNNEL_SERVICE_CLOSE_STATE);

        return;
    }

    closeTunnelTimeout = ZCLTunnelService_GetCloseConnectionTimeout(&zclTunnelControl);

    if(((closeTunnelTimeout == TIMEOUT_EXPIRED) && (ZCLTunnelService_IsListening(&zclTunnelControl) == TRUE) ) ||
    (closeTunnelTimeout == TIMEOUT_EXPIRED)  ||
    ((ZCLTunnelService_IsListening(&zclTunnelControl) != TRUE) &&(ZCLTunnelService_IsWaitingForAnswer(&zclTunnelControl) != TRUE)))
    {
       ZCLTunnelService_SetStateMachine(ZCL_TUNNEL_SERVICE_CLOSE_STATE, ZCL_TUNNEL_SERVICE_CLOSE_STATE);
    }

    return;
}

void ZCLTunnelService_PendingDataTransferState(void) {

}

void ZCLTunnelService_StartDataTransferState(void) {

    WORD shortAddress;
    BYTE transferDataRetries;
    ZCL_TUNNEL_TIMEOUT transferDataTimeout;
    BYTE actualState;
    WORD tunnelId;
    BYTE * data;
    WORD dataLen;

    if (ZCLTunnelService_IsConnected(&zclTunnelControl) != TRUE) {

        ZCLTunnelService_ErrorReset();
    }

    if(ZCLTunnelService_GetSendingAcknowledgement(&zclTunnelControl) == TRUE)
        return;

    shortAddress = ZCLTunnelService_GetShortAddress(&zclTunnelControl);
    tunnelId = ZCLTunnelService_GetTunnelId(&zclTunnelControl);

    if (ZCLTunnelService_GetAcknowledgement(&zclTunnelControl) == TRUE) {

        actualState = ZCLTunnelService_GetNextState();

        ZCLTunnelService_SetStateMachine(actualState, ZCL_TUNNEL_SERVICE_CONNECTED_STATE);

        println_info(SYSTEM_LOG_ZCL_TUNNEL_SERVICE_ID,
                "ZCL Tunnel Transfer Data Processed Successfully ShortAddress: %04X and TunnelId: %04X",
                shortAddress,
                tunnelId);

        return;
    }

    transferDataTimeout = ZCLTunnelService_GetTransferDataTimeout(&zclTunnelControl);

    if (transferDataTimeout == TIMEOUT_INITIALIZED)
        return;

    transferDataRetries = ZCLTunnelService_GetTransferDataRetries(&zclTunnelControl);

    if ((transferDataTimeout != TIMEOUT_INITIALIZED) &&
            (transferDataRetries < ZCL_TUNNEL_SERVICE_MAX_TRANSFER_DATA_RETRIES)) {

        
        dataLen = ZCLTunnelService_GetTxDataBufferSize(&zclTunnelControl);

        ZCLTunnelService_SendDataWithLoadedData(shortAddress, tunnelId, dataLen, ZCL_TUNNEL_SERVICE_DST_END_POINT);
        ZCLTunnelService_SetTransferDataTimeout(&zclTunnelControl, TIMEOUT_INITIALIZED, ZCL_TUNNEL_SERVICE_TRANSFER_DATA_TIMEOUT);

        transferDataRetries++;
        ZCLTunnelService_SetTransferDataRetries(&zclTunnelControl, transferDataRetries);
        return;
    }

    if ((transferDataTimeout == TIMEOUT_EXPIRED) &&
            (transferDataRetries == ZCL_TUNNEL_SERVICE_MAX_TRANSFER_DATA_RETRIES)) {
        println_error(SYSTEM_LOG_ZCL_TUNNEL_SERVICE_ID, "Short Address %04X ZCl Tunnel Transfer Data Error", shortAddress);

        ZCLTunnelService_SetStateMachine(ZCL_TUNNEL_SERVICE_CLOSE_STATE, ZCL_TUNNEL_SERVICE_CLOSE_STATE);
    }

    return;
}

void ZCLTunnelService_CloseState(void){

    WORD shortAddress;
    WORD tunnelId;

    if(ZCLTunnelService_GetSendingAcknowledgement(&zclTunnelControl) == TRUE)
        return;

    shortAddress = ZCLTunnelService_GetShortAddress(&zclTunnelControl);
    tunnelId = ZCLTunnelService_GetTunnelId(&zclTunnelControl);

    println_info(SYSTEM_LOG_ZCL_TUNNEL_SERVICE_ID, "Short Address %04X ZCl Tunnel Close", shortAddress);
    ZCLTunnelService_Close(shortAddress, tunnelId, ZCL_TUNNEL_SERVICE_DST_END_POINT, ZCLTunnelService_IsListening(&zclTunnelControl));
    ZCLTunnelService_ErrorReset();

}

void ZCLTunnelService_EndState(void) {

}


