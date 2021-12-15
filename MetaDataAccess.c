// Code Implementation File Information ///////////////////////////////
/*
File: MetaDataAccess.c
Brief: Implementation file for MetaDataAccess code
Details: Implements all functions of the MetaDataAccess utilities
Version: 1.0
         01 February 2019
         Initial development of MetaDataAccess code
Note: Code based on 'Introduction to C Programming' tutorial
      by Michael E. Leverington
*/

// Header Files ///////////////////////////////////////////////////
#include "MetaDataAccess.h"

/*
Function name: getOpCodes
Algorithm: Opens file, acquires op code data,
           returns point to head of linked list
Precondition: For correct operation, file is available, is formatted correctly,
              and has all correctly formed op codes
Postcondition: In correct operation,
               returns pointer to head of op code linked list
Exceptions: Correctly and appropriately (without program failure)
            responds to and reports file access failure,
            incorrectly formatted lead or end descriptors,
            incorrectly formatted prompt, incorrect op code letter,
            incorrect op code name, op code value out of range,
            and incomplete file conditions
Note: None
*/
int getOpCodes( char *fileName, OpCodeType **opCodeDataHead )
{
    // Initialize function/variables
    const char READ_ONLY_FLAG[] = "r";
    int startCount = 0; int endCount = 0;
    OpCodeType *localHeadPtr = NULL;
    int accessResult;
    char dataBuffer[ MAX_STR_LEN ];
    OpCodeType *newNodePtr;
    FILE *fileAccessPtr;
    *opCodeDataHead = NULL;

    fileAccessPtr = fopen( fileName, READ_ONLY_FLAG );

    if( fileAccessPtr == NULL )
    {
        return MD_FILE_ACCESS_ER;
    }

    if( getLineTo( fileAccessPtr, MAX_STR_LEN, COLON,
      dataBuffer, IGNORE_LEADING_WS ) != NO_ERR
      || compareString( dataBuffer, "Start Program Meta-Data Code" ) != STR_EQ )
    {
        fclose( fileAccessPtr );
        return MD_CORRUPT_DESCRIPTOR_ERR;
    }

    newNodePtr = (OpCodeType *) malloc( sizeof( OpCodeType ) );
    accessResult = getOpCommand( fileAccessPtr, newNodePtr );
    startCount = updateStartCount( startCount, newNodePtr->opName );
    endCount = updateStartCount( endCount, newNodePtr->opName );

    if( accessResult != COMPLETE_OPCMD_FOUND_MSG )
    {
        fclose( fileAccessPtr );
        *opCodeDataHead = clearMetaDataList( localHeadPtr );
        free( newNodePtr );
        return accessResult;
    }

    while( accessResult == COMPLETE_OPCMD_FOUND_MSG )
    {
        localHeadPtr = addNode( localHeadPtr, newNodePtr );
        accessResult = getOpCommand( fileAccessPtr, newNodePtr );
        startCount = updateStartCount( startCount, newNodePtr->opName );
        endCount = updateStartCount( endCount, newNodePtr->opName );
    }

    if( accessResult == LAST_OPCMD_FOUND_MSG )
    {
        if( startCount == endCount )
        {
            localHeadPtr = addNode( localHeadPtr, newNodePtr );
            accessResult = NO_ERR;
            if( getLineTo( fileAccessPtr, MAX_STR_LEN, PERIOD,
                dataBuffer, IGNORE_LEADING_WS ) != NO_ERR
                || compareString( dataBuffer, "End Program Meta-Data Code" )
                != STR_EQ )
            {
                accessResult = MD_CORRUPT_DESCRIPTOR_ERR;
            }
        }
        else
        {
            accessResult = UNBALANCED_START_END_ERR;
        }
    }

    if( accessResult != NO_ERR )
    {
        localHeadPtr = clearMetaDataList( localHeadPtr );
    }

    fclose( fileAccessPtr );
    free( newNodePtr );
    *opCodeDataHead = localHeadPtr;
    return accessResult;
}

