// Code Implementation File Information ///////////////////////////////
/*
File: Simulator.c
Brief: Implementation file for OS simulator code
Details: Implements all functions of the OS simulator
Version: 1.0
         21 February 2019
         Initial development of simulator code
Note:
*/

// Header Files ///////////////////////////////////////////////////
#include "Simulator.h"

/*
Function name: runSimulator
Algorithm: Starts a timer, initializes pcbs, simulates processes, and closes
Precondition: Given simulator configuration and metadata linked list
Postcondition: Simlutator output is displayed to monitor
               and/or written to logfile
Exceptions: Correctly and appropriately (without program failure)
            responds to and reports pcb initialization error
Note: None
*/
int runSimulator( ConfigDataType *configDataPtr, OpCodeType *mdData )
{
    PcbType *pcbQueue = NULL, *pcbQueueStart, *tempPcbQueue, *currentPcb;
    OpCodeType *currentOpCode;
    OutputType *outputLog = NULL;
    MMU *memory = NULL;
    int initializePcbsResult, writeOutputLogToFileResult, *opTime;
    int segment, base, offset, memCommand, memRemaining, quantum;
    float currentTime, endingTime;
    char displayString[ STD_STR_LEN ], time[ STD_STR_LEN ];
    char stateStr[ STD_STR_LEN ];
    pthread_t threadID;
    Interrupt *interruptQueue;

    if( configDataPtr->logToCode == LOGTO_FILE_CODE
        || configDataPtr->logToCode == LOGTO_BOTH_CODE )
    {
        outputLog = (OutputType *)malloc( sizeof( OutputType ) );
    }

    memRemaining = configDataPtr->memAvailable;

    outputLine( configDataPtr, outputLog, "================\n" );
    outputLine( configDataPtr, outputLog, "Begin Simulation\n\n" );

    accessTimer( ZERO_TIMER, time );
    sprintf( displayString, " %s, OS: System Start\n", time );
    outputLine( configDataPtr, outputLog, displayString );

    accessTimer( LAP_TIMER, time );
    sprintf( displayString, " %s, OS: Create Process Control Blocks\n", time );
    outputLine( configDataPtr, outputLog, displayString );
    initializePcbsResult = initializePcbs(configDataPtr, mdData, &pcbQueue);

    if( initializePcbsResult == NO_ERR )
    {
        accessTimer( LAP_TIMER, time );
        sprintf( displayString,
                " %s, OS: All processes initialized in New state\n", time );
        outputLine( configDataPtr, outputLog, displayString );
    }
    else
    {
        clearPcbQueue( pcbQueue );
        return initializePcbsResult;
    }

    setAllPcbStates( pcbQueue, READY );
    accessTimer( LAP_TIMER, time );
    sprintf( displayString,
                " %s, OS: All processes now set in Ready state\n", time );
    outputLine( configDataPtr, outputLog, displayString );

    pcbQueueStart = pcbQueue;

    // Run Simulator without preemption
    if( configDataPtr->cpuSchedCode == CPU_SCHED_SJF_N_CODE ||
        configDataPtr->cpuSchedCode == CPU_SCHED_FCFS_N_CODE )
    {
        while( pcbQueue != NULL )
        {
            pcbQueue = sortPCB( pcbQueue, configDataPtr->cpuSchedCode );
            accessTimer( LAP_TIMER, time );
            sprintf( displayString,
                " %s, OS: Process %d selected with %d ms remaining\n",
                time, pcbQueue->PID, pcbQueue->timeRemaining );
            outputLine( configDataPtr, outputLog, displayString );

            pcbQueue->state = RUNNING;
            accessTimer( LAP_TIMER, time );
            pcbStateToString( pcbQueue->state, stateStr );
            sprintf( displayString,
                " %s, OS: Process %d set in %s state\n\n",
                time, pcbQueue->PID, stateStr );
            outputLine( configDataPtr, outputLog, displayString );

            currentOpCode = pcbQueue->programCounter;
            while( currentOpCode != NULL )
            {
                accessTimer( LAP_TIMER, time );
                if( currentOpCode->opLtr == 'P'
                    || currentOpCode->opLtr == 'O'
                    || currentOpCode->opLtr == 'I' )
                {
                    opTime = (int *)malloc( sizeof( int ) );
                    if( currentOpCode->opLtr == 'P' )
                    {
                        sprintf( displayString,
                            " %s, Process: %d, %s operation start\n",
                            time, pcbQueue->PID, currentOpCode->opName );
                        *opTime = configDataPtr->procCycleRate *
                                                        currentOpCode->opValue;
                    }
                    else if( currentOpCode->opLtr == 'I' )
                    {
                        sprintf( displayString,
                            " %s, Process: %d, %s input start\n",
                            time, pcbQueue->PID, currentOpCode->opName );
                        *opTime = configDataPtr->ioCycleRate *
                                                        currentOpCode->opValue;
                    }
                    else if( currentOpCode->opLtr == 'O' )
                    {
                        sprintf( displayString,
                            " %s, Process: %d, %s output start\n",
                            time, pcbQueue->PID, currentOpCode->opName );
                        *opTime = configDataPtr->ioCycleRate *
                                                        currentOpCode->opValue;
                    }

                    outputLine( configDataPtr, outputLog, displayString );
                    pthread_create( &threadID, NULL,
                                    simulateOperation, (void *)opTime );
                    pthread_join( threadID, NULL );
                    free( opTime );

                    accessTimer( LAP_TIMER, time );
                    if( currentOpCode->opLtr == 'P' )
                    {
                        sprintf( displayString,
                            " %s, Process: %d, %s operation end\n",
                            time, pcbQueue->PID, currentOpCode->opName );
                        outputLine( configDataPtr, outputLog, displayString );
                    }
                    else if( currentOpCode->opLtr == 'O' )
                    {
                        sprintf( displayString,
                            " %s, Process: %d, %s output end\n",
                            time, pcbQueue->PID, currentOpCode->opName );
                        outputLine( configDataPtr, outputLog, displayString );
                    }
                    else if( currentOpCode->opLtr == 'I' )
                    {
                        sprintf( displayString,
                            " %s, Process: %d, %s input end\n",
                            time, pcbQueue->PID, currentOpCode->opName );
                        outputLine( configDataPtr, outputLog, displayString );
                    }
                }
                else if( currentOpCode->opLtr == 'M' )
                {
                    memCommand = currentOpCode->opValue;
                    segment = (int)( memCommand / SEG_OFFSET );
                    base = (int)( memCommand % SEG_OFFSET) / BASE_OFFSET;
                    offset = (int)( memCommand % SEG_OFFSET) % BASE_OFFSET;

                    sprintf( displayString,
                        " %s, Process: %d, MMU attempt to %s %d/%d/%d\n",
                        time, pcbQueue->PID, currentOpCode->opName,
                        segment, base, offset);
                    outputLine( configDataPtr, outputLog, displayString );

                    if( compareString( currentOpCode->opName , "allocate") == 0)
                    {
                        if ( isValidAllocate( memory, base, offset ) &&
                                                        offset <= memRemaining )
                        {
                            memory = addMem(memory, pcbQueue->PID, segment,
                                                                base, offset);
                            memRemaining -= offset;

                            accessTimer( LAP_TIMER, time );
                            sprintf( displayString,
                                " %s, Process: %d, MMU successful %s\n",
                                time, pcbQueue->PID, currentOpCode->opName);
                            outputLine( configDataPtr, outputLog,
                                                                displayString );
                        }
                        else
                        {
                            accessTimer( LAP_TIMER, time );
                            sprintf( displayString,
                                " %s, Process: %d, MMU failed to %s\n\n",
                                time, pcbQueue->PID, currentOpCode->opName);
                            outputLine( configDataPtr, outputLog,
                                                                displayString );

                            accessTimer( LAP_TIMER, time );
                            sprintf( displayString,
                             " %s, OS: Process %d experiences segmentation fault\n",
                             time, pcbQueue->PID );
                            outputLine( configDataPtr, outputLog,
                                                                displayString );
                            break;
                        }
                    }
                    else if
                        ( compareString( currentOpCode->opName , "access") == 0)
                    {
                        if ( isValidAccess( memory, pcbQueue->PID, segment,
                                                                base, offset ) )
                        {
                            accessTimer( LAP_TIMER, time );
                            sprintf( displayString,
                                " %s, Process: %d, MMU successful %s\n",
                                time, pcbQueue->PID, currentOpCode->opName);
                            outputLine( configDataPtr, outputLog,
                                                                displayString );
                        }
                        else
                        {
                            accessTimer( LAP_TIMER, time );
                            sprintf( displayString,
                                " %s, Process: %d, MMU failed to %s\n\n",
                                time, pcbQueue->PID, currentOpCode->opName);
                            outputLine( configDataPtr, outputLog,
                                                                displayString );

                            accessTimer( LAP_TIMER, time );
                            sprintf( displayString,
                             " %s, OS: Process %d experiences segmentation fault\n",
                             time, pcbQueue->PID );
                            outputLine( configDataPtr, outputLog,
                                                                displayString );
                            break;
                        }
                    }
                }
                else if ( currentOpCode->opLtr == 'A' )
                {
                    outputLine( configDataPtr, outputLog, "\n" );
                    break;
                }
                currentOpCode = currentOpCode->next;
            }

            pcbQueue->state = EXIT;
            accessTimer( LAP_TIMER, time );
            memory = clearMMU( memory );
            memRemaining = configDataPtr->memAvailable;
            pcbStateToString( pcbQueue->state, stateStr );
            sprintf( displayString,
                " %s, OS: Process %d ended and set in %s state\n",
                time, pcbQueue->PID, stateStr );
            outputLine( configDataPtr, outputLog, displayString );
            pcbQueue = pcbQueue->next;
        }

        pcbQueueStart = clearPcbQueue( pcbQueueStart );
    }

    // Run simulator with preemption
    else if ( configDataPtr->cpuSchedCode == CPU_SCHED_SRTF_P_CODE ||
              configDataPtr->cpuSchedCode == CPU_SCHED_FCFS_P_CODE ||
              configDataPtr->cpuSchedCode == CPU_SCHED_RR_P_CODE )
    {
        interruptQueue = NULL;

        while( checkIfEnded( pcbQueue ) == False )
        {
            if( checkIfIdle( pcbQueue ) )
            {
                accessTimer( LAP_TIMER, time );
                sprintf( displayString, " %s, OS: System/CPU idle\n", time );
                outputLine( configDataPtr, outputLog, displayString );

                while( checkIfIdle( pcbQueue ) )
                {
                    accessTimer( LAP_TIMER, time );
                    currentTime = stringToFloat( time );

                    while( haveInterrupt( interruptQueue, currentTime ) )
                    {
                        accessTimer( LAP_TIMER, time );
                        sprintf( displayString,
                            " %s, OS: Interrupt called by process %d\n\n",
                            time, interruptQueue->PID );
                        outputLine( configDataPtr, outputLog, displayString );

                        accessTimer( LAP_TIMER, time );
                        sprintf( displayString,
                            " %s, Process: %d, %s %s end\n\n",
                            time, interruptQueue->PID,
                            interruptQueue->opName,
                            interruptQueue->opType );
                        outputLine( configDataPtr, outputLog, displayString );

                        tempPcbQueue = pcbQueue;
                        while( tempPcbQueue != NULL )
                        {
                            if( tempPcbQueue->PID == interruptQueue->PID )
                            {
                                tempPcbQueue->state = READY;
                                break;
                            }
                            tempPcbQueue = tempPcbQueue->next;
                        }

                        accessTimer( LAP_TIMER, time );
                        sprintf( displayString,
                            " %s, OS: Process %d set in Ready state\n",
                            time, interruptQueue->PID );
                        outputLine( configDataPtr, outputLog, displayString );

                        interruptQueue = removeInterupt( &interruptQueue );
                        accessTimer( LAP_TIMER, time );
                        currentTime = stringToFloat( time );
                    }
                }
            }

            pcbQueue = sortPCB( pcbQueue, configDataPtr->cpuSchedCode );
            currentPcb = getNextPcb( pcbQueue, READY );

            accessTimer( LAP_TIMER, time );
            sprintf( displayString,
                " %s, OS: Process %d selected with %d ms remaining\n",
                time, currentPcb->PID, currentPcb->timeRemaining );
            outputLine( configDataPtr, outputLog, displayString );

            currentPcb->state = RUNNING;
            accessTimer( LAP_TIMER, time );
            pcbStateToString( currentPcb->state, stateStr );
            sprintf( displayString,
                " %s, OS: Process %d set in %s state\n\n",
                time, currentPcb->PID, stateStr );
            outputLine( configDataPtr, outputLog, displayString );

            quantum = configDataPtr->quantumCycles;

            while( quantum > 0 && currentPcb->programCounter != NULL &&
                currentPcb->timeRemaining > 0 )
            {
                accessTimer( LAP_TIMER, time );
                currentOpCode = currentPcb->programCounter;

                if( currentOpCode->opLtr == 'O'
                    || currentOpCode->opLtr == 'I' )
                {

                    currentTime = stringToFloat( time );
                    endingTime = configDataPtr->ioCycleRate * currentOpCode->opValue * 0.001;
                    currentPcb->timeRemaining -= configDataPtr->ioCycleRate * currentOpCode->opValue;
                    endingTime += currentTime;

                    if( currentOpCode->opLtr == 'I' )

                    {
                        sprintf( displayString,
                            " %s, Process: %d, %s input start\n\n",
                            time, currentPcb->PID, currentOpCode->opName );
                        outputLine( configDataPtr, outputLog, displayString );

                        interruptQueue = addInterrupt( interruptQueue,
                            currentPcb->PID, "input",
                            currentOpCode->opName , endingTime );

                    }
                    else if( currentOpCode->opLtr == 'O' )
                    {
                        sprintf( displayString,
                            " %s, Process: %d, %s output start\n\n",
                            time, currentPcb->PID, currentOpCode->opName );
                        outputLine( configDataPtr, outputLog, displayString );

                        interruptQueue = addInterrupt( interruptQueue,
                            currentPcb->PID, "input",
                            currentOpCode->opName , endingTime );
                    }

                    currentPcb->programCounter =
                                            currentPcb->programCounter->next;
                    currentPcb->state = BLOCKED;
                    break;
                }
                else if( currentOpCode->opLtr == 'P')
                {
                    if( quantum == configDataPtr->quantumCycles)
                    {
                        sprintf( displayString,
                            " %s, Process: %d, %s operation start\n",
                            time, currentPcb->PID, currentOpCode->opName );
                        outputLine( configDataPtr, outputLog, displayString );
                    }

                    runTimer( configDataPtr->procCycleRate );
                    currentOpCode->opValue--;
                    currentPcb->timeRemaining -= configDataPtr->procCycleRate;
                    quantum--;

                    if( currentOpCode->opValue == 0 )
                    {
                        currentPcb->programCounter =
                                            currentPcb->programCounter->next;
                        break;

                    }
                    accessTimer( LAP_TIMER, time );
                    currentTime = stringToFloat( time );
                    if( haveInterrupt( interruptQueue, currentTime ) )
                    {
                        break;
                    }
                }
                else if( currentOpCode->opLtr == 'M' )
                {
                    memCommand = currentOpCode->opValue;
                    segment = (int)( memCommand / SEG_OFFSET );
                    base = (int)( memCommand % SEG_OFFSET) / BASE_OFFSET;
                    offset = (int)( memCommand % SEG_OFFSET) % BASE_OFFSET;

                    sprintf( displayString,
                        " %s, Process: %d, MMU attempt to %s %d/%d/%d\n",
                        time, currentPcb->PID, currentOpCode->opName,
                        segment, base, offset);
                    outputLine( configDataPtr, outputLog, displayString );

                    if( compareString( currentOpCode->opName , "allocate") == 0)
                    {
                        if ( isValidAllocate( memory, base, offset ) &&
                                                        offset <= memRemaining )
                        {
                            memory = addMem(memory, currentPcb->PID, segment,
                                                                base, offset);
                            memRemaining -= offset;

                            accessTimer( LAP_TIMER, time );
                            sprintf( displayString,
                                " %s, Process: %d, MMU successful %s\n",
                                time, currentPcb->PID, currentOpCode->opName);
                            outputLine( configDataPtr, outputLog,
                                                                displayString );
                            currentPcb->programCounter = currentPcb->programCounter->next;
                        }
                        else
                        {
                            accessTimer( LAP_TIMER, time );
                            sprintf( displayString,
                                " %s, Process: %d, MMU failed to %s\n\n",
                                time, currentPcb->PID, currentOpCode->opName);
                            outputLine( configDataPtr, outputLog,
                                                                displayString );

                            accessTimer( LAP_TIMER, time );
                            sprintf( displayString,
                             " %s, OS: Process %d experiences segmentation fault\n",
                             time, currentPcb->PID );
                            outputLine( configDataPtr, outputLog,
                                                                displayString );
                            currentPcb->state = EXIT;
                            break;
                        }
                    }
                    else if
                        ( compareString( currentOpCode->opName , "access") == 0)
                    {
                        if ( isValidAccess( memory, currentPcb->PID, segment,
                                                                base, offset ) )
                        {
                            accessTimer( LAP_TIMER, time );
                            sprintf( displayString,
                                " %s, Process: %d, MMU successful %s\n",
                                time, currentPcb->PID, currentOpCode->opName);
                            outputLine( configDataPtr, outputLog,
                                                                displayString );
                            currentPcb->programCounter = currentPcb->programCounter->next;
                        }
                        else
                        {
                            accessTimer( LAP_TIMER, time );
                            sprintf( displayString,
                                " %s, Process: %d, MMU failed to %s\n\n",
                                time, currentPcb->PID, currentOpCode->opName);
                            outputLine( configDataPtr, outputLog,
                                                                displayString );

                            accessTimer( LAP_TIMER, time );
                            sprintf( displayString,
                             " %s, OS: Process %d experiences segmentation fault\n",
                             time, currentPcb->PID );
                            outputLine( configDataPtr, outputLog,
                                                                displayString );

                            currentPcb->state = EXIT;
                            break;
                        }
                    }
                }
                else if ( currentOpCode->opLtr == 'A' )
                {
                    currentPcb->state = EXIT;
                    outputLine( configDataPtr, outputLog, "\n" );
                    break;
                }
            }

            if( currentPcb->state == BLOCKED )
            {
                accessTimer( LAP_TIMER, time );
                sprintf( displayString,
                    " %s, OS: Process %d set in BLOCKED state\n",
                    time, currentPcb->PID);
                outputLine( configDataPtr, outputLog, displayString );
            }
            else if( currentPcb->state == EXIT ||
                currentPcb->timeRemaining == 0)
            {
                currentPcb->state = EXIT ;
                accessTimer( LAP_TIMER, time );
                sprintf( displayString,
                    " %s, OS: Process %d ended and set in EXIT state\n",
                    time, currentPcb->PID);
                outputLine( configDataPtr, outputLog, displayString );
            }
            else if( haveInterrupt( interruptQueue, currentTime ) )
            {
                accessTimer( LAP_TIMER, time );
                currentTime = stringToFloat( time );

                while( haveInterrupt( interruptQueue, currentTime ) )
                {
                    accessTimer( LAP_TIMER, time );
                    sprintf( displayString,
                        "\n %s, OS: Process %d interrupted by process %d\n",
                        time, currentPcb->PID, interruptQueue->PID );
                    outputLine( configDataPtr, outputLog, displayString );

                    if( currentPcb->state != READY )
                    {
                        currentPcb->state = READY;
                        accessTimer( LAP_TIMER, time );
                        sprintf( displayString,
                            " %s, OS: Process %d set in READY state\n\n",
                            time, interruptQueue->PID );
                        outputLine( configDataPtr, outputLog, displayString );
                    }

                    accessTimer( LAP_TIMER, time );
                    sprintf( displayString,
                        " %s, Process: %d, %s %s end\n\n",
                        time, interruptQueue->PID,
                        interruptQueue->opName,
                        interruptQueue->opType );
                    outputLine( configDataPtr, outputLog, displayString );

                    tempPcbQueue = pcbQueue;
                    while( tempPcbQueue != NULL )
                    {
                        if( tempPcbQueue->PID == interruptQueue->PID )
                        {
                            tempPcbQueue->state = READY;
                            break;
                        }
                        tempPcbQueue = tempPcbQueue->next;
                    }

                    accessTimer( LAP_TIMER, time );
                    sprintf( displayString,
                        " %s, OS: Process %d set in READY state\n",
                        time, interruptQueue->PID );
                    outputLine( configDataPtr, outputLog, displayString );

                    interruptQueue = removeInterupt(&interruptQueue);
                    accessTimer( LAP_TIMER, time );
                    currentTime = stringToFloat( time );
                }
            }
            if( currentPcb->state == RUNNING)
            {
                currentPcb->state = READY;
                accessTimer( LAP_TIMER, time );
                sprintf( displayString,
                    " %s, Process: %d, %s operation end\n\n",
                    time, currentPcb->PID, currentOpCode->opName );
                outputLine( configDataPtr, outputLog, displayString );

                accessTimer( LAP_TIMER, time );
                sprintf( displayString,
                    " %s, OS: Process %d set in READY state\n",
                    time, currentPcb->PID);
                outputLine( configDataPtr, outputLog, displayString );
            }
        }
    }

    pcbQueue = sortPCB( pcbQueue, configDataPtr->cpuSchedCode );
    pcbQueue = clearPcbQueue( pcbQueue );
    interruptQueue = clearInterruptQueue( interruptQueue );
    memory = clearMMU( memory );
    accessTimer( LAP_TIMER, time );
    sprintf( displayString, " %s, OS: System stop\n\n", time );
    outputLine( configDataPtr, outputLog, displayString );
    outputLine( configDataPtr, outputLog, "End Simulation - Complete\n");
    outputLine( configDataPtr, outputLog, "=========================\n");

    writeOutputLogToFileResult = writeOutputLogToFile( configDataPtr,
                                                                outputLog );
    if( writeOutputLogToFileResult != NO_ERR )
    {
        displayOutputError( writeOutputLogToFileResult );
        outputLog = clearOutputLog( outputLog );
        return writeOutputLogToFileResult;
    }

    outputLog = clearOutputLog( outputLog );
    return 0;
}

