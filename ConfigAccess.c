// Code Implementation File Information ///////////////////////////////
/*
File: ConfigAccess.c
Brief: Implementation file for ConfigAccess code
Details: Implements all functions of the ConfigAccess utilities
Version: 1.0
         01 February 2019
         Initial development of ConfigAccess code
Note: Code based on 'Introduction to C Programming' tutorial
      by Michael E. Leverington
*/

// Header Files ///////////////////////////////////////////////////
#include "ConfigAccess.h"

/*
Function name: getConfigData
Algorithm: Opens file, acquires configuration data,
           returns pointer to data structure
Precondition: For correct operation, file is available, is formatted correctly,
              and has all configuration linex and data although
              the configuration lines are not required to be
              in a specfic order
Postcondition: In correct operation,
               returns pointer to correct configuration data structure
Exceptions: Correct and appropriately (without program failure)
            responds to and reports file access failure,
            incorrectly formatted lead or end descriptors,
            incorrectly formatted prompt, data out of range,
            and incomplete file conditions
Note: None
*/
int getConfigData( char *fileName, ConfigDataType **configData )
{
    // Initialize function/variables
    const int NUM_DATA_LINES = 9;
    int lineCtr = 0;
    const char READ_ONLY_FLAG[] = "r";
    ConfigDataType *tempData;
    FILE *fileAccessPtr;
    char dataBuffer[ MAX_STR_LEN ];
    int intData, dataLineCode;
    double doubleData;
    *configData = NULL;

    fileAccessPtr = fopen( fileName, READ_ONLY_FLAG );

    if( fileAccessPtr == NULL )
    {
        return CFG_FILE_ACCESS_ERR;
    }


    if( getLineTo( fileAccessPtr, MAX_STR_LEN, COLON,
        dataBuffer, IGNORE_LEADING_WS ) != NO_ERR
        || compareString( dataBuffer, "Start Simulator Configuration File" )
        != STR_EQ )
    {
        fclose( fileAccessPtr );
        return CFG_CORRUPT_DESCRIPTOR_ERR;
    }

    tempData = (ConfigDataType *) malloc( sizeof( ConfigDataType ) );

    while( lineCtr < NUM_DATA_LINES )
    {
        if( getLineTo( fileAccessPtr, MAX_STR_LEN, COLON,
            dataBuffer, IGNORE_LEADING_WS ) != NO_ERR )
        {
            free( tempData );
            fclose( fileAccessPtr );
            return INCOMPLETE_FILE_ERR;
        }

        dataLineCode = getDataLineCode( dataBuffer );

        if( dataLineCode != CFG_CORRUPT_PROMPT_ERR )
        {
            if( dataLineCode == CFG_VERSION_CODE )
            {
                fscanf( fileAccessPtr, "%lf", &doubleData );
            }

            else if( dataLineCode == CFG_MD_FILE_NAME_CODE
                  || dataLineCode == CFG_LOG_FILE_NAME_CODE
                  || dataLineCode == CFG_CPU_SCHED_CODE
                  || dataLineCode == CFG_LOG_TO_CODE )
            {
                fscanf( fileAccessPtr, "%s", dataBuffer );
            }

            else
            {
                fscanf( fileAccessPtr, "%d", &intData );
            }

            if( valueInRange( dataLineCode, intData, doubleData, dataBuffer )
                                                                      == True )
            {
                switch( dataLineCode )
                {
                    case CFG_VERSION_CODE:
                        tempData->version = doubleData;
                        break;

                    case CFG_MD_FILE_NAME_CODE:
                        copyString( tempData->metaDataFileName, dataBuffer );
                        break;

                    case CFG_CPU_SCHED_CODE:
                        tempData->cpuSchedCode = getCpuSchedCode( dataBuffer );
                        break;

                    case CFG_QUANT_CYCLES_CODE:
                        tempData->quantumCycles = intData;
                        break;

                    case CFG_MEM_AVAILABLE_CODE:
                        tempData->memAvailable = intData;
                        break;

                    case CFG_PROC_CYCLES_CODE:
                        tempData->procCycleRate = intData;
                        break;

                    case CFG_IO_CYCLES_CODE:
                        tempData->ioCycleRate = intData;
                        break;

                    case CFG_LOG_TO_CODE:
                        tempData->logToCode = getLogToCode( dataBuffer );
                        break;

                    case CFG_LOG_FILE_NAME_CODE:
                        copyString( tempData->logToFileName, dataBuffer );
                        break;
                }
            }

            else
            {
                free( tempData );
                fclose( fileAccessPtr );
                return CFG_DATA_OUT_OF_RANGE_ERR;
            }
        }

        else
        {
            free( tempData );
            fclose( fileAccessPtr );
            return CFG_CORRUPT_PROMPT_ERR;
        }

        lineCtr++;
    }

    if( getLineTo( fileAccessPtr, MAX_STR_LEN, PERIOD,
                                    dataBuffer, IGNORE_LEADING_WS ) != NO_ERR
        || compareString( dataBuffer, "End Simulator Configuration File" )
                                                                   != STR_EQ )
    {
        free( tempData );
        fclose( fileAccessPtr );
        return CFG_CORRUPT_DESCRIPTOR_ERR;
    }

    *configData = tempData;
    fclose( fileAccessPtr );
    return NO_ERR;
}