/*
Function name: getOpCommand
Algorithm: Acquires one op command, verifies all parts of it,
           returns as parameter
Precondition: File is open and file cursor is at beginning
              of an op code
Postcondition: In correct operation,
               finds, tests, and returns op command as parameter,
               and returns stats  as integer
               - either complete op command found,
               or last op command found
Exceptions: Correctly and appropriately (without program failure)
            responds to and reports file access failure,
            incorrectly formatted op command letter,
            incorrectly formatted op command name,
            incorrect or out of range op command value
Note: None
*/
int getOpCommand( FILE *filePtr, OpCodeType *inData )
{
    // Initialize function/variables
    const int MAX_OP_NAME_LENGTH = 10;  // actual max name length
    const int MAX_OP_VALUE_LENGTH = 9;  // actual max value length
    int intBuffer = 0;
    int sourceIndex = 0; int destIndex = 0;
    int accessResult;
    char strBuffer[ STD_STR_LEN ];

    accessResult = getLineTo( filePtr, STD_STR_LEN, SEMICOLON,
        strBuffer, IGNORE_LEADING_WS );

    if( accessResult == NO_ERR )
    {
        inData->opLtr = strBuffer[ sourceIndex ];
    }

    else
    {
        inData = NULL;
        return OPCMD_ACCESS_ERR;
    }

    switch( inData->opLtr )
    {
        case 'S':
        case 'A':
        case 'P':
        case 'M':
        case 'I':
        case 'O':
            break;

        default:
        inData = NULL;
        return CORRUPT_OPCMD_LETTER_ERR;
    }

    while( sourceIndex < STD_STR_LEN && strBuffer[ sourceIndex ] != LEFT_PAREN )
    {
        sourceIndex++;
    }

    sourceIndex++;

    while( sourceIndex < STD_STR_LEN
        && destIndex < MAX_OP_NAME_LENGTH
        && strBuffer[ sourceIndex ] != RIGHT_PAREN )
    {
        inData->opName[ destIndex ] = strBuffer[ sourceIndex ];
        destIndex++; sourceIndex++;
        inData->opName[ destIndex ] = NULL_CHAR;
    }

    if( checkOpString( inData->opName ) == False )
    {
        inData = NULL;
        return CORRUPT_OPCMD_NAME_ERR;
    }

    sourceIndex++;
    destIndex = 0;

    while( sourceIndex < STD_STR_LEN
        && destIndex < MAX_OP_VALUE_LENGTH
        && isDigit( strBuffer[ sourceIndex ] ) == True )
    {
        intBuffer *= 10;
        intBuffer += (int) ( strBuffer[ sourceIndex ] - '0' );
        destIndex++; sourceIndex++;
    }

    if( sourceIndex == STD_STR_LEN || destIndex == MAX_OP_VALUE_LENGTH )
    {
        inData = NULL;
        return CORRUPT_OPCMD_VALUE_ERR;
    }

    inData->opValue = intBuffer;

    if( inData->opLtr == 'S'
    && compareString( inData->opName, "end" ) == STR_EQ )
    {
        return LAST_OPCMD_FOUND_MSG;
    }

    return COMPLETE_OPCMD_FOUND_MSG;
}

/*
Function name: updateStartCount
Algorithm: Updates number of "start" op commands found in file
Precondition: Count >= 0, op string has "start" or other op name
Postcondition: If op string has "start", input count + 1 is returned;
               otherwise, input count is returned unchanged
Exceptions: None
Note: Bone
*/
int updateStartCount( int count, char *opString )
{
    if( compareString( opString, "start" ) == STR_EQ )
    {
        return count + 1;
    }

    return count;
}

/*
Function name: updateEndCount
Algorithm: Updates number of "end" op commands found in file
Precondition: Count >= 0, op string has "end" or other op name
Postcondition: If op string has "end", input count + 1 is returned;
               otherwise, input count is returned unchanged
Exceptions: None
Note: None
*/
int updateEndCount( int count, char *opString )
{
    if( compareString( opString, "end" ) == STR_EQ )
    {
        return count + 1;
    }

    return count;
}