/*
Function name: initializePbc
Algorithm: Runs through metadata linked list and at each process start creates
           a new pcb node in the pcb queue
Precondition: Given a simulator config, metadata linked list,
              and pcb queue pointer
Postcondition: The pcb queue pointer contains the head of the created
               pcb queue
Exceptions: Correctly and appropriately (without program failure)
            responds to and reports pcb initialization error
Note: None
*/
int initializePcbs(ConfigDataType *configDataPtr, OpCodeType *mdData,
                                                    PcbType **pcbQueue)
{
    int processCount = 0;
    OpCodeType *currentOpCode;
    PcbType *currentPcb;

    currentOpCode = mdData->next;

    while( currentOpCode != NULL )
    {
        if( currentOpCode->opLtr == 'A')
        {
            if( compareString( "start", currentOpCode->opName ) == 0 )
            {

                currentPcb = (PcbType *) malloc( sizeof( PcbType ) );
                currentPcb->programCounter = currentOpCode->next;
                currentPcb->state = NEW;
                currentPcb->PID = processCount;
                currentPcb->timeRemaining =
                            calculateTimeRemaining( currentPcb->programCounter,
                                                    configDataPtr );;
                currentPcb->next = NULL;
                *pcbQueue = addPcb( *pcbQueue, currentPcb );
                currentPcb = clearPcbQueue( currentPcb );
            }
            else if( compareString( "end", currentOpCode->opName ) == 0 )
            {
                processCount++;
            }
            else
            {
                currentPcb = clearPcbQueue( currentPcb );
                return PCB_INITIALIZATION_ERROR;
            }
        }
        else if( currentOpCode->opLtr == 'S')
        {
            currentPcb = clearPcbQueue( currentPcb );
            return NO_ERR;
        }
        currentOpCode = currentOpCode->next;
    }
    currentPcb = clearPcbQueue( currentPcb );
    return PCB_INITIALIZATION_ERROR;
}

