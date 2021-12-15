// Code Implementation File Information ///////////////////////////////
/*
File: InterruptHandling.c
Brief: Implementation file for InterruptHandling code
Details: Implements all functions of the InterruptHandling utilities
Version: 1.0
         24 April 2019
         Initial development of InterruptHandling code
Note: None
*/

// Header Files ///////////////////////////////////////////////////
#include "InterruptHandling.h"

/*
Function name: addInterrupt
Algorithm: Creates a new inturrupt node and adds it to the interruptQueue link
           list.
Precondition: Given a pointer to an interruptQueue
Postcondition: Return interruptQueue with node added and sorted by endingTime
Exceptions: None
Note: None
*/

Interrupt *addInterrupt( Interrupt *interruptQueue, int pid, char *opType,
    char *opName, float endingTime )
{
    Interrupt *currentInterrupt, *tempInterrupt;

    if( interruptQueue == NULL )
    {
        interruptQueue = malloc( sizeof( Interrupt ) );
        interruptQueue->PID = pid;
        copyString( interruptQueue->opType, opType );
        copyString( interruptQueue->opName, opName );
        interruptQueue->endingTime = endingTime;
        interruptQueue->next = NULL;

        return interruptQueue;
    }
    else
    {
        currentInterrupt = interruptQueue;

        if( endingTime < currentInterrupt->endingTime )
        {
            interruptQueue = malloc( sizeof( Interrupt ) );
            interruptQueue->PID = pid;
            copyString( interruptQueue->opType, opType );
            copyString( interruptQueue->opName, opName );
            interruptQueue->endingTime = endingTime;
            interruptQueue->next = currentInterrupt;

            return interruptQueue;
        }

        while( currentInterrupt->next != NULL )
        {
            if( endingTime < currentInterrupt->endingTime )
            {
                tempInterrupt = currentInterrupt;
                currentInterrupt->next = malloc( sizeof( Interrupt ) );
                currentInterrupt->next->PID = pid;
                copyString( currentInterrupt->next->opType, opType );
                copyString(currentInterrupt->next->opName, opName );
                currentInterrupt->next->endingTime = endingTime;
                currentInterrupt->next->next = tempInterrupt;

                return interruptQueue;
            }

            currentInterrupt = currentInterrupt->next;
        }

        currentInterrupt->next = malloc( sizeof( Interrupt ) );
        currentInterrupt->next->PID = pid;
        copyString( currentInterrupt->next->opType, opType );
        copyString(currentInterrupt->next->opName, opName );
        currentInterrupt->next->endingTime = endingTime;
        currentInterrupt->next->next = NULL;

        return interruptQueue;
    }
}

/*
Function name: haveInterrupt
Algorithm: Checks if interruptQueue has an interupt with a endingTime less than
           the current time
Precondition: Given interruptQueue to check
Postcondition: Return Boolean value if interrupt is found
Exceptions: None
Note: None
*/

Boolean haveInterrupt( Interrupt *interruptQueue, float currentTime )
{
    if( interruptQueue != NULL )
    {
        if( currentTime >= interruptQueue->endingTime )
        {
            return True;
        }
    }

    return False;
}


/*
Function name: removeInterupt
Algorithm: Removes the first node of an interupt queue and returns back the
           queue.
Precondition: Given non-empty interruptQueue list
Postcondition: Returned interruptQueue minus the head
Exceptions: None
Note: None
*/

Interrupt *removeInterupt( Interrupt **interruptQueue )
{
    Interrupt *tempInterrupt;

    tempInterrupt = *interruptQueue;
    tempInterrupt = tempInterrupt->next;
    free(*interruptQueue);
    *interruptQueue = tempInterrupt;

    return *interruptQueue;
}

/*
Function name: clearInterruptQueue
Algorithm: Recusively calls itself through a Interrupt queue,
           returns memeory to OS from the bottom of the list upward
Precondition: Given Interrupt queue, with or without data
Postcondition: All node memory, if any, is returned to OS,
               return pointer (head) is set to null
Exceptions: None
Note: None
*/
Interrupt *clearInterruptQueue( Interrupt *interruptQueue )
{
    if( interruptQueue != NULL )
    {
        if( interruptQueue->next != NULL )
        {
            clearInterruptQueue( interruptQueue->next );
        }
        free( interruptQueue );
    }
    return NULL;
}

/*
Function name: displayInterruptQueue
Algorithm: Displays data from InterruptQueue nodes for testing
Precondition: Given an InterruptQueue
Postcondition: All processes in interrupts have there information displayed
Exceptions: None
Note: Used for debugging and not for simulator operation
*/
void displayInterruptQueue( Interrupt *interruptQueue )
{

    while(interruptQueue != NULL )
    {
        printf( "PID            : %d\n", interruptQueue->PID );
        printf( "opType          : %s\n", interruptQueue->opType );
        printf( "opName          : %s\n", interruptQueue->opName );
        printf( "Ending time : %f\n\n", interruptQueue->endingTime );
        interruptQueue = interruptQueue->next;
    }
}
