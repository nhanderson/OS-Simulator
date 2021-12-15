// Header File Information ////////////////////////////////////////////
/*
File: MetaDataAccess.h
Brief: Header file for MetaDataAccess code
Details: Specifies functions, constants, and other information
         related to MetaDataAccess code
Version: 1.0
         01 February 2019
         Initial development of MetaDataAccess code
Note: Code based on 'Introduction to C Programming' tutorial
      by Michael E. Leverington
*/

// Precompiler Directives //////////////////////////////////////////
#ifndef METADATA_ACCESS_H
#define METADATA_ACCESS_H

// Header Files ///////////////////////////////////////////////////
#include <stdio.h>  // for file access
#include "StringUtils.h"

// Data Structure Definitions (structs, enums, etc.)///////////////

typedef enum OpCodeMessages
{
    MD_FILE_ACCESS_ER = 3,
    MD_CORRUPT_DESCRIPTOR_ERR,
    OPCMD_ACCESS_ERR,
    CORRUPT_OPCMD_LETTER_ERR,
    CORRUPT_OPCMD_NAME_ERR,
    CORRUPT_OPCMD_VALUE_ERR,
    UNBALANCED_START_END_ERR,
    COMPLETE_OPCMD_FOUND_MSG,
    LAST_OPCMD_FOUND_MSG
} OpCodeMessages;

typedef struct OpCodeType
{
    char opLtr;
    char opName[ 100 ]; // length of op name
    int opValue;
    struct OpCodeType *next;
} OpCodeType;

// Function Prototypes ///////////////////////////////////////////
int getOpCodes( char *fileName, OpCodeType **opCodeDataHead );
int getOpCommand( FILE *filePtr, OpCodeType *inData );
Boolean checkOpString( char *testStr );
Boolean isDigit( char testChar );
int updateStartCount( int count, char *opString );
int updateEndCount( int count, char *opString );
OpCodeType *addNode( OpCodeType * localPtr, OpCodeType *newNode );
void displayMetaData( OpCodeType *localPtr );
void displayMetaDataError( int code );
OpCodeType *clearMetaDataList( OpCodeType *localPtr );

// Terminating Precompiler Directives ////////////////////////////
#endif  // METADATA_ACCESS_H
