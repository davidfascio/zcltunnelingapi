//**********************************************************************
//* ZCL_TUNNEL_PROTOCOL Includes
//**********************************************************************
#include "ZCLTunnelProtocol.h"

//**********************************************************************
//* ZCL_TUNNEL_PROTOCOL Variables
//**********************************************************************
ZCL_TUNNEL_PROTOCOL zclTunnelProtocol;

//**********************************************************************
//* ZCL_TUNNEL_PROTOCOL Functions
//**********************************************************************

void ZCLTunnelProtocol_SetFrameSize(WORD frameSize) {
    
    zclTunnelProtocol.frameSize = frameSize;
}

WORD ZCLTunnelProtocol_GetFrameSize(void) {

    return zclTunnelProtocol.frameSize;
}

void ZCLTunnelProtocol_SetCommandId(BYTE commandId) {

    zclTunnelProtocol.commandId = commandId;
}

BYTE ZCLTunnelProtocol_GetCommandId(void) {

    return zclTunnelProtocol.commandId;
}

INT16 ZCLTunnelProtocol_SetData(BYTE * data, WORD dataLen) {

    if (dataLen > ZCL_TUNNEL_PROTOCOL_MAX_BUFFER_SIZE)
        return ZCL_TUNNEL_PROTOCOL_MAX_BUFFER_SIZE_ERROR_CODE;

    memset(zclTunnelProtocol.data, 0, ZCL_TUNNEL_PROTOCOL_MAX_BUFFER_SIZE);
    memcpy(zclTunnelProtocol.data, data, dataLen);
    zclTunnelProtocol.dataLen = dataLen;

    return ZCL_TUNNEL_PROTOCOL_NO_ERROR_CODE;
}

BYTE * ZCLTunnelProtocol_GetData(void) {

    return zclTunnelProtocol.data;
}

WORD ZCLTunnelProtocol_GetDataLen(void) {

    return zclTunnelProtocol.dataLen;
}

INT16 ZCLTunnelProtocol_SetTxHeader(BYTE * txHeader){

    if (txHeader == NULL)
        return ZCL_TUNNEL_PROTOCOL_NULL_PTR_ERROR_CODE;
    
    zclTunnelProtocol.txHeader = txHeader;

    return ZCL_TUNNEL_PROTOCOL_NO_ERROR_CODE;
}

BYTE * ZCLTunnelProtocol_GetTxHeader(void){
    
    return zclTunnelProtocol.txHeader;
}

INT16 ZCLTunnelProtocol_SetTxData(BYTE * txData){
    
    if (txData == NULL)
        return ZCL_TUNNEL_PROTOCOL_NULL_PTR_ERROR_CODE;
    
    zclTunnelProtocol.txData = txData;

    return ZCL_TUNNEL_PROTOCOL_NO_ERROR_CODE;
}

BYTE * ZCLTunnelProtocol_GetTxData(void){
    
    return zclTunnelProtocol.txData;
}

void ZCLTunnelProtocol_SetTxDataSize(WORD txDataSize){

    zclTunnelProtocol.txDataSize = txDataSize;
}

WORD ZCLTunnelProtocol_GetTxDataSize(void){

    return zclTunnelProtocol.txDataSize;
}

INT16 ZCLTunnelProtocol_Setup(BYTE *  txBuffer){

    ZCLTunnelProtocol_SetTxHeader(txBuffer);
    ZCLTunnelProtocol_SetTxData(txBuffer + ZCL_TUNNEL_PROTOCOL_HEADER_SIZE);
    ZCLTunnelProtocol_SetTxDataSize(0);

    return ZCL_TUNNEL_PROTOCOL_NO_ERROR_CODE;
}

INT16 ZCLTunnelProtocol_SetReception(WORD frameSize, BYTE commandId, BYTE * data, WORD dataLen) {

    ZCLTunnelProtocol_SetFrameSize( frameSize);
    ZCLTunnelProtocol_SetCommandId( commandId);
    return ZCLTunnelProtocol_SetData( data, dataLen);
}

//******************************************************************
//*
//* IMPORTANT
//* Pagging is not supported at this momment
//*
//******************************************************************