/*
Function name: addPcb
Algorithm: If pcb queue is empty, creates a new pcb node,
           otherwise, recursively calls itself until end of pcb is found
           and creates a new pcb node
Precondition: Given a pcb queue pointer and a new pcb node
Postcondition: Returns pcb queue
Exceptions: None
Note: None
*/
PcbType *addPcb( PcbType *pcbQueue, PcbType *newPcb )
{
    if( pcbQueue == NULL )
    {
        pcbQueue = (PcbType *)malloc( sizeof( PcbType ) );
        pcbQueue->programCounter = newPcb->programCounter;
        pcbQueue->PID = newPcb->PID;
        pcbQueue->timeRemaining = newPcb->timeRemaining;
        pcbQueue->state = newPcb->state;
        pcbQueue->next = NULL;
    }
    else
    {
        pcbQueue->next = addPcb( pcbQueue->next, newPcb );
    }
    return pcbQueue;
}

/*
Function name: calculateTimeRemaining
Algorithm: Runs through metadata linked list from current program counter
           adding up operation time until process end is found,
           then returns the total process time remaining
Precondition: Given a pointer to a opcode in a metadata linked list add
              a pointer to a simulator configuration
Postcondition: Returns total time remaining in process execution
Exceptions: None
Note: None
*/
int calculateTimeRemaining( OpCodeType *programCounter,
                                                ConfigDataType *configDataPtr )
{
    OpCodeType *currentOpCode;
    int totalTimeRemaining;

    currentOpCode = programCounter;
    totalTimeRemaining = 0;

    while( currentOpCode->opLtr != 'A' )
    {
        if( currentOpCode->opLtr == 'P' )
        {
            totalTimeRemaining += configDataPtr->procCycleRate *
                                                        currentOpCode->opValue;
        }
        else if( currentOpCode->opLtr == 'I' || currentOpCode->opLtr == 'O' )
        {
            totalTimeRemaining += configDataPtr->ioCycleRate *
                                                        currentOpCode->opValue;
        }
        currentOpCode = currentOpCode->next;
    }
    return totalTimeRemaining;
}

