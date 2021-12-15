// Header File Information ////////////////////////////////////////////
/*
File: Simulator.h
Brief: Header file for Simulator code
Details: Specifies functions, constants, and other information
         related to Simulator code
Version: 1.0
         21 February 2019
         Initial development of Simulator code
Note: None
*/

// Precompiler directives /////////////////////////////////////////////////////
#ifndef SIMULATOR_H
#define SIMULATOR_H

// Header files ///////////////////////////////////////////////////////////////

#include "simtimer.h"
#include "ConfigAccess.h"
#include "MetaDataAccess.h"
#include "OutputHandling.h"
#include "MMU.h"
#include "InterruptHandling.h"
#include <pthread.h>


// Data Structure Definitions (structs, enums, etc.)//////////////////////////

typedef enum SimulatorErrorMessages
{
    PCB_INITIALIZATION_ERROR = 3
} SimulatorErrorMessages;

typedef enum PcbState
{
    NEW,
    READY,
    RUNNING,
    BLOCKED,
    EXIT
} PcbState;

typedef struct PcbType
{
    OpCodeType *programCounter;
    PcbState state;
    int PID;
    int timeRemaining;
    struct PcbType *next;
} PcbType;

// Function Prototypes  ///////////////////////////////////////////////////////

int runSimulator( ConfigDataType *configDataPtr, OpCodeType *mdData);
int initializePcbs(ConfigDataType *configDataPtr, OpCodeType *mdData,
                                                    PcbType **pcbQueue);
PcbType *addPcb( PcbType *pcbQueue, PcbType *newPcb );
int calculateTimeRemaining( OpCodeType *programCounter,
                                            ConfigDataType *configDataPtr );
void setAllPcbStates( PcbType *pcb, int pcbState );
void *simulateOperation( void *time );
void displayPcbQueue( PcbType *pcb );
void pcbStateToString( int code, char *outString );
PcbType *clearPcbQueue( PcbType *localPtr );
void displaySimulatorError( int errCode );
PcbType *sortPCB( PcbType *pcbQueue, ConfigDataCodes cpuSchedCode);
Boolean checkIfIdle( PcbType *pcbQueue );
Boolean checkIfEnded( PcbType *pcbQueue );
PcbType *getNextPcb( PcbType *pcbQueue, PcbState state );

// Terminating Precompiler Directives ////////////////////////////////////////
#endif // SIMULATOR_H
