// Code Implementation File Information ///////////////////////////////
/*
File: OutputHandling.c
Brief: Implementation file for OutputHandling code
Details: Implements all functions of the OutputHandling for OS simulator
Version: 1.0
         21 February 2019
         Initial development of OutputHandling code
Note:
*/

// Header Files ///////////////////////////////////////////////////
#include "OutputHandling.h"

/*
Function name: outputLine
Algorithm: Checks os configuration to determine where output should be placed;
            for monitor or both, displays line to the screen;
            for file or both, adds node to outputLog linked list
Precondition: Given config, an output log pointer, and an output line
Postcondition: For monitor or both, displays line to the screen;
               for file or both, adds node to outputLog linked list
Exceptions: None
Note: None
*/
void outputLine( ConfigDataType *configDataPtr, OutputType *outputLog,
                                                             char *outputLine )
{
    if( configDataPtr->logToCode == LOGTO_MONITOR_CODE
        || configDataPtr->logToCode == LOGTO_BOTH_CODE )
    {
        printf( "%s", outputLine );
    }
    if( configDataPtr->logToCode == LOGTO_FILE_CODE
        || configDataPtr->logToCode == LOGTO_BOTH_CODE )
    {
        OutputType *currentLine = outputLog;
        while( currentLine->next != NULL)
        {
            currentLine = currentLine->next;
        }
        currentLine->next = (OutputType *)malloc( sizeof( OutputType ) );
        currentLine = currentLine->next;
        copyString( currentLine->text, outputLine );
        currentLine->next = NULL;
    }
}

/*
Function name: writeOutputLogToFile
Algorithm: If file or both is specied in OS simulator configuration,
           opens the file specied in os config, writes the log file header,
           and runs through an output log linked list writing each line to
           the file
Precondition: Given a os config and an output log, with or without data
Postcondition: Specified file contains header and all log information,
                only if file or both is specied in OS simulator configuration
Exceptions: None
Note: None
*/
int writeOutputLogToFile( ConfigDataType *configDataPtr,
                                                    OutputType *outputLog )
{
    if(configDataPtr->logToCode == LOGTO_FILE_CODE
        || configDataPtr->logToCode == LOGTO_BOTH_CODE)
    {
        const char WRITE_ONLY_FLAG[] = "w";
        FILE *fileAccessPtr;
        OutputType *currentLine;
        char codeString[ STD_STR_LEN ];

        fileAccessPtr = fopen( configDataPtr->logToFileName, WRITE_ONLY_FLAG );
        if( fileAccessPtr == NULL )
        {
            return LOG_FILE_ACCESS_ER;
        }

        fprintf( fileAccessPtr,
            "==================================================\n" );
        fprintf( fileAccessPtr, "Simulator Log File Header\n\n" );

        fprintf( fileAccessPtr, "File Name                       : %s\n",
                                            configDataPtr->metaDataFileName );
        configCodeToString( configDataPtr->cpuSchedCode, codeString );
        fprintf( fileAccessPtr, "CPU Scheduling                  : %s\n",
                                                                codeString );
        fprintf( fileAccessPtr, "Quantum Cycles                  : %d\n",
                                                configDataPtr->quantumCycles );
        fprintf( fileAccessPtr, "Memory Available (KB)           : %d\n",
                                                configDataPtr->memAvailable );
        fprintf( fileAccessPtr, "Processor Cycle Rate (ms/cycle) : %d\n",
                                                configDataPtr->procCycleRate );
        fprintf( fileAccessPtr, "I/O Cycle Rate (ms/cycle)       : %d\n\n",
                                                configDataPtr->ioCycleRate );

        currentLine = outputLog->next;
        while( currentLine != NULL )
        {
            fprintf( fileAccessPtr, "%s", currentLine->text );
            currentLine = currentLine->next;
        }

        fclose( fileAccessPtr );
    }
    return NO_ERR;
}

/*
Function name: clearOutputLog
Algorithm: Recusively calls itself through a output log linked list,
           returns memeory to OS from the bottom of the list upward
Precondition: Given output log, with or without data
Postcondition: All node memory, if any, is returned to OS,
               return pointer (head) is set to null
Exceptions: None
Note: None
*/
OutputType *clearOutputLog( OutputType *outputLog )
{
    if( outputLog != NULL )
    {
        if( outputLog->next != NULL )
        {
            clearOutputLog( outputLog->next );
        }
        free( outputLog );
    }
    return NULL;
}

/*
Function name: displayOutputError
Algorithm: Uses enum/constant values as indices to select display string,
           then displays error message with selected string
Precondition: Integer input code has one of the enumerated code values
Postcondition: Error message is displated with the correct
               local error item
Exceptions: None
Note: None
*/
void displayOutputError( int errCode )
{
    char displayStrings[ 1 ][ 40 ] = { "Logfile Access Error" };
    printf("\nFATAL ERROR: %s, Program aborted\n", displayStrings[ errCode ] );
}
