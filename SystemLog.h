#ifndef __SYSTEM_LOG_H__
#define __SYSTEM_LOG_H__

#include <stdarg.h>
#include <stdio.h>
#include "Shell.h"

//! #define NDEBUG

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define SYSTEM_LOG_PRINT_MAX_BUFFER_SIZE                                (300)

#define SYSTEM_LOG_NO_COLOR                           "\e[0m"
#define SYSTEM_LOG_RED_COLOR                          "\e[1;31m"
#define SYSTEM_LOG_GREEN_COLOR                        "\e[1;32m"
#define SYSTEM_LOG_YELLOW_COLOR                       "\e[1;33m"
#define SYSTEM_LOG_BLUE_COLOR                         "\e[1;34m"
#define SYSTEM_LOG_PURPLE_COLOR                       "\e[1;35m"
#define SYSTEM_LOG_CYAN_COLOR                         "\e[1;36m"
#define SYSTEM_LOG_WHITE_COLOR                        "\e[1;37m"

typedef enum{
    SYSTEM_LOG_NO_PRINT         = 0,
    SYSTEM_LOG_PRINT_MESSAGE    = (1 << 0),
    SYSTEM_LOG_PRINT_DEBUG      = (1 << 1),
    SYSTEM_LOG_PRINT_LOG        = (1 << 2),
    SYSTEM_LOG_PRINT_ERROR      = (1 << 3),
    SYSTEM_LOG_PRINT_WARN       = (1 << 4),
    SYSTEM_LOG_PRINT_INFO       = (1 << 5),
    SYSTEM_LOG_PRINT            = (1 << 7)
} SYSTEM_LOG_TRACE_TAG;

typedef enum{
    SYSTEM_LOG_NO_TRACE_ID,
    SYSTEM_LOG_SHELL_ID,
    SYSTEM_LOG_QUEUE_ID,
    SYSTEM_LOG_COM485_ID,
    SYSTEM_LOG_METERING_ID,
    SYSTEM_LOG_I2C_ID,
    SYSTEM_LOG_MEMEEPROM_ID,
    SYSTEM_LOG_DBMS_ID,
    SYSTEM_LOG_ZCL_COM_ID,
    SYSTEM_LOG_MMU_ID,
    SYSTEM_LOG_ZCL_TUNNEL_SERVICE_ID,
    SYSTEM_LOG_ZCL_TUNNEL_PROTOCOL_ID
} SYTEM_LOG_TRACE_ID;

#define SYSTEM_LOG_PRINT_ALL                            (SYSTEM_LOG_PRINT_MESSAGE | \
                                                        SYSTEM_LOG_PRINT_DEBUG | \
                                                        SYSTEM_LOG_PRINT_LOG | \
                                                        SYSTEM_LOG_PRINT_ERROR | \
                                                        SYSTEM_LOG_PRINT_WARN | \
                                                        SYSTEM_LOG_PRINT_INFO | \
                                                        SYSTEM_LOG_PRINT)

#define SYSTEM_LOG_PRINT_DEFAULT                       ( SYSTEM_LOG_PRINT | \
                                                         SYSTEM_LOG_PRINT_ERROR )

typedef struct{
    WORD traceId;
    WORD traceTags;
} SYSTEM_LOG, * SYSTEM_LOG_PTR;

#define SYSTEM_LOG_NULL_TRACE       {SYSTEM_LOG_NO_TRACE_ID, SYSTEM_LOG_NO_PRINT}

void SystemLog_Printf(BOOL hasPrompt, WORD traceId, WORD traceTag, const char * format, ...);
void SystemLog_PrintBuffer(WORD traceId, WORD traceTag, BYTE * buffer, WORD bufferLen);

#define SystemLog_Println(TRACE_ID, TRACE_TAG, M, ...) SystemLog_Printf(TRUE, TRACE_ID, TRACE_TAG, M, ##__VA_ARGS__)

#ifdef NDEBUG

#define println_message( X, M, ...)
#define print_message( X, M,...)
#define print_buffer_message( X, ...)

#define println_debug( X, M, ...)
#define print_debug( X, M, ...)
#define print_buffer_debug( X, ...)

#define println_log( X, M, ...)
#define print_log( X, M, ...)
#define print_buffer_log( X, ...)

#define println_error( X, M, ...)
#define print_error( X, M, ...)
#define print_buffer_error( X, ...)

#define println_warn( X, M, ...)
#define print_warn( X, M, ...)
#define print_buffer_warn( X, ...)

#define println_info( X, M, ...)
#define print_info( X, M, ...)
#define print_buffer_info( X, ...)

#define println( X, M,...)
#define print( X, M,...)
#define print_buffer( X, ...)