/*
Function name: addNode
Algorithm: Adds op command sturcture with data to a linked list
Precondition: Linked list pointer assigned to null or to one op command link,
              struct pointer assigned to op command struct data
Postcondition: Assigns new structure node to beginning of linked list
               or end of established linked list
Exceptions: None
Note: Assumes memory access/availablity
*/
OpCodeType *addNode( OpCodeType * localPtr, OpCodeType *newNode )
{
    if( localPtr == NULL )
    {
        localPtr = (OpCodeType *) malloc( sizeof( OpCodeType ) );
        localPtr->opLtr = newNode->opLtr;
        copyString( localPtr->opName, newNode->opName );
        localPtr->opValue = newNode->opValue;
        localPtr->next = NULL;
        return localPtr;
    }

    localPtr->next = addNode( localPtr->next, newNode );
    return localPtr;
}

/*
Function name: checkOpString
Algorithm: Checks tested op string against list of possibles
Precondition: Tested op string is correct C-Style string
              with potential op command name in it
Postcondition: In correct operation,
               verifies the test string with one
               of the potential op strings and returns truel
               otherwise, returns false
Exceptions: None
Note: None
*/
Boolean checkOpString( char *testStr )
{

    if( compareString( testStr, "access" )      == STR_EQ
        || compareString( testStr, "allocate" )    == STR_EQ
        || compareString( testStr, "end" )         == STR_EQ
        || compareString( testStr, "hard drive" )  == STR_EQ
        || compareString( testStr, "keyboard" )    == STR_EQ
        || compareString( testStr, "printer" )     == STR_EQ
        || compareString( testStr, "monitor" )     == STR_EQ
        || compareString( testStr, "run" )         == STR_EQ
        || compareString( testStr, "start" )       == STR_EQ )
    {
        return True;
    }

    return False;
}

/*
Function name: isDigit
Algorithm: Checks for character digit, returns result
Precondition: Test value is character
Postcondition: If test value is a value '0' < value '9',
               returns true, otherwise returns false
Exceptions: None
Note: None
*/
Boolean isDigit( char testChar )
{
    return (testChar >= '0' && testChar <= '9' );
}

/*
Function name: displayMetaData
Algorithm: Iterates through op code linked list,
           displays op code data individually
Precondition: Linked list, with or without data
              (should not be called if no data)
Postcondition: Displays all op codes in list
Exceptions: None
Note: None
*/
void displayMetaData( OpCodeType *localPtr )
{
    printf( "\nMeta-Data File Display\n" );
    printf( "======================\n" );

    while( localPtr != NULL)
    {
        printf( "Op code letter: %c\n", localPtr->opLtr );
        printf( "Op code name: %s\n", localPtr->opName );
        printf( "Op code value: %d\n\n", localPtr->opValue );
        localPtr = localPtr->next;
    }
}

/*
Function name: displayMetaDataError
Algorithm: Uses enum/constant values as indices to select display string,
           then displays error message with selected string
Precondition: Integer input code has one of the enumerated code values
Postcondition: Error message is displated with the correct
               local error item
Exceptions: None
Note: None
*/
void displayMetaDataError( int code )
{
    char errList[ 10 ][ 35 ] = { "No Error",
                                 "Incomplete File Error",
                                 "Input Buffer Overrun",
                                 "MD File Access Error",
                                 "MD Corrupt Descriptor Error",
                                 "Op Command Access Error",
                                 "Corrupt Op Command Lettter Error",
                                 "Corrupt Op Command Name Error",
                                 "Corrupt Op Command Value Error",
                                 "Unbalanced Start-End Code Error" };

    printf("\nFATAL ERROR: %s, Program aborted\n", errList[ code ] );
}

/*
Function name: clearMetaDataList
Algorithm: Recursively iterates through op code linked list,
           returns memory to OS from the bottom of the list upward
Precondition: Linked list, with or without data
Postcondition: All node memory, if any, is returned to OS,
               return pointer (head) is set to null
Exceptions: None
Note: None
*/
OpCodeType *clearMetaDataList( OpCodeType *localPtr )
{
    if( localPtr != NULL )
    {
        if( localPtr->next != NULL )
        {
            clearMetaDataList( localPtr->next );
        }

        free( localPtr );
    }

    return NULL;
}
