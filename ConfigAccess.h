// Header File Information ////////////////////////////////////////////
/*
File: ConfigAccess.h
Brief: Header file for ConfigAccess code
Details: Specifies functions, constants, and other information
         related to ConfigAccess code
Version: 1.0
         01 February 2019
         Initial development of ConfigAccess code
Note: Code based on 'Introduction to C Programming' tutorial
      by Michael E. Leverington
*/

// Precompiler Directives //////////////////////////////////////////
#ifndef CONFIG_ACCESS_H
#define CONFIG_ACCESS_H

// Header Files ///////////////////////////////////////////////////
#include "StringUtils.h"

// Data Structure Definitions (structs, enums, etc.)///////////////

typedef enum ConfigCodeMessages
{
    CFG_FILE_ACCESS_ERR = 3,
    CFG_CORRUPT_DESCRIPTOR_ERR,
    CFG_DATA_OUT_OF_RANGE_ERR,
    CFG_CORRUPT_PROMPT_ERR,
    CFG_VERSION_CODE,
    CFG_MD_FILE_NAME_CODE,
    CFG_CPU_SCHED_CODE,
    CFG_QUANT_CYCLES_CODE,
    CFG_MEM_AVAILABLE_CODE,
    CFG_PROC_CYCLES_CODE,
    CFG_IO_CYCLES_CODE,
    CFG_LOG_TO_CODE,
    CFG_LOG_FILE_NAME_CODE
} ConfigCodeMessages;

typedef enum ConfigDataCodes
{
    CPU_SCHED_SJF_N_CODE,
    CPU_SCHED_SRTF_P_CODE,
    CPU_SCHED_FCFS_P_CODE,
    CPU_SCHED_RR_P_CODE,
    CPU_SCHED_FCFS_N_CODE,
    LOGTO_MONITOR_CODE,
    LOGTO_FILE_CODE,
    LOGTO_BOTH_CODE
} ConfigDataCodes;

typedef struct ConfigDataType
{
    double version;
    char metaDataFileName[ 100 ];
    int cpuSchedCode;
    int quantumCycles;
    int memAvailable;
    int procCycleRate;
    int ioCycleRate;
    int logToCode;
    char logToFileName[ 100 ];
} ConfigDataType;

// Function Prototypes ///////////////////////////////////////////
int getConfigData( char *fileName, ConfigDataType **configData );
int getDataLineCode( char *dataBuffer );
Boolean valueInRange( int lineCode, int intVal,
                                          double doubleVal, char *stringVal );
ConfigDataCodes getCpuSchedCode( char *codeStr );
ConfigDataCodes getLogToCode( char *logToStr );
void displayConfigData( ConfigDataType *configData );
void configCodeToString( int code, char *outString );
void displayConfigError( int errCode );
void clearConfigData( ConfigDataType **configDataPtr );

// Terminating Precompiler Directives ///////////////////////////////
#endif  // CONFIG_ACCESS_H
