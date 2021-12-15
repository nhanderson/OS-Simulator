// Code Implementation File Information ///////////////////////////////
/*
File: MMU.c
Brief: Implementation file for OS memory mangement unit (MMU) code
Details: Implements all functions of the OS memory mangement unit (MMU)
Version: 1.0
         28 March 2019
         Initial development of MMU code
Note:
*/

// Header Files ///////////////////////////////////////////////////
#include "MMU.h"

const int SEG_OFFSET = 1000000;
const int BASE_OFFSET = 1000;

/*
Function name: addMem
Algorithm: Adds an entry in the MMU table for memory allocation
Precondition: Given a memory table, PID, memeory segment, memory base, and
              memory offset
Postcondition: Returns memory table with new entry
Exceptions:
Note:
*/
MMU *addMem(MMU *memory, int procID, int segment, int base, int offset)
{
    if( memory == NULL)
    {
        memory = (MMU *)malloc( sizeof( MMU ) );
        memory->PID = procID;
        memory->segment = segment;
        memory->base = base;
        memory->offset = offset;
        memory->next = NULL;
    }
    else
    {
        memory->next = addMem(memory->next, procID, segment, base, offset);
    }
    return memory;
}

/*
Function name: isValidAllocate
Algorithm: Checks MMU table to determine if requested memory allocation is
           valid
Precondition: Given memory table, memory base, and memory offset
Postcondition: Returns False if memory is allready allocated, True if available.
Exceptions: None
Note: None
*/
Boolean isValidAllocate(MMU *memory, int base, int offset)
{
    while( memory != NULL )
    {
        if( memory->base == base )
        {
            return False;
        }

        memory = memory->next;
    }

    return True;
}

/*
Function name: isValidAccess
Algorithm: Checks MMU table to determine if requested memory access request is
           valid. Checks PID, segment ID, and the rquest memory to access.
Precondition: Given memory table, PID, memory segment, memory base,
              and memory offset
Postcondition: Returns True if PID has access to the specified memory in the
               MMU, otherwise false.
Exceptions: None
Note: None
*/
Boolean isValidAccess(MMU *memory, int procID, int segment, int base,
                                                                    int offset)
{
    while( memory != NULL )
    {
        if( memory->PID == procID && memory->segment == segment
                                                       && memory->base == base)
        {
            if( offset <= memory->offset )
            {
                return True;
            }
        }

        memory = memory->next;
    }

    return False;
}

/*
Function name: clearMMU
Algorithm: Recusively calls itself through a MMU linked list,
           returns memeory to OS from the bottom of the list upward
Precondition: Given MMU, with or without data
Postcondition: All node memory, if any, is returned to OS,
               return pointer (head) is set to null
Exceptions: None
Note: None
*/
MMU *clearMMU( MMU *memory )
{
    if( memory != NULL )
    {
        if( memory->next != NULL )
        {
            clearMMU( memory->next );
        }
        free( memory );
    }
    return NULL;
}