/*
Function name: setAllPcbStates
Algorithm: Sets all pcb to state
Precondition: Given a pcb queue and a valid pcb state
Postcondition: Pcbs are all in given state
Exceptions: None
Note: None
*/
void setAllPcbStates( PcbType *pcb, int pcbState )
{
    while( pcb != NULL )
    {
    pcb->state = pcbState;
    pcb = pcb->next;
    }
}

/*
Function name: simulateOperation
Algorithm: Simulates a process function by spinning for a set ammount of time
Precondition: Given a pointer to a integer value
Postcondition: Set ammount of time has passed
Exceptions: None
Note: None
*/
void *simulateOperation( void *time )
{
    runTimer( *(int *)time );
    pthread_exit( NULL );
}

/*
Function name: displayPcbQueue
Algorithm: Displays PID, state, and time remaining of all processes
           in pcb queue
Precondition: Given pcb queue
Postcondition: All processes in pcb have there information displayed
Exceptions: None
Note: Used for debugging and not for simulator operation
*/
void displayPcbQueue( PcbType *pcb )
{
    char displayString[ STD_STR_LEN ];

    while(pcb != NULL )
    {
        printf( "PID            : %d\n", pcb->PID );
        pcbStateToString( pcb->state, displayString );
        printf( "State          : %s\n", displayString );
        printf( "Time remaining : %d\n\n", pcb->timeRemaining );
        pcb = pcb->next;
    }
}

