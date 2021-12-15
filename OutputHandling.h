// Header File Information ////////////////////////////////////////////
/*
File: OutputHandling.h
Brief: Header file for OutputHandling code
Details: Specifies functions, constants, and other information
         related to OutputHandling code
Version: 1.0
         21 February 2019
         Initial development of OutputHandling code
Note: None
*/

// Precompiler directives /////////////////////////////////////////////////////
#ifndef OUTPUT_HANDLING_H
#define OUTPUT_HANDLING_H

// Header files ///////////////////////////////////////////////////////////////

#include "ConfigAccess.h"
#include "StringUtils.h"

// Data Structure Definitions (structs, enums, etc.)//////////////////////////

typedef enum OutputHandlingMessages
{
    LOG_FILE_ACCESS_ER = 3
} OutputHandlingMessages;

typedef struct OutputType
{
    char text[ 100 ];
    struct OutputType *next;
} OutputType;

// Function Prototypes  //////////////////////////////////////////////////////
void outputLine(ConfigDataType *configDataPtr, OutputType *outputLog,
                                                             char *outputLine);
int writeOutputLogToFile( ConfigDataType *configDataPtr,
                                                    OutputType *outputLog );
OutputType *clearOutputLog( OutputType *outputLog );
void displayOutputError( int errCode );

// Terminating Precompiler Directives ////////////////////////////////////////
#endif // OUTPUT_HANDLING_H
