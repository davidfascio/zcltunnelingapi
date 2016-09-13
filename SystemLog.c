#include "SystemLog.h"

SYSTEM_LOG systemLogTrace[] = {
                                {SYSTEM_LOG_SHELL_ID,       SYSTEM_LOG_PRINT_DEFAULT},
                                {SYSTEM_LOG_QUEUE_ID,       SYSTEM_LOG_PRINT_DEFAULT},
                                {SYSTEM_LOG_COM485_ID,      SYSTEM_LOG_PRINT_DEFAULT},
                                {SYSTEM_LOG_METERING_ID,    SYSTEM_LOG_PRINT_DEFAULT},
                                {SYSTEM_LOG_I2C_ID,         SYSTEM_LOG_PRINT_DEFAULT},
                                {SYSTEM_LOG_MEMEEPROM_ID,   SYSTEM_LOG_PRINT_DEFAULT},
                                {SYSTEM_LOG_DBMS_ID,        SYSTEM_LOG_PRINT_DEFAULT},
                                {SYSTEM_LOG_ZCL_COM_ID,     SYSTEM_LOG_PRINT_DEFAULT},
                                {SYSTEM_LOG_MMU_ID,         SYSTEM_LOG_PRINT_DEFAULT},
                                {SYSTEM_LOG_ZCL_TUNNEL_SERVICE_ID, SYSTEM_LOG_PRINT_ALL},
                                {SYSTEM_LOG_ZCL_TUNNEL_PROTOCOL_ID, SYSTEM_LOG_PRINT_ALL},

                                SYSTEM_LOG_NULL_TRACE
                              };

SYSTEM_LOG_PTR SystemLog_GetTraceById(WORD traceId){

    SYSTEM_LOG_PTR trace_ptr;

    for(trace_ptr = systemLogTrace; trace_ptr->traceId != SYSTEM_LOG_NO_TRACE_ID; trace_ptr++){

        if(trace_ptr->traceId == traceId)
            break;
    }

    if(trace_ptr->traceId == SYSTEM_LOG_NO_TRACE_ID)
        return NULL;

    return trace_ptr;
}

void SystemLog_Printf(BOOL hasPrompt, WORD traceId, WORD traceTag, const char * format, ...){

    char buffer[SYSTEM_LOG_PRINT_MAX_BUFFER_SIZE];
    va_list argptr;
    SYSTEM_LOG_PTR trace;

    trace = SystemLog_GetTraceById(traceId);

    if(trace == NULL)
        return;

    if(!(traceTag & trace->traceTags))
        return;

    va_start(argptr, format);
    vsprintf(buffer, format, argptr);
    va_end(argptr);

    printTerminal(buffer, hasPrompt);
}

void SystemLog_PrintBuffer(WORD traceId, WORD traceTag, BYTE * buffer, WORD bufferLen){

    WORD index;

    for (index = 0; index < bufferLen; index++)
        SystemLog_Printf(FALSE, traceId, traceTag, "%02X ", *buffer++);
}

INT8 Shell_trace(UINT8 argc, char * argv[]){

    BOOL print_trace;
    WORD traceTag;
    WORD traceId;
    SYSTEM_LOG_PTR trace;

    switch (argc){

        case 4:
            if(strcmp(argv[3],"on") == 0)
                print_trace = TRUE;

            else if (strcmp(argv[3], "off") == 0)
                print_trace = FALSE;

            else
                break;

            if (strcmp(argv[2], "msg") == 0){

                traceTag = SYSTEM_LOG_PRINT_MESSAGE;
            } else if (strcmp(argv[2], "error") == 0){

                traceTag = SYSTEM_LOG_PRINT_ERROR;
            } else if (strcmp(argv[2], "info") == 0){

                traceTag = SYSTEM_LOG_PRINT_INFO;
            } else if (strcmp(argv[2], "log") == 0){

                traceTag = SYSTEM_LOG_PRINT_LOG;
            } else if (strcmp(argv[2], "warn") == 0){

                traceTag = SYSTEM_LOG_PRINT_WARN;
            } else if (strcmp(argv[2], "debug") == 0){

                traceTag = SYSTEM_LOG_PRINT_DEBUG;
            } else if (strcmp(argv[2], "-all") == 0){

                traceTag = SYSTEM_LOG_PRINT_ALL;
            }

            else
                break;

            if (strcmp(argv[1], "queue") == 0){

                traceId = SYSTEM_LOG_QUEUE_ID;
            } else if (strcmp(argv[1], "com485") == 0){

                traceId = SYSTEM_LOG_COM485_ID;
            } else if (strcmp(argv[1], "metering") == 0){

                traceId = SYSTEM_LOG_METERING_ID;
            } else if (strcmp(argv[1], "i2c") == 0){

                traceId = SYSTEM_LOG_I2C_ID;
            } else if (strcmp(argv[1], "eeprom") == 0){

                traceId = SYSTEM_LOG_MEMEEPROM_ID;
            } else if (strcmp(argv[1], "dbms") == 0){

                traceId = SYSTEM_LOG_DBMS_ID;
            } else if (strcmp(argv[1], "zclcom") == 0){

                traceId = SYSTEM_LOG_ZCL_COM_ID;
            } else if (strcmp(argv[1], "mmu") == 0){

                traceId = SYSTEM_LOG_MMU_ID;
            }

            else
                break;



            trace = SystemLog_GetTraceById(traceId);

            if(trace == NULL)
                break;

            trace->traceTags =    (print_trace == TRUE) ?
                                (traceTag | trace->traceTags) :
                                (trace->traceTags & (~traceTag));

            trace->traceTags |= SYSTEM_LOG_PRINT;

            return;
            
        default:
            break;
    }

    print(SYSTEM_LOG_SHELL_ID, "\n\n\r\ttrace [traceId] [traceTag] [on | off]\n\r");
    print(SYSTEM_LOG_SHELL_ID, "\ttraceId:\n\r");
    print(SYSTEM_LOG_SHELL_ID, "\t[queue | com485 | metering | i2c | eeprom | dbms | zclcom | mmu]\n\r");
    print(SYSTEM_LOG_SHELL_ID, "\ttraceTag:\n\r");
    print(SYSTEM_LOG_SHELL_ID, "\t[msg | info | log | warn | error | debug | -all ]\n\n\r");
}