/*
Function name: displayPcbState
Algorithm: Takes in a integer value of a pcb state and returns a
           string representation
Precondition: Given pcb state as an integer
Postcondition: Returned a string version of pcb state
Exceptions: None
Note: None
*/
void pcbStateToString( int code, char *outString )
{
    char displayStrings[ 5 ][ 8 ] = { "NEW", "READY", "RUNNING", "BLOCKED",
                                                                      "EXIT" };
    copyString( outString, displayStrings[ code ] );
}

/*
Function name: clearPcbQueue
Algorithm: Recusively calls itself through a pcb queue linked list,
           returns memeory to OS from the bottom of the list upward
Precondition: Given pcb queue, with or without data
Postcondition: All node memory, if any, is returned to OS,
               return pointer (head) is set to null
Exceptions: None
Note: None
*/
PcbType *clearPcbQueue( PcbType *pcbQueue )
{
    if( pcbQueue != NULL )
    {
        if( pcbQueue->next != NULL )
        {
            clearPcbQueue( pcbQueue->next );
        }
        free( pcbQueue );
    }
    return NULL;
}

/*
Function name: displaySimulatorError
Algorithm: Uses enum/constant values as indices to select display string,
           then displays error message with selected string
Precondition: Integer input code has one of the enumerated code values
Postcondition: Error message is displated with the correct
               local error item
Exceptions: None
Note: Expecting to expand the list in PA03 and PA04
*/
void displaySimulatorError( int errCode )
{
    char displayStrings[ 1 ][ 40 ] = { "PCB Initializtion Error" };
    printf("\nFATAL ERROR: %s, Program aborted\n", displayStrings[ errCode ] );
}