//******************************************************************************
//                      Z C L  T U N N E L  P R O T O C O L
//******************************************************************************
//    WORD frameSize;
//    BYTE functionCode;
//    BYTE dataReceived[COM_PROTOCOL_DATA_RECEIVED_MAX_SIZE];
//    WORD dataReceivedSize;
//    WORD crcValue;
/*
INT16 ZCLTunnelProtocol_BuildFrame(BYTE commandId, BYTE * data, WORD dataSize, BYTE * frame, WORD * frameLen) {

    BYTE * frame_ptr = frame;
    WORD frameSize;
    WORD crc;

    frameSize = dataSize + sizeof (commandId);

    memcpy(frame_ptr, &frameSize, sizeof (frameSize));
    frame_ptr += sizeof (frameSize);

    * frame_ptr = commandId;
    frame_ptr += sizeof (commandId);

    memcpy(frame_ptr, data, dataSize);
    frame_ptr += dataSize;

    crc = wfnCRC_CALC(frame + sizeof (frameSize), frame_ptr - (frame + sizeof (frameSize)), 0xFFFF);

    memcpy(frame_ptr, &crc, sizeof (crc));
    frame_ptr += sizeof (crc);

    *frameLen = (frame_ptr - frame);

    return ZCL_TUNNEL_PROTOCOL_NO_ERROR_CODE;
}*/

//******************************************************************************
//                      Z C L  T U N N E L  P R O T O C O L
//******************************************************************************
//    WORD frameSize;
//    BYTE functionCode;
//    BYTE dataReceived[COM_PROTOCOL_DATA_RECEIVED_MAX_SIZE];
//    WORD dataReceivedSize;
//    WORD crcValue;

INT16 ZCLTunnelProtocol_BuildFrameWithLoadedData(BYTE commandId, WORD dataSize, BYTE * frame, WORD * frameLen) {

    BYTE * frame_ptr = frame;
    WORD frameSize;
    WORD crc;    

    frameSize = dataSize + sizeof (commandId);

    memcpy(frame_ptr, &frameSize, sizeof (frameSize));
    frame_ptr += sizeof (frameSize);

    * frame_ptr = commandId;
    frame_ptr += sizeof (commandId);

    frame_ptr += dataSize;

    crc = wfnCRC_CALC(frame + sizeof (frameSize), frame_ptr - (frame + sizeof (frameSize)), 0xFFFF);

    memcpy(frame_ptr, &crc, sizeof (crc));
    frame_ptr += sizeof (crc);

    *frameLen = (frame_ptr - frame);

    return ZCL_TUNNEL_PROTOCOL_NO_ERROR_CODE;
}

INT16 ZCLTunnelProtocol_SendFrameWithLoadedData( WORD shortAddress, BYTE commandId, BOOL isWaitingForAnswer){

    BYTE * frame;
    WORD frameLen;
    WORD txDataSize;
    INT16 error_code;
    
    frame = ZCLTunnelProtocol_GetTxHeader();
    txDataSize = ZCLTunnelProtocol_GetTxDataSize();

    error_code = ZCLTunnelProtocol_BuildFrameWithLoadedData(commandId, txDataSize, frame, &frameLen);

    if (error_code != ZCL_TUNNEL_PROTOCOL_NO_ERROR_CODE) {

        println_error(SYSTEM_LOG_ZCL_TUNNEL_PROTOCOL_ID, "It could not build frame");
        return error_code;
    }

    return API_ZCLTunnelService_SendDataWithLoadedData(shortAddress, frameLen, isWaitingForAnswer);
}

INT16 ZCLTunnelProtocol_SendFrame( WORD shortAddress, BYTE commandId, BYTE * data, WORD dataLen, BOOL isWaitingForAnswer){

    BYTE * txData = ZCLTunnelProtocol_GetTxData();

    memcpy(txData, data, dataLen);
    ZCLTunnelProtocol_SetTxDataSize(dataLen);

    return ZCLTunnelProtocol_SendFrameWithLoadedData(shortAddress, commandId, isWaitingForAnswer);
}

