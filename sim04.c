// Program Information ////////////////////////////////////////////
/*
File: sim04.c
Brief: Driver program to upload config file, upload metadata files,
       and run through a simluated OS
Details: Demonstrates upload and display of config and metadata files
Version: 1.0
         28 March 2019
         Initial development of sim03 driver program
Note: None
*/

// Header Files ///////////////////////////////////////////////////
#include <stdio.h>
#include "ConfigAccess.h"
#include "MetaDataAccess.h"
#include "Simulator.h"

/*
Function name: main
Algorithm: Driver function upload config file, upload metadata files,
           and run through a simluated OS
Precondition: none
Postcondition: Returns zero (0) on success
Exceptions: None
Note: None
*/
int main( int argc, char **argv )
{

    // Global Constant Definitions ////////////////////////////////
    int configAccessResult, mdAccessResult, runSimulatorResult;
    char configFileName[ MAX_STR_LEN ];
    char mdFileName[ MAX_STR_LEN ];
    ConfigDataType *configDataPtr;
    OpCodeType *mdData;

    printf( "\nSimulator Program\n" );
    printf( "=================\n\n" );

    if( argc < 2)
    {
        printf( "ERROR: Program requires file name for config file " );
        printf( "as command line argument\n" );
        printf( "Program Terminated\n" );
        return 1;
    }
    else if( argc > 2)
    {
        printf( "ERROR: Program requires file name for config file " );
        printf( "as only command line argument\n" );
        printf( "Program Terminated\n" );
        return 1;
    }
    else
    {
        printf( "Uploading Configuration Files\n\n" );
        copyString( configFileName, argv[1] );
        configAccessResult = getConfigData( configFileName, &configDataPtr );
    }

    if( configAccessResult == NO_ERR )
    {
        printf( "Uploading Meta Data Files\n\n" );
        copyString( mdFileName, configDataPtr->metaDataFileName );
        mdAccessResult = getOpCodes( mdFileName, &mdData );
        if( mdAccessResult == NO_ERR )
        {
            runSimulatorResult = runSimulator(configDataPtr, mdData);
            if( runSimulatorResult != NO_ERR )
            {
                displaySimulatorError( runSimulatorResult );
            }
        }
        else
        {
            clearConfigData( &configDataPtr );
            mdData = clearMetaDataList( mdData );
            displayMetaDataError( mdAccessResult );
            printf( "\n" );
            return 1;
        }
    }
    else
    {
        clearConfigData( &configDataPtr );
        displayConfigError( configAccessResult );
        printf( "\n" );
        return 1;
    }

    // Shut down, Clean up program
    mdData = clearMetaDataList( mdData );
    clearConfigData( &configDataPtr );
    printf( "\n" );
    return 0;
}