/*
Function name: getDataLineCode
Algorithm: Tests string for one of known leader strings, returns line number
           if string is correct, returns CFG_CORRUPT_DATA_ERR
           if string is not found
Precondition: DataBuffer is valid C-Style string
Postcondition: Returns line number of data item in terms of a constant
              (e.g., CFG_VERSION_CODE, CFG_CPU_SCHED_CODE, etc.)
Exceptions: Returns CFG_CORRUPT_FILE_ERR if string is not identified
Note: None
*/
int getDataLineCode( char *dataBuffer )
{
    if( compareString( dataBuffer, "Version/Phase" ) == STR_EQ )
    {
        return CFG_VERSION_CODE;
    }

    if( compareString( dataBuffer, "File Path" ) == STR_EQ )
    {
        return CFG_MD_FILE_NAME_CODE;
    }

    if( compareString( dataBuffer, "CPU Scheduling Code" ) == STR_EQ )
    {
        return CFG_CPU_SCHED_CODE;
    }

    if( compareString( dataBuffer, "Quantum Time (cycles)" ) == STR_EQ )
    {
        return CFG_QUANT_CYCLES_CODE;
    }

    if( compareString( dataBuffer, "Memory Available (KB)" ) == STR_EQ )
    {
        return CFG_MEM_AVAILABLE_CODE;
    }

    if( compareString( dataBuffer, "Processor Cycle Time (msec)" ) == STR_EQ )

    {
        return CFG_PROC_CYCLES_CODE;
    }

    if( compareString( dataBuffer, "I/O Cycle Time (msec)" ) == STR_EQ )
    {
        return CFG_IO_CYCLES_CODE;
    }

    if( compareString( dataBuffer, "Log To" ) == STR_EQ )
    {
        return CFG_LOG_TO_CODE;
    }

    if( compareString( dataBuffer, "Log File Path" ) == STR_EQ )
    {
        return CFG_LOG_FILE_NAME_CODE;
    }

    return CFG_CORRUPT_PROMPT_ERR;
}

/*
Function name: getCpuSchedCode
Algorithm: Converts string data (e.g., "SJF-N", "SRTF-P")
           to constant code number to be stored as integer
Precondition: codeStr is a C-Style string with one
              of the specified cpu scheduling operations
Postcondition: Returns code reprsenting scheduling actions
Exceptions: Defaults to FCFS-N code
Note: none
*/
ConfigDataCodes getCpuSchedCode( char *codeStr )
{
    // Initialize function/variables
    int strLen = getStringLength( codeStr );
    char *tempStr = (char *) malloc( strLen + 1 );
    int returnVal = CPU_SCHED_FCFS_N_CODE;

    setStrToLowerCase( tempStr, codeStr );

    if( compareString( tempStr, "sjf-n" ) == STR_EQ )
    {
        returnVal = CPU_SCHED_SJF_N_CODE;
    }

    if( compareString( tempStr, "srtf-p" ) == STR_EQ )
    {
        returnVal = CPU_SCHED_SRTF_P_CODE;
    }

    if( compareString( tempStr, "fcfs-p" ) == STR_EQ )
    {
        returnVal = CPU_SCHED_FCFS_P_CODE;
    }

    if( compareString( tempStr, "rr-p" ) == STR_EQ )
    {
        returnVal = CPU_SCHED_RR_P_CODE;
    }

    free( tempStr );
    return returnVal;
}

/*
Function name: valueInRange
Algorithm: Tests one of three values (int, double, string) for being
           in specified range, depending on data code
           (i.e., specified config value)
Precondition: One of the three data values is valid
Postcondition: Returns True is data is within specified parameters,
               False otherwise
Exceptions: Metadata or logfile names are ignored and return True
Note: None
*/
Boolean valueInRange( int lineCode, int intVal,
    double doubleVal, char *stringVal )
{
    // Initialize function/variables
    Boolean result = True;
    char *tempStr;
    int strLen;

    switch ( lineCode )
    {
        case CFG_VERSION_CODE:
            if( doubleVal < 0.00 || doubleVal > 10.00 )
            {
                result = False;
            }
            break;

        case CFG_CPU_SCHED_CODE:
            strLen = getStringLength( stringVal );
            tempStr = (char *) malloc( strLen + 1 );
            setStrToLowerCase( tempStr, stringVal);

            if( compareString( tempStr, "none" ) != STR_EQ
             && compareString( tempStr, "fcfs-n" ) != STR_EQ
             && compareString( tempStr, "sjf-n" ) != STR_EQ
             && compareString( tempStr, "srtf-p" ) != STR_EQ
             && compareString( tempStr, "fcfs-p" ) != STR_EQ
             && compareString( tempStr, "rr-p" ) != STR_EQ )
            {
                result = False;
            }

            free( tempStr );
            break;

        case CFG_QUANT_CYCLES_CODE:
            if( intVal < 0 || intVal > 100 )
            {
                result = False;
            }
            break;

        case CFG_MEM_AVAILABLE_CODE:
            if( intVal < 0 || intVal > 102400 )
            {
                result = False;
            }
            break;

        case CFG_PROC_CYCLES_CODE:
            if( intVal < 1 || intVal > 1000 )
            {
                result = False;
            }
            break;

        case CFG_IO_CYCLES_CODE:
            if( intVal < 1 || intVal > 10000 )
            {
                result = False;
            }
            break;

        case CFG_LOG_TO_CODE:
            strLen = getStringLength( stringVal );
            tempStr = (char *) malloc( strLen + 1 );
            setStrToLowerCase( tempStr, stringVal);

            if( compareString( tempStr, "both" ) != STR_EQ
             && compareString( tempStr, "monitor" ) != STR_EQ
             && compareString( tempStr, "file" ) != STR_EQ )
            {
                result = False;
            }

            free( tempStr );
            break;
    }

    return result;
}

