// Header File Information ////////////////////////////////////////////
/*
File: InterruptHandling.h
Brief: Header file for interrupt handling code
Details: Specifies functions, constants, and other information
         related to interrupt handling code
Version: 1.0
         24 April 2019
         Initial development of InterruptHandling code
Note: None
*/

// Precompiler directives /////////////////////////////////////////////////////
#ifndef INTERRUPT_HANDLING_H
#define INTERRUPT_HANDLING_H

// Header files ///////////////////////////////////////////////////////////////

#include "StringUtils.h"

// Data Structure Definitions (structs, enums, etc.)//////////////////////////

typedef struct Interrupt
{
    int PID;
    char opType[ 100 ];
    char opName[ 100 ];
    float endingTime;
    struct  Interrupt *next;
} Interrupt;

// Function Prototypes  ///////////////////////////////////////////////////////
Interrupt *addInterrupt( Interrupt *interruptQueue, int pid, char *opType,
    char *opName, float endingTime );
Boolean haveInterrupt( Interrupt *interruptQueue, float currentTime );
Interrupt *removeInterupt( Interrupt **interruptQueue );
Interrupt *clearInterruptQueue( Interrupt *interruptQueue );
void displayInterruptQueue( Interrupt *interruptQueue );

// Terminating Precompiler Directives ////////////////////////////////////////
#endif // INTERRUPT_HANDLING_H