/*
Function name: sortPCB
Algorithm: Uses the cpu schedule code within the config file to determine the
           correct ordering of pcbs for scheduling and sorts the pcb queue
Precondition: Given pcb queue, with or without data
Postcondition: Returned sorted pcb queue based on cpu schedule code
Exceptions: None
Note:
*/
PcbType *sortPCB( PcbType *pcbQueue, ConfigDataCodes cpuSchedCode)
{
    OpCodeType *tempProgramCounter;
    int tempState, tempPID, tempTimeRemaining;
    PcbType *current, *iterator;
    Boolean swap;

    if( pcbQueue == NULL || cpuSchedCode == CPU_SCHED_FCFS_N_CODE
        || cpuSchedCode == CPU_SCHED_FCFS_P_CODE )
    {
        return pcbQueue;
    }

    else if( cpuSchedCode == CPU_SCHED_SJF_N_CODE ||
             cpuSchedCode == CPU_SCHED_SRTF_P_CODE )
    {
        swap = True;
        iterator = NULL;

        while( swap == True )
        {
            current = pcbQueue;
            swap = False;

            while( current->next != iterator )
            {
                if( current->timeRemaining > current->next->timeRemaining )
                {
                    swap = True;

                    tempProgramCounter = current->programCounter;
                    tempState = current->state;
                    tempPID = current->PID;
                    tempTimeRemaining = current->timeRemaining;

                    current->programCounter = current->next->programCounter;
                    current->state = current->next->state;
                    current->PID = current->next->PID;
                    current->timeRemaining = current->next->timeRemaining;

                    current->next->programCounter = tempProgramCounter;
                    current->next->state = tempState;
                    current->next->PID = tempPID;
                    current->next->timeRemaining = tempTimeRemaining;
                }
                current = current->next;
            }

            iterator = current;
        }
    }

    else if( cpuSchedCode == CPU_SCHED_RR_P_CODE )
    {
        swap = True;
        iterator = NULL;

        while( swap == True )
        {
            current = pcbQueue;
            swap = False;

            while( current->next != iterator )
            {
                if( current->state == BLOCKED )
                {
                    swap = True;

                    tempProgramCounter = current->programCounter;
                    tempState = current->state;
                    tempPID = current->PID;
                    tempTimeRemaining = current->timeRemaining;

                    current->programCounter = current->next->programCounter;
                    current->state = current->next->state;
                    current->PID = current->next->PID;
                    current->timeRemaining = current->next->timeRemaining;

                    current->next->programCounter = tempProgramCounter;
                    current->next->state = tempState;
                    current->next->PID = tempPID;
                    current->next->timeRemaining = tempTimeRemaining;
                }
                current = current->next;
            }

            iterator = current;
        }
    }

    return pcbQueue;
}