/*
Function name: getLogToCode
Algorithm: Converts string data (e.g., "File", "Monitor")
           to constant code number to be stored as integer
Precondition: codeStr is a C-Style string with one
              of the specified log to operations
Postcondition: Returns code representing log to actions
Exceptions: Defaults to monitor code
Note: None
*/
ConfigDataCodes getLogToCode( char *logToStr )
{
    // Initialize function/variables
    int strLen = getStringLength( logToStr );
    char *tempStr = (char *) malloc( strLen +1 );
    int returnVal = LOGTO_MONITOR_CODE;

    setStrToLowerCase( tempStr, logToStr );

    if( compareString( tempStr, "both" ) == STR_EQ )
    {
        returnVal = LOGTO_BOTH_CODE;
    }

    if( compareString( tempStr, "file" ) == STR_EQ )
    {
        returnVal = LOGTO_FILE_CODE;
    }

    free( tempStr );
    return returnVal;
}

/*
Function name: displayConfigData
Algorithm: Diagnostic function to show config data output
Precondition: Parameter has pointer to allocated data set
Postcondition: Configuration data is displayed to the screen
Exceptions: None
Note: None
*/
void displayConfigData( ConfigDataType *configData )
{
    // Initialize function/variables
    char displayString[ STD_STR_LEN ];

    printf( "\nConfig File Display\n" );
    printf( "===================\n" );
    printf( "Version                : %3.2f\n", configData->version );
    printf( "Program file name      : %s\n",configData->metaDataFileName );
    configCodeToString( configData->cpuSchedCode, displayString );
    printf( "CPU schedule selection : %s\n", displayString );
    printf( "Quantum time           : %d\n", configData->quantumCycles );
    printf( "Memory Available       : %d\n", configData->memAvailable );
    printf( "Process cycle rate     : %d\n", configData->procCycleRate );
    printf( "I/O cycle rate         : %d\n", configData->ioCycleRate );
    configCodeToString( configData->logToCode, displayString );
    printf( "Log to selection       : %s\n", displayString );
    printf( "Log file name          : %s\n", configData->logToFileName );
}

/*
Function name: configCodeToString
Algorithm: Utility function to support display
           of CPU scheduling or Log To code strings
Precondition: Code variable holds constant value from ConfigDataCodes
              for item (e.g., CPU_SCHED_SRTF_P_CODE, LOGTO_FILE_CODE, etc.)
Postcondition: String parameter holds correct string
               associated with the given constant
Exceptions: None
Note: None
*/
void configCodeToString( int code, char *outString )
{
    char displayStrings[ 8 ][ 10 ] = { "SJF-N", "SRTF-P", "FCFS-P",
                                       "RR-P", "FCFS-N", "Monitor",
                                       "File", "Both" };

    copyString( outString, displayStrings[ code ] );
}

/*
Function name: displayConfigError
Algorithm: Utility function to support display
           of error code strings
Precondition: Parameter holds constant value from error code list
              for item (e.g., CFG_FILE_ACCESS_ERR, etc.)
Postcondition: Correct error string (related to integer error code)
               is displayed
Exceptions: None
Note: None
*/
void displayConfigError( int errCode )
{
    char displayStrings[ 7 ][ 40 ] = { "No Error",
                                       "Incomplete File Error",
                                       "Input Buffer Overrun",
                                       "Configuration File Access Error",
                                       "Corrupt Configuration Descriptor Error",
                                       "Data Out Of Range Configuration Error",
                                       "Corrupt Configuration Prompt Error" };

    printf("\nFATAL ERROR: %s, Program aborted\n", displayStrings[ errCode ] );
}

/*
Function name: clearConfigData
Algorithm: Frees all allocated memory for config data
Precondition: Pointer to config data linked list passed
              in as parameter
Postcondition: Config data memory is freed, pointer is set to null
Exceptions: None
Note: None
*/
void clearConfigData( ConfigDataType **configDataPtr )
{
    if( *configDataPtr != NULL )
    {
        free( *configDataPtr );
    }

    *configDataPtr = NULL;
}
