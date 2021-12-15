// Header File Information ////////////////////////////////////////////
/*
File: MMU.h
Brief: Header file for OS memory mangement unit (MMU) code
Details: Specifies functions, constants, and other information
         related to memory mangement unit (MMU) code
Version: 1.0
         28 March 2019
         Initial development of memory mangement unit (MMU) code
Note: None
*/

// Precompiler directives /////////////////////////////////////////////////////
#ifndef MMU_H
#define MMU_H

// Header files ///////////////////////////////////////////////////////////////

#include "StringUtils.h"

// Data Structure Definitions (structs, enums, etc.)//////////////////////////

typedef struct MMU
{
    int PID;
    int segment;
    int base;
    int offset;
    struct MMU *next;
} MMU;

const int SEG_OFFSET;
const int BASE_OFFSET;

// Function Prototypes  ///////////////////////////////////////////////////////

MMU *addMem(MMU *memory, int procID, int segment, int base, int offset);
Boolean isValidAllocate(MMU *memory, int base, int offset);
Boolean isValidAccess(MMU *memory, int procID, int segment, int base,
                                                                    int offset);
MMU *clearMMU( MMU *memory );

// Terminating Precompiler Directives ////////////////////////////////////////
#endif // MMU_H