/*
Function name: checkIfIdle
Algorithm: Checks a pcbQueue to see if any pcbs are in a ready state
Precondition: Given pcbQueue linked list
Postcondition: Return boolean value of pcbQueue state
Exceptions: None
Note: None
*/
Boolean checkIfIdle( PcbType *pcbQueue )
{
    while( pcbQueue != NULL)
    {
        if( pcbQueue->state == READY )
        {
            return False;
        }
        pcbQueue = pcbQueue->next;
    }
    return True;
}

/*
Function name: checkIfEnded
Algorithm: Checks a pcbQueue to see if all pcbs are in an exit state
Precondition: Given pcbQueue linked list
Postcondition: Return boolean value of pcbQueue state
Exceptions: None
Note: None
*/
Boolean checkIfEnded( PcbType *pcbQueue )
{
    while( pcbQueue != NULL)
    {
        if( pcbQueue->state != EXIT )
        {
            return False;
        }
        pcbQueue = pcbQueue->next;
    }
    return True;
}

/*
Function name: getNextPcb
Algorithm: Search pcbQueue list to find the next node with given state
Precondition: Given pcbQueue linked list
Postcondition: Returned pcb with given state
Exceptions: None
Note: None
*/
PcbType *getNextPcb( PcbType *pcbQueue, PcbState pcbState )
{
    while( pcbQueue != NULL)
    {
        if( pcbQueue->state == pcbState )
        {
            return pcbQueue;
        }
        pcbQueue = pcbQueue->next;
    }
    return NULL;
}