#else

#define println_message( X, M, ...) SystemLog_Println( X, SYSTEM_LOG_PRINT_MESSAGE, SYSTEM_LOG_BLUE_COLOR "[MSG] " SYSTEM_LOG_NO_COLOR "(%s) " M,__FILENAME__, ##__VA_ARGS__)
#define print_message( X, M,...) SystemLog_Printf(FALSE, X, SYSTEM_LOG_PRINT_MESSAGE,  M, ##__VA_ARGS__)
#define print_buffer_message( X, ...) SystemLog_PrintBuffer( X,  SYSTEM_LOG_PRINT_MESSAGE,  ##__VA_ARGS__ )

#define println_debug( X, M, ...) SystemLog_Println( X, SYSTEM_LOG_PRINT_DEBUG, SYSTEM_LOG_PURPLE_COLOR "[DEBUG] " SYSTEM_LOG_NO_COLOR "(%s:%d: function: %s) " M, __FILENAME__ ,__LINE__, __func__, ##__VA_ARGS__)
#define print_debug( X, M, ...) SystemLog_Printf(FALSE, X, SYSTEM_LOG_PRINT_DEBUG, M, ##__VA_ARGS__)
#define print_buffer_debug( X, ...) SystemLog_PrintBuffer( X, SYSTEM_LOG_PRINT_DEBUG, ##__VA_ARGS__ )

#define println_log( X, M, ...) SystemLog_Println( X, SYSTEM_LOG_PRINT_LOG, SYSTEM_LOG_CYAN_COLOR "[LOG] " SYSTEM_LOG_NO_COLOR "(%s) " M, __FILENAME__, ##__VA_ARGS__)
#define print_log( X, M, ...)  SystemLog_Printf(FALSE, X, SYSTEM_LOG_PRINT_LOG,  M, ##__VA_ARGS__)
#define print_buffer_log( X, ...) SystemLog_PrintBuffer( X, SYSTEM_LOG_PRINT_LOG, ##__VA_ARGS__ )

#define println_error( X, M, ...) SystemLog_Println( X, SYSTEM_LOG_PRINT_ERROR, SYSTEM_LOG_RED_COLOR "[ERROR] " "(%s:%d: function: %s) " M SYSTEM_LOG_NO_COLOR, __FILENAME__ ,__LINE__, __func__, ##__VA_ARGS__)
#define print_error( X, M, ...) SystemLog_Printf(FALSE, X, SYSTEM_LOG_PRINT_ERROR,  M,  ##__VA_ARGS__)
#define print_buffer_error( X, ...) SystemLog_PrintBuffer( X, SYSTEM_LOG_PRINT_ERROR, ##__VA_ARGS__ )

#define println_warn( X, M, ...) SystemLog_Println( X, SYSTEM_LOG_PRINT_WARN, SYSTEM_LOG_YELLOW_COLOR "[WARN] " SYSTEM_LOG_NO_COLOR "(%s:%d: function: %s) " M, __FILENAME__ ,__LINE__, __func__, ##__VA_ARGS__)
#define print_warn( X, M, ...) SystemLog_Printf(FALSE, X, SYSTEM_LOG_PRINT_WARN,  M,  ##__VA_ARGS__)
#define print_buffer_warn( X, ...) SystemLog_PrintBuffer( X, SYSTEM_LOG_PRINT_WARN, ##__VA_ARGS__ )

#define println_info( X, M, ...) SystemLog_Println( X, SYSTEM_LOG_PRINT_INFO, SYSTEM_LOG_GREEN_COLOR "[INFO] " SYSTEM_LOG_NO_COLOR "(%s) " M, __FILENAME__,  ##__VA_ARGS__)
#define print_info( X, M, ...) SystemLog_Printf(FALSE, X, SYSTEM_LOG_PRINT_INFO,  M,  ##__VA_ARGS__)
#define print_buffer_info( X, ...) SystemLog_PrintBuffer( X, SYSTEM_LOG_PRINT_INFO, ##__VA_ARGS__ )

#define println( X, M,...) SystemLog_Println( X, SYSTEM_LOG_PRINT, M,##__VA_ARGS__)
#define print( X, M,...) SystemLog_Printf(FALSE, X, SYSTEM_LOG_PRINT, M,##__VA_ARGS__)
#define print_buffer( X, ...) SystemLog_PrintBuffer( X, SYSTEM_LOG_PRINT,  ##__VA_ARGS__ )

#endif

SYSTEM_LOG_PTR SystemLog_GetTraceById(WORD traceId);
INT8 Shell_trace(UINT8 argc, char * argv[]);


#endif /* __SYSTEM_LOG_H__ */