/*
INT16 ZCLTunnelProtocol_SendCustomFrame(WORD shortAddress, BYTE commandId, BYTE * data, WORD dataLen, BOOL isWaitingForAnswer) {

    BYTE * frame;
    WORD frameLen;
    INT16 error_code;


    error_code = ZCLTunnelProtocol_BuildFrame(commandId, data, dataLen, frame, & frameLen);

    if (error_code != ZCL_TUNNEL_PROTOCOL_NO_ERROR_CODE) {

        println_error(SYSTEM_LOG_ZCL_TUNNEL_PROTOCOL_ID, "It could not build frame");
        return error_code;
    }


    return API_ZCLTunnelService_SendData(shortAddress, frame, frameLen, isWaitingForAnswer);
}
*/
INT16 ZCLTunnelProtocol_HandleReceive(BYTE * data, WORD dataLen) {

    WORD frameSize;
    BYTE commandId;
    WORD crc;
    BYTE * data_ptr = data;

    //******************************************************************
    //* Size validation
    //*
    //* IMPORTANT
    //* Pagging is not supported at this momment
    //*
    //******************************************************************

    memcpy((BYTE *) & frameSize, data_ptr, sizeof (frameSize));
    data_ptr += sizeof (frameSize);
    dataLen -= sizeof (frameSize);

    if (frameSize != dataLen - sizeof(crc))
        return ZCL_TUNNEL_PROTOCOL_FRAME_SIZE_ERROR_CODE;


    //******************************************************************
    //* CRC validation
    //*
    //* IMPORTANT
    //* Pagging is not supported at this momment
    //*
    //******************************************************************

    crc = wfnCRC_CALC(data_ptr, dataLen, ZCL_TUNNEL_PROTOCOL_HANDLER_CRC_BASE);

    if (crc != ZCL_TUNNEL_PROTOCOL_HANDLER_CRC_OK)
        return ZCL_TUNNEL_PROTOCOL_CRC_ERROR_CODE;

    dataLen -= sizeof (crc);

    //******************************************************************
    //* Command Id
    //*
    //* IMPORTANT
    //* Pagging is not supported at this momment
    //*
    //******************************************************************

    commandId = *data_ptr;
    data_ptr += sizeof (commandId);
    dataLen -= sizeof (commandId);

    ZCLTunnelProtocol_SetReception( frameSize, commandId, data_ptr, dataLen);

    return ZCL_TUNNEL_PROTOCOL_NO_ERROR_CODE;
}

INT16 ZCLTunnelProtocol_ErrorProcess( void ) {

    

    //!memset(zclTunnelProtocol, 0, sizeof (ZCL_TUNNEL_PROTOCOL));

    //**************************************************************************
    //* It needs to implement another clean process for reception handling
    //**************************************************************************

    return ZCL_TUNNEL_PROTOCOL_NO_ERROR_CODE;
}

//******************************************************************************
//                  ZCL TUNNEL SERVICE SHELL
//******************************************************************************

INT8 Shell_tunnel(UINT8 argc, char * argv[]) {

    SHORT_ADDR short_format;
    WORD shortAddress;
    BYTE commandId;

    switch (argc) {

        case 3:
            if (strcmp(argv[1], "connect") == 0) {


                if (zcl_get_short_format(argv[2], &short_format))
                    break;


                memcpy((BYTE *) & shortAddress, (BYTE *) short_format.v, 2);
                API_ZCLTunnelService_Connect(shortAddress);
                return;
            }

            break;


        case 5:

            if (zcl_get_short_format(argv[3], &short_format))
                break;

            memcpy((BYTE *) & shortAddress, (BYTE *) short_format.v, 2);

            if (strcmp(argv[1], "meter") == 0) {

                if (strcmp(argv[2], "read") == 0){

                    commandId = 0x10;

                }

                else{
                    break;
                }
                
                ZCLTunnelProtocol_SendFrame( shortAddress, commandId, argv[4], strlen(argv[4]), TRUE);

                return;
            }

            break;

        default:
            break;
    }

    print(SYSTEM_LOG_ZCL_TUNNEL_SERVICE_ID, "\n\n\r\ttunnel [action] [option]\n\r");
}
